/**
  ******************************************************************************
  * @file    uart.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__UART_H
#define __UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include <stdio.h>

/* Private define ------------------------------------------------------------*/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

// UART use printf
#define     USART_DEBUG  USART3


#define 	RX_BUFF_LEN		255
#define		TX_BUFF_LEN		255

typedef struct
{
	USART_TypeDef * uartx;
	uint8_t TxBuffer[TX_BUFF_LEN];
	uint8_t RxBuffer[RX_BUFF_LEN];
	uint8_t RxPackedIncomeFlag;
	uint8_t TxSendingFlag;
	uint8_t RxCounter;
	uint8_t Length;
} uart_param;

/* Extern variables ----------------------------------------------------------*/
extern volatile uart_param uart1, uart3, uart5;

/* Private function prototypes -----------------------------------------------*/
void UART_Config(USART_TypeDef* u, uint32_t baud);
void UART1_Init(uint32_t baud_rate);
void UART2_Init(uint32_t baud_rate);
void UART3_Init(uint32_t baud_rate);
void UART4_Init(uint32_t baud_rate);
void UART5_Init(uint32_t baud_rate);

uint8_t u_send(USART_TypeDef *uart, uint8_t data);
void u_printf(USART_TypeDef *uart, char* s);

#endif
