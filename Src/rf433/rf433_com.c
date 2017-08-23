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
#include "rf433_com.h"

#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t rf433RxBuff[256];
RINGBUF rf433RingBuff;

static uint8_t rxCnt = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void RF433_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Config pin
    RCC_AHB1PeriphClockCmd(RF433_SETA_RCC, ENABLE);
    RCC_AHB1PeriphClockCmd(RF433_SETB_RCC, ENABLE);
    RCC_AHB1PeriphClockCmd(RF433_AUX_RCC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = RF433_SETA_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(RF433_SETA_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = RF433_SETB_Pin;
    GPIO_Init(RF433_SETB_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = RF433_AUX_Pin;
    GPIO_Init(RF433_AUX_Port, &GPIO_InitStruct);
    
    // Config UART
    UART_Config(RF433_COMM, baud);
    
    // SET Normal MODE
    SETA_LOW;
    SETB_LOW;
    
    /*Ring Buff*/
    RINGBUF_Init(&rf433RingBuff, rf433RxBuff, sizeof(rf433RxBuff));
}

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void RF433_UART_Handler(void)
{
	uint8_t data;
	
	/* USART Receive Interrupt */
	if (USART_GetITStatus(RF433_COMM, USART_IT_RXNE) != RESET)
	{
		// Read one byte from the receive data register
		data = USART_ReceiveData(RF433_COMM);
        
        if (data == ':') rxCnt = 0; // start of frame
        rf433RxBuff[rxCnt++] = data;
        
        if (rxCnt >= 2) 
        {
            if ((rf433RxBuff[rxCnt-1] == '\n') && (rf433RxBuff[rxCnt-2] == '\r'))
            {
                
            }
        }
        
        //RINGBUF_Put(&rf433RingBuff, data);
	}
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
