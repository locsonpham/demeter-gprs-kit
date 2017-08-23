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
#include "gsm_handler.h"

#include "systick.h"
#include "modem.h"
#include "at_command_parser.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// uint8_t mainBuf[768];
// uint32_t receiveLen;
// uint8_t flagCalling = 0;

uint8_t smsSendMessageFlag = 1;
uint8_t smsAddr[16];
uint8_t smsMsg[160];

// Timeout_Type tCheck;
// uint8_t modemAvailableCnt = 0;

// GPRS
// uint8_t gprsTryNum = 5;

// Timeout type
// Timeout_Type tMODEMInit;
// Timeout_Type tMODEM_AvailableCheck;
// Timeout_Type tGPRSInit;
// Timeout_Type tGetCellLoc;
// Timeout_Type tSMSCheckTimeout;
// Timeout_Type tSendSMS;

/* Private function prototypes -----------------------------------------------*/
// void GSM_Handler(void);
// void GPRS_Handler(uint8_t task);
// void Cell_Handler(void);
void Socket_DataComingHandler(uint8_t socket, int length);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  GPRS Handler
  *         Connect socket, send/receive data
  * @param  None
  * @retval None
  */
// void GSM_Handler(void)
// {
//     // Init MODEM (if not initalized yet)
//     if ( (modemInitFlag == 1) && (CheckTimeout(&tMODEMInit) == 0))
//     {
//         if (MODEM_Init() == 0)
//         {
//             MODEM_Info("MODEM Init OK\r\n");
//             modemInitFlag = 0;  // Init done -> Clear Flag
//             modemAvailableCnt = 0;
//             InitTimeout(&tMODEM_AvailableCheck, TIME_SEC(60));
//             
//             #ifdef USE_GPRS
//             // Start GPRS
//             gprsStatus = 0;
//             gprsInitFlag = 1;
//             InitTimeout(&tGPRSInit, TIME_SEC(1));
//             
//             // Close all sockets
//             for (int i = 0; i < SOCKET_NUM; i++)
//             {
//                 socketMask[i] = 0xff;
//                 tcpSocketStatus[i] = SOCKET_CLOSE;
//             }
//             #endif
//             
//             #ifdef USE_CELLLOC
//             // Start CellLocation
//             cellStatus = 0;
//             cellInitFlag = 1;
//             InitTimeout(&tGetCellLoc, TIME_SEC(10));
//             #endif
//         }
//         else
//         {
//             modemStatus = 0;    // MODEM Init Fail
//             gprsStatus = 0;
//             cellStatus = 0;
//             
//             // ReInit after while
//             MODEM_Info("MODEM Init FAIL\r\n");
//             InitTimeout(&tMODEMInit, TIME_SEC(5));
//         }
//     }
//     
// //     // Check MODEM available (not halt)
// //     if (modemStatus == 1 && CheckTimeout(&tMODEM_AvailableCheck) == 0)
// //     {
// //         if(MODEM_SendCommand(modemOk, modemError, 5000, 3, "AT\r") == 0)
// //         {
// //             modemAvailableCnt = 0;  // MODEM Ready
// //         }
// //         else
// //         {
// //             modemAvailableCnt++;
// //         }
// //         
// //         InitTimeout(&tMODEM_AvailableCheck, TIME_SEC(60));
// //         
// //         if (modemAvailableCnt >= 3) // Already Stop => ReInit
// //         {
// //             SIM900_HardReset();
// //             modemStatus = 0;
// //             modemInitFlag = 1;
// //             
// //             InitTimeout(&tMODEMInit, TIME_SEC(5));
// //         }
// //     }
//     
//     /* SMS & CALL Manage --------------------------------------------------*/
//     if (modemStatus)    // GSM READY -> SMS READY
//     {
//         if (CheckTimeout(&tSendSMS) == TIMEOUT)
//         {
//         }
//         
//         
//         /*---- SMS CHECK ----------------------------------*/
//         if(smsNewMessageFlag || (CheckTimeout(&tSMSCheckTimeout) == TIMEOUT))
//         {
//             SMS_Manage(mainBuf, sizeof(mainBuf));
//             smsNewMessageFlag = 0;
//             InitTimeout(&tSMSCheckTimeout, TIME_SEC(30));
//         }
//         /*---- END SMS CHECK ------------------------------*/
//         
//         /*---- CALL MANAGE --------------------------------*/
//         if (ringFlag)  // Receive call
//         {
//             MODEM_Info("MAIN: CALLER->%s\r\n", callerPhoneNum);
//             ringFlag = 0;
//             CallingProcess(0);
//         }
//         
//         // MAKE CALL
//         if(flagCalling == 1)   // Step1: Make call and wait
//         {
//             inCalling = 0;
//          
//             if(VoiceSetup() == 0)
//             {
// //                 sprintf((char *)mainBuf,"ATD%s;\r",sysCfg.whiteCallerNo);
// //                 COMM_Puts(mainBuf);
// //                 
// //                 if(MODEM_CheckResponse("OK",5000) == 0)
// //                 {
// //                     flagCalling = 2;
// //                     inCalling = 1;
// //                 }
//             }
//         }
//         else if(inCalling && (flagCalling == 0))        // Cancel call
//         {
//             if(CallingProcess(0) == 0)
//             {
//                 inCalling = 0;
//             }
//         }
//         else if((inCalling == 0) && (flagCalling == 2))  // Busy - no answer
//         {
//              flagCalling = 0;
//         }
//         /*---- END CALL MANAGE ------------------------------*/
//     }
// }

