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
#ifndef	__MBHANDLER_H
#define __MBHANDLER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#include "defines.h"

/* Private define ------------------------------------------------------------*/
#define REG_HOLDING_START   1
#define REG_HOLDING_NREGS   20

/* Extern variables ----------------------------------------------------------*/
 extern USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];
 
/* Private function prototypes -----------------------------------------------*/
void mbPolling(void);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
