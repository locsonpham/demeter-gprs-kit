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
#include "stm32f10x.h"


/* Private define ------------------------------------------------------------*/
/* 
SPIx_MASTER 0: Slave Mode
            1: Master Mode
*/

#define SPI1_MASTER     1
#define SPI1_CAPTURE    0
#define SPI1_DATASIZE   SPI_DataSize_8b
#define SPI1_FIRSTBIT   SPI_FirstBit_MSB
#define SPI1_PRESCALER  SPI_BaudRatePrescaler_4

#define SPI2_MASTER     1
#define SPI2_CAPTURE    1
#define SPI2_DATASIZE   SPI_DataSize_8b
#define SPI2_FIRSTBIT   SPI_FirstBit_LSB
#define SPI2_PRESCALER  SPI_BaudRatePrescaler_64


// Region define (not change)
#if SPI1_MASTER == 1
#define SPI1_MODE   SPI_Mode_Master
#define SPI1_NSS    SPI_NSS_Soft
#else   
#define SPI1_MODE   SPI_Mode_Slave
#define SPI1_NSS    SPI_NSS_Hard
#endif

#if     SPI1_CAPTURE == 3
#define SPI1_CPOL   SPI_CPOL_High
#define SPI1_CPHA   SPI_CPHA_2Edge
#elif   SPI1_CAPTURE == 2
#define SPI1_CPOL   SPI_CPOL_High
#define SPI1_CPHA   SPI_CPHA_1Edge
#elif   SPI1_CAPTURE == 1
#define SPI1_CPOL   SPI_CPOL_Low
#define SPI1_CPHA   SPI_CPHA_2Edge
#elif   SPI1_CAPTURE == 0
#define SPI1_CPOL   SPI_CPOL_Low
#define SPI1_CPHA   SPI_CPHA_1Edge
#endif

#if SPI2_MASTER == 1
#define SPI2_MODE   SPI_Mode_Master
#define SPI2_NSS    SPI_NSS_Soft
#else
#define SPI2_MODE   SPI_Mode_Slave
#define SPI2_NSS    SPI_NSS_Hard
#endif

#if     SPI2_CAPTURE == 3
#define SPI2_CPOL   SPI_CPOL_High
#define SPI2_CPHA   SPI_CPHA_2Edge
#elif   SPI2_CAPTURE == 2
#define SPI2_CPOL   SPI_CPOL_High
#define SPI2_CPHA   SPI_CPHA_1Edge
#elif   SPI2_CAPTURE == 1
#define SPI2_CPOL   SPI_CPOL_Low
#define SPI2_CPHA   SPI_CPHA_2Edge
#elif   SPI2_CAPTURE == 0
#define SPI2_CPOL   SPI_CPOL_Low
#define SPI2_CPHA   SPI_CPHA_1Edge
#endif

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SPI_Config(SPI_TypeDef* comm);
void SPI1_Config(void);
void SPI2_Config(void);
uint8_t SPI_SendByte(SPI_TypeDef* comm, unsigned char data);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
