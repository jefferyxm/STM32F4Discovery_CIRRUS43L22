#ifndef __CIR43L22_H_
#define __CIR43L22_H_
#include "perip_func.h"

/*CIR43L22 IIC REGESTER ADDR*/

#define  DEV_CIR43L22           0x94            //IIC Éè±¸µØÖ·

#define  ID                     0x01
#define  PowerCtl1              0x02
#define  PowerCtl2              0x04
#define  ClockCtl               0x05
#define  InterfaceCtl1          0x06
#define  InterfaceCtl2          0x07
#define  PassthA                0x08
#define  PassthB                0x09
#define  ZC_SR                  0x0a
#define  PassthGC               0x0c
#define  Playbackctl1           0x0d
#define  MiscCtl                0x0e
#define  PlaybackCtl2           0x0f
#define  PassthA_ol             0x14
#define  PassthBvol             0x15
#define  PCMAvol                0x1a
#define  PCMBvol                0x1b
#define  Beepfre                0x1c  //ontime also
#define  Beepvol                0x1d  //offtime also
#define  Beep_ToneCfg           0x1e
#define  ToneCtl                0x1f
#define  MasterAvol             0x20
#define  MasterBvol             0x21
#define  HeadphoneAvol          0x22
#define  HeadphoneBvol          0x23
#define  SpeakerAvol            0x24
#define  SpeakerBvol            0x25
#define  ChMixer_Swap           0x26
#define  LimCtl1_Thre           0x27
#define  LimCtl2_RRate          0x28
#define  LimAttackRate          0x29
#define  OverFlow_ClockStatus   0x2e
#define  BatteryCompensation    0x2f
#define  VPBatteryLevel         0x30
#define  SpeakerStatus          0x31
#define  ChargePumpFre          0x34

/*CIR43L22 Necessary_Configuration macro*/
#define  IIC_WRITE(reg,val)     i2c_writeByte(I2C1,DEV_CIR43L22,reg,val)
#define  IIC_READ(reg)          i2c_readByte(I2C1,DEV_CIR43L22,reg)

#define  getChipID()              IIC_READ(ID)&0XF8
#define  getChipRevsion()         IIC_READ(ID)&0X07
#define  PowerUp()                IIC_WRITE(PowerCtl1, 0x9e)
#define  PowerDown()	          IIC_WRITE(PowerCtl1, 0x01)
#define  headPhonePW()	          IIC_WRITE(PowerCtl2, 0xaf)  // headPhone power control, speaker power is always off
#define  SlaveMode()              IIC_WRITE(InterfaceCtl1,0x03)  //slave mode iis data formate  16bit  07// iis   0b--right  03--left  
#define  ClockControl()	          IIC_WRITE(ClockCtl, 0x40)   // mck 11.2896, fs 22.05khz

#define  reset_high()			  GPIOD->BSRRL = GPIO_Pin_4
#define  reset_low()			  GPIOD->BSRRH = GPIO_Pin_4

//function 
void Init_CIR43L22();

#endif

