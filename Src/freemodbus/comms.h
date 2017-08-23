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
#ifndef	__COMMS_H
#define __COMMS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "defines.h"

/* Private define ------------------------------------------------------------*/
#define MODBUS_MASTER_SLAVE1    1
#define myMB_UART1              USART3
#define myMB_Handler1           USART3_IRQHandler
#define myMB_ADDR1              1
#define myRegHolding1           10

#define FUNC_READ1              0
#define FUNC_WRITE1             1

// #define RW_RCC  RCC_AHB1Periph_GPIOD
// #define RW_Port GPIOD
// #define RW_Pin  GPIO_Pin_1

// #define RW_H    GPIO_WriteBit(RW_Port, RW_Pin, Bit_SET)
// #define RW_L    GPIO_WriteBit(RW_Port, RW_Pin, Bit_RESET)

#define myMB_SILENT1 10


/* Extern variables ----------------------------------------------------------*/
extern uint16_t myRegBuf1[myRegHolding1];
 
/* Private function prototypes -----------------------------------------------*/
void myMB_Init1(uint32_t baud);
void MODBUS_Handler1(void);
uint8_t sendRequest1(uint8_t addr, uint8_t func, uint8_t size, uint8_t startaddr);
void responseMessage1(uint8_t addr, uint8_t func, uint8_t size, uint8_t startReg, uint8_t* buf);
void myMB_Process1(void);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
