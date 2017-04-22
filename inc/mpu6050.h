

#include"stm32f4xx.h"
#ifndef __DEV_MPU6050_H__
#define __DEV_MPU6050_H__


#define     MPU6050_DEVICE          I2C2        //定义MPU6050 所用的接口 为 I2C0

/* mpu6050 common Register Address ------------------------------------------------------------*/
#define	SMPLRT_DIV		(0x19)	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			(0x1A)	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		(0x1B)	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	(0x1C)	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

#define	ACCEL_XOUT_H    (0x3B)  //X轴加速度  加速度
#define	ACCEL_XOUT_L	(0x3C)	
#define	ACCEL_YOUT_H	(0x3D)  //Y轴加速度
#define	ACCEL_YOUT_L	(0x3E)
#define	ACCEL_ZOUT_H	(0x3F)  //Z轴加速度
#define	ACCEL_ZOUT_L	(0x40)

#define	TEMP_OUT_H		(0x41)
#define	TEMP_OUT_L		(0x42)

#define	GYRO_XOUT_H		(0x43)  //X轴角速度  陀螺仪
#define	GYRO_XOUT_L		(0x44)	
#define	GYRO_YOUT_H		(0x45)  //Y轴角速度
#define	GYRO_YOUT_L		(0x46)
#define	GYRO_ZOUT_H		(0x47)  //Z轴角速度
#define	GYRO_ZOUT_L		(0x48)

#define	PWR_MGMT_1		(0x6B)	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		(0x75)	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	(0xd0)	//硬件I2C地址0x68 软件I2C地址0xD0




/*  interrupt register */
#define INT_PIN_CFG     (0x37)   //  外部中断引脚配置
#define INT_ENABLE      (0x38)   //外部中断信号使能
#define INT_STATUS      (0x39)   //中断状态寄存器

/*FIFO*/

#define FIFO_EN         (0x23)
#define FIFO_COUNT_H    (0x72)   // FIFO计数
#define FIFO_COUNT_L    (0x73) 
#define FIFO_R_W        (0X74)
#define USER_CTRL       (0x6A)



//function declartion
void  mpu6050_init(void);                        //初始化MPU6050
void mpu6050_writeByte(uint8_t regAddr,uint8_t value);
uint8_t mpu6050_readByte(uint8_t regAddr);
void read_6050();
void get_mpuinfo();
void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);
// global var
extern int16_t g_i16_accX,g_i16_accY,g_i16_accZ;
extern int16_t g_i16_gyroX,g_i16_gyroY,g_i16_gyroZ;

extern float g_f32_accX,g_f32_accY,g_f32_accZ;
extern float g_f32_gyroX,g_f32_gyroY,g_f32_gyroZ;
extern float Roll,Pit,Yaw;

extern uint8_t g_u8_info;
#endif  //__FIRE_MPU6050_H__