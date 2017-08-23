/**
  ******************************************************************************
  * @file    
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "db.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LOG_RING_TYPE ringLog;
uint32_t logPositionCnt = 0;
uint32_t flagNewLog = 1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
  
  void DB_InitLog(void)
{
	uint8_t u8temp;
	int32_t i;
	LOG_RING_TYPE logTemp;
	ringLog.head = 0;
	ringLog.tail = 0;
	ringLog.crc = 0;
	logPositionCnt = 0;
    
	if(sizeof(MSG_STATUS_RECORD) % LOG_WRITE_BYTES)
		while(1); // Khai bao lai kieu bien
    
	for(i = 4095 - sizeof(LOG_RING_TYPE); i >= 0; i -= sizeof(LOG_RING_TYPE))
	{
		SST25_Read(LOG_POSITION_ADDR + i,(uint8_t *)&logTemp, sizeof(LOG_RING_TYPE));
		if(logTemp.head != 0xffffffff)
		{
			u8temp = DbCalcCheckSum((uint8_t *)&logTemp,sizeof(LOG_RING_TYPE) - 1);
			if(logPositionCnt == 0)
				logPositionCnt = i;
			if(logTemp.crc == u8temp)
			{
				ringLog = logTemp;
				break;
			}
		}
	}
}


void DB_RingLogReset(void)
{
	ringLog.head = 0;
	ringLog.tail = 0;
	DB_RingLogSave();
}
void DB_RingLogSave(void)
{
    // calc new crc
	ringLog.crc = DbCalcCheckSum((uint8_t *)&ringLog,sizeof(LOG_RING_TYPE) - 1);
    // write HEAD
	SST25_Write(LOG_POSITION_ADDR + logPositionCnt,(uint8_t *)&ringLog.head,sizeof(ringLog.head));
    
    // increase cnt to save RINGLOG TAIL
	logPositionCnt += sizeof(ringLog.head);
    // write TAIL
	SST25_Write(LOG_POSITION_ADDR + logPositionCnt ,(uint8_t *)&ringLog.tail,sizeof(ringLog.tail));
    
    // increase cnt to save CRC
	logPositionCnt += sizeof(ringLog.tail);
    // write CRC
	SST25_Write(LOG_POSITION_ADDR + logPositionCnt ,(uint8_t *)&ringLog.crc,sizeof(ringLog.crc));
    
    // increase cnt for next time
	logPositionCnt += sizeof(ringLog.crc);
    
    // if outof sector size => reset all RING LOG
	if(logPositionCnt >= 4095)
	{
		SST25_Erase(LOG_POSITION_ADDR, block4k);
		logPositionCnt = 0;
	}	
}

uint32_t DB_LogFill(void)
{
	if(ringLog.head >= ringLog.tail)
	{
		return (ringLog.head - ringLog.tail);
	}
	else
	{
	   return(LOG_DATA_SIZE_MAX / sizeof(MSG_STATUS_RECORD) - ringLog.tail + ringLog.head);
	}
}

int8_t DB_LoadLog(MSG_STATUS_RECORD *log)
{
	uint8_t u8temp;
	
	if(ringLog.tail == ringLog.head) return 0xff;   // no log
	
    // read data from flash
	SST25_Read(LOG_DATABASE_ADDR + ringLog.tail*sizeof(MSG_STATUS_RECORD), (uint8_t *)log, sizeof(MSG_STATUS_RECORD));
	// calc checksum
    u8temp = DbCalcCheckSum((uint8_t *)log,sizeof(MSG_STATUS_RECORD) - 1);
    // increase ring log tail
	ringLog.tail++;
    
 
	if(ringLog.tail >= LOG_DATA_SIZE_MAX / sizeof(MSG_STATUS_RECORD))   // overflow, reset tail
		ringLog.tail = 0;
    
    // save ring log
	DB_RingLogSave();
    
	if(log->crc == u8temp)  // valid data
	{
		return 0;
	}
	else 
	{
		return 0xff;
	}
}

int8_t DB_SaveLog(MSG_STATUS_RECORD *log)
{
	uint32_t u32temp,tailSector = 0, headSector = 0, headSectorOld = 0, i;
    
	tailSector = ringLog.tail * sizeof(MSG_STATUS_RECORD) / SECTOR_SIZE;
	headSectorOld = ringLog.head * sizeof(MSG_STATUS_RECORD) / SECTOR_SIZE;
	u32temp = ringLog.head;
	u32temp++;
    
	if(u32temp >= (LOG_DATA_SIZE_MAX / sizeof(MSG_STATUS_RECORD)))	
	{
		u32temp = 0;
	}
    
	headSector = u32temp * sizeof(MSG_STATUS_RECORD) / SECTOR_SIZE;
	if(headSector != headSectorOld)
	{
		SST25_Erase(LOG_DATABASE_ADDR + headSector * SECTOR_SIZE,block4k);
	}
    
	if((headSector == tailSector) && (u32temp <= ringLog.tail))
	{
		tailSector++;
		ringLog.tail = tailSector * SECTOR_SIZE / sizeof(MSG_STATUS_RECORD);
		if((tailSector * SECTOR_SIZE) % sizeof(MSG_STATUS_RECORD))
			 ringLog.tail++;
		if(ringLog.tail >= LOG_DATA_SIZE_MAX / sizeof(MSG_STATUS_RECORD))
			ringLog.tail = 0;
	}

	log->crc = DbCalcCheckSum((uint8_t *)log,sizeof(MSG_STATUS_RECORD) - 1);
	
	for(i = 0;i < sizeof(MSG_STATUS_RECORD);i += LOG_WRITE_BYTES)
	{
		SST25_Write(LOG_DATABASE_ADDR + ringLog.head*sizeof(MSG_STATUS_RECORD) + i,(uint8_t *)log + i,LOG_WRITE_BYTES);
	}
    
	ringLog.head = u32temp;
	DB_RingLogSave();
	
	return 0;
}

uint8_t DbCalcCheckSum(uint8_t *buff, uint32_t length)
{
	uint32_t i;
	uint8_t crc = 0;
	for(i = 0;i < length; i++)
	{
		crc += buff[i];
	}
	return crc;
}

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/