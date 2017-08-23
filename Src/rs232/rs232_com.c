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
#include "rs232_com.h"

#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t rs232RxBuff[256];
RINGBUF rs232RingBuff;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void RS232_Init(uint32_t baud)
{
    // Config UART4
    UART_Config(RS232_COMM, baud);
    
    // init ringbuff
    RINGBUF_Init(&rs232RingBuff, rs232RxBuff, sizeof(rs232RxBuff));
}

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
// void RS232_UART_Handler(void)
// {
// 	uint8_t data;
// 	
// 	/* USART Receive Interrupt */
// 	if (USART_GetITStatus(RS232_COMM, USART_IT_RXNE) != RESET)
// 	{
// 		// Read one byte from the receive data register
// 		data = USART_ReceiveData(RS232_COMM);
//         
//         RINGBUF_Put(&rs232RingBuff, data);
// 	}
// }


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
