/**
  ******************************************************************************
  * @file    sim900.h 
  * @author  lspham
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__SIM900_H
#define __SIM900_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#include "defines.h"

#include "ringbuf.h"

/* Private define ------------------------------------------------------------*/
/* Uncomment to use */
#define SIM_USE_KEY
#define SIM_USE_RST
#define SIM_USE_RI
#define SIM_USE_STT

#define SIM_COMM            USART1
#define SIM_UART_Handler    USART1_IRQHandler

#define SIM_KEY_RCC     RCC_APB2Periph_GPIOA
#define SIM_KEY_Pin     GPIO_Pin_8
#define SIM_KEY_Port    GPIOA

#define SIM_RST_RCC     RCC_APB2Periph_GPIOC
#define SIM_RST_Pin     GPIO_Pin_9
#define SIM_RST_Port    GPIOC

#define SIM_RI_RCC      RCC_APB2Periph_GPIOC
#define SIM_RI_Pin      GPIO_Pin_7
#define SIM_RI_Port     GPIOC

#define SIM_STT_RCC     RCC_APB2Periph_GPIOC
#define SIM_STT_Pin     GPIO_Pin_8
#define SIM_STT_Port    GPIOC

#define KEY_OFF          GPIO_SetBits(SIM_KEY_Port, SIM_KEY_Pin)
#define KEY_ON           GPIO_ResetBits(SIM_KEY_Port, SIM_KEY_Pin)

#define SIM_RST_HIGH    GPIO_SetBits(SIM_RST_Port, SIM_RST_Pin)
#define SIM_RST_LOW     GPIO_ResetBits(SIM_RST_Port, SIM_RST_Pin)

#define SIM_STT         GPIO_ReadInputDataBit(SIM_STT_Port, SIM_STT_Pin)

/* Extern variables ----------------------------------------------------------*/
extern RINGBUF UART1_RxRingBuff;

/* Private function prototypes -----------------------------------------------*/
void SIM900_Init(uint32_t baud);
void SIM900_Config(void);
void POWKEY_On(void);
void POWKEY_Off(void);
void SIM900_Reset(void);

uint32_t COMM_Puts(uint8_t *s);
uint32_t COMM_Putc(uint8_t c);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
