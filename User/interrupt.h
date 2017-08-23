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
#ifndef	__INTERRUPT_H
#define __INTERRUPT_H

/* Includes ------------------------------------------------------------------*/
#include "defines.h"

/* Private define ------------------------------------------------------------*/

#define SYSTICK_IRQ_Group   0

#define UART1_IRQ_Group     1
#define UART1_IRQ_Priority  0

#define UART2_IRQ_Group     1
#define UART2_IRQ_Priority  1

#define UART3_IRQ_Group     1
#define UART3_IRQ_Priority  2

#define UART6_IRQ_Group     2
#define UART6_IRQ_Priority  0

#define TIM3_IRQ_Group      3
#define TIM3_IRQ_Priority   15

#define TIM4_IRQ_Group      0
#define TIM4_IRQ_Priority   14

/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
