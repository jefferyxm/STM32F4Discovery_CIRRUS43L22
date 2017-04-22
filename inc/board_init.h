#ifndef __BOARD_INIT_H
#define __BOARD_INIT_H

void bsp_led_init();
void perip_uart_init();

void perip_I2C1_init();
void perip_I2C2_init();

void TIM_init();
void EXTI_config();
void MCO_init();
void SPI1_Init(uint8_t speed);

void I2S3_Init();
void I2S3_TX_DMAInit(const uint16_t *buffer0, const uint16_t *buffer1, const uint32_t num);


#define DISCOVERY 1
#ifdef DISCOVERY
#define led3on() GPIOD->BSRRL = GPIO_Pin_13
#define led4on() GPIOD->BSRRL = GPIO_Pin_12
#define led5on() GPIOD->BSRRL = GPIO_Pin_14
#define led6on() GPIOD->BSRRL = GPIO_Pin_15

#define led3off() GPIOD->BSRRH = GPIO_Pin_13
#define led4off() GPIOD->BSRRH = GPIO_Pin_12
#define led5off() GPIOD->BSRRH = GPIO_Pin_14
#define led6off() GPIOD->BSRRH = GPIO_Pin_15

#define led3toggle() GPIOD->ODR^=GPIO_Pin_13
#define led4toggle() GPIOD->ODR^=GPIO_Pin_12
#define led5toggle() GPIOD->ODR^=GPIO_Pin_14
#define led6toggle() GPIOD->ODR^=GPIO_Pin_15

#else
#define led2off() GPIOF->BSRRL=GPIO_Pin_9
#define led3off() GPIOF->BSRRL=GPIO_Pin_10

#define led2on() GPIOF->BSRRH=GPIO_Pin_9
#define led3on() GPIOF->BSRRH=GPIO_Pin_10

#define led2toggle() GPIOF->ODR^=GPIO_Pin_9
#define led3toggle() GPIOF->ODR^=GPIO_Pin_10
#endif


#endif