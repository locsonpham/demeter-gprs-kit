/**
  ******************************************************************************
  * @file    main.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__COM_H
#define __COM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "defines.h"

/* Private define ------------------------------------------------------------*/
#define MODBUS_MASTER_SLAVE 1
#define myMB_UART       UART5
#define myMB_Handler    UART5_IRQHandler
#define myMB_ADDR       0
#define myRegHolding    10

#define FUNC_READ   0
#define FUNC_WRITE  1

#define RW_RCC  RCC_AHB1Periph_GPIOD
#define RW_Port GPIOD
#define RW_Pin  GPIO_Pin_1

#define RW_H    GPIO_WriteBit(RW_Port, RW_Pin, Bit_SET)
#define RW_L    GPIO_WriteBit(RW_Port, RW_Pin, Bit_RESET)

#define myMB_SILENT 10


/* Extern variables ----------------------------------------------------------*/
extern uint16_t myRegBuf[myRegHolding];
 
/* Private function prototypes -----------------------------------------------*/
void myMB_Init(uint32_t baud);
void MODBUS_Handler(void);
uint8_t sendRequest(uint8_t addr, uint8_t func, uint8_t size, uint8_t startaddr);
void responseMessage(uint8_t addr, uint8_t func, uint8_t size, uint8_t startReg, uint8_t* buf);
void myMB_Process(void);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
