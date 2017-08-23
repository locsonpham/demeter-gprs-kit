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
#ifndef __SYS_TIME_H__
#define __SYS_TIME_H__

#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define TIME_ZONE			(+7)
#define SECS_OF_DAY		86400 // = 24h*60m*60s

/* Private typedef -----------------------------------------------------------*/
typedef struct __attribute__((packed)){
	int16_t year;			// year with all 4-digit (2011)
	int8_t month;			// month 1 - 12 (1 = Jan)
	int8_t mday;			// day of month 1 - 31
	int8_t wday;			// day of week 1 - 7 (1 = Sunday)
	int8_t hour;			// hour 0 - 23
	int8_t min;				// min 0 - 59
	int8_t sec;				// sec 0 - 59
} DATE_TIME;

/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern DATE_TIME sysTime;

/* Private function prototypes -----------------------------------------------*/
int8_t TIME_AddSec(DATE_TIME *t, int32_t sec);
int32_t TIME_GetSec(DATE_TIME *t);
int8_t TIME_FromSec(DATE_TIME *t, int32_t sec);
int8_t TIME_FromGsm(DATE_TIME *t, DATE_TIME *time);
void TIME_Copy(DATE_TIME *t1, DATE_TIME *t2);

#ifdef USE_GPS
int8_t TIME_FromGps(DATE_TIME *t, nmeaINFO *nmeaInfo);
#endif

#endif

/************************ END OF FILE ****/