/**
  * @brief  GPRS Handler
  * 
  * @param  None
  * @retval None
  */

// void GPRS_Handler(uint8_t task)
// {
//     if ( (modemStatus == 1) && (gprsInitFlag == 1) && (CheckTimeout(&tGPRSInit) == TIMEOUT))  // Init GPRS
//     {
//         if (MODEM_EnableGPRS() == 0)    // Init OK
//         {
//             MODEM_Info("GPRS OK\r\n");
//             gprsStatus = 1;
//             gprsInitFlag = 0;
//             gprsTryNum = 5;
//         }
//         else
//         {
//             // Reinit GPRS if fails
//             InitTimeout(&tGPRSInit, TIME_INITGPRS);
//             gprsTryNum--;
//         }
//     }
//     
//     if (gprsTryNum == 0)    // Can't init GPRS, reinit GSM
//     {
//         modemStatus = 0;
//         gprsInitFlag = 0;
//         cellStatus = 0;
//         modemInitFlag = 1;
//         InitTimeout(&tMODEMInit, TIME_SEC(5));
//     }
//     
//     if (gprsStatus && inCalling == 0) // GPRS Ready
//     {       
//     }
// }

/**
  * @brief  GPRS Data Control
  * @param  
  * @param  
  * @retval 
  */
// void GPRS_DataControl(void)
// {
//     if (gprsStatus)
//     {
//         if (gprsRecvFlag)
//         {
//             uint8_t i;
//             uint16_t length;
//             uint8_t c;
//             uint8_t tmpBuf[512];
//             
//             gprsRecvFlag = 0;
//             length = gprsRecvDataLen;
//             
//             // Check data in which socket
//             for (i = 0; i < SOCKET_NUM; i++)
//             {
//                 if (socketRecvFlag[i])
//                 {
//                     Socket_DataComingHandler(i, length);
//                 }
//             }
//         }
//     }
// }

/**
  * @brief  Process data coming socket
  * @param  
  * @param  
  * @retval 
  */
void Socket_DataComingHandler(uint8_t socket, int length)
{
    
}

/**
  * @brief  Cellular Location Handler
  * @param  
  * @param  
  * @retval 
  */
