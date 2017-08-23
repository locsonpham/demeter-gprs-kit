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
#ifndef __DB_H
#define __DB_H
 
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "SPIFlash.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SECTOR_SIZE	4096

#define FLASH_SIZE_MAX	  (16*1024*1024) //16Mbytes

#define FIRMWARE_SIZE_MAX (128*1024)    //128Kbytes

#define FIRMWARE_FILE_SIZE_ERR		0xAAAAAAAA
#define FIRMWARE_START_ADDR_ERR		0xBBBBBBBB
#define FIRMWARE_END_ADDR_ERR	    0xCCCCCCCC
#define FIRMWARE_CRC_ERR			0xDDDDDDDD


#define FIRMWARE_INFO_ADDR                  0
#define FIRMWARE_OFFSET_COUNTER_BASE_ADDR	0x00001000
#define FIRMWARE_BASE_ADDR	                0x00002000
#define FRIMWARE_MAX_SIZE			0x00020000
#define CONFIG_ADDR				    (0x00001000 + FRIMWARE_MAX_SIZE)
#define MILEAGE_ADDR				(0x00002000 + FRIMWARE_MAX_SIZE)
#define TOTAL_PULSE_ADDR			(0x00003000 + FRIMWARE_MAX_SIZE)
#define LOG_POSITION_ADDR			(0x00004000 + FRIMWARE_MAX_SIZE)
#define DRIVING_TIME_ADDR			(0x00005000 + FRIMWARE_MAX_SIZE)
#define TOTAL_DRIVING_TIME_ADDR		(0x00006000 + FRIMWARE_MAX_SIZE)
#define OPEN_DOOR_CNT_ADDR			(0x00007000 + FRIMWARE_MAX_SIZE)
#define OVER_SPEED_ADDR				(0x00008000 + FRIMWARE_MAX_SIZE)
#define RUN_MODE    				(0x00009000 + FRIMWARE_MAX_SIZE)
#define LOG_PRINT_REPORT_ADDR		(0x00010000 + FRIMWARE_MAX_SIZE)
#define LOG_PRINT_REPORT_SIZE		184320 // <=> 0x2D000 = 180 kBytes

#define LOG_IMAGE_SIZE      (0*1024*1024)   // 2 Mbytes to save Image
#define LOG_IMAGE_ADDR      (FLASH_SIZE_MAX - SECTOR_SIZE - LOG_IMAGE_SIZE)

#define LOG_DATABASE_ADDR	(0x000080000)
#define LOG_DATA_SIZE_MAX  (FLASH_SIZE_MAX - SECTOR_SIZE - LOG_DATABASE_ADDR - SECTOR_SIZE - LOG_IMAGE_SIZE)

#define FIRMWARE_STATUS_OFFSET		(1)
#define FIRMWARE_FILE_SIZE_OFFSET	(2)
#define FIRMWARE_CRC_OFFSET			(3)
#define FIRMWARE_START_ADDR_OFFSET	(4)
#define FIRMWARE_ERR_ADDR_OFFSET	(5)

#define LOG_WRITE_BYTES 4

typedef struct __attribute__((packed, aligned(4)))
{
	DATE_TIME currentTime; 
	float temp;   
	float humid;            
	float temp1;                
	float humid1;               
	int lux;  
	int pa;
    uint8_t rainPercent; 
	uint8_t rainPanelTemp;      
	int rainSum;  
	int windDir;          
	int windSpeed;
    uint8_t out0;
    uint8_t out1;
    uint8_t out2;
	float voltage;           
    uint8_t serverSent;
	uint8_t crc;                
} MSG_STATUS_RECORD; 

typedef struct __attribute__((packed))
{
	uint8_t oldMsgIsEmpty;
	uint8_t newMsgIsEmpty;
	uint8_t lastMsgIsEmpty;
	MSG_STATUS_RECORD	oldMsg;
	MSG_STATUS_RECORD	newMsg;
	MSG_STATUS_RECORD   lastMsg;
} TRACKER_MSG_TYPE;

typedef struct __attribute__((packed))
{
	int8_t hour;			// hour 0 - 23
	int8_t min;				// min 0 - 59
	int8_t sec;				// sec 0 - 59
} RECORD_TIME;

typedef struct __attribute__((packed)){
	uint32_t head;
	uint32_t tail;
	uint8_t crc;
} LOG_RING_TYPE;

/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
uint8_t DbCalcCheckSum(uint8_t *buff, uint32_t length);

//Flash log
void DB_InitLog(void);
int8_t DB_SaveLog(MSG_STATUS_RECORD *log);
int8_t DB_LoadLog(MSG_STATUS_RECORD *log);
void DB_RingLogReset(void);
void DB_RingLogSave(void);
void DB_ResetCntLoad(void);
void DB_ResetCntSave(void);
void DB_TotalPulseLoad(void);
void DB_TotalPulseSave(void);

#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
