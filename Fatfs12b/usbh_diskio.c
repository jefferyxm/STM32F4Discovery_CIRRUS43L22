/**
  ******************************************************************************
  * @file    usbh_diskio.c 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    08-May-2015
  * @brief   USB Key Disk I/O driver.
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
#include "ff_gen_drv.h"
#include "usbh_msc_core.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#if _USE_BUFF_WO_ALIGNMENT == 0
/* Local buffer use to handle buffer not aligned 32bits*/
#endif

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
extern USBH_HOST                     USB_Host;

/* Private function prototypes -----------------------------------------------*/
DSTATUS USBH_initialize (BYTE);
DSTATUS USBH_status (BYTE);
DRESULT USBH_read (BYTE, BYTE*, DWORD, UINT);

#if _USE_WRITE == 1
  DRESULT USBH_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
  DRESULT USBH_ioctl (BYTE, BYTE, void*);
#endif /* _USE_IOCTL == 1 */
  
const Diskio_drvTypeDef  USBH_Driver =
{
  USBH_initialize,
  USBH_status,
  USBH_read, 
#if  _USE_WRITE == 1
  USBH_write,
#endif /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
  USBH_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  lun : lun id
  * @retval DSTATUS: Operation status
  */
DSTATUS USBH_initialize(BYTE pdrv)
{
    if(HCD_IsDeviceConnected(&USB_OTG_Core))
    {  
      Stat &= ~STA_NOINIT;
    }
    
    return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  lun : lun id
  * @retval DSTATUS: Operation status
  */
DSTATUS USBH_status(BYTE pdrv)
{
    if (pdrv) return STA_NOINIT;		/* Supports only single drive */
    return Stat;
}

/**
  * @brief  Reads Sector(s) 
  * @param  lun : lun id
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USBH_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    BYTE status = USBH_MSC_OK;
  
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    
    
    if(HCD_IsDeviceConnected(&USB_OTG_Core))
    {  
      
      do
      {
        status = USBH_MSC_Read10(&USB_OTG_Core, buff,sector,512 * count);
        USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
        
        if(!HCD_IsDeviceConnected(&USB_OTG_Core))
        { 
          return RES_ERROR;
        }      
      }
      while(status == USBH_MSC_BUSY );
    }
    
    if(status == USBH_MSC_OK)
      return RES_OK;
    return RES_ERROR;
}

/**
  * @brief  Writes Sector(s)
  * @param  lun : lun id 
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USBH_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    BYTE status = USBH_MSC_OK;
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (Stat & STA_PROTECT) return RES_WRPRT;
    
    
    if(HCD_IsDeviceConnected(&USB_OTG_Core))
    {  
      do
      {
        status = USBH_MSC_Write10(&USB_OTG_Core,(BYTE*)buff,sector,512 * count);
        USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);
        
        if(!HCD_IsDeviceConnected(&USB_OTG_Core))
        { 
          return RES_ERROR;
        }
      }
      
      while(status == USBH_MSC_BUSY );
      
    }
    
    if(status == USBH_MSC_OK)
      return RES_OK;
    return RES_ERROR;
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  lun : lun id
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USBH_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
    DRESULT res = RES_OK;
  
    if (drv) return RES_PARERR;
    
    res = RES_ERROR;
    
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    
    switch (ctrl) {
    case CTRL_SYNC :		/* Make sure that no pending write process */
      
      res = RES_OK;
      break;
      
    case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
      
      *(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
      res = RES_OK;
      break;
      
    case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
      *(WORD*)buff = 512;
      res = RES_OK;
      break;
      
    case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
      
      *(DWORD*)buff = 512;
      
      break;
       
    default:
      res = RES_PARERR;
    }
    
    
    
    return res;
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

