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
#include "rs485_com.h"

#include "defines.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t rs485RxBuff[256];
RINGBUF rs485RingBuff;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name: RS485_Init
Decription:
Input: None
Output: None
*******************************************************************************/
void RS485_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Config RW pin
    RCC_AHB1PeriphClockCmd(RS845_RW_RCC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = RS485_RW_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(RS485_RW_Port, &GPIO_InitStruct);
    
    // Config UART
    UART_Config(RS485_COMM, baud);
    
    // init ringbuf
    RINGBUF_Init(&rs485RingBuff, rs485RxBuff, sizeof(rs485RxBuff));
}

void RS485_Send(char *s)
{
    RS485_WRITE;
    u_printf(RS485_COMM,s);
    RS485_READ;
}

void RS485_FlushBuffer(void)
{
    uint8_t c;
	while(RINGBUF_Get(&rs232RingBuff, &c)==0);
}

/*******************************************************************************
Function name: RS485_Init
Decription:
Input: None
Output: None
*******************************************************************************/
void RS485_UART_Handler(void)
{
	uint8_t data;
	
	/* USART Receive Interrupt */
	if (USART_GetITStatus(RS485_COMM, USART_IT_RXNE) != RESET)
	{
		// Read one byte from the receive data register
		data = USART_ReceiveData(RS485_COMM);
        
        RINGBUF_Put(&rs485RingBuff, data);
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
