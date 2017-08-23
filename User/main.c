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
// Rain SUm Area 55x115

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t sendPri = 0;

uint8_t main_started = 0;

Timeout_Type tGetInfo;

uint16_t mainWDG = 0;

uint16_t resetCnt = 0;


float temp, humid, light, pressure, soilT, soilH;

// GSM region
Timeout_Type tMODEMInit;
Timeout_Type tMODEM_AvailableCheck;
Timeout_Type tGPRSInit;
Timeout_Type tSMSCheckTimeout;
Timeout_Type tReConnectServer[SOCKET_NUM];
Timeout_Type tTimeSync;

uint8_t modemAvailableCnt = 0;
uint8_t mainBuf[768];
uint32_t receiveLen;
uint8_t flagCalling = 0;

// GPRS
uint8_t gprsTryNum = 5;
uint16_t u16Temp;
uint32_t u32Temp;
uint32_t nackCnt = 0;
uint32_t serverNumTry[SOCKET_NUM] = {0};
uint8_t svUseIP;
uint8_t *priServerIp;
uint8_t svUseIP_ts;
uint8_t *priServerIp_ts;
uint16_t priServerPort_ts;
uint8_t *secServerIp;
uint16_t priServerPort;
uint16_t secServerPort;
uint8_t  curServer[SOCKET_NUM];
Timeout_Type tReConnectServer[SOCKET_NUM];

char *server_domain = "128.199.107.77";
int server_port = 3000;

Timeout_Type tCheckPeriodSendData[SOCKET_NUM];
Timeout_Type tTimeReCheckAck;

Timeout_Type tThingSpeak;

const uint16_t periodSendDataMaxTime[SOCKET_NUM];

// CELLULAR INFO
Timeout_Type tGetCellID;

// SOCKET
Timeout_Type tSocketOn;


/* Private function prototypes -----------------------------------------------*/
void init(void);
void GSM_Handler(void);
void GPRS_Handler(uint8_t task);
void CELL_Handler(void);

/* Private functions ---------------------------------------------------------*/
void init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Init();
    
    /* WWDG configuration */
    /* Enable WWDG clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    /* WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)  */
    WWDG_SetPrescaler(WWDG_Prescaler_8);

    /* Set Window value to 80; WWDG counter should be refreshed only when the counter
    is below 80 (and greater than 64) otherwise a reset will be generated */
    WWDG_SetWindowValue(80);

    /* Enable WWDG and set counter value to 127, WWDG timeout = ~780 us * 64 = 49.92 ms 
     In this case the refresh window is: 
           ~780 * (127-80) = 36.6ms < refresh window < ~780 * 64 = 49.9ms
    */
    WWDG_Enable(127);

    delay_ms(2000);
    
    // Config UART3 for debug
    UART_Config(USART3, 9600);
}

// RF Config Enable pin as Output
// Enable pin: PD13
void RF_PinConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void RF_Enable(void)
{
    GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_RESET);
}

void RF_Disable(void)
{
    GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_SET);
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
    
    // Init Sensor
    Sensor_Init();
    
    // Track Init
    Tracking_Init();
    
    // Enable RF Power
    RF_PinConfig();
    RF_Enable();
    
    // Modbus for Weather Station Module
    // USART6
    eMBErrorCode eStatus;
    eStatus = eMBInit(  MB_RTU,
                        110,
                        0,
                        9600,
                        MB_PAR_NONE );
    
    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable();
    
    // INFO
    InitTimeout(&tGetInfo, 0);
    INFO("\n\nSystem Init Done!\n");
    main_started = 1; // Start
    
    InitTimeout(&tThingSpeak, TIME_SEC(1));
    Timeout_Type tUpdate;
    InitTimeout(&tUpdate, TIME_SEC (1));
    
    while (1)
    {
        static uint8_t main_task = 0;
        
        
        mainWDG = 0; // Counter for WDG
 
        if (CheckTimeout(&tUpdate) == TIMEOUT)
        {
            // Update Sensor
            Sensor_Update();
            
            INFO("*C: %f\n", temperature);
            INFO("H: %f\n", humidity);
            INFO("lux: %d\n", lux);
            INFO("rainSum: %d\n", rainSum);
            INFO("windSpeed: %d\n", windSpeed);
            INFO("windDir: %d\n", windDir);
            INFO("volt: %f\n", voltage);
            INFO("rst: %d\n", resetCnt);
            INFO("\n");

            InitTimeout(&tUpdate, TIME_SEC(1));
        }
//         INFO("%d | %d %d %d\n",windDir, _tmp[2], _tmp[1],_tmp[0]);
//         delay_ms(100);
        
        // Increase main_task
        main_task++;
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
