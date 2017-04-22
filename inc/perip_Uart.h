#include"stm32f4xx.h"
void usart_sendByte(USART_TypeDef * pUartx,char data);
void usart_sendString(USART_TypeDef * pUartx,char * pString);