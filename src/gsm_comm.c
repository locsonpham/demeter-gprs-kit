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
#include "sim900.h"

#include "ringbuf.h"
#include "systick.h"
#include "at_command_parser.h"
#include "uart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t UART1_RxBuff[256];
RINGBUF UART1_RxRingBuff;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
uint32_t COMM_Puts(uint8_t *s)	
{
	u_printf(SIM_COMM, (char *)s);
}

uint32_t COMM_Putc(uint8_t c)	
{
	return u_send(SIM_COMM, c);
}

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void SIM900_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    
    #ifdef SIM_USE_KEY
    RCC_APB2PeriphClockCmd(SIM_KEY_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = SIM_KEY_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM_KEY_Port, &GPIO_InitStructure);
    #endif
    
    #ifdef SIM_USE_RST
    RCC_APB2PeriphClockCmd(SIM_RST_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = SIM_RST_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM_RST_Port, &GPIO_InitStructure);
    #endif
    
    #ifdef SIM_USE_RI
    RCC_APB2PeriphClockCmd(SIM_RI_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = SIM_RI_Pin;                    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM_RI_Port, &GPIO_InitStructure);
    #endif
    
    #ifdef SIM_USE_STT
    RCC_APB2PeriphClockCmd(SIM_STT_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = SIM_STT_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SIM_STT_Port, &GPIO_InitStructure);
    #endif
    
    SIM_RST_HIGH;
    
    // Config UART
    UART_Config(SIM_COMM, baud);
    
    /*Ring Buff*/
    RINGBUF_Init(&UART1_RxRingBuff, UART1_RxBuff, sizeof(UART1_RxBuff));
}

void POWKEY_On(void)
{
    KEY_ON;
    delay_ms(1200);
    KEY_OFF;
    delay_ms(1200);
    KEY_ON;
    delay_ms(2200);
}

void POWKEY_Off(void)
{
    KEY_OFF;
    delay_ms(1100);
    KEY_ON;
    delay_ms(1800);
}

void SIM_UART_Handler(void)
{
	uint8_t data;
	
	/* USART Receive Interrupt */
	if (USART_GetITStatus(SIM_COMM, USART_IT_RXNE) != RESET)
	{
		// Read one byte from the receive data register
		data = USART_ReceiveData(SIM_COMM);
        //u_send(SIM_COMM, data);
        
        RINGBUF_Put(&UART1_RxRingBuff, data);
        
        // AT Command parser
        AT_ComnandParser(data);
	}
}

void SIM900_Reset(void)
{
    SIM_RST_LOW;
    delay_ms(2000);
    SIM_RST_HIGH;
    delay_ms(3000);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
