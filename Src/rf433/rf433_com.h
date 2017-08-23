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
#ifndef	__TEMPLATE_H
#define __TEMPLATE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "includes.h"

/* Private define ------------------------------------------------------------*/
#define RF433_COMM  USART3
#define RF433_UART_Handler  USART3_IRQHandler

#define RF433_SETA_RCC  RCC_AHB1Periph_GPIOD
#define RF433_SETA_Pin  GPIO_Pin_10
#define RF433_SETA_Port GPIOD

#define RF433_SETB_RCC  RCC_AHB1Periph_GPIOD
#define RF433_SETB_Pin  GPIO_Pin_11
#define RF433_SETB_Port  GPIOD

#define RF433_AUX_RCC   RCC_AHB1Periph_GPIOD
#define RF433_AUX_Pin   GPIO_Pin_12
#define RF433_AUX_Port  GPIOD

#define SETA_HIGH   GPIO_SetBits(RF433_SETA_Port, RF433_SETA_Pin)
#define SETA_LOW    GPIO_ResetBits(RF433_SETA_Port, RF433_SETA_Pin)

#define SETB_HIGH   GPIO_SetBits(RF433_SETB_Port, RF433_SETB_Pin)
#define SETB_LOW    GPIO_ResetBits(RF433_SETB_Port, RF433_SETB_Pin)

/* Extern variables ----------------------------------------------------------*/
extern RINGBUF rf433RingBuff;

/* Private function prototypes -----------------------------------------------*/
void RF433_Init(uint32_t baud);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
