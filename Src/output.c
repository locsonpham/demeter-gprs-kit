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
#include "output.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Y_Out Y000, Y001, Y002, Y003, Y004, Y005, Y006, Y007, Y008, Y009, Y010, Y011, Y012, Y013, Y014;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void Output_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(Y000_RCC | Y001_RCC | Y002_RCC | Y003_RCC | Y004_RCC | Y005_RCC | Y006_RCC |
                           Y007_RCC | Y008_RCC | Y009_RCC | Y010_RCC | Y011_RCC | Y012_RCC | Y013_RCC, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = Y000_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(Y000_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y001_Pin;
    GPIO_Init(Y001_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y002_Pin;
    GPIO_Init(Y002_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y003_Pin;
    GPIO_Init(Y003_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y004_Pin;
    GPIO_Init(Y004_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y005_Pin;
    GPIO_Init(Y005_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y006_Pin;
    GPIO_Init(Y006_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y007_Pin;
    GPIO_Init(Y007_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y008_Pin;
    GPIO_Init(Y008_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y009_Pin;
    GPIO_Init(Y009_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y010_Pin;
    GPIO_Init(Y010_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y011_Pin;
    GPIO_Init(Y011_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y012_Pin;
    GPIO_Init(Y012_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y013_Pin;
    GPIO_Init(Y013_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = Y014_Pin;
    GPIO_Init(Y014_Port, &GPIO_InitStruct);
    
    // attach output
    Y000.port = Y000_Port;
    Y000.pin  = Y000_Pin;
    
    Y001.port = Y001_Port;
    Y001.pin  = Y001_Pin;
    
    Y002.port = Y002_Port;
    Y002.pin  = Y002_Pin;
    
    Y003.port = Y003_Port;
    Y003.pin  = Y003_Pin;
    
    Y004.port = Y004_Port;
    Y004.pin  = Y004_Pin;
   
    Y005.port = Y005_Port;
    Y005.pin  = Y005_Pin;
    
    Y006.port = Y006_Port;
    Y006.pin  = Y006_Pin;
    
    Y007.port = Y007_Port;
    Y007.pin  = Y007_Pin;
    
    Y008.port = Y008_Port;
    Y008.pin  = Y008_Pin;
    
    Y009.port = Y009_Port;
    Y009.pin  = Y009_Pin;
    
    Y010.port = Y010_Port;
    Y010.pin  = Y010_Pin;
    
    Y011.port = Y011_Port;
    Y011.pin  = Y011_Pin;
    
    Y012.port = Y012_Port;
    Y012.pin  = Y012_Pin;
    
    Y013.port = Y013_Port;
    Y013.pin  = Y013_Pin;
    
    Y014.port = Y014_Port;
    Y014.pin  = Y014_Pin;
}

void Output_High(Y_Out* y)
{
    GPIO_SetBits(y->port, y->pin);
}

void Output_Low(Y_Out* y)
{
    GPIO_ResetBits(y->port, y->pin);
}
uint8_t Output_State(Y_Out* y)
{
    return GPIO_ReadOutputDataBit(y->port, y->pin);
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