// void Cell_Handler(void)
// {
//     if (modemStatus == 1 && CheckTimeout(&tGetCellLoc) == 0)
//     {
//         static uint8_t cellInitTry = 5;
//         
//         if (cellStatus == 0)  // not init yet
//         {
//             if (MODEM_CellInit() == 0)    // init OK
//             {
//                 cellStatus = 1;
//             }
//             else
//             {
//                 cellInitTry--;
//                 InitTimeout(&tGetCellLoc, TIME_SEC(10));
//             }
//             
//             if (cellInitTry == 0)
//             {
//                 modemStatus = 0;
//                 modemInitFlag = 1;
//             }
//         }
//         
//         if (cellStatus)   // ready get CellID
//         {
//             if (MODEM_GetCellLoc() == 0)
//             {
//                 if (cellLocationFlag)
//                 {
//                     float lat, lon;
//                     DATE_TIME time;
//                     
//                     if (CellLoc_Parser(&cellLocate.lat, &cellLocate.lon, &cellLocate.time) == 0)
//                     {
//                         if (cellLocate.lat == 0.0 && cellLocate.lon == 0.0)
//                         {
//                             cellLocate.avaiable = 0;
//                             INFO("GPS INVALID\n");
//                         }
//                         else
//                         {
//                             cellLocate.avaiable = 1;
//                             TIME_FromGsm(&sysTime, &cellLocate.time);
//                             
//                             // Update time for GSM
// //                             char buff[64];
// //                             sprintf(buff, "AT+CCLK=\"%02d/%02d/%02d,%02d:%02d:%02d+28\"\r", sysTime.year%100, sysTime.month, sysTime.mday, sysTime.hour, sysTime.min, sysTime.sec);
// //                             COMM_Puts(buff);
//                             
//                             INFO("Cellular Location OK\n");
//                         }
//                     }
//                     else
//                     {
//                         cellLocate.avaiable = 0;
//                     }
//                 }
//             }
//             
//             InitTimeout(&tGetCellLoc, TIME_GETCELLINFO);
//         }
//     }
// }

/**
  * @brief  SMS Manage
  * @param  
  * @param  
  * @retval 
  */
void SMS_Manage(uint8_t *buff, uint32_t lengBuf)
{
	Timeout_Type t;
	uint8_t tmpBuf[32], c;	
	uint16_t smsLoc, len, i, res;
    
	// read the newly received SMS
	INFO("\r\nSMS->CHECK ALL\r\n");
	COMM_Puts("AT+CMGL=\"ALL\"\r");
	delay_ms(1000);
    
	while(RINGBUF_GetFill(&smsUnreadRingBuff) >=2)
	{

		RINGBUF_Get(&smsUnreadRingBuff,&c);
		smsLoc = c;
		RINGBUF_Get(&smsUnreadRingBuff,&c);
		smsLoc |= (((uint16_t)c) << 8 & 0xff00);
        
		// read the newly received SMS
		INFO("\r\nSMS->READ SMS\r\n");
		flagSmsReadFinish = 0;
		sprintf((char *)tmpBuf, "AT+CMGR=%d\r", smsLoc);
		COMM_Puts(tmpBuf);
		InitTimeout(&t,TIME_SEC(1));//1s
        
		while(!flagSmsReadFinish)
		{
			if(CheckTimeout(&t) == TIMEOUT) 
			{
				break;
			}
		}
        
		smsBuf[sizeof(smsBuf) - 1] = 0;
		INFO("\n\rSMS->PHONE:%s\n\r", smsSender);	
		INFO("\r\nSMS->DATA:%s\r\n", smsBuf);
		// delete just received SMS
		INFO("\n\rSMS->DELETE:%d\n\r",smsLoc);
		sprintf((char *)tmpBuf, "AT+CMGD=%d\r", smsLoc);
		COMM_Puts(tmpBuf);
		MODEM_CheckResponse("OK", 1000);	
		if(flagSmsReadFinish == 0) continue;
		len = 0;
		res = CMD_CfgParse((char *)smsBuf, buff, lengBuf, &len, 1);
		if(len >= 160)
		{
			for(i = 0;i < len; i += 160)
			{
				delay_ms(5000);
				SendSMS(smsSender,&buff[i]);
			}
		}
		else if(len)
		{
			SendSMS(smsSender,buff);
		}
		if(res == 0xa5)
        {
            //NVIC_SystemReset();
        }
	}
}

