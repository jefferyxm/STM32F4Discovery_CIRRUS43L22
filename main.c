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
#include "file_test.h"
#include "player.h"
#include "CIR43L22.h"

static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

uint64_t capacity;
extern MSD_CARDINFO SD0_CardInfo;

char USBH_Path[4];  
char MSD_Path[4];
extern Diskio_drvTypeDef  USBH_Driver;
extern Diskio_drvTypeDef  MSD_Driver;

RCC_ClocksTypeDef rcc_clock;
int main(void)
{
    SystemInit();
    bsp_led_init();
    perip_uart_init();
    led3on();
    led4on();
    perip_I2C1_init();

//    TIM_init();
//    MCO_init();
//    
//    SPI1_Init(0);
//    MSD0_Init();
//    SPI1_Init(1);
//    
//    capacity = 7948206080;
//
//    MSD0_GetCardInfo(&SD0_CardInfo);

    Init_CIR43L22();
    I2S3_Init();
    
    
    RCC_GetClocksFreq(&rcc_clock);
        
    USBH_Init(&USB_OTG_Core, 
#ifdef USE_USB_OTG_FS  
            USB_OTG_FS_CORE_ID,
#else 
            USB_OTG_HS_CORE_ID,
#endif 
            &USB_Host,
            &USBH_MSC_cb, 
            &USR_cb);

    FATFS_LinkDriver(&USBH_Driver, USBH_Path);
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
