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
#ifndef __I2C_H
#define __I2C_H
 
/* Includes ------------------------------------------------------------------*/
#include "defines.h"

#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {Error = 0, Success = !Error } I2C_Status;
#define I2C_Timeout	100		// 100ms

#define Timed_I2C(x) {Timeout_Type timeout; InitTimeout(&timeout, I2C_Timeout); while (x) { if (CheckTimeout(&timeout) == 0) {goto errReturn;}}}

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void I2C1_Init(uint32_t baud);


#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
