#ifndef __PERIP_FUNC_H
#define __PERIP_FUNC_H


void usart_sendByte(USART_TypeDef *USARTx, uint8_t data);

void usart_sendString(USART_TypeDef *USARTx,char*str);

void usart_sendMessage(USART_TypeDef *USARTx, uint8_t *p_data, uint8_t by_num);

void usart_debugMessage(char*str);

void i2c_writeByte(I2C_TypeDef * I2Cx, uint8_t slaveAddr, uint8_t regAddr, uint8_t data);

uint8_t i2c_readByte(I2C_TypeDef * I2Cx, uint8_t slaveAddr, uint8_t regAddr);

uint8_t spi1_sendByte(uint8_t data);

uint8_t spi1_readByte(void);

u32 RNG_Get_RandomNum(void);

int RNG_Get_RandomRange(int min,int max);
#endif