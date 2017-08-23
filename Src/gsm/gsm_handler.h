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
#ifndef __GSMHANDLER_H
#define __GSMHANDLER_H

#include "defines.h"

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SMS_Manage(uint8_t *buff,uint32_t lengBuf);
uint16_t CMD_CfgParse(char *buff, uint8_t *smsSendBuff, uint32_t smsLenBuf, uint16_t *dataOutLen, uint8_t pwdCheck);

#endif

/************************ END OF FILE ****/
