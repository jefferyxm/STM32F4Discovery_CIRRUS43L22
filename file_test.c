#define MMC_Root_Path "1:"

#include "file_test.h"

static FRESULT fresult;

static FIL newfile;
static FIL newfile2;

static FATFS fatfs;


static UINT fnum;
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
    fresult = f_mount(&fatfs,"0:",1);
    if(fresult)
    {
        usart_sendString(USART2,"�����ļ�ϵͳʧ��");
    }
    fresult = f_open(&newfile, "0:music4.wav",FA_READ);

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













