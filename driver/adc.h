/**
  ******************************************************************************
  * @file    adc.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__ADC_H
#define __ADC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)


/* Extern variables ----------------------------------------------------------*/
extern uint16_t ADC_Val[4];
extern uint16_t ADC_Med[4];

/* Private function prototypes -----------------------------------------------*/
void delay_adc(uint16_t num);
void ADC_Config(void);
void ADC_Average(void);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
