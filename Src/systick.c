/**
  ******************************************************************************
  * @file    systick.c 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief	Su dung systick lam timer nen cho he thong
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "systick.h"

#include "defines.h"
#include "com.h"
#include "comms.h"
#include "tracking.h"

/* Private macro -------------------------------------------------------------*/
#define GetCurrentTime GetTick

/* Private variables ---------------------------------------------------------*/
uint32_t tick_cnt = 0;

uint16_t cnt0 = 0, cnt1 = 0;
uint16_t timeout = 0;
uint64_t ms_tick = 0;

uint32_t tickTimeSec;
uint16_t watchDogCnt = 0;

int sys_delay = 5000;

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name: SysTick_Handler
Decription: Intterupt every 1ms
Input: None
Output: None
*******************************************************************************/
void SysTick_Handler(void)
{
	tick_cnt++; // every 1ms
    watchDogCnt++;
    
    
    if (sys_delay>0) sys_delay--;
    
    if (watchDogCnt >= 40)
    {
        watchDogCnt = 0;
        WWDG_SetCounter(127);
    }
    
    #ifdef USE_HMI
        mbPolling();
    #endif
    
    // ADC Average
    ADC_Average();
    
    if (tick_cnt % 10 == 0) // 100 ms
    {
        WindDir_Sampling();
    }
    
    if (tick_cnt % 1000 == 0)   // 1s
    {
        TIME_AddSec(&sysTime, 1);
        
        mainWDG++;
        
        if (mainWDG >= 60)
        {
            NVIC_SystemReset();
        }
        
        // WindSpeed Sample
        WindSpeed_Sampling();
        
        // Add log package
        addLogPackage();
    }
}

/*******************************************************************************
Function name: SysTick_Init
Decription: This function initialize SysTick Timebase.
Input: None
Output: None
*******************************************************************************/
void SysTick_Init(void)
{
    if (SysTick_Config(SystemCoreClock / SysTick_Freq))
    { 
        /* Capture error */ 
        while (1);
    }
    
    NVIC_SetPriority(SysTick_IRQn, 0);
}

uint32_t TIME_MS(uint32_t time)
{
    return time;
}

uint32_t TIME_SEC(uint32_t time)
{
    return time*1000;
}

/*******************************************************************************
Function name: delay_ms
Decription: Delay ms
Input: None
Output: None
*******************************************************************************/
void delay_ms(uint16_t time)
{
	Timeout_Type t_out;
    
    InitTimeout(&t_out, time);  
    while (CheckTimeout(&t_out));
}

void RINGBUF_DelayMs(void)
{
    delay_ms(1);
}

/*******************************************************************************
Function name: delay_us
Decription: Delay us
Input: None
Output: None
*******************************************************************************/
void delay_us(uint32_t time)
{
	time *= 33;	// 168 Mhz/5 = 14; 5 for while loop
	while (time--);
}

/*******************************************************************************
Function name: GetTickCount
Decription: Return current Tick Value
Input: None
Output: None
*******************************************************************************/
uint32_t GetTick(void)
{
	return tick_cnt;
}

/*******************************************************************************
Function name: InitTimeout
Decription: 
Input: None
Output:
*******************************************************************************/
void InitTimeout(Timeout_Type *t, uint32_t timeout)
{
	t->start_time = GetTick();
	t->timeout = timeout;
	t->crc = t->start_time + t->timeout;
}

/*******************************************************************************
Function name: CheckTimeout
Decription: 
Input: None
Output: 0: timeout
        1: time remain
*******************************************************************************/
uint32_t CheckTimeout(Timeout_Type *t)
{
	uint32_t u32temp, u32temp1;
    
	u32temp = t->start_time + t->timeout;
	if(u32temp != t->crc)
    {
			NVIC_SystemReset();   // reset system
    }
    
	u32temp = GetTick();
	t->crc = t->start_time + t->timeout;
	if(u32temp >= t->start_time)
		u32temp1 = u32temp - t->start_time;
	else
		u32temp1 = (0xFFFFFFFF - t->start_time) + u32temp;
	if(u32temp1 >= t->timeout) return 0;    // timeout
	return (t->timeout - u32temp1);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/

