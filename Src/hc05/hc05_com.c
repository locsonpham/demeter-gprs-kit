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
#include "hc05_com.h"

#include "includes.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t hc05RxBuff[256];
RINGBUF hc05RingBuff;

extern  USHORT  usRegHoldingBuf[REG_HOLDING_NREGS];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void HC05_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Config pin
    RCC_AHB1PeriphClockCmd(HC05_STATE_RCC, ENABLE);
    RCC_AHB1PeriphClockCmd(HC05_EN_RCC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = HC05_STATE_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(HC05_STATE_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = HC05_EN_Pin;
    GPIO_Init(HC05_EN_Port, &GPIO_InitStruct);
    
    // Config UART
    UART_Config(HC05_COMM, baud);
    
    // running mode
    HC05_EN_HIGH;
    
    // init ringbuf
    RINGBUF_Init(&hc05RingBuff, hc05RxBuff, sizeof(hc05RxBuff));
}

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void HC05_UART_Handler(void)
{
	uint8_t data;
	
	/* USART Receive Interrupt */
	if (USART_GetITStatus(HC05_COMM, USART_IT_RXNE) != RESET)
	{
		// Read one byte from the receive data register
		data = USART_ReceiveData(HC05_COMM);
        
        switch (data)
        {
            case 'A': usRegHoldingBuf[0] = 1; break;
            case 'a': usRegHoldingBuf[0] = 0; break;
            case 'B': usRegHoldingBuf[1] = 1; break;
            case 'b': usRegHoldingBuf[1] = 0; break;
            case 'C': usRegHoldingBuf[2] = 1; break;
            case 'c': usRegHoldingBuf[2] = 0; break;
            case 'D': usRegHoldingBuf[3] = 1; break;
            case 'd': usRegHoldingBuf[3] = 0; break;
            case 'E': usRegHoldingBuf[4] = 1; break;
            case 'e': usRegHoldingBuf[4] = 0; break;
            case 'F': usRegHoldingBuf[5] = 1; break;
            case 'f': usRegHoldingBuf[5] = 0; break;
            case 'G': usRegHoldingBuf[6] = 1; break;
            case 'g': usRegHoldingBuf[6] = 0; break;
            case 'H': usRegHoldingBuf[7] = 1; break;
            case 'h': usRegHoldingBuf[7] = 0; break;
            case 'I': usRegHoldingBuf[8] = 1; break;
            case 'i': usRegHoldingBuf[8] = 0; break;
            case 'J': usRegHoldingBuf[9] = 1; break;
            case 'j': usRegHoldingBuf[9] = 0; break;
//             case 'K': usRegHoldingBuf[10] = 1; break;
//             case 'k': usRegHoldingBuf[10] = 0; break;
//             case 'L': usRegHoldingBuf[11] = 1; break;
//             case 'l': usRegHoldingBuf[11] = 0; break;
//             case 'M': usRegHoldingBuf[12] = 1; break;
//             case 'm': usRegHoldingBuf[12] = 0; break;
//             case 'N': usRegHoldingBuf[13] = 1; break;
//             case 'n': usRegHoldingBuf[13] = 0; break;
//             case 'O': usRegHoldingBuf[14] = 1; break;
//             case 'o': usRegHoldingBuf[14] = 0; break;
//             case 'P': usRegHoldingBuf[15] = 1; break;
//             case 'p': usRegHoldingBuf[15] = 0; break;
            
            default: break;
        }
        
        RINGBUF_Put(&hc05RingBuff, data);
	}
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
