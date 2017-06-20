/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file includes the usb host library user callbacks
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "player.h"
#include "stdio.h"
#include "file_test.h"
    
extern char USBH_Path[4];

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
#define IMAGE_BUFFER_SIZE    512
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[15]  = {0};

static FATFS fatfs;

uint8_t Image_Buf[IMAGE_BUFFER_SIZE];
uint8_t line_idx = 0;   

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized\n";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached \n";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed \n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected\n";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault \n";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\n";

/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level);
static void     Toggle_Leds(void);

void usart_debugMessage(char*str);

/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 


/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
  
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
  //LCD_UsrLog((void *)MSG_DEV_ATTACHED);
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
  
  /* Set default screen color*/ 
  //LCD_ErrLog((void *)MSG_UNREC_ERROR); 
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Status
*/
void USBH_USR_DeviceDisconnected (void)
{
  //LCD_ErrLog((void *)MSG_DEV_DISCONNECTED);
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
  /* callback for USB-Reset */
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
    //LCD_UsrLog((void *)MSG_DEV_HIGHSPEED);
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
    //LCD_UsrLog((void *)MSG_DEV_FULLSPEED);
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
    //LCD_UsrLog((void *)MSG_DEV_LOWSPEED);
  }
  else
  {
    //LCD_UsrLog((void *)MSG_DEV_ERROR);
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
  //USBH_DevDesc_TypeDef *hs;
  //hs = DeviceDesc;  
  //LCD_UsrLog("VID : %04Xh\n" , (uint32_t)(*hs).idVendor); 
  //LCD_UsrLog("PID : %04Xh\n" , (uint32_t)(*hs).idProduct); 
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
  USBH_InterfaceDesc_TypeDef *id;
  
  id = itfDesc;  
  
  if((*id).bInterfaceClass  == 0x08)
  {
    //LCD_UsrLog((void *)MSG_MSC_CLASS);
  }
  else if((*id).bInterfaceClass  == 0x03)
  {
    //LCD_UsrLog((void *)MSG_HID_CLASS);
  }    
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
  //LCD_UsrLog("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
  //LCD_UsrLog("Product : %s\n", (char *)ProductString);  
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
  //LCD_UsrLog( "Serial Number : %s\n", (char *)SerialNumString);    
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
  
//  /* Enumeration complete */
//  LCD_UsrLog((void *)MSG_DEV_ENUMERATED);
//  
//  LCD_SetTextColor(Green);
//  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, "To see the root content of the disk : " );
//  LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 30, "Press Key...                       ");
//  LCD_SetTextColor(LCD_LOG_DEFAULT_COLOR); 
  
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
  //LCD_ErrLog ("No registered class for this device. \n\r");
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
  USBH_USR_Status usbh_usr_status;
  
  usbh_usr_status = USBH_USR_NO_RESP;  
  
  /*Key B3 is in polling mode to detect user action */
  if(0) 
  {
    
    usbh_usr_status = USBH_USR_RESP_OK;
    
  } 
  usbh_usr_status = USBH_USR_RESP_OK;
  return usbh_usr_status;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Status
*/
void USBH_USR_OverCurrentDetected (void)
{
  //LCD_ErrLog ("Overcurrent detected.");
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Status
*/
static FIL newfile;
static FRESULT fresult;
static int itemCnt =1;
char playlistBuff[4096] = {0};
int USBH_USR_MSC_Application(void)
{
  uint8_t writeTextBuff[] = "STM32 Connectivity line Host Demo application using FAT_FS   ";
  uint16_t bytesWritten, bytesToWrite;
  int musicIndex = 0;
  char musicName[256] = {0};
  char musicIndexString[10] = {0};
  char *posPtr = NULL;
  
  switch(USBH_USR_ApplicationState)
  {
      case USH_USR_FS_INIT: 
        
          /* Initialises the File System*/
          if ( f_mount(&fatfs, "", 0) != FR_OK ) 
          {
            /* efs initialisation fails*/
            usart_debugMessage("> Cannot initialize File System. \r\n");
            return(-1);
          }
          usart_debugMessage("> File System initialized. \r\n");
       
          if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
          {
            //LCD_ErrLog((void *)MSG_WR_PROTECT);
          }
          
          USBH_USR_ApplicationState = USH_USR_FS_READLIST;
          break;
        
      case USH_USR_FS_READLIST:
        
          //LCD_UsrLog((void *)MSG_ROOT_CONT);
          Explore_Disk(USBH_Path, 1);
          line_idx = 0;   
          USBH_USR_ApplicationState = USH_USR_FS_PLAY;
          
          break;
        
      case USH_USR_FS_WRITEFILE:
        
          USB_OTG_BSP_mDelay(100);
          
          /*Key B3 in polling*/
          //while(HCD_IsDeviceConnected(&USB_OTG_Core))        
          {
            Toggle_Leds();
          }
          /* Writes a text file, STM32.TXT in the disk*/
          usart_debugMessage("> Writing File to disk flash ...\r\n");
          if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
          {
            
            usart_debugMessage ( "> Disk flash is write protected\r\n");
            USBH_USR_ApplicationState = USH_USR_FS_PLAY;
            break;
          }
          
          char fileName[] = "usbfile.txt";
          char fileBuff[] = "STM32 Connectivity line Host Demo application using FAT_FS";
          
          /* Register work area for logical drives */
          f_mount(&fatfs, "", 0);

          if(writFile(USBH_Path, fileName, fileBuff, sizeof(fileBuff)))
          {
              usart_debugMessage ( "> wirte file success\r\n");
          }
          else
          {
              usart_debugMessage ( "> wirte file failed\r\n");
          }

          USBH_USR_ApplicationState = USH_USR_FS_PLAY; 
          break;
        
      case USH_USR_FS_PLAY:
          usart_debugMessage("> play \r\n");//
          /*Key B3 in polling*/
          //while(HCD_IsDeviceConnected(&USB_OTG_Core)) 
          {
            Toggle_Leds();
          }
        
          while(HCD_IsDeviceConnected(&USB_OTG_Core))
          {
            if (f_mount(&fatfs, USBH_Path, 0) != FR_OK )   //挂载
            {
              /* fat_fs initialisation fails*/
              return(-1);
            }

            // 读写速度测试
            //read_speedtest(USBH_Path,"music3.wav");
        
            //播放音乐
            //playWAV(USBH_Path,"music3.wav",0);
            
//            fresult = f_open(&newfile, "0:2.mp3",FA_READ);            
//            if(fresult==FR_OK)
//            {
//                MpegAudioDecoder(&newfile);
//            }
//            f_close(&newfile);
            
            //随机生成要播放的条目
            musicIndexString[0] = '&';
            musicIndex = RNG_Get_RandomRange(1,itemCnt-1);
            //musicIndex = 5;
            sprintf(&musicIndexString[1],"%d",musicIndex);
            strcat(musicIndexString,"*");
            
            //读取playlist,查找相应的条目
            readFile(USBH_Path,"playlist.txt",playlistBuff,sizeof(playlistBuff));
            posPtr = strstr(playlistBuff,musicIndexString);
            if(posPtr != NULL)   //搜索到了
            {
                posPtr = posPtr + strlen(musicIndexString);
                u8 i = 0;
                do
                {
                    musicName[i] = *posPtr;
                    i++;
                    posPtr++;
                }while(*posPtr!='#');
                musicName[i] = '\0';
                play(USBH_Path,musicName,0);
            }
            else //没有搜索到
            {
                
            }
            
        }
        break;
      default: break;
  }
  return(0);
}

/**
* @brief  Explore_Disk 
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/
static FIL playList;
static UINT fnum;
static uint8_t Explore_Disk (char* path , uint8_t recu_level)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    UINT length;
    char *fn;
    char tmp[256] = {0};
    char playlistPath[20] = {0};
    char * posPtr = NULL;
    
    strcat(playlistPath,path);
    strcat(playlistPath,"playlist.txt");
    fresult = f_open(&playList, playlistPath, FA_CREATE_ALWAYS|FA_WRITE);//创建播放列表文件
    if(fresult!=FR_OK)
    {
        return 0;
    }
    
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
      while(HCD_IsDeviceConnected(&USB_OTG_Core)) 
      {
          res = f_readdir(&dir, &fno);
          if (res != FR_OK || fno.fname[0] == 0) 
          { 
              break;
          }
          if (fno.fname[0] == '.')
          {
              continue;
          }

          fn = fno.fname;
          tmp[0] = 0;
          strcat(tmp, fn);
          posPtr = strchr(tmp,'.');
          if((strcmp(posPtr,".wav")==0)||(strcmp(posPtr,".mp3")==0))
          {
              tmp[0] = '&';
              sprintf(&tmp[1],"%d",itemCnt);   //sprintf 会添加 '\0'
              itemCnt++;
              strcat(tmp, "*"); 
              strcat(tmp, fn);
              strcat(tmp, "#");
              strcat(tmp, "\r\n");
              
              length = strlen(tmp) ;   //不写入 '\0'
              fresult = f_write (
                          &playList,			/* Pointer to the file object */
                          tmp,	                        /* Pointer to the data to be written */
                          length,			        /* Number of bytes to write */
                          &fnum			        /* Pointer to number of bytes written */
                         );
          }
      }
    }
    f_close(&playList);
    return res;
}


/**
* @brief  Toggle_Leds
*         Toggle leds to shows user input state
* @param  None
* @retval None
*/
static void Toggle_Leds(void)
{
   
}
/**
* @brief  USBH_USR_DeInit
*         Deinit User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

