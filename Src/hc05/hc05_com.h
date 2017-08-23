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
#ifndef	__HC05_COM_H
#define __HC05_COM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "includes.h"

/* Private define ------------------------------------------------------------*/
#define HC05_COMM   USART2
#define HC05_UART_Handler   USART2_IRQHandler

#define HC05_STATE_RCC  RCC_AHB1Periph_GPIOA
#define HC05_STATE_Pin  GPIO_Pin_11
#define HC05_STATE_Port GPIOA

#define HC05_EN_RCC     RCC_AHB1Periph_GPIOA
#define HC05_EN_Pin     GPIO_Pin_15
#define HC05_EN_Port    GPIOA

#define HC05_EN_HIGH    GPIO_SetBits(HC05_EN_Port, HC05_EN_Pin)
#define HC05_EN_LOW     GPIO_ResetBits(HC05_EN_Port, HC05_EN_Pin)

/* Extern variables ----------------------------------------------------------*/
extern RINGBUF hc05RingBuff;

/* Private function prototypes -----------------------------------------------*/
void HC05_Init(uint32_t baud);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