/**
  * @brief  SMS CMD Parse
  * @param  
  * @param  
  * @retval 
  */
uint16_t CMD_CfgParse(char *buff, uint8_t *smsSendBuff, uint32_t smsLenBuf, uint16_t *dataOutLen, uint8_t pwdCheck)
{
    char *pt, *u8pt, tempBuff0[32], tempBuff1[16], tempBuff2[16], tempBuff3[16], tempBuff4[16], flagCfgSave = 0;
	uint32_t i,t1,t2,t3,t4,t5,t6,len,error,flagResetDevice = 0;
	DATE_TIME t;
	uint8_t cmdOkFlag = 0;
	
	len = 0;
	
	//check password
	if (pwdCheck)
	{	
		if((strstr(buff,(char *)sysCfg.smsPwd) != NULL) || (strstr(buff,"ZOTA") != NULL))
		{
				INFO("\n\rSMS->PASSWORD OK\n\r");
		}
		else
		{
				INFO("\n\rSMS->PASSWORD FAILS\n\r");
				return 1;
		}
	}
    
	// Check monney
	pt = strstr(buff,"*101#");
	if(pt != NULL)
	{
		delay_ms(1000);
		COMM_Puts("ATD*101#\r");
		delay_ms(1000);
		if(!MODEM_CheckResponse("+CUSD:", 5000) == 0) return 1;
		delay_ms(1000);
		while(RINGBUF_Get(&commBuf, &smsSendBuff[len])==0)
		{
			len++;
			if(len >= 160) break;
		}
		smsSendBuff[len] = 0;
		cmdOkFlag = 1;
	}
    
	//check monney
	pt = strstr(buff,"*102#");
	if(pt != NULL)
	{
		delay_ms(1000);
		COMM_Puts("ATD*102#\r");
		delay_ms(1000);
		if(!MODEM_CheckResponse("+CUSD:", 5000) == 0) return 1;
		delay_ms(1000);
		i = 0;
		while(RINGBUF_Get(&commBuf, &smsSendBuff[len + i])==0)
		{
			i++;
			if(i >= 160) break;
		}
		len += i;
		smsSendBuff[len] = 0;
		cmdOkFlag = 1;
	}
    
	// Charge monney
	pt = strstr(buff,"*100*");
	if(pt != NULL)
	{
		sprintf((char *)smsSendBuff,"ATD%s\r",pt);
		delay_ms(1000);
		COMM_Puts(smsSendBuff);
		if(!MODEM_CheckResponse("+CUSD:", 10000) == 0) return 1;
		delay_ms(1000);
		i = 0;
		while(RINGBUF_Get(&commBuf, &smsSendBuff[len + i])==0)
		{
			i++;
			if(i >= 160) break;
		}
		len += i;
		smsSendBuff[len] = 0;
		cmdOkFlag = 1;
	}
    
	//Reset device
	flagResetDevice = 0;
	pt = strstr(buff,"DMCFG,RESET");
	if(pt != NULL)
	{
		flagResetDevice = 1;
		INFO("\n\rSMS->RESET DEVICE\\n\r");
		len += sprintf((char *)&smsSendBuff[len], "DEVICE RESETING...\n");
		cmdOkFlag = 1;
	}

	if(len >= smsLenBuf)	len = smsLenBuf;
	
	smsSendBuff[len] = 0;
	*dataOutLen = len;
	if(flagCfgSave)
	{
// 		CFG_Save();
	}
    
	if(flagResetDevice)
	{
		return 0xa5;
	}
    
	return 0;
}
/************************ END OF FILE ****/
