
#include"stm32f4xx.h"
static uint32_t Exception_callback(char *str);

/***********************   usart   ******************************/

void usart_sendByte(USART_TypeDef *USARTx, uint8_t data)
{
    	USART_SendData(USARTx, data);
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);
}

void usart_sendString(USART_TypeDef *USARTx,char*str)
{
	uint8_t k = 0;
	do
	{
		usart_sendByte(USARTx,*(str+k));
		k++;
	}while(*(str+k)!='\0');
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC));	
}

void usart_sendMessage(USART_TypeDef *USARTx, uint8_t *p_data, uint8_t by_num)
{
   for(uint8_t i = 0; i<by_num; i++)
   {
      usart_sendByte(USARTx,*(p_data+i));
   }
   while(USART_GetFlagStatus(USARTx,USART_FLAG_TC));
}




/***********************   I2CX   ******************************/
#define I2C_COM_TIMEOUT ((uint32_t)0x1000)
#define I2C_LONG_TIMEOUT ((uint32_t)(I2C_FLAG_TIMEOUT*10))

void i2c_writeByte(I2C_TypeDef * I2Cx, uint8_t slaveAddr, uint8_t regAddr, uint8_t data)
{
    //i2c 起始信号
    I2C_GenerateSTART(I2Cx,ENABLE);
    uint32_t i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((i2c_timeout--)==0) Exception_callback("i2c start");
    }
    
    //设置地址
    I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Transmitter);
    i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((i2c_timeout--)==0) Exception_callback("i2c selectSlaveDevice");
    }

    //设置寄存器
    I2C_SendData(I2Cx,regAddr);
    i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((i2c_timeout--)==0) Exception_callback("i2c configRegister");
    }
    
    //发送数据
    I2C_SendData(I2Cx,data);
    i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((i2c_timeout--)==0) Exception_callback("i2c sendingData");
    }
    
    //i2c 停止信号
    I2C_GenerateSTOP(I2Cx,ENABLE);
}

uint8_t i2c_readByte(I2C_TypeDef * I2Cx, uint8_t slaveAddr, uint8_t regAddr)
{
    //i2c 起始信号
    I2C_GenerateSTART(I2Cx,ENABLE);
    uint32_t i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((i2c_timeout--)==0) Exception_callback("4");
    }

    //设置地址
    I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Transmitter);
    i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((i2c_timeout--)==0) Exception_callback("5");
    }
    I2C_Cmd(I2Cx, ENABLE);
	
    //设置寄存器
    I2C_SendData(I2Cx,regAddr);
    i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if((i2c_timeout--)==0) Exception_callback("6");
    }
    
//换方向，需要重新发送起始信号
    I2C_GenerateSTART(I2Cx,ENABLE);
    i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))
    {
        if((i2c_timeout--)==0) Exception_callback("7");
    }

    I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Receiver);
    i2c_timeout = I2C_COM_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if((i2c_timeout--)==0) Exception_callback("8");
    }

    i2c_timeout = I2C_LONG_TIMEOUT;
    while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
	if((i2c_timeout--)==0)Exception_callback("9");
    }
    uint8_t u8_result = I2C_ReceiveData(I2Cx);
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    I2C_GenerateSTOP(I2Cx,ENABLE);
    return u8_result;
}

/***********************   SPI1 operation   ******************************/
#define dummy_byte  0xff
#define SPI_COM_TIMEOUT ((uint32_t)0x1000)
uint8_t spi1_sendByte(uint8_t data)
{

        uint32_t spi_timeout  = SPI_COM_TIMEOUT;
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET)
        {
                if((spi_timeout--)==0) return Exception_callback("TXE");
        }

        SPI_I2S_SendData(SPI1, data);

        spi_timeout  = SPI_COM_TIMEOUT;

        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        {
            if(spi_timeout-- == 0) return Exception_callback("RXEN");
        }

        return SPI_I2S_ReceiveData(SPI1);
}

uint8_t spi1_readByte(void)
{
	return (spi1_sendByte(dummy_byte));
}

/***********************   common   ******************************/
static uint32_t Exception_callback(char *str)
{
  usart_sendString(USART2, "Exception generate at");
  usart_sendString(USART2, str);
  return 0;
}


void usart_debugMessage(char*str)
{
      uint8_t k = 0;
      do
      {
              usart_sendByte(USART2,*(str+k));
              k++;
      }while(*(str+k)!='\0');
      while(USART_GetFlagStatus(USART2,USART_FLAG_TC));	

}
