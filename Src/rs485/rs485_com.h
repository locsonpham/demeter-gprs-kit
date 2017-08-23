/**
  ******************************************************************************
  * @file    rs485_com.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__RS485_COM_H
#define __RS485_COM_H

/* Includes ------------------------------------------------------------------*/
#include "defines.h"

/* Private define ------------------------------------------------------------*/
#define RS485_COMM  USART2
#define RS485_UART_Handler USART2_IRQHandler

#define RS845_RW_RCC    RCC_AHB1Periph_GPIOD
#define RS485_RW_Pin    GPIO_Pin_4
#define RS485_RW_Port   GPIOD

#define RS485_WRITE      GPIO_SetBits(RS485_RW_Port, RS485_RW_Pin)
#define RS485_READ     GPIO_ResetBits(RS485_RW_Port, RS485_RW_Pin)

/* Extern variables ----------------------------------------------------------*/
extern RINGBUF rs485RingBuff;

/* Private function prototypes -----------------------------------------------*/
void RS485_Init(uint32_t baud);
void RS485_Send(char *s);
void RS485_FlushBuffer(void);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
