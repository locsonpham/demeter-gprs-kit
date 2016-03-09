/**
  ******************************************************************************
  * @file    
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT_COMMAND_PARSER_H
#define __AT_COMMAND_PARSER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#include "defines.h"

#include "ringbuf.h"
#include "sys_time.h"
#include "system_config.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

#define GPRS_DATA_MAX_LENGTH	600
#define GPRS_KEEP_DATA_INFO_LENGTH	16
#define SOCKET_CLOSE	0
#define SOCKET_OPEN 	1

/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern uint8_t socketMask[SOCKET_NUM];

extern uint8_t createSocketNow;

extern uint32_t smsNewMessageFlag, ringFlag, tcpSocketStatus[SOCKET_NUM];

extern uint32_t tcpTimeReCheckData[SOCKET_NUM];

extern uint8_t DNS_serverIp[16];

extern uint8_t callerPhoneNum[16];

extern uint8_t inCalling;

extern uint8_t smsSender[18];

extern RINGBUF smsUnreadRingBuff;

//extern RINGBUF gprsRingBuff;

extern uint32_t GPRS_dataUnackLength[SOCKET_NUM];

extern uint8_t cellLocationBuf[128];
extern uint8_t cellLocationFlag;

extern uint32_t gprsRecvDataLen;
extern uint16_t gprsRecvFlag;

extern RINGBUF gprsRingBuff[SOCKET_NUM];

/* Private function prototypes -----------------------------------------------*/
/* GPRS & SMS */

/* END GPRS & SMS */

uint8_t RemaskSocket(uint8_t socket);
void AT_CmdProcessInit(void);
void AT_ComnandParser(char c);
void SMS_Manage(uint8_t *buff,uint32_t lengBuf);

/* read and solve data received from GPRS */
uint32_t GPRS_CmdRecvData(uint8_t *gprsBuff,uint16_t gprsBuffLen,uint32_t *dataLen);
uint32_t GPRS_SendCmdRecvData(uint8_t socketNum,uint8_t *gprsBuff,uint16_t gprsBuffLen,uint32_t *dataLen);
void AT_CommandCtl(void);
uint16_t CMD_CfgParse(char *buff,uint8_t *smsSendBuff,uint32_t smsLenBuf,uint16_t *dataOutLen,uint8_t pwdCheck);

uint8_t CellID_Parser(float *lat, float *lon, DATE_TIME *gsmtime);
#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
