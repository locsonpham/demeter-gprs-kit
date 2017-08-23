/**
  ******************************************************************************
  * @file    template.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__RS232_COM_H
#define __RS232_COM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "includes.h"

/* Private define ------------------------------------------------------------*/
#define RS232_COMM  UART4
#define RS232_UART_Handler  //UART4_IRQHandler


/* Extern variables ----------------------------------------------------------*/
extern RINGBUF rs232RingBuff;

/* Private function prototypes -----------------------------------------------*/
void RS232_Init(uint32_t baud);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
