#define MMC_Root_Path "1:"

#include "file_test.h"

FATFS fatfs;
FIL newfile;
FIL newfile2;

FRESULT fresult;
UINT fnum;
BYTE buffer[1024];
BYTE file_buff[] = "hello everyone, this is the test of the fatfs, thanks zzzzzzzz;中文测试";
BYTE read_buff[1024];
BYTE workbuff[_MAX_SS];

uint32_t speedBytes = 0;
uint32_t count;
BYTE speedtestBuff[1024];

// 格式化文件系统
void mount_fatfs(void)
{
    fresult = f_mount(&fatfs, "1:", 1);
    
    fresult = f_mkfs("1:",FM_FAT32, 0, workbuff,sizeof(workbuff));
        if(fresult==FR_OK)
            usart_sendString(USART2,"格式化sd卡成功");
        else
            usart_sendString(USART2,"格式化sd卡失败");
}

void writFile()
{
        fresult = f_mount(&fatfs, "1:", 1);  //挂载文件系统

        //打开文件
        fresult = f_open(&newfile, "1:中文测试.txt", FA_CREATE_ALWAYS|FA_WRITE);
	if(fresult==FR_OK)
            usart_sendString(USART2,"创建文件成功");
        else
            usart_sendString(USART2,"创建文件失败");
        
        //写文件内容
        
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
    fresult = f_open(&newfile, "1:中文测试.txt",FA_READ);
	if(fresult==FR_OK)
	{
		usart_sendString(USART2,"打开文件成功");
		fresult = f_read(&newfile, read_buff, sizeof(file_buff), &fnum);
		usart_sendMessage(USART2, read_buff, fnum);
                f_close(&newfile);
                usart_sendString(USART2,"文件读取完成");
	}
            
        else
            usart_sendString(USART2,"打开文件失败");
	
}


/*读写速度简易测试*/
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
          *pspeedtestBuff++ = *p++;  //将数据放到缓冲区，今后可直接输出，不必再写入存储中
          speedBytes++;
          cnt++;
      }
      while(cnt < c);
    }
    return cnt;
}


void read_speedtest()
{
    //初始化一个定时器，1s定时
    //开启时钟
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
    
    //打开一个较大的文件文件
    fresult = f_mount(&fatfs,"1:",1);
    if(fresult)
    {
        usart_sendString(USART2,"挂载文件系统失败");
    }
    fresult = f_open(&newfile, "1:music.wav",FA_READ);

    if(fresult)
    {
        usart_sendString(USART2,"打开文件失败");
    }
    
    while(1)
    {
        if(fresult==FR_OK && !f_eof(&newfile))   //文件打开成功，并且没有到达文件末尾
        {
            fresult = f_forward(&newfile, outStream, 1024, &count);
            //usart_sendString(USART2,"1111111");
        }
        else
          break;
    }
    f_close(&newfile);
}

//定时器
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
uint16_t buffer0[257],buffer1[257];//不要管我，我是强迫症。。。
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
		usart_sendString(USART2,"打开music2文件成功");
                //读取wave音频头信息
		fresult = f_read(&newfile, &wavhead, sizeof(wavhead), &fnum);
                
                //先读取一段音频信息到缓冲区
                fresult = f_read(&newfile, (uint16_t *)buffer0, 512,&fnum);
                fresult = f_read(&newfile, (uint16_t *)buffer1, 512,&fnum);
                
                I2S3_TX_DMAInit(buffer0, buffer1, 256);
                
                startPlay();  // 开启dma传输，开始播放了  先使用存储器0播放
                int i  = 0;
                while(1)
                {
                    if(isread==1)
                    {
                        if(bufferflag==0)  // buffer0 空闲  //   
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
                                usart_sendString(USART2,"播放出错");
                                stopPlay();
                                f_close(&newfile);
                        }                             
                     }
                     if(f_eof(&newfile))  // 打开一个新的文件
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
                
                        //先读取一段音频信息到缓冲区
                        fresult = f_read(&newfile, (uint16_t *)buffer0, 512,&fnum);
                        fresult = f_read(&newfile, (uint16_t *)buffer1, 512,&fnum);
                        
                        I2S3_TX_DMAInit(buffer0, buffer1, 256);  //开始播放
                        startPlay(); 
                     }
		}
		
                //f_close(&newfile);
                //usart_sendString(USART2,"文件读取完成");
	}
            
        else
            usart_sendString(USART2,"打开文件失败");
}




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
		}
		else
		{
			bufferflag = 1;
		}
		DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
	}
}











