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
#include "input.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
X_Input X000, X001, X002, X003, X004, X005;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void Input_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(X000_RCC | X001_RCC | X002_RCC | X003_RCC | X004_RCC | X005_RCC, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = X000_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(X000_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = X001_Pin;
    GPIO_Init(X001_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = X002_Pin;
    GPIO_Init(X002_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = X003_Pin;
    GPIO_Init(X003_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = X004_Pin;
    GPIO_Init(X004_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = X005_Pin;
    GPIO_Init(X005_Port, &GPIO_InitStruct);
    
    // attach X
    X000.port = X000_Port;
    X000.pin  = X000_Pin;
    
    X001.port = X001_Port;
    X001.pin  = X001_Pin;
    
    X002.port = X002_Port;
    X002.pin  = X002_Pin;
   
    X003.port = X003_Port;
    X003.pin  = X003_Pin;
    
    X004.port = X004_Port;
    X004.pin  = X004_Pin;
    
    X005.port = X005_Port;
    X005.pin  = X005_Pin;
}

uint8_t Input_State(X_Input* x)
{
    return GPIO_ReadInputDataBit(x->port, x->pin);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
