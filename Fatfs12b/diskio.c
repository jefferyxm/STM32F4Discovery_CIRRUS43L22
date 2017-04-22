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

  /* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

extern MSD_CARDINFO SD0_CardInfo;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	//DSTATUS stat;
	//int result;
	//stat = STA_NOINIT;
	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_status();

		// translate the reslut code here

		return RES_OK;

	case DEV_MMC :
		
		// translate the reslut code here
		return RES_OK;

	case DEV_USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return RES_OK;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

        stat = STA_NOINIT;
	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_MMC :
                result = MSD0_Init();
                if(result==0)
                        stat = RES_OK;
                else 
                        stat = STA_NOINIT;

		// translate the reslut code here

		return stat;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

        res = RES_PARERR;
        
	if( !count )
	{    
        return RES_PARERR;  /* count不能等于0，否则返回参数错误 */
	}

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

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

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

        res = RES_PARERR;
        
	if( !count )
	{    
		return RES_PARERR;  /* count不能等于0，否则返回参数错误 */
	}

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

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
		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	//int result;

        res = RES_PARERR;
        
	switch (pdrv) {
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

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
	/* MMC/SDC command */
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

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}


DWORD get_fattime (void)
{
   return ((DWORD)(2015 - 1980) << 25) /* Year 2015 */
        | ((DWORD)1 << 21) /* Month 1 */
        | ((DWORD)1 << 16) /* Mday 1 */
        | ((DWORD)0 << 11) /* Hour 0 */
        | ((DWORD)0 << 5) /* Min 0 */
        | ((DWORD)0 >> 1); /* Sec 0 */
}
