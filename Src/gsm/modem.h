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
#ifndef __MODEM_H
#define __MODEM_H

#include "stm32f4xx.h"

#include "defines.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include "sim900.h"
#include "sys_time.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define commBuf UART1_RxRingBuff

#ifdef DEBUG_GSM
#define MODEM_Info(...)		printf(__VA_ARGS__)
#define MODEM_Debug(...) //DbgCfgPrintf(__VA_ARGS__)
#else
#define MODEM_Info(...)
#define MODEM_Debug(...)
#endif

#define MODEM_INIT_OK			0
#define MODEM_SIM_CARD_ERROR 	0xf1
#define MODEM_CPOS_ERROR 		0xf2
#define MODEM_UPSDA_3_ERROR 	0xf3

typedef struct _cellLocate
{
    DATE_TIME time;     /**< UTC of position */
    double  lat;        /**< Latitude in NDEG - [degree][min].[sec/60] */
    double  lon;        /**< Longitude in NDEG - [degree][min].[sec/60] */
    double  speed;      /**< Speed over the ground in knots */
    double  dir;        /**< Track angle in degrees True */
    double  uncertainty; 
    double alt;
    char ew;
    char ns;
    char avaiable;
} cellLocateType;

extern cellLocateType cellLocate;

/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern uint8_t modemId[16];
extern const uint8_t *modemOk;
extern const uint8_t *modemError;
extern uint32_t batteryPercent;
extern uint8_t gsmSignal;
extern uint8_t csqValue;

extern uint8_t modemInitFlag;
extern volatile uint8_t modemStatus;

extern uint8_t gprsInitFlag;
extern volatile uint8_t gprsStatus;

extern uint8_t cellInitFlag;
extern volatile uint8_t cellStatus;

extern uint8_t simInserted;

/* Private function prototypes -----------------------------------------------*/
uint8_t MODEM_Init(void);
uint8_t MODEM_Sleep(void);
uint8_t MODEM_SleepCMD(void);
uint8_t InternetSetProfiles(uint8_t *apn,uint8_t *usr,uint8_t *pwd);
uint8_t MODEM_EnableGPRS(void);
uint8_t MODEM_DisableGPRS(void);
uint8_t MODEM_ConnectSocket(uint8_t socket, uint8_t *addr, uint16_t port, uint8_t useIp);
uint8_t MODEM_CloseSocket(uint8_t socket);
uint8_t MODEM_GprsSendData(uint8_t socket, uint8_t *data, uint32_t length);
uint16_t MODEM_CheckGPRSDataOut(uint8_t socket);
static void GetIMEI(uint8_t *buf, uint16_t bufSize);
uint8_t InternetSetProfiles(uint8_t *apn,uint8_t *usr,uint8_t *pwd);
uint8_t MODEM_SendCommand(const uint8_t *resOk,const uint8_t *resFails,uint32_t timeout,uint8_t tryAgainNum,const uint8_t *format, ...);
uint8_t MODEM_CheckResponse(uint8_t *str, uint32_t t);
uint8_t SendSMS(const uint8_t *recipient, const uint8_t *data);

uint32_t MODEM_CheckAccount(void);

uint8_t MODEM_GetCBC(void);
uint8_t MODEM_GetCSQ(void);

uint8_t MODEM_CellInit(void);
uint8_t MODEM_GetCellLoc(void);
uint8_t ParserCellLocate(void);
uint8_t CallingProcess(uint8_t callAction);

void CFG_Save(void);
void CFG_Load(void);

uint8_t VoiceSetup(void);
#endif

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
