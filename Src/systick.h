/**
  ******************************************************************************
  * @file    systick.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTICK_H
#define __SYSTICK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "ringbuf.h"

/* Private define ------------------------------------------------------------*/
#define SysTick_Freq 	1000  // 1 ms

#if SysTick_Freq == 1000000
#define SysTick_1ms	1000
#elif SysTick_Freq == 100000
#define SysTick_1ms	100
#elif SysTick_Freq == 10000
#define SysTick_1ms	10
#elif SysTick_Freq == 1000
#define SysTick_1ms	1
#endif

#define TIMEOUT		0
#define TIMEIN		1

typedef struct {
	uint32_t start_time; 		
	uint32_t timeout;
	uint32_t crc; 
} Timeout_Type;



/* Extern variables ----------------------------------------------------------*/
extern uint16_t cnt0, cnt1;
extern uint16_t timeout;
extern uint8_t led_tick;
extern int sys_delay;

/* Private function prototypes -----------------------------------------------*/
void SysTick_Init(void);
void SysTick_Handler(void);
void delay_ms(uint16_t time);
void delay_us(uint32_t time);

uint32_t GetTick(void);
void InitTimeout(Timeout_Type *t, uint32_t timeout);
uint32_t CheckTimeout(Timeout_Type *t);

// Find data
void InitFindData(COMPARE_TYPE *cmpData,uint8_t *data);
uint8_t FindData(COMPARE_TYPE *cmpData,uint8_t c);

uint32_t TIME_MS(uint32_t time);
uint32_t TIME_SEC(uint32_t time);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/

