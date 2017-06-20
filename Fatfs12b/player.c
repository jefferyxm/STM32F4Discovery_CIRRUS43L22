#include "player.h"
#include "string.h"
#include "diskio.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>   
#include "mad.h"
#include "libmad_config.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>   /* for pow() and log10() */
#include "mad.h"
#include "libmad_config.h"
#include "ff.h"
#include "stm32f4xx.h"
#include "board_init.h"
       
          
int MpegAudioDecoder(FIL *InputFp);
void (*I2S_DMA_Tx_Callback)(void);

static FRESULT fresult;
static UINT fnum;


void startDMA()
{
    DMA_Cmd(DMA1_Stream5, ENABLE);
}

void stopDMA()
{
    DMA_Cmd(DMA1_Stream5, DISABLE);
}

static FIL wavFile;
WavHead wavhead;
uint16_t buffer0[257],buffer1[257];
uint8_t bufferflag = 0;
uint8_t isread = 0;

void playWAV(char *filePath, char cmd)
{
    fresult = f_open(&wavFile, filePath ,FA_READ);
    if(fresult==FR_OK)
    {
        usart_debugMessage("打开音乐文件 ");
        usart_debugMessage(filePath);
        usart_debugMessage(" 成功\r\n");
    }
    else
    {
       usart_debugMessage("打开音乐文件失败\r\n") ;
    }
    
    fresult = f_read(&wavFile, &wavhead, sizeof(wavhead), &fnum);
    if(fresult==FR_OK)
    {
        usart_debugMessage("读取wavHeard成功\r\n");
    }
    else
    {
       usart_debugMessage("读取wavHeard失败\r\n") ;
    }
    
    fresult = f_read(&wavFile, (uint16_t *)buffer0, 512,&fnum);
    fresult = f_read(&wavFile, (uint16_t *)buffer1, 512,&fnum);
    
    I2S3_TX_DMAInit(buffer0, buffer1, 256);
    
    startDMA();

    while(1)
    {
        if(isread==1)
        {
            if(bufferflag==0)  // buffer0 空闲  //   
            {
                fresult = f_read(&wavFile, (uint16_t *)buffer0, 512,&fnum);
                //usart_sendByte(USART2,(uint8_t)(buffer0[0]&0xff));
            }
            else
            {
                fresult = f_read(&wavFile, (uint16_t *)buffer1, 512,&fnum);
                //usart_sendString(USART2,"XXXX");
            }
            isread = 0;
                
            if(fresult!=FR_OK)
            {
                    usart_sendString(USART2,"播放出错1");
                    stopDMA();
                    f_close(&wavFile);
            }                             
         }
         if(f_eof(&wavFile)) 
         {
            stopDMA();
            f_close(&wavFile);
            break;
         }
    }
}


FIL mp3File;
int playMP3(char *filePath, char cmd)
{
    fresult = f_open(&mp3File, filePath ,FA_READ);
    if(fresult==FR_OK)
    {
        usart_debugMessage("打开音乐文件 ");
        usart_debugMessage(filePath);
        usart_debugMessage(" 成功\r\n");
    }
    else
    {
       usart_debugMessage("打开音乐文件失败\r\n") ;
       return 1;
    }
    
    MpegAudioDecoder(&mp3File);
    
    return 0;
}


void play(char *rootPath, char *fileName, char cmd)
{ 
    char filePath[50] = {0};
    char *posPtr = NULL;
    strcat(filePath,rootPath);
    strcat(filePath,fileName);
    posPtr = strchr(filePath,'.');
    if(strcmp(posPtr,".wav")==0)   //wav 文件
    {
        usart_debugMessage("播放wav文件\r\n");
        playWAV(filePath,cmd);
        
    }else if(strcmp(posPtr,".mp3")==0)
    {
        usart_debugMessage("播放MP3文件\r\n");
        int result = playMP3(filePath,cmd);
        
    }else
    {
        usart_debugMessage("不支持的文件类型\r\n");
    }
}


//extern uint8_t bufferflag;
//extern uint8_t isread;
extern int nowUsingBuff0;
extern int nowUsingBuff1;

void DMA1_Stream5_IRQHandler()
{
    if(DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5)==SET)
   {
		
                isread = 1;  // dma 读取数据完成
		//I2S_DMA_Tx_Callback();
		//判断dma当前所使用的存储器
		if(DMA1_Stream5->CR&(1<<19))   //当前DMA使用存储器1，只能更改存储器0的数据
		{
			bufferflag = 0;
                        nowUsingBuff1 = 1;
                        nowUsingBuff0 = 0;
		}
		else
		{
			bufferflag = 1;
                        nowUsingBuff0 = 1;
                        nowUsingBuff1 = 0;
		}
		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5|DMA_IT_FEIF5|DMA_IT_DMEIF5|DMA_IT_TEIF5|DMA_IT_HTIF5);
	}
}


