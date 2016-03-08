/**
  ******************************************************************************
  * @file    main.c 
  * @author  Robot Club BK HCM
  * @version 1.0
  * @date
  * @brief	Main program body
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "defines.h"
#include "gsm_handler.h"
#include "com.h"
#include "comms.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t sendPri = 0;

uint8_t main_started = 0;

Timeout_Type tGetInfo;

uint16_t temperature;
uint16_t humid;
uint16_t light;
uint16_t pressure;

void init(void);

/* Private function prototypes -----------------------------------------------*/
void init(void);

/* Private functions ---------------------------------------------------------*/
void init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Init();
    
    // Config UART1 for debug
    UART_Config(USART1, 9600);
    
    // UART5 for Get Sensor Data
    myMB_Init(9600);
    
    // UART3 for MainControl Get data
    myMB_Init1(9600);
}



/*******************************************************************************
Function name: Main program
Decription: None
Input: None
Output: None
*******************************************************************************/
int main(void)
{
    // Init
    init();
    
    eMBErrorCode eStatus;
    
    eStatus = eMBInit(  //MB_ASCII,
                        MB_RTU,
                        0x0A,
                        0,
                        9600,
                        MB_PAR_NONE );
    
    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable();
    
    // INFO
    InitTimeout(&tGetInfo, 0);
    INFO("\n\nSystem Init Done!\n");
    main_started = 1; // Start
    
    //while (1);
    
    while (1)
    {
        INFO("Send request\n");
        float temp, humid, light, pressure, soilT, soilH;
        
        // Get SOIL Sensor
        if (sendRequest(109, 0, 2, 0) == 0) // get soil sensor
        {
            INFO("SOIL Error\n");
        }
        else
        {
            usRegHoldingBuf[5] = myRegBuf[0];
            usRegHoldingBuf[6] = myRegBuf[1];
        }
        
        delay_ms(100);
        
        // Get TEMP, HUMID, LIGHT, PRESSURE
        if (sendRequest(100, 0, 5, 0) == 0) // get TEMP sensor
        {
            INFO("TEMPHUD Error\n");
        }
        else
        {
            usRegHoldingBuf[0] = myRegBuf[0];
            usRegHoldingBuf[1] = myRegBuf[1];
            usRegHoldingBuf[2] = myRegBuf[2];
            usRegHoldingBuf[4] = myRegBuf[3];
            usRegHoldingBuf[3] = myRegBuf[4];
        }
        
        // Update for MainControl Get data
        for (int i = 0; i < myRegHolding1; i++)
        {
            myRegBuf1[i] = usRegHoldingBuf[i];
        }
        
        // Calc display
        temp = (float)usRegHoldingBuf[0] / 10;
        humid = (float)usRegHoldingBuf[1] / 10;
        light = (float)usRegHoldingBuf[2];
        pressure = (float)((int32_t)usRegHoldingBuf[4] << 16 | usRegHoldingBuf[3]);
        soilT = (float)usRegHoldingBuf[5] / 10;
        soilH = (float)usRegHoldingBuf[6] /10;
        
        INFO("Temp: %f\nHud: %f\nLight: %f\npa: %f\nSoilT: %f\nSoilH: %f\n------------------------------\n\n", 
        temp, humid, light, pressure, soilT, soilH);
        delay_ms(2000);
    }
}



/*******************************************************************************
Function name: USE_FULL_ASSERT
Decription: None
Input: None
Output: None
*******************************************************************************/
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

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
