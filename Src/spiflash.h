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
#include "defines.h"

/* Private typedef -----------------------------------------------------------*/
enum SST25_ERASE_MODE {
    all,
    block4k,
    block32k,
    block64k
};

/* Private define ------------------------------------------------------------*/
// Define SPI and Pin
#define SST_SPI     SPI2

#define SST_CS_RCC  RCC_AHB1Periph_GPIOB
#define SST_CS_Pin  GPIO_Pin_12
#define SST_CS_Port GPIOB

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

// Remap 
uint8_t SST25_Write(uint32_t addr, uint8_t *buf, uint32_t count);
uint8_t SST25_Read(uint32_t addr, uint8_t *buf, uint32_t count);
uint8_t SST25_Erase(uint32_t addr, enum SST25_ERASE_MODE mode);


#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/

