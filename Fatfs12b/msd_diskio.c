/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sdCard.h"


/* Private function prototypes -----------------------------------------------*/
DSTATUS MSD_initialize (BYTE);
DSTATUS MSD_status (BYTE);
DRESULT MSD_read (BYTE, BYTE*, DWORD, UINT);

#if _USE_WRITE == 1
    DRESULT MSD_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
  
#if _USE_IOCTL == 1
    DRESULT MSD_ioctl (BYTE, BYTE, void*);
#endif /* _USE_IOCTL == 1 */
  
  
const Diskio_drvTypeDef  MSD_Driver =
{
    MSD_initialize,
    MSD_status,
    MSD_read, 
#if  _USE_WRITE == 1
    MSD_write,
#endif /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
    MSD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

extern MSD_CARDINFO SD0_CardInfo;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS MSD_initialize (BYTE)
{
    result = MSD0_Init();
    if(result==0)
            stat = RES_OK;
    else 
            stat = STA_NOINIT;

    // translate the reslut code here

    return stat;
}


DSTATUS MSD_status (BYTE)
{
    return RES_OK;
}


DRESULT MSD_read (BYTE, BYTE*, DWORD, UINT)
{
    DRESULT res;
    int result;

    res = RES_PARERR;
    
    if( !count )
    {    
        return RES_PARERR;  /* count不能等于0，否则返回参数错误 */
    }
    
    if(count==1)            /* 1个sector的读操作 */      
    {   
            result =  MSD0_ReadSingleBlock( sector ,buff );  
    }                                                
    else                    /* 多个sector的读操作 */     
    {  
            result = MSD0_ReadMultiBlock(sector , buff ,count);
    }

    if(result == 0) 
            res = RES_OK;
    else 
            res =  RES_ERROR; 
    return res; 
}





#if _USE_WRITE == 1
DRESULT MSD_write (BYTE, const BYTE*, DWORD, UINT)
{
    DRESULT res;
    int result;

    res = RES_PARERR;
    
    if( !count )
    {    
        return RES_PARERR;  /* count不能等于0，否则返回参数错误 */
    }
    
    if(count == 1)            /* 1个sector的写操作 */      
    {   
        result = MSD0_WriteSingleBlock( sector , (uint8_t *)(&buff[0]) ); 
    }                                                
    else                    /* 多个sector的写操作 */    
    {  
        result = MSD0_WriteMultiBlock( sector , (uint8_t *)(&buff[0]) , count );
    } 

    if(result == 0)
        res =  RES_OK; 
    else 
        res = RES_ERROR;  

    return res;
}
#endif /* _USE_WRITE == 1 */
  

#if _USE_IOCTL == 1
DRESULT MSD_ioctl (BYTE, BYTE, void*)
{
    DRESULT res;
    res = RES_PARERR;

    switch (cmd) 
    {
        case CTRL_SYNC : 
                        return RES_OK;
        case GET_SECTOR_COUNT : 
                        *(DWORD*)buff = SD0_CardInfo.Capacity/SD0_CardInfo.BlockSize;
                        return RES_OK;
        case GET_BLOCK_SIZE :
                        *(WORD*)buff = SD0_CardInfo.BlockSize;
                        return RES_OK;	
        case CTRL_POWER :
                        break;
        case CTRL_LOCK :
                        break;
        case CTRL_EJECT :
                        break;
        case MMC_GET_TYPE :
                        break;
        case MMC_GET_CSD :
                        break;
        case MMC_GET_CID :
                        break;
        case MMC_GET_OCR :
                        break;
        case MMC_GET_SDSTAT :
                        break;	
    } 
    return RES_PARERR;  
}
#endif /* _USE_IOCTL == 1 */
