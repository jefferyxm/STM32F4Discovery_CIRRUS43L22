/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "perip_func.h"

#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_msc_core.h"

static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

uint8_t mount_fatfs(void);
void writFile();
void readFile();
void read_speedtest();

u32 blocksize ,sector,rdBlockLen,deviceSize,deviceSizeMul;
uint64_t capacity;
extern MSD_CARDINFO SD0_CardInfo;

void play();
void Init_CIR43L22();

int main(void)
{
    SystemInit();
    bsp_led_init();
    perip_uart_init();
    //START_SYSTICK();
    //perip_I2C2_init();
    
    perip_I2C1_init();

    //TIM_init();
    //MCO_init();
    
    SPI1_Init(0);
    MSD0_Init();
    SPI1_Init(1);
    
    capacity = 7948206080;
    
    MSD0_GetCardInfo(&SD0_CardInfo);
    
    blocksize = SD0_CardInfo.BlockSize;
    capacity = SD0_CardInfo.Capacity;
    sector = SD0_CardInfo.Capacity/SD0_CardInfo.BlockSize;
    rdBlockLen = SD0_CardInfo.CSD.RdBlockLen;            //9
    deviceSize = SD0_CardInfo.CSD.DeviceSize;           //15159
    deviceSizeMul = SD0_CardInfo.CSD.DeviceSizeMul;     //6
    
    
    Init_CIR43L22();
    I2S3_Init();
    
    
    USBH_Init(&USB_OTG_Core, 
#ifdef USE_USB_OTG_FS  
            USB_OTG_FS_CORE_ID,
#else 
            USB_OTG_HS_CORE_ID,
#endif 
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_cb);
    
    //play();
	
    //mount_fatfs();
    
    //writFile();
    //readFile();
    //read_speedtest();
      
    //RCC_GetClocksFreq(&RCC_Clocks);
    //SysTick_Config(RCC_Clocks.HCLK_Frequency/100);
    

    //GPIO_Init(GPIOB,&GPIO_Init_Struct);
    
    //ano_sendDebug(0,6);
    //usart_sendString(USART2,"helloworld");

    //EXTI_config();
    while(1)
    {
       USBH_Process(&USB_OTG_Core, &USB_Host);
    };
}

void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
