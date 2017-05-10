#include "main.h"
#define DISCOVERY 1
void bsp_led_init()
{
  /*    FOR discovery
        led3 ----pd13
  	led4 ----pd12
  	led5 ----pd14
  	led6 ----pd15

        For ZG
        PF9  led2;
        PF10 led3;
  */  
#if DISCOVERY
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
  GPIO_InitTypeDef GPIO_Init_Struct;
  GPIO_Init_Struct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init_Struct.GPIO_OType= GPIO_OType_PP;
  GPIO_Init_Struct.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_Init_Struct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init_Struct.GPIO_Speed = GPIO_High_Speed;

  GPIO_Init(GPIOD,&GPIO_Init_Struct);
#else
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
  GPIO_InitTypeDef GPIO_Init_Struct;
  GPIO_Init_Struct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init_Struct.GPIO_OType= GPIO_OType_PP;
  GPIO_Init_Struct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
  GPIO_Init_Struct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init_Struct.GPIO_Speed = GPIO_Fast_Speed;

  GPIO_Init(GPIOF,&GPIO_Init_Struct);
  led3off();
  led2off();
#endif  
}

void perip_uart_init()
{
  /*
  USART2:
    pin configuration
    PA2----  TX
    PA3----  RX
                            */  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    //A2--uart2_TX .  A3  uart2_Rx
    GPIO_InitTypeDef GPIO_Init_Struct;
    GPIO_Init_Struct.GPIO_OType =GPIO_OType_PP;
    GPIO_Init_Struct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init_Struct.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init_Struct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init_Struct.GPIO_Pin =GPIO_Pin_2|GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_Init_Struct);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);


    // uart configuration
    USART_InitTypeDef UsartInit_Struct;
    UsartInit_Struct.USART_BaudRate = 115200;
    UsartInit_Struct.USART_HardwareFlowControl =  USART_HardwareFlowControl_None;
    UsartInit_Struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    UsartInit_Struct.USART_Parity = USART_Parity_No;
    UsartInit_Struct.USART_StopBits = USART_StopBits_1;
    UsartInit_Struct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2,&UsartInit_Struct);

    NVIC_InitTypeDef NVICInit_Struct;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVICInit_Struct.NVIC_IRQChannel = USART2_IRQn;
    NVICInit_Struct.NVIC_IRQChannelPreemptionPriority = 1;
    NVICInit_Struct.NVIC_IRQChannelSubPriority = 1;
    NVICInit_Struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVICInit_Struct);


    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

    USART_Cmd(USART2,ENABLE);
    
    
    /*maybe some other uart Init here*/
}

/***********************   IIC2   ******************************/
void perip_I2C2_init()
{
  
   /*
  IIC2:
    pin configuration
    PB10----   SCK   
    PB11----   SDA
                     */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

    GPIO_InitTypeDef GPIOB_Init_Struct;
    GPIOB_Init_Struct.GPIO_Mode = GPIO_Mode_AF;
    GPIOB_Init_Struct.GPIO_OType = GPIO_OType_OD;
    GPIOB_Init_Struct.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIOB_Init_Struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOB_Init_Struct.GPIO_Speed = GPIO_Fast_Speed;

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_I2C2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_I2C2);
    GPIO_Init(GPIOB,&GPIOB_Init_Struct);

    I2C_InitTypeDef i2c2_InitStruct;
    i2c2_InitStruct.I2C_Mode = I2C_Mode_I2C;
    i2c2_InitStruct.I2C_ClockSpeed = 300000;
    i2c2_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c2_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c2_InitStruct.I2C_Ack = I2C_Ack_Enable;
    i2c2_InitStruct.I2C_OwnAddress1 = 0;
    I2C_Init(I2C2,&i2c2_InitStruct);
    I2C_Cmd(I2C2, ENABLE);
    
    /*maybe some other IIC Init here*/
}

void perip_I2C1_init()     //for stm32f401 discovery board. cirrus 43l22 IIC interface 
{
        /*
  IIC1:
    pin configuration
    PB6----   SCK   
    PB9----   SDA
                     */
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

    GPIO_InitTypeDef GPIOB_Init_Struct;
    GPIOB_Init_Struct.GPIO_Mode = GPIO_Mode_AF;
    GPIOB_Init_Struct.GPIO_OType = GPIO_OType_OD;
    GPIOB_Init_Struct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_6;
    GPIOB_Init_Struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIOB_Init_Struct.GPIO_Speed = GPIO_Fast_Speed;

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
    GPIO_Init(GPIOB,&GPIOB_Init_Struct);

    I2C_InitTypeDef i2c1_InitStruct;
    i2c1_InitStruct.I2C_Mode = I2C_Mode_I2C;
    i2c1_InitStruct.I2C_ClockSpeed = 300000;
    i2c1_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c1_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c1_InitStruct.I2C_Ack = I2C_Ack_Enable;
    i2c1_InitStruct.I2C_OwnAddress1 = 0;
    I2C_Init(I2C1,&i2c1_InitStruct);
    I2C_Cmd(I2C1, ENABLE);
}

/***********************   timer   ******************************/
void TIM_init()
{
   
      /*maybe some other timer Init here*/
}

/***********************   EXIT   ******************************/
void EXTI_config()
{
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
      GPIO_InitTypeDef GPIO_InitStruct;
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
      GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
      GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;
      GPIO_Init(GPIOA,&GPIO_InitStruct);

      RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
      SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource0);

      NVIC_InitTypeDef NVIC_InitStruct;
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
      NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
      NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
      NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
      NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStruct);

      EXTI_InitTypeDef EXTI_InitStruct;
      EXTI_InitStruct.EXTI_Line = EXTI_Line0;
      EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
      EXTI_InitStruct.EXTI_LineCmd = ENABLE;
      EXTI_Init(&EXTI_InitStruct);	
}


/***********************   MCO   ******************************/
void MCO_init()
{
  
      /*
  MCO:
      pin configuration
      PA8----  MCO1
      PC9----  MCO2
                      */
      GPIO_InitTypeDef GPIO_InitStructure;
      
      /* Output HSE clock on MCO1 pin(PA8) ****************************************/ 
      /* Enable the GPIOA peripheral */ 
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

      /* Configure MCO1 pin(PA8) in alternate function */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
      GPIO_Init(GPIOA, &GPIO_InitStructure);

      /* HSE clock selected to output on MCO1 pin(PA8)*/
      RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);

      /* Output SYSCLK/4 clock on MCO2 pin(PC9) ***********************************/ 
      /* Enable the GPIOACperipheral */ 
      RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

      /* Configure MCO2 pin(PC9) in alternate function */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
      GPIO_Init(GPIOC, &GPIO_InitStructure);

      /* SYSCLK/4 clock selected to output on MCO2 pin(PC9)*/
      RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO2Div_4);
}

/***********************   SPI1   ******************************/
void SPI1_Init(uint8_t speed)
{
    /*
  SPI1:
     pin configuration
     PA4----  SPI1 CS (soft)
     PA5----  SPI1 SCK
     PA6----  SPI1 MISO
     PA7----  SPI1 MOSI
                                                    */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

    //spi cs
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    
    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    
    if(speed==0)
    {
        SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    }
    else if(speed == 1)
    {
        SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    }
    else if(speed == 2)
    {
        SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    }
    
    SPI_Init(SPI1, &SPI_InitStruct);

    SPI_Cmd(SPI1, ENABLE);	
    
    /*maybe some other spi Init here*/
}


/***********************   I2S3 Init   ******************************/
void I2S3_Init()
{
/*I2S3 Init
	discovery board
	pin configuration
	PC7  ----- MCK
	PC10 ----- SCLK
	PC12 ----- SDIN
	PA4  ----- LRCK
*/

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_10|GPIO_Pin_12;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3);

    //÷ÿ–¬≈‰÷√iispll ±÷”

    RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
    RCC_PLLI2SConfig(271, 2);
    RCC_PLLI2SCmd(ENABLE);

    for(int n = 0; n<500; n++)
    {
            if(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY)==1)
                    break;
    }

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    SPI_I2S_DeInit(SPI3);

    I2S_InitTypeDef I2S_InitStruct;
    I2S_InitStruct.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStruct.I2S_Standard = I2S_Standard_LSB;
    I2S_InitStruct.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStruct.I2S_AudioFreq = I2S_AudioFreq_44k;
    I2S_InitStruct.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
    I2S_InitStruct.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI3, &I2S_InitStruct);

    I2S_Cmd(SPI3, ENABLE);
}

void I2S3_TX_DMAInit(const uint16_t *buffer0, const uint16_t *buffer1, const uint32_t num)
{
    /* I2S3 DMA  À´ª∫≥Â*/
    NVIC_InitTypeDef NVIC_InitStruct;
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Stream5);
    while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);

    DMA_ClearITPendingBit(DMA1_Stream5, 
                            DMA_IT_FEIF5|DMA_IT_DMEIF5|DMA_IT_HTIF5|DMA_IT_TCIF5|DMA_IT_TEIF5);

    DMA_InitStruct.DMA_Channel = DMA_Channel_0;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&SPI3->DR;//(uint32_t)&SPI3->DR;

    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)buffer0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;

    DMA_InitStruct.DMA_BufferSize = num;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    //DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStruct);

    //DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t)buffer0, DMA_Memory_0);
    DMA_DoubleBufferModeConfig(DMA1_Stream5, (uint32_t)buffer1, DMA_Memory_1);

    DMA_DoubleBufferModeCmd(DMA1_Stream5, ENABLE);

    DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
    SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


