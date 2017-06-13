#include "player.h"
#include "string.h"
#include "perip_func.h"
#include "ff.h"
#include "diskio.h"

WavHead wavhead;
void (*I2S_DMA_Tx_Callback)(void);
uint16_t buffer0[257],buffer1[257];//��Ҫ���ң�����ǿ��֢������
uint8_t bufferflag = 0;
uint8_t isread = 0;

static FRESULT fresult;
static FIL newfile;

static UINT fnum;


void startDMA()
{
    DMA_Cmd(DMA1_Stream5, ENABLE);
}

void stopDMA()
{
    DMA_Cmd(DMA1_Stream5, DISABLE);
}

FIL wavFile;
void playWAV(char *rootPath, char *fileName,char cmd)
{
    char filePath[50] = {0};
    strcat(filePath,rootPath);
    strcat(filePath,fileName);
    fresult = f_open(&wavFile, filePath ,FA_READ);
    if(fresult==FR_OK)
    {
        usart_debugMessage("�������ļ� ");
        usart_debugMessage(filePath);
        usart_debugMessage(" �ɹ�\r\n");
    }
    else
    {
       usart_debugMessage("�������ļ�ʧ��\r\n") ;
    }
    
    fresult = f_read(&wavFile, &wavhead, sizeof(wavhead), &fnum);
    if(fresult==FR_OK)
    {
        usart_debugMessage("��ȡwavHeard�ɹ�\r\n");
    }
    else
    {
       usart_debugMessage("��ȡwavHeardʧ��\r\n") ;
    }
    
    fresult = f_read(&wavFile, (uint16_t *)buffer0, 512,&fnum);
    fresult = f_read(&wavFile, (uint16_t *)buffer1, 512,&fnum);
    
    I2S3_TX_DMAInit(buffer0, buffer1, 256);
    
    startDMA();

    while(1)
    {
        if(isread==1)
        {
            if(bufferflag==0)  // buffer0 ����  //   
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
                    usart_sendString(USART2,"���ų���1");
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


void play()
{ 
    
    //fresult = f_mount(&fatfs,"1:",1);
    fresult = f_open(&newfile, "0:\music3.wav",FA_READ);
	if(fresult==FR_OK)
	{
		usart_sendString(USART2,"��music0�ļ��ɹ�");
                //��ȡwave��Ƶͷ��Ϣ
		fresult = f_read(&newfile, &wavhead, sizeof(wavhead), &fnum);
                
                //�ȶ�ȡһ����Ƶ��Ϣ��������
                fresult = f_read(&newfile, (uint16_t *)buffer0, 512,&fnum);
                fresult = f_read(&newfile, (uint16_t *)buffer1, 512,&fnum);
                
                I2S3_TX_DMAInit(buffer0, buffer1, 256);
                
                
                startDMA();  // ����dma���䣬��ʼ������  ��ʹ�ô洢��0����
                int i  = 0;
                while(1)
                {
                    if(isread==1)
                    {
                        if(bufferflag==0)  // buffer0 ����  //   
                        {
                            fresult = f_read(&newfile, (uint16_t *)buffer0, 512,&fnum);
                            //usart_sendByte(USART2,(uint8_t)(buffer0[0]&0xff));
                        }
                        else
                        {
                            fresult = f_read(&newfile, (uint16_t *)buffer1, 512,&fnum);
                            //usart_sendString(USART2,"XXXX");
                        }
                        isread = 0;
                            
                        if(fresult!=FR_OK)
                        {
                                usart_sendString(USART2,"���ų���1");
                                stopDMA();
                                f_close(&newfile);
                        }                             
                     }
                     if(f_eof(&newfile))  // ��һ���µ��ļ�
                     {
                        i++;
                        stopDMA();
                        f_close(&newfile);
                        if(i==0)
                          fresult = f_open(&newfile, "0:music3.wav",FA_READ);
                        else if(i==1)
                          fresult = f_open(&newfile, "0:music4.wav",FA_READ);
                        else if(i==2)
                        {
                            fresult = f_open(&newfile, "0:music5.wav",FA_READ);
                            i=0;
                        }
                        fresult = f_read(&newfile, &wavhead, sizeof(wavhead), &fnum);
                
                        //�ȶ�ȡһ����Ƶ��Ϣ��������
                        fresult = f_read(&newfile, (uint16_t *)buffer0, 512,&fnum);
                        fresult = f_read(&newfile, (uint16_t *)buffer1, 512,&fnum);
                        
                        I2S3_TX_DMAInit(buffer0, buffer1, 256);  //��ʼ����
                        startDMA(); 
                     }
		}
		
                //f_close(&newfile);
                //usart_sendString(USART2,"�ļ���ȡ���");
	}
            
        else
            usart_sendString(USART2,"���ļ�ʧ��");
}




void DMA1_Stream5_IRQHandler()
{
    if(DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5)==SET)
   {
		
                isread = 1;  // dma ��ȡ�������
		//I2S_DMA_Tx_Callback();
		//�ж�dma��ǰ��ʹ�õĴ洢��
		if(DMA1_Stream5->CR&(1<<19))   //��ǰDMAʹ�ô洢��1��ֻ�ܸ��Ĵ洢��0������
		{
			bufferflag = 0;
		}
		else
		{
			bufferflag = 1;
		}
		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
	}
}
