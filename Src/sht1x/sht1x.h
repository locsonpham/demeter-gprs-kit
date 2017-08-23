/**
  ******************************************************************************
  * @file    
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SHT1x_H
#define __SHT1x_H
 
/* Includes ------------------------------------------------------------------*/
#include "defines.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


#ifdef SHT_DEBUG
#define SHT_Printf(...)  printf(__VA_ARGS__)
#else
#define SHT_Printf(...)
#endif

#define USE_TEMP_EN

#ifdef USE_TEMP_EN
#define TEMP_EN_RCC     RCC_APB2Periph_GPIOA
#define TEMP_EN_Port    GPIOA
#define TEMP_EN_Pin     GPIO_Pin_11

#define TEMP_EN     GPIO_WriteBit(TEMP_EN_Port, TEMP_EN_Pin, Bit_RESET)
#define TEMP_OFF    GPIO_WriteBit(TEMP_EN_Port, TEMP_EN_Pin, Bit_SET)
#endif



/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern GPIO_TypeDef* _datPort;
extern GPIO_TypeDef* _clkPort;
extern uint16_t _datPin, _clkPin;
extern uint8_t SHT_Status;

/* Private function prototypes -----------------------------------------------*/
void SHT_InitPin(GPIO_TypeDef* dataPort, uint16_t dataPin, GPIO_TypeDef* clkPort, uint16_t clkPin);
uint8_t SHT_readTemperatureC(float *val);
uint8_t SHT_readHumidity(float *val);

#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
