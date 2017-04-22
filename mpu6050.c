/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "mpu6050.h"
#include "perip_func.h"
#include "math.h"

//宏定义调用底层的I2C接口
#define MPU6050_WR(reg,value)   i2c_writeByte(I2C2,SlaveAddress,reg,value)  //MPU6050 写寄存器
#define MPU6050_RD(reg)         i2c_readByte(I2C2,SlaveAddress,reg)             //MPU6050 读寄存器

int16_t g_i16_accX,g_i16_accY,g_i16_accZ;
int16_t g_i16_gyroX,g_i16_gyroY,g_i16_gyroZ;

float g_f32_accX,g_f32_accY,g_f32_accZ;
float g_f32_gyroX,g_f32_gyroY,g_f32_gyroZ;

float Roll,Pit,Yaw;

uint8_t g_u8_info;


void mpu6050_init(void)
{
    MPU6050_WR(PWR_MGMT_1, 0x00);	//解除休眠状态
    MPU6050_WR(SMPLRT_DIV, 0x07);	//陀螺仪采样率，典型值：0x07(125Hz)
    MPU6050_WR(CONFIG, 0x06);		//低通滤波频率，典型值：0x06(5Hz)
    MPU6050_WR(GYRO_CONFIG, 0x18);	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
    MPU6050_WR(ACCEL_CONFIG, 0x01);	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
    //MPU6050_WR(INT_PIN_CFG,0x00);
    //MPU6050_WR(INT_ENABLE,0x01);
}

int32_t start2 = 0;
void read_6050()
{
    //start2 = SysTick->VAL;
    g_i16_accX = MPU6050_RD(ACCEL_XOUT_H);			//加速计读数据
    g_i16_accX = (g_i16_accX << 8) | MPU6050_RD(ACCEL_XOUT_L);
    //start2 = start2 - SysTick->VAL;
    
    g_i16_accY = MPU6050_RD(ACCEL_YOUT_H);
    g_i16_accY = (g_i16_accY << 8) | MPU6050_RD(ACCEL_YOUT_L);
    g_i16_accZ = MPU6050_RD(ACCEL_ZOUT_H);
    g_i16_accZ = (g_i16_accZ << 8) | MPU6050_RD(ACCEL_ZOUT_L);

    g_i16_gyroX = MPU6050_RD(GYRO_XOUT_H);				//陀螺仪数据
    g_i16_gyroX = (g_i16_gyroX << 8) | MPU6050_RD(GYRO_XOUT_L);
    g_i16_gyroY = MPU6050_RD(GYRO_YOUT_H);
    g_i16_gyroY = (g_i16_gyroY << 8) | MPU6050_RD(GYRO_YOUT_L);
    g_i16_gyroZ = MPU6050_RD(GYRO_ZOUT_H);
    g_i16_gyroZ = (g_i16_gyroZ << 8) | MPU6050_RD(GYRO_ZOUT_L);
    
    //加速度计校准
    g_f32_accX = (g_i16_accX -550)/1.0;
    g_f32_accY = (g_i16_accY +500)/1.0;
    g_f32_accZ = (g_i16_accZ+3500)/1.02;
    
    g_i16_gyroX += 21;     //陀螺仪抑制零漂    
    g_i16_gyroY -= 18;
    g_i16_gyroZ += 11;

    g_f32_gyroX = (float)(g_i16_gyroX/16.4);
    g_f32_gyroY = (float)(g_i16_gyroY/16.4);
    g_f32_gyroZ = (float)(g_i16_gyroZ/16.4);

}

/**姿态解算**/
#define Kp 25.0f			//控制加速度计修正陀螺仪积分姿态的速度
#define Ki 0.02f
#define halfT 0.0015f		// 姿态解算时间的一半。此处解算姿态速度为500HZ，因此halfT 为0.001
#define Gyro_G 		0.0010642f	

//初始化四元数
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;

//定义姿态解算误差的积分
float exInt = 0, eyInt = 0, ezInt = 0;

//以下为姿态解算函数。
//参数gx，gy，gz分别对应三个轴的角速度，单位是弧度 / 秒;
//参数ax，ay，az分别对应三个轴的加速度原始数据
//由于加速度的噪声较大，此处应采用滤波后的数据

