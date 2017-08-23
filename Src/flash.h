/**
  ******************************************************************************
  * @file    flash.h 
  * @author  Nguyen Van Hoa Vu - Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_H
#define __FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_8   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_9

#define FLASH_BUFFER    100

/* Base address of the Flash sectors */ 
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x08100000) /* Base @ of Sector 12, 16 Kbytes */
#define ADDR_FLASH_SECTOR_13     ((uint32_t)0x08104000) /* Base @ of Sector 13, 16 Kbytes */
#define ADDR_FLASH_SECTOR_14     ((uint32_t)0x08108000) /* Base @ of Sector 14, 16 Kbytes */
#define ADDR_FLASH_SECTOR_15     ((uint32_t)0x0810C000) /* Base @ of Sector 15, 16 Kbytes */
#define ADDR_FLASH_SECTOR_16     ((uint32_t)0x08110000) /* Base @ of Sector 16, 64 Kbytes */
#define ADDR_FLASH_SECTOR_17     ((uint32_t)0x08120000) /* Base @ of Sector 17, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18     ((uint32_t)0x08140000) /* Base @ of Sector 18, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19     ((uint32_t)0x08160000) /* Base @ of Sector 19, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20     ((uint32_t)0x08180000) /* Base @ of Sector 20, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_21     ((uint32_t)0x081A0000) /* Base @ of Sector 21, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_22     ((uint32_t)0x081C0000) /* Base @ of Sector 22, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23     ((uint32_t)0x081E0000) /* Base @ of Sector 23, 128 Kbytes */

#define USER_NAME_ADDR  0
#define USER_TEL_ADDR   40

/* Extern variables ----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

extern  uint8_t flash_data[FLASH_BUFFER];		// Data array

/* Private function prototypes -----------------------------------------------*/
void Flash_Write(void);
void Flash_Read(void);
static uint32_t GetSector(uint32_t Address);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
