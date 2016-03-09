/**
  ******************************************************************************
  * @file    
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "gps_uart_parser.h"

#include "uart.h"
#include "gps.h"
#include "ringbuf.h"

uint8_t GPS_RxBuff[256];
RINGBUF GPS_RxRingBuff;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
void GPS_Init(void)
{
    UART_Config(GPS_COMM, 9600);
    
    /*Ring Buff*/
    RINGBUF_Init(&GPS_RxRingBuff, GPS_RxBuff, sizeof(GPS_RxBuff));
}

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
void GPS_UART_Handler(void)
{
    uint8_t data;
	
	/* USART Receive Interrupt */
	if (USART_GetITStatus(GPS_COMM, USART_IT_RXNE) != RESET)
	{
		// Read one byte from the receive data register
		data = USART_ReceiveData(GPS_COMM);
        //u_send(USART1, data);
        
        RINGBUF_Put(&GPS_RxRingBuff, data);
        
        // GPS Command parser
        GPS_ComnandParser(data);
	}
}


/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/