#include "main.h"
#include "CIR43L22.h"


void resetPinInit()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD,&GPIO_InitStruct);
    //reset_high();
}

uint8_t IDD = 0;
void Init_CIR43L22()
{
// 1 reset pin high
	resetPinInit();
        reset_low();
        for(int delay = 0; delay < 0x3ff; delay++);
        reset_high();
   

// 2 keep PowerCtll1 with value of 0x01, set desired regeister value
	IIC_WRITE(PowerCtl1, 0x01);
	headPhonePW();   //open headphone power
        IIC_WRITE(ClockCtl, 0x80);  // clock: mck 11.2896  fs 44.1khz  auto detet
        
	SlaveMode();  // data formate iis formate
	//IIC_WRITE(Playbackctl1, 0xc0);  // headphone gain 1.0, with 
	//IIC_WRITE(PlaybackCtl2, 0x00);
	IIC_WRITE(MasterAvol, 0x01); 
	IIC_WRITE(MasterBvol, 0x01);
        
        PowerUp();
        
        IIC_WRITE(PCMAvol,0x53);  //53
        IIC_WRITE(PCMBvol,0x53);
        
	
        IIC_WRITE(0x0a, 0x00);
	IIC_WRITE(0x27, 0x00);
	IIC_WRITE(0x1f, 0x0f);
	IIC_WRITE(0x32, 0x00);
	IIC_WRITE(0x00, 0x00);
        
// 3 required settings
//	IIC_WRITE(0x99, 0x00);
//	IIC_WRITE(0x80, 0x47);
//	IIC_WRITE(0x32, 0x80);
//	IIC_WRITE(0x32, 0x00);
//	IIC_WRITE(0x00, 0x00);
        
        

// 4 set mck and other necessary clock	
	
        IDD = IIC_READ(ClockCtl);
// 5 power up
	

        
// 6 reset low
	//reset_low();
        
        
}















