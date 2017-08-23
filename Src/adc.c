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
#include "adc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t ADC_Val[3];
uint64_t ADC_Sum[3];
uint16_t ADC_Med[3];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void ADC_Config(void)
{
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;

    /* Enable ADCx, DMA and GPIO clocks ****************************************/ 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    
    /* Configure ADC1 Channelx to Channelx pin as analog input ******************************/
    GPIO_InitStructure.GPIO_Pin = ADC0_PIN | ADC1_PIN | ADC2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* DMA2 Stream0 channel2 configuration **************************************/
    DMA_DeInit(DMA2_Stream0);
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_Val;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = 3;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /* ADC Common Init **********************************************************/
    ADC_DeInit();
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC1 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
//     ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 3;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channel10 to channel15 configuration *************************************/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_28Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_28Cycles);

    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);
    
    /* Start ADC Software Conversion */ 
    ADC_SoftwareStartConv(ADC1);
}

void ADC_Average(void)
{
    static int adc_cnt = 0;
    
    if (adc_cnt == 100)
    {
        for (uint8_t i = 0; i < 3; i++)
        {
            ADC_Med[i] = (uint16_t)(ADC_Sum[i] / adc_cnt);
            ADC_Sum[i] = 0;
        }
        
        adc_cnt = 0;
    }
    
    adc_cnt++;
    
    for (uint8_t i = 0; i < 3; i++)
    {
        ADC_Sum[i] += ADC_Val[i];
    }
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
