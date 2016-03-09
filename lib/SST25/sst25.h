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
#ifndef __SPIFLASH_H
#define __SPIFLASH_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Define SPI and Pin
#define SST_SPI     SPI1

#define SST_CS_RCC  RCC_APB2Periph_GPIOA
#define SST_CS_Pin  GPIO_Pin_4
#define SST_CS_Port GPIOA

#define SST_CS_Assert()			GPIO_ResetBits(SST_CS_Port, SST_CS_Pin)
#define SST_CS_DeAssert()		GPIO_SetBits(SST_CS_Port, SST_CS_Pin)

/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SPIFlashInit(void);
void SPIFlashReadArray(uint32_t dwAddress, uint8_t *vData, uint16_t wLength);
void SPIFlashBeginWrite(uint32_t dwAddr);
void SPIFlashWrite(uint8_t vData);
void SPIFlashWriteArray(uint8_t* vData, uint16_t wLen);
void SPIFlashEraseSector(uint32_t dwAddr);
void SPIFlashEraseAll(void);


#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/