void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
	float norm;
	float vx, vy, vz;
	float ex, ey, ez;

	//将加速度的原始数据，归一化，得到单位加速度
	norm = sqrt(ax*ax + ay*ay + az*az);
	ax = ax / norm;
	ay = ay / norm;
	az = az / norm;

	//把四元数换算成“方向余弦矩阵”中的第三列的三个元素。根据余弦矩阵和欧拉角的定义，地理坐标系的重力向量，转到机体坐标系，正好是这三个元素。所以这里的vx、vy、vz，其实就是当前的机体坐标参照系上，换算出来的重力单位向量。(用表示机体姿态的四元数进行换算)
	vx = 2 * (q1*q3 - q0*q2);
	vy = 2 * (q0*q1 + q2*q3);
	vz = q0*q0 - q1*q1 - q2*q2 + q3*q3;

	/*这里说明一点，加速度计由于噪声比较大，而且在飞行过程中，受机体振动影响比陀螺仪明显，短时间内的可靠性不高。陀螺仪噪声小，但是由于积分是离散的，长时间的积分会出现漂移的情况，因此需要将用加速度计求得的姿态来矫正陀螺仪积分姿态的漂移。
	在机体坐标参照系上，加速度计测出来的重力向量是ax、ay、az; 陀螺积分后的姿态来推算出的重力向量是vx、vy、vz; 它们之间的误差向量，就是陀螺积分后的姿态和加速度计测出来的姿态之间的误差。
	向量间的误差，可以用向量积(也叫外积、叉乘)来表示，ex、ey、ez就是两个重力向量的叉积。这个叉积向量仍旧是位于机体坐标系上的，而陀螺积分误差也是在机体坐标系，而且叉积的大小与陀螺积分误差成正比，正好拿来纠正陀螺。由于陀螺是对机体直接积分，所以对陀螺的纠正量会直接体现在对机体坐标系的纠正。
	叉乘是数学基础，百度百科里有详细解释。*/
	ex = (ay*vz - az*vy);
	ey = (az*vx - ax*vz);
	ez = (ax*vy - ay*vx);

	//将叉乘误差进行积分
	exInt = exInt + ex*Ki;
	eyInt = eyInt + ey*Ki;
	ezInt = ezInt + ez*Ki;

	//用叉乘误差来做PI修正陀螺零偏，通过调节Kp，Ki两个参数，可以控制加速度计修正陀螺仪积分姿态的速度
	gx = gx* Gyro_G;
	gy = gy* Gyro_G;
	gz = gz* Gyro_G;

	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;

	/*四元数微分方程，最后转化成欧拉角即可了.*/
	q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
	q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
	q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
	q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

	//四元数单位化
	norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	//转化为欧拉角
	//Roll = atan(2.0*(q2*q3 + q0*q1) / (q0*q0 - q1*q1 - q2*q2 + q3*q3)) * 57.3;
	//Pit = asin(-2.0*q1*q3 + 2 * q0*q2) * 57.3;
	//Yaw = atan(2.0*(2*q0*q1+2*q2*q3))*57.3;
	
	//转化为欧拉角
	Roll = 57.3*atan2((2 * q0*q1 + 2 * q2*q3), (1 - 2 * q1*q1 - 2 * q2*q2));
	Pit = 57.3*asin(2 * q0*q2 - 2 * q1*q3);
	Yaw = 57.3*atan2((2 * q0*q3 + 2 * q1*q2), (1 - 2 * q2*q2 - 2 * q3*q3));
	//Yaw += gz*0.1f;
}


/******kalmsn滤波********/

float angle_dotx, angle_doty, angle_dotz;
//-------------------------------------------------------
// 0.00015     //0.0001
const float Q_angle = 0.001, Q_gyro = 0.003, R_angle = 0.5, dt = 0.0002;
//0.0001         //0.00015        //1.2
//注意:dt的取值kalman滤波器采样时间;         //0.8
float P[2][2] = {
	{ 1, 0 },
	{ 0, 1 }
};

float Pdot[4] = { 0, 0, 0, 0 };

const char C_0 = 1;

float  PCt_0, PCt_1, E, K_0, K_1, t_0, t_1;

float q_biasx, q_biasy, q_biasz;
float angle_errx, angle_erry, angle_errz;

float angle_x, angle_y, angle_z;
float gyro_x, gyro_y, gyro_z;

#define GYRO_G_X	20.95	
#define GYRO_G_Y	10.75	
#define GYRO_G_Z	0.0010642f	

float K_Roll, K_Pit;

void smoothing()
{

	gyro_x = g1 * GYRO_G_X;
	gyro_y = g2 * GYRO_G_Y;
	//gyro_z = g3 * GYRO_G_Z;

	angle_x += (gyro_x - q_biasx) * dt;
	angle_y += (gyro_y - q_biasy) * dt;
	//angle_z += (gyro_z - q_biasz) * dt;

	Pdot[0] = Q_angle - P[0][1] - P[1][0];
	Pdot[1] = -P[1][1];
	Pdot[2] = -P[1][1];
	Pdot[3] = Q_gyro;

	P[0][0] += Pdot[0] * dt;
	P[0][1] += Pdot[1] * dt;
	P[1][0] += Pdot[2] * dt;
	P[1][1] += Pdot[3] * dt;

	angle_errx = Roll - angle_x;
	angle_erry = Pit - angle_y;

	PCt_0 = C_0 * P[0][0];
	PCt_1 = C_0 * P[1][0];

	E = R_angle + C_0 * PCt_0;

	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;

	t_0 = PCt_0;
	t_1 = C_0 * P[0][1];

	P[0][0] -= K_0 * t_0;
	P[0][1] -= K_0 * t_1;
	P[1][0] -= K_1 * t_0;
	P[1][1] -= K_1 * t_1;

	angle_x += K_0 * angle_errx;
	angle_y += K_0 * angle_erry;
	//angle_z += K_0 * angle_errz;

	q_biasx += K_1 * angle_errx;
	q_biasy += K_1 * angle_erry;
	//q_biasz += K_1 * angle_errz;

	angle_dotx = gyro_x - q_biasx;
	angle_doty = gyro_y - q_biasy;
	//angle_dotz = gyro_z - q_biasz;

	K_Roll = angle_x;
	K_Pit = angle_y;

	//g1 = angle_dotx;
	//g2 = angle_doty;
	//g3 = angle_dotz;
}



void get_mpuinfo()
{
    g_u8_info = MPU6050_RD(WHO_AM_I);
}

void mpu6050_writeByte(uint8_t regAddr,uint8_t value)
{
    MPU6050_WR(regAddr,value);
}

uint8_t mpu6050_readByte(uint8_t regAddr)
{
    return MPU6050_RD(regAddr);
}


