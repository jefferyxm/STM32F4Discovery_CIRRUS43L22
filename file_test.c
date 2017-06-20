#define MMC_Root_Path "1:"

#include "file_test.h"
#include <string.h>

static FRESULT fresult;

static FIL newfile;
static FIL newfile2;

static FATFS fatfs;


static UINT fnum;
BYTE buffer[1024];
BYTE file_buff[] = "hello everyone, this is the test of the fatfs, thanks zzzzzzzz;中文测试";
BYTE read_buff[1024];
BYTE workbuff[_MAX_SS];

uint32_t speedBytes = 0;
uint32_t count;
BYTE speedtestBuff[1024];

// 格式化文件系统
void format_fatfs(char *rootPath)
{
    fresult = f_mount(&fatfs, rootPath, 1);
    
    if(fresult==FR_OK)
            usart_sendString(USART2,"挂载成功");
        else
            usart_sendString(USART2,"挂载失败");
    
    
    fresult = f_mkfs(rootPath,FM_FAT32, 0, workbuff,sizeof(workbuff));
        if(fresult==FR_OK)
            usart_sendString(USART2,"格式化sd卡成功");
        else
            usart_sendString(USART2,"格式化sd卡失败");
}

bool writFile(char *rootPath, char *fileName, char* fileBuff, uint32_t buffSize)
{
      fresult = f_mount(&fatfs, rootPath, 1);  //挂载文件系统
      
      char filePath[50] = {0};
      strcat(filePath,rootPath);
      strcat(filePath,fileName);

      //打开文件
      fresult = f_open(&newfile, filePath, FA_CREATE_ALWAYS|FA_WRITE);
      if(fresult==FR_OK)
      {
          //写文件内容
          f_write (
                    &newfile,			/* Pointer to the file object */
                    fileBuff,	                /* Pointer to the data to be written */
                    buffSize,			/* Number of bytes to write */
                    &fnum			/* Pointer to number of bytes written */
              );
          usart_sendString(USART2,"> 创建文件成功 \r\n");
          f_close(&newfile);
          return true;
      }
      else
      {
          usart_sendString(USART2,"创建文件失败");
          return false;
      }
}


void readFile(char *rootPath, char *fileName, char* fileBuff, uint32_t buffSize)
{
    fresult = f_mount(&fatfs,rootPath,1);
    
    char filePath[50] = {0};
    strcat(filePath,rootPath);
    strcat(filePath,fileName);
    
    fresult = f_open(&newfile, filePath,FA_READ);
    if(fresult==FR_OK)
    {
            usart_sendString(USART2,"打开文件成功");
            fresult = f_read(&newfile, fileBuff, buffSize, &fnum);
            if(fresult==FR_OK)
            {
                usart_sendMessage(USART2, fileBuff, fnum);
                f_close(&newfile);
                usart_sendString(USART2,"文件读取完成");
            }
            
    }      
    else
    {
        usart_sendString(USART2,"打开文件失败");
    }    
	
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


void read_speedtest(char *rootPath, char *fileName)
{
    //初始化一个定时器，1s定时
    //开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseInitTypeDef Timer2_InitStruct;
    Timer2_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    Timer2_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
#if defined(F407Discovery)
    Timer2_InitStruct.TIM_Period = 168000-1;
#else
    Timer2_InitStruct.TIM_Period = 84000-1;
#endif
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
    
    
    char filePath[50] = {0};
    strcat(filePath,rootPath);
    strcat(filePath,fileName);
    
    //打开一个较大的文件文件
    fresult = f_mount(&fatfs,rootPath,1);
    if(fresult)
    {
        usart_sendString(USART2,"挂载文件系统失败");
    }
    fresult = f_open(&newfile, filePath,FA_READ);

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
        usart_sendString(USART2, " kByte/s \r\n");
        speedBytes = 0;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}













