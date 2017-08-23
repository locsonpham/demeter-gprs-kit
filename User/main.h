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
#ifndef	__MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "defines.h"

#include "systick.h"
#include "system_config.h"

/* Private define ------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern uint8_t etherOK;
extern uint8_t relayStt;
extern Timeout_Type tSend[SOCKET_NUM];
extern uint8_t main_started;
extern char* deviceID;
extern uint8_t sendPri;
extern Timeout_Type tSendSMS;
extern uint16_t resetCnt;

/* Private function prototypes -----------------------------------------------*/

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
