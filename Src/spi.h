/**
  ******************************************************************************
  * @file    spi.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__SPI_H
#define __SPI_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Private define ------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SPI_Config(SPI_TypeDef* comm);
void SPI1_Config(void);
void SPI2_Config(void);
uint8_t SPI_SendByte(SPI_TypeDef* comm, unsigned char data);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
