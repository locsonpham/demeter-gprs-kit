/**
  ******************************************************************************
  * @file    template.h 
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
#include "stm32f4xx.h"

/* Private define ------------------------------------------------------------*/
#define AD2Voltage         3.315f/1256.0f // [(1/4096) * 3.315] / (47/147)
//#define AD2Voltage         3.315/4096 // [(1/4096) * 3.3] / (47/147)

#define ADCx                     ADC1
#define DMA_CHANNELx             DMA_Channel_0
#define DMA_STREAMx              DMA2_Stream0
#define ADCx_DR_ADDRESS          ((uint32_t)0x4001224C)

#define ADC0_CHANNEL             ADC_Channel_0
#define ADC0_PIN                 GPIO_Pin_0
#define ADC0_PORT                GPIOA

#define ADC1_CHANNEL             ADC_Channel_1
#define ADC1_PIN                 GPIO_Pin_1
#define ADC1_PORT                GPIOA

#define ADC2_CHANNEL             ADC_Channel_2
#define ADC2_PIN                 GPIO_Pin_2
#define ADC2_PORT                GPIOA

#define ADC3_CHANNEL             ADC_Channel_13
#define ADC3_PIN                 GPIO_Pin_3
#define ADC3_PORT                GPIOC

#define ADC4_CHANNEL             ADC_Channel_14
#define ADC4_PIN                 GPIO_Pin_4
#define ADC4_PORT                GPIOC

#define ADC5_CHANNEL             ADC_Channel_15
#define ADC5_PIN                 GPIO_Pin_5
#define ADC5_PORT                GPIOC

#define AIN0    0
#define AIN1    1
#define AIN2    2
#define AIN3    3   
#define AIN4    4
#define AIN5    5


/* Extern variables ----------------------------------------------------------*/
extern uint16_t ADC_Val[3];
extern uint64_t ADC_Sum[3];
extern uint16_t ADC_Med[3];
/* Private function prototypes -----------------------------------------------*/
void ADC_Config(void);
void ADC_Average(void);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
