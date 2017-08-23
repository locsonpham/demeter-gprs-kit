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
#ifndef	__INPUT_H
#define __INPUT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Private define ------------------------------------------------------------*/
//
//  Xxxx State: Low: Input HIGH
//              High: Input LOW
//
#define X000_RCC     RCC_AHB1Periph_GPIOE
#define X000_Pin     GPIO_Pin_0
#define X000_Port    GPIOE

#define X001_RCC     RCC_AHB1Periph_GPIOE
#define X001_Pin     GPIO_Pin_1
#define X001_Port    GPIOE

#define X002_RCC     RCC_AHB1Periph_GPIOE
#define X002_Pin     GPIO_Pin_2
#define X002_Port    GPIOE

#define X003_RCC     RCC_AHB1Periph_GPIOE
#define X003_Pin     GPIO_Pin_3
#define X003_Port    GPIOE

#define X004_RCC     RCC_AHB1Periph_GPIOE
#define X004_Pin     GPIO_Pin_4
#define X004_Port    GPIOE

#define X005_RCC     RCC_AHB1Periph_GPIOE
#define X005_Pin     GPIO_Pin_5
#define X005_Port    GPIOE

typedef struct 
{
    uint16_t pin;
    GPIO_TypeDef* port;
} X_Input;

/* Extern variables ----------------------------------------------------------*/
extern X_Input X000, X001, X002, X003, X004, X005;

/* Private function prototypes -----------------------------------------------*/
void Input_Init(void);
uint8_t Input_State(X_Input* x);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
