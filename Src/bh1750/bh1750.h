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
#ifndef __BH1750_H
#define __BH1750_H
 
/* Includes ------------------------------------------------------------------*/
#include "defines.h"

#include "i2c.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

//#define BH1750_POWER_CONTROL
// #define BH_DEBUG

#ifdef BH_DEBUG
#define BH_Info(...)	printf(__VA_ARGS__)
#else
#define BH_Info(...)
#endif

#define BH1750_I2C      I2C1
#define BH1750_I2CADDR  0x23 << 1
#define BH1750_TIMEOUT  1000

#ifdef BH1750_POWER_CONTROL
// BH1750 POWER pin
#define BH1750_Pin  GPIO_Pin_1
#define BH1750_Port GPIOA

#define BH1750_EN       GPIO_WriteBit(BH1750_Port, BH1750_Pin, Bit_RESET)
#define BH1750_OFF      GPIO_WriteBit(BH1750_Port, BH1750_Pin, Bit_SET)
#endif

/* Special Defines -----------------------------------------------*/
// No active state
#define BH1750_POWER_DOWN 0x00

// Wating for measurment command
#define BH1750_POWER_ON 0x01

// Reset data register value - not accepted in POWER_DOWN mode
#define BH1750_RESET 0x07

// Start measurement at 1lx resolution. Measurement time is approx 120ms.
#define BH1750_CONTINUOUS_HIGH_RES_MODE  0x10

// Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
#define BH1750_CONTINUOUS_HIGH_RES_MODE_2  0x11

// Start measurement at 4lx resolution. Measurement time is approx 16ms.
#define BH1750_CONTINUOUS_LOW_RES_MODE  0x13

// Start measurement at 1lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_HIGH_RES_MODE  0x20

// Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_HIGH_RES_MODE_2  0x21

// Start measurement at 1lx resolution. Measurement time is approx 120ms.
// Device is automatically set to Power Down after measurement.
#define BH1750_ONE_TIME_LOW_RES_MODE  0x23

/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void BH1750_Begin(uint8_t mode);    // Please initialize I2Cx before use this function
void BH1750_Configure(uint8_t mode);
uint8_t BH1750_ReadLightLevel(int32_t* level);

#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
