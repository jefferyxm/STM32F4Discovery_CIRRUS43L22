#define MMC_Root_Path "1:"

#include "file_test.h"

FATFS fatfs;
FIL newfile;
FIL newfile2;

FRESULT fresult;
UINT fnum;
BYTE buffer[1024];
BYTE file_buff[] = "hello everyone, this is the test of the fatfs, thanks zzzzzzzz;���Ĳ���";
BYTE read_buff[1024];
BYTE workbuff[_MAX_SS];

uint32_t speedBytes = 0;
uint32_t count;
BYTE speedtestBuff[1024];

// ��ʽ���ļ�ϵͳ
void mount_fatfs(void)
{
    fresult = f_mount(&fatfs, "1:", 1);
    
    fresult = f_mkfs("1:",FM_FAT32, 0, workbuff,sizeof(workbuff));
        if(fresult==FR_OK)
            usart_sendString(USART2,"��ʽ��sd���ɹ�");
        else
            usart_sendString(USART2,"��ʽ��sd��ʧ��");
}

void writFile()
{
        fresult = f_mount(&fatfs, "1:", 1);  //�����ļ�ϵͳ

        //���ļ�
        fresult = f_open(&newfile, "1:���Ĳ���.txt", FA_CREATE_ALWAYS|FA_WRITE);
	if(fresult==FR_OK)
            usart_sendString(USART2,"�����ļ��ɹ�");
        else
            usart_sendString(USART2,"�����ļ�ʧ��");
        
        //д�ļ�����
        
        f_write (
	&newfile,			/* Pointer to the file object */
	file_buff,	/* Pointer to the data to be written */
	sizeof(file_buff),			/* Number of bytes to write */
	&fnum			/* Pointer to number of bytes written */
);
        
	f_close(&newfile);
        
}


void readFile()
{
    fresult = f_mount(&fatfs,"1:",1);
    fresult = f_open(&newfile, "1:���Ĳ���.txt",FA_READ);
	if(fresult==FR_OK)
	{
		usart_sendString(USART2,"���ļ��ɹ�");
		fresult = f_read(&newfile, read_buff, sizeof(file_buff), &fnum);
		usart_sendMessage(USART2, read_buff, fnum);
                f_close(&newfile);
                usart_sendString(USART2,"�ļ���ȡ���");
	}
            
        else
            usart_sendString(USART2,"���ļ�ʧ��");
	
}


/*��д�ٶȼ��ײ���*/
UINT outStream(const BYTE *p, UINT c)
{
    UINT cnt = 0;
    BYTE * pspeedtestBuff;
    
    if(c==0)
    {
        cnt = 1;
    }
    else
    {
      do
      {
          pspeedtestBuff = speedtestBuff;
          *pspeedtestBuff++ = *p++;  //�����ݷŵ�������������ֱ�������������д��洢��
          speedBytes++;
          cnt++;
      }
      while(cnt < c);
    }
    return cnt;
}


void read_speedtest()
{
    //��ʼ��һ����ʱ����1s��ʱ
    //����ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseInitTypeDef Timer2_InitStruct;
    Timer2_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    Timer2_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    Timer2_InitStruct.TIM_Period = 84000-1;
    Timer2_InitStruct.TIM_Prescaler = 1000-1;
    TIM_TimeBaseInit(TIM2, &Timer2_InitStruct);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    TIM_Cmd(TIM2, ENABLE);
    
    //��һ���ϴ���ļ��ļ�
    fresult = f_mount(&fatfs,"1:",1);
    if(fresult)
    {
        usart_sendString(USART2,"�����ļ�ϵͳʧ��");
    }
    fresult = f_open(&newfile, "1:music.wav",FA_READ);

    if(fresult)
    {
        usart_sendString(USART2,"���ļ�ʧ��");
    }
    
    while(1)
    {
        if(fresult==FR_OK && !f_eof(&newfile))   //�ļ��򿪳ɹ�������û�е����ļ�ĩβ
        {
            fresult = f_forward(&newfile, outStream, 1024, &count);
            //usart_sendString(USART2,"1111111");
        }
        else
          break;
    }
    f_close(&newfile);
}

//��ʱ��
char sss[10];
void TIM2_IRQHandler()
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
    	led3toggle();
        sprintf(sss,"%d",speedBytes/1024);
        usart_sendString(USART2,"speed: ");
        usart_sendString(USART2, sss);
        usart_sendString(USART2, " kByte/s");
        speedBytes = 0;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}


WavHead wavhead;
void (*I2S_DMA_Tx_Callback)(void);
uint16_t buffer0[257],buffer1[257];//��Ҫ���ң�����ǿ��֢������
uint8_t bufferflag = 0;
uint8_t isread = 0;

void startPlay()
{
    DMA_Cmd(DMA1_Stream5, ENABLE);
}

void stopPlay()
{
    DMA_Cmd(DMA1_Stream5, DISABLE);
}


uint8_t i_main_cnt = 0;

void play()
{ 
    
    fresult = f_mount(&fatfs,"1:",1);
    fresult = f_open(&newfile, "1:music2.wav",FA_READ);
	if(fresult==FR_OK)
	{
		usart_sendString(USART2,"��music2�ļ��ɹ�");
                //��ȡwave��Ƶͷ��Ϣ
		fresult = f_read(&newfile, &wavhead, sizeof(wavhead), &fnum);
                
                //�ȶ�ȡһ����Ƶ��Ϣ��������
                fresult = f_read(&newfile, (uint16_t *)buffer0, 512,&fnum);
                fresult = f_read(&newfile, (uint16_t *)buffer1, 512,&fnum);
                
                I2S3_TX_DMAInit(buffer0, buffer1, 256);
                
                startPlay();  // ����dma���䣬��ʼ������  ��ʹ�ô洢��0����
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
                                usart_sendString(USART2,"���ų���");
                                stopPlay();
                                f_close(&newfile);
                        }                             
                     }
                     if(f_eof(&newfile))  // ��һ���µ��ļ�
                     {
                        i++;
                        stopPlay();
                        f_close(&newfile);
                        if(i==0)
                          fresult = f_open(&newfile, "1:music2.wav",FA_READ);
                        else if(i==1)
                          fresult = f_open(&newfile, "1:music3.wav",FA_READ);
                        else if(i==2)
                        {
                            fresult = f_open(&newfile, "1:music4.wav",FA_READ);
                            i=0;
                        }
                        fresult = f_read(&newfile, &wavhead, sizeof(wavhead), &fnum);
                
                        //�ȶ�ȡһ����Ƶ��Ϣ��������
                        fresult = f_read(&newfile, (uint16_t *)buffer0, 512,&fnum);
                        fresult = f_read(&newfile, (uint16_t *)buffer1, 512,&fnum);
                        
                        I2S3_TX_DMAInit(buffer0, buffer1, 256);  //��ʼ����
                        startPlay(); 
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











