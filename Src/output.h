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
#ifndef	__OUTPUT_H
#define __OUTPUT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define Y000_RCC     RCC_AHB1Periph_GPIOB
#define Y000_Pin     GPIO_Pin_0
#define Y000_Port    GPIOB

#define Y001_RCC     RCC_AHB1Periph_GPIOB
#define Y001_Pin     GPIO_Pin_1
#define Y001_Port    GPIOB

#define Y002_RCC     RCC_AHB1Periph_GPIOB
#define Y002_Pin     GPIO_Pin_2
#define Y002_Port    GPIOB

#define Y003_RCC     RCC_AHB1Periph_GPIOB
#define Y003_Pin     GPIO_Pin_4
#define Y003_Port    GPIOB

#define Y004_RCC     RCC_AHB1Periph_GPIOB
#define Y004_Pin     GPIO_Pin_5
#define Y004_Port    GPIOB

#define Y005_RCC     RCC_AHB1Periph_GPIOB
#define Y005_Pin     GPIO_Pin_6
#define Y005_Port    GPIOB

#define Y006_RCC     RCC_AHB1Periph_GPIOB
#define Y006_Pin     GPIO_Pin_7
#define Y006_Port    GPIOB

#define Y007_RCC     RCC_AHB1Periph_GPIOB
#define Y007_Pin     GPIO_Pin_8
#define Y007_Port    GPIOB

#define Y008_RCC     RCC_AHB1Periph_GPIOB
#define Y008_Pin     GPIO_Pin_9
#define Y008_Port    GPIOB

#define Y009_RCC     RCC_AHB1Periph_GPIOB
#define Y009_Pin     GPIO_Pin_10
#define Y009_Port    GPIOB

#define Y010_RCC     RCC_AHB1Periph_GPIOB
#define Y010_Pin     GPIO_Pin_11
#define Y010_Port    GPIOB

#define Y011_RCC     RCC_AHB1Periph_GPIOB
#define Y011_Pin     GPIO_Pin_12
#define Y011_Port    GPIOB

#define Y012_RCC     RCC_AHB1Periph_GPIOB
#define Y012_Pin     GPIO_Pin_13
#define Y012_Port    GPIOB

#define Y013_RCC     RCC_AHB1Periph_GPIOB
#define Y013_Pin     GPIO_Pin_14
#define Y013_Port    GPIOB

#define Y014_RCC     RCC_AHB1Periph_GPIOB
#define Y014_Pin     GPIO_Pin_15
#define Y014_Port    GPIOB

typedef struct 
{
    uint16_t pin;
    GPIO_TypeDef* port;
} Y_Out;

/* Extern variables ----------------------------------------------------------*/
extern Y_Out Y000, Y001, Y002, Y003, Y004, Y005, Y006, Y007, Y008, Y009, Y010, Y011, Y012, Y013, Y014;

/* Private function prototypes -----------------------------------------------*/
void Output_Init(void);
void Output_High(Y_Out* y);
void Output_Low(Y_Out* y);
uint8_t Output_State(Y_Out* y);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
