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
#include "at_command_parser.h"

#include "defines.h"

#include "systick.h"
#include "modem.h"
#include "sys_time.h"

#include <string.h>
#include "system_config.h"
// #include "sst25.h"

/* Private typedef -----------------------------------------------------------*/

/* Command definitions structure. */
typedef struct scmd {
   char cmdInfo[16];
   char (*func)(char c);
} SCMD;

enum{
	AT_CMD_NEW_STATE,
	AT_CMD_WAIT_FINISH,
	AT_CMD_FINISH
} AT_cmdState = AT_CMD_FINISH;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#define CMD_COUNT   		(sizeof (AT_ProcessCmd) / sizeof (AT_ProcessCmd[0]))
#define AT_CMD_PARSER_SIZE	16

const uint8_t terminateStr[7] = "\r\nOK\r\n";

uint8_t cellLocationBuf[128];   // not use in SIM900 (16/7/15 now accept)
uint8_t cellLocationFlag = 0;   // not use in SIM900 (now OK)
uint8_t cellLength = 0;

char GetCallerNumber(char c);
char GetCallerNumber2(char c);
char RingCallProcess(char c);
char SMS_NewMessage(char c);
char SMS_Process(char c);
char GPRS_GetLength(char c);
char GPRS_GetData(char c);
char CLOSE_TCP_Process(char c);
char Parse(char *buff, uint8_t *smsSendBuff,uint32_t smsLenBuf);
char GPRS_CloseSocket(char c);
char GPRS_CloseSocket1(char c);
char GPRS_SocketControl(char c);
char SMS_ReadMsg(char c);
char CellLocateParser(char c);
char GPRS_CreateSocket(char c);
char CallBusy(char c);
char GPRS_GetDNS(char c);
char GetBatteryStatus(char c);
char GPRS_DeActive(char c);
char CellLoc_Process(char c);
char CellLoc_DeActive(char c);
char GetModemClock(char c);
char Socket0Close(char c);
char Socket1Close(char c);
char Socket2Close(char c);
char Socket3Close(char c);
char Socket4Close(char c);
char Socket5Close(char c);
char Socket6Close(char c);
char Socket7Close(char c);

static const SCMD AT_ProcessCmd[] = {
	"+CLIP: \"", GetCallerNumber,
 	"RING" , RingCallProcess,
    "+CMGL: ", SMS_Process,
	"+CMTI:", SMS_NewMessage,
	"+RECEIVE,", GPRS_GetData,  // get data from gprs
	//"+CIPSEND: ", GPRS_CloseSocket,
    "+PDP: DEACT", GPRS_DeActive,
	"+CMGR: ", SMS_ReadMsg,
	"BUSY", CallBusy,
	"NO ANSWER", CallBusy,
	"NO CARRIER", CallBusy,
	"+CMTI: \"SM\",", SMS_Process,
	"+CMTI: \"ME\",", SMS_Process,
	"+CBC: ", GetBatteryStatus,
    
    // Cell Locate (only for SIM900)
    "+CIPGSMLOC: ", CellLoc_Process,
    "+SAPBR 1: DEACT", CellLoc_DeActive,
    
    // Time SIM900
    "+CCLK: ", GetModemClock,
    "0, CLOSED", Socket0Close,
    "1, CLOSED", Socket1Close,
    "2, CLOSED", Socket2Close,
    "3, CLOSED", Socket3Close,
    "4, CLOSED", Socket4Close,
    "5, CLOSED", Socket5Close,
    "6, CLOSED", Socket6Close,
    "7, CLOSED", Socket7Close
};

uint32_t cmdCnt[CMD_COUNT];
uint8_t numCmd;

uint8_t callerPhoneNum[16];

uint8_t smsBuf[255];
uint8_t smsSender[18];
uint32_t smsCnt = 0, smsNewMessageFlag = 0, ringFlag = 0;
uint8_t inCalling = 0;

uint8_t smsUnreadBuff[32] = {0};
RINGBUF smsUnreadRingBuff;

uint8_t *gprsRecvPt;
uint32_t gprsRecvDataLen;
uint16_t gprsRecvFlag = 0;
uint8_t  socketRecvFlag[SOCKET_NUM] = {0};
uint8_t gprsLengthBuff[SOCKET_NUM][GPRS_KEEP_DATA_INFO_LENGTH] = {0};
RINGBUF gprsRingLengthBuff[SOCKET_NUM];

RINGBUF gprsRingBuff[SOCKET_NUM];
uint8_t gprsRxBuff[SOCKET_NUM][GPRS_DATA_MAX_LENGTH];

uint32_t gprsDataOffset = 0, tcpSocketStatus[SOCKET_NUM] = {SOCKET_CLOSE};
uint32_t GPRS_dataUnackLength[SOCKET_NUM];
uint8_t socketMask[SOCKET_NUM] = {0xff};
uint8_t createSocketNow = 0;
uint32_t AT_cmdRecvLength;
uint32_t socketRecvEnable = 0;
uint32_t socketNo = 0;
uint32_t tcpTimeReCheckData[SOCKET_NUM] = {0xffffffff};
uint8_t DNS_serverIp[16];

uint8_t str_split(char* s1, char* s2, char* buffer[])
{
    int cnt = 0;
    
    buffer[0] = strtok (s1 , s2);
    while (buffer[cnt] != NULL)
    {
        // increase cnt
        cnt++;
        buffer[cnt] = strtok (NULL, s2);
    }
    
    return cnt;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Init buffer for AT Command parser
  * @param         
  * @param
  * @retval
  */
void AT_CmdProcessInit(void)
{
	uint32_t i;
    
    // Init buffer for SMS
	RINGBUF_Init(&smsUnreadRingBuff, smsUnreadBuff, sizeof(smsUnreadBuff));
    
    // Init buffer for GPRS data
	for(i = 0;i < SOCKET_NUM; i++)
	{
		RINGBUF_Init(&gprsRingBuff[i], gprsRxBuff[i], GPRS_DATA_MAX_LENGTH);
	}
}

/**
  * @brief  End call by resetting inCalling flag 
  * @param         
  * @param
  * @retval
  */
char CallBusy(char c)
{
	inCalling = 0;
    return 0;
}

/**
  * @brief get battery percent and update batteryPercent
  * @param         
  * @param
  * @retval
  */
/*
+CBC: 0,46,2300
*/

char GetBatteryStatus(char c)
{
	static uint32_t level = 0;
	switch(AT_cmdRecvLength)
	{
		case 0:
		case 1:
			break;
		default:
			if(AT_cmdRecvLength >= 6) 
			{
				level = 0;
				return 0;
			}
            
			if((c >= '0') && (c <= '9'))
			{
				level *= 10;
				level += c - '0';
			}
			else
			{
				if(level > 0 && level < 100)
					batteryPercent = level;
				level = 0;
				return 0;
			}
			break;
	}
	return 0xff;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
char CellLoc_Process(char c)
{
    cellLocationBuf[AT_cmdRecvLength] = c;
	
	if(c == '\r')
	{
		cellLocationBuf[AT_cmdRecvLength + 1] = 0;
		cellLocationFlag = 1;
        cellLength = AT_cmdRecvLength;
		return 0;
	}
	
    return 0xff;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
/*
+CIPGSMLOC: 0,106.728612,10.830515,2015/07/16,06:28:29
*/
uint8_t CellLoc_Parser(double *lat, double *lon, DATE_TIME *gsmtime)
{
    char* str;
    char* sbuf[10];
    uint8_t seg_cnt = 0;
    uint8_t i = 0;
    
    uint8_t length = sizeof(cellLocationBuf);
    
    if (length < 5) // no data (+CIPGSMLOC: 601\r)
    {
        cellStatus = 0;
        return 0xff;
    }

    // cap phat vung nho cho chuoi
    str = malloc(sizeof(char) * length);

    // convert frame to string (char*)
    for (i = 0; i < length; i++)
    {
        *(str+i) = (char)cellLocationBuf[i];
    }

    //INFO("GSM LOC: %s\n", str);

    // split string
    seg_cnt = str_split(str, ",", sbuf);
   // printf("%d\n", seg_cnt);
    
    if (seg_cnt < 5) return 0xff;
    
    // Get lat, lon
    *lat = atof(sbuf[2]);
    *lon = atof(sbuf[1]);

    // Get date time
    gsmtime->year = (sbuf[3][0] - 0x30) * 1000 + (sbuf[3][1] - 0x30) * 100 + (sbuf[3][2] - 0x30) * 10 + (sbuf[3][3] - 0x30);
    gsmtime->month = (sbuf[3][5] - 0x30) * 10 + (sbuf[3][6] - 0x30);
    gsmtime->mday = (sbuf[3][8] - 0x30) * 10 + (sbuf[3][9] - 0x30);
    
    gsmtime->hour = (sbuf[4][0] - 0x30) * 10 + (sbuf[4][1] - 0x30);
    gsmtime->min = (sbuf[4][3] - 0x30) * 10 + (sbuf[4][4] - 0x30);
    gsmtime->sec = (sbuf[4][6] - 0x30) * 10 + (sbuf[4][7] - 0x30);

    // Free memory local    IMPORTANT
    free(str);
    
    return 0;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
char CellLoc_DeActive(char c)
{
    cellStatus = 0;
    return 0;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
char GetModemClock(char c)
{
    return 0;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
char Socket0Close(char c)
{
    socketMask[0] = 0xff;
    tcpSocketStatus[0] = SOCKET_CLOSE;
    
    return 0;
}

char Socket1Close(char c)
{
    socketMask[1] = 0xff;
    tcpSocketStatus[1] = SOCKET_CLOSE;
    
    return 0;
}

char Socket2Close(char c)
{
    socketMask[2] = 0xff;
    tcpSocketStatus[2] = SOCKET_CLOSE;
    
    return 0;
}

char Socket3Close(char c)
{
    socketMask[3] = 0xff;
    tcpSocketStatus[3] = SOCKET_CLOSE;
    
    return 0;
}

char Socket4Close(char c)
{
    socketMask[4] = 0xff;
    tcpSocketStatus[4] = SOCKET_CLOSE;
    
    return 0;
}

char Socket5Close(char c)
{
    socketMask[5] = 0xff;
    tcpSocketStatus[5] = SOCKET_CLOSE;
    
    return 0;
}

char Socket6Close(char c)
{
    socketMask[6] = 0xff;
    tcpSocketStatus[6] = SOCKET_CLOSE;
    
    return 0;
}

char Socket7Close(char c)
{
    socketMask[7] = 0xff;
    tcpSocketStatus[7] = SOCKET_CLOSE;
    
    return 0;
}

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
/*
	+UULOC: 27/09/2012,18:26:13.363,21.0213870,105.8091050,0,127,0,0 
*/

char CellLocateParser(char c)
{
	
// 	cellLocationBuf[AT_cmdRecvLength] = c;
// 	if(c == '\r')
// 	{
// 		cellLocationBuf[AT_cmdRecvLength + 1] = 0;
// 		cellLocationFlag = 1;
// 		return 0;
// 	}
	return 0xff;
}

/**
  * @brief Get caller number, update callerPhoneNum
  * @param         
  * @param
  * @retval
  */
/*
+CLIP: "0978779222",161,,,,0
*/
char GetCallerNumber(char c)
{
	if((c >= '0') && (c <= '9'))
		callerPhoneNum[AT_cmdRecvLength] = c;
	else 
	{
		ringFlag = 1;
		return 0;
	}
 return 0xff;
}

/*
+CLIP:+1655236473
*/
char GetCallerNumber2(char c)
{
	if((c >= '0') && (c <= '9'))
		callerPhoneNum[AT_cmdRecvLength] = c;
	else 
	{
		ringFlag = 1;
		return 0;
	}
 return 0xff;
}

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
char RingCallProcess(char c)
{
	return 0;
}

/**
  * @brief new message coming, turn on flag
  * @param         
  * @param
  * @retval
  */
char SMS_NewMessage(char c)
{
	smsNewMessageFlag = 1;
	return 0;
}
/*
AT+USOCTL=0,11 +USOCTL: 0,11,2501
*/
char GPRS_SocketControl(char c)
{
// 	static uint32_t length = 0;
// 	switch(AT_cmdRecvLength)
// 	{
// 		case 0:
// 				socketNo = RemaskSocket((c - '0'));
// 				length = 0;
// 				if(socketNo >= SOCKET_NUM)
// 				{
// 					return 0;
// 				}
// 			break;
// 		case 1:
// 		case 2:
// 		case 3:
// 		case 4:
// 			break;
// 		default:
// 			if(AT_cmdRecvLength >= 9) 
// 			{
// 				return 0;
// 			}
// 			if((c >= '0') && (c <= '9'))
// 			{
// 				length *= 10;
// 				length += c - '0';
// 			}
// 			else
// 			{
// 				GPRS_dataUnackLength[socketNo] = length;
// 				return 0;
// 			}
// 			break;
// 	}
	return 0xff;
}

char GPRS_CreateSocket(char c)
{
// 	if((c >= '0') && (c <= '9'))
// 	{
// 		socketMask[createSocketNow] = c - '0';
// 	}
	return 0;
}

/*+UUSOCL: */
char GPRS_CloseSocket1(char c)
{ 
//     uint8_t socketNo;
// 	socketNo = RemaskSocket((c - '0'));
// 	if(socketNo < SOCKET_NUM)
// 	{
// 		tcpSocketStatus[socketNo] = SOCKET_CLOSE;
// 	}
	return 0;
}

/*+USOWR: 0,0*/ // ??? write data ???
// Ya :) check data if write socket
// 0: mean socket close, then update status of tcpSocket

/**
  * @brief  Don't know, not impliment yet
  * @param  
  * @param  
  * @retval 
  */
char GPRS_CloseSocket(char c)
{
	switch(AT_cmdRecvLength)
	{
		case 0:
				socketNo = RemaskSocket((c - '0'));
				if(socketNo >= SOCKET_NUM)
				{
					return 0;
				}
			break;
		case 2:
				if(c == '0')
				{
					tcpSocketStatus[socketNo] = SOCKET_CLOSE;
				}
				return 0;
		default:
			break;
	}
	return 0xff;
}


/*   14,"REC READ","+84972046096","","12/07/26,11:10:17+28"   */
char SMS_Process(char c)
{ 
	static uint32_t length = 0;
	switch(AT_cmdRecvLength)
	{
		case 0:
				if((c >= '0') && (c <= '9'))
				{
					length = c - '0';
					break;
				}
				return 0;
		case 1:
		case 2:
		case 3:
				if((c >= '0') && (c <= '9'))
				{
					length *= 10;
					length += c - '0';
					break;
				}
				else
				{
					RINGBUF_Put(&smsUnreadRingBuff, (uint8_t)(length & 0x00ff));
					RINGBUF_Put(&smsUnreadRingBuff, (uint8_t)((length >> 8) & 0x00ff));
					smsCnt++;
					smsNewMessageFlag = 1;
					return 0;
				}	
		default:
			smsNewMessageFlag = 1;
			return 0;
	}
	return 0xff;
}

/**
  * @brief remask to check socket status ??? see later
  * @param         
  * @param
  * @retval
  */
uint8_t RemaskSocket(uint8_t socket)
{
	uint8_t i;
    
	for(i = 0; i < SOCKET_NUM; i++)
	{
		if(tcpSocketStatus[i] == SOCKET_OPEN)
        {
			if(socketMask[i] == socket) return i;
        }
	}
    
	return 0xff;
}

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
/*				
+RECEIVE,0,5:
hello
*/
/* mean receive 5 from socket 0
data is : "hello"
*/

char GPRS_GetLength(char c)
{
// 	static uint32_t length = 0;
// 	switch(AT_cmdRecvLength)
// 	{
// 		case 0:
// 				socketNo = RemaskSocket((c - '0')); // update socket receive status and
// 				if(socketNo >= SOCKET_NUM)
// 				{
// 					return 0;
// 				}
// 			break;
// 		case 2:
// 			if((c >= '0') && (c <= '9'))
// 				length = c - '0';
// 			else
// 				return 0;
// 			break;
// 		case 3:
// 			if((c >= '0') && (c <= '9'))
// 			{
// 				length *= 10;
// 				length += c - '0';
// 				RINGBUF_Put(&gprsRingLengthBuff[socketNo],(uint8_t)(length & 0x00ff));
// 				RINGBUF_Put(&gprsRingLengthBuff[socketNo],(uint8_t)((length >> 8) & 0x00ff));
// 				length = 0;
// 			}
// 			return 0;
// 		default:
// 			break;
// 	}
	return 0xff;
}


enum{
	GPRS_START_RECV_DATA,
	GPRS_GET_LENGTH,
	GPRS_PREPARE_GET_DATA,
	GPRS_GET_DATA,
	GPRS_FINISH
} dataPhase = GPRS_FINISH;

uint32_t GPRS_SendCmdRecvData(uint8_t socketNum, uint8_t *gprsBuff, uint16_t gprsBuffLen, uint32_t *dataLen)
{
// 	uint8_t buf[20];
// 	Timeout_Type tout;
// 	MODEM_Info("\r\nGSM->GPRS READ DATA\r\n");
// 	delay_ms(100);
// 	dataPhase = GPRS_START_RECV_DATA;
// 	gprsRecvDataLen = 0;
// 	gprsRecvPt = gprsBuff;
// 	socketRecvEnable = 1;
// 	sprintf((char *)buf, "AT+USORD=%d,%d\r",socketMask[socketNum],(GPRS_DATA_MAX_LENGTH-32));
// 	COMM_Puts(buf);
// 	InitTimeout(&tout,TIME_MS(1000));
// 	while(dataPhase != GPRS_FINISH)
// 	{
// 		if(CheckTimeout(&tout) == TIMEOUT)
// 		{
// 			return 1;
// 		}
// 	}
// 	socketRecvEnable = 0;
// 	*dataLen = gprsRecvDataLen;
	return 0;
}

/* read and solve data received from GPRS */
uint32_t GPRS_CmdRecvData(uint8_t *gprsBuff,uint16_t gprsBuffLen,uint32_t *dataLen)
{
// 	static uint32_t timeRead[SOCKET_NUM] = {0};
// 	uint8_t c,buf[20];
// 	uint16_t len = 0;
// 	Timeout_Type tout;
// 	uint32_t socketNum;
// 	for(socketNum = 0;socketNum < SOCKET_NUM;socketNum++)
// 	{
// 		if(tcpSocketStatus[socketNum] == SOCKET_OPEN)
// 		{
// 			if((TICK_Get() - timeRead[socketNum]) >= tcpTimeReCheckData[socketNum]) //5SEC
// 			{
// 				len = (GPRS_DATA_MAX_LENGTH-32);
// 			}
// 			else if((RINGBUF_GetFill(&gprsRingLengthBuff[socketNum]) >= 2))
// 			{
// 				RINGBUF_Get(&gprsRingLengthBuff[socketNum],&c);
// 				len = c;
// 				RINGBUF_Get(&gprsRingLengthBuff[socketNum],&c);
// 				len |= (((uint16_t)c) << 8 & 0xff00);
// 			}	
// 			while(len)
// 			{
// 				timeRead[socketNum] = TICK_Get();
// 				delay_ms(100);
// 				if(len > (GPRS_DATA_MAX_LENGTH-32))
// 				{
// 					len -= (GPRS_DATA_MAX_LENGTH-32);
// 				}
// 				else
// 				{
// 					len = 0;
// 				}
// 				dataPhase = GPRS_START_RECV_DATA;
// 				gprsRecvDataLen = 0;
// 				gprsRecvPt = gprsBuff;
// 				socketRecvEnable = 1;
// 				MODEM_Info("\r\nGSM->GPRS READ DATA\r\n");
//                 // send AT Cmd to read data received
// 				sprintf((char *)buf, "AT+USORD=%d,%d\r",socketMask[socketNum],(GPRS_DATA_MAX_LENGTH-32));
// 				COMM_Puts(buf);
//                 // Set timeout
// 				InitTimeout(&tout,TIME_MS(3000));
// 				while(dataPhase != GPRS_FINISH)
// 				{
// 					if(CheckTimeout(&tout) == TIMEOUT)
// 					{
// 						 socketRecvEnable = 0;
// 						 return 0xff;
// 					}
// 				}
//                 
//                 // Get data succesful
// 				socketRecvEnable = 0;
// 				*dataLen = gprsRecvDataLen;
// 				return socketNum;
// 			}
// 		}
// 		else
// 		{
// 			timeRead[socketNum] = TICK_Get();
// 		}
// 	}
// 	socketRecvEnable = 0;
	return 0xff;
}

char GPRS_GetDNS(char c)
{
// 	static uint8_t *pt;
// 	switch(AT_cmdRecvLength)
// 	{
// 		case 0:
// 				pt = DNS_serverIp;
// 				*pt = c;
// 				pt++;
// 			break;
// 		default:
// 				if((c == '"') || (pt >= DNS_serverIp + sizeof(DNS_serverIp)))
// 				{
// 					return 0;
// 				}
// 				else
// 				{
// 					*pt = c;
// 					pt++;
// 					*pt = 0;
// 				}
// 			break;
// 	}
	
	return 0xff;
}

/*
+RECEIVE,0,14:
@MESS:"hello "
*/
char GPRS_GetData(char c)
{
	static uint32_t dataLen = 0;
    
	switch(AT_cmdRecvLength)
	{
		case 0: // get socket No.
				socketNo = RemaskSocket((c - '0'));
				if(socketNo >= SOCKET_NUM)
				{
					return 0;
				}
				dataPhase = GPRS_GET_LENGTH;
			break;
		case 1:
				if(c != ',') return 0;
			break;
		case 2:
			if((c > '0') && (c <= '9'))
				dataLen = c - '0';
			else
				return 0;
			break;
		default:
			switch(dataPhase)
			{
				case GPRS_GET_LENGTH:
					if((c >= '0') && (c <= '9'))
					{
						dataLen *= 10;
						dataLen += c - '0';
					}
					else if(c == ':');
                    else if(c == '\r')
                    {
						dataPhase = GPRS_PREPARE_GET_DATA;
                    }
					else 
						return 0;
					break;
				case GPRS_PREPARE_GET_DATA:
					if(c == '\n')
					{
						dataPhase = GPRS_GET_DATA;
						gprsRecvDataLen = 0;
					}
					else 
						return 0;
					break;
				case GPRS_GET_DATA:
					//if(socketRecvEnable)
					{
// 						gprsRecvPt[gprsRecvDataLen] = c;
 						gprsRecvDataLen++;
                        RINGBUF_Put(&gprsRingBuff[socketNo], c);
					}
                    
					dataLen--;
					if(dataLen == 0)
					{
                        gprsRecvFlag  = 1;
						socketRecvFlag[socketNo] = 1;
						dataPhase = GPRS_FINISH;
						return 0;
					}
					break;
			}
			break;
	}
	return 0xff;
}

char CLOSE_TCP_Process(char c)
{
	//tcpStatus = TCP_CLOSED;
	return 0;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
char GPRS_DeActive(char c)
{
    gprsStatus = 0;
    gprsInitFlag = 1;
    return 0;
}

void AT_CommandCtl(void)
{	
// 	static uint32_t timeout = 0;
// 	uint8_t i;
//     
// 	if(AT_cmdState == AT_CMD_FINISH) timeout = 0;
// 	else
// 	{
// 		timeout++;
// 		if(timeout >= 3000)
// 		{
// 			AT_cmdState = AT_CMD_FINISH;
// 			for(i = 0; i < CMD_COUNT;i++)
// 			{
// 				cmdCnt[i] = 0;
// 			}
// 		}
//	}
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
void AT_ComnandParser(char c)
{
	uint32_t i;
	static uint32_t RingBuffInit = 0;
    
	if(RingBuffInit == 0)// int ring buff
	{
		RingBuffInit = 1;
		AT_CmdProcessInit();
	}
    
	switch(AT_cmdState)
	{
		case AT_CMD_FINISH:
        {
			for(i = 0; i < CMD_COUNT; i++)
			{
					if(c == AT_ProcessCmd[i].cmdInfo[cmdCnt[i]])
					{
                        cmdCnt[i]++;
                        if(AT_ProcessCmd[i].cmdInfo[cmdCnt[i]] == '\0')
                        {
                                numCmd = i;
                                AT_cmdState = AT_CMD_WAIT_FINISH;
                                AT_cmdRecvLength = 0;
                        }
					}
					else
					{
                        cmdCnt[i] = 0;
					}
			}
        } break;
        
		case AT_CMD_WAIT_FINISH:
        {            
			if(AT_ProcessCmd[numCmd].func(c) == 0)
			{
				AT_cmdState = AT_CMD_FINISH;
				for(i = 0; i < CMD_COUNT;i++)
				{
					cmdCnt[i] = 0;
				}
			}
            
			AT_cmdRecvLength++;
			if(AT_cmdRecvLength >= GPRS_DATA_MAX_LENGTH)
			{
				AT_cmdState = AT_CMD_FINISH;
				for(i = 0; i < CMD_COUNT;i++)
				{
					cmdCnt[i] = 0;
				}
			}
        } break;
        
		default:
        {
			AT_cmdState = AT_CMD_FINISH;
			for(i = 0; i < CMD_COUNT;i++)
			{
				cmdCnt[i] = 0;
			}
        } break;
	}	
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
/*+CMGR: "REC READ","+841645715282","","12/07/26,20:50:07+28"
"abc"
*/
volatile uint8_t flagSmsReadFinish = 0;
char SMS_ReadMsg(char c)
{ 
	static uint8_t comma = 0,getSmsDataFlag = 0;
	static uint8_t *u8pt;
	static uint8_t *u8pt1;
	static uint8_t *u8pt2;
    
	if(AT_cmdRecvLength == 0)
	{
		comma = 0;
		getSmsDataFlag = 0;
		u8pt = smsSender;
		u8pt2 = smsBuf;
		u8pt1 = (uint8_t *)terminateStr;
		return 0xff;
	}
    
	if(c == ',') 
	{
		comma++;
	}
	
	if(getSmsDataFlag)
	{
		if(c == *u8pt1)
		{
			u8pt1++;
			if(*u8pt1 == '\0')
			{
				*u8pt2 = 0;
				flagSmsReadFinish = 1;
				return 0;
			}
		}
		else
		{
			u8pt1 = (uint8_t *)terminateStr;
			if(c == *u8pt1) u8pt1++;
		}
        
		if((u8pt2 - smsBuf) >= sizeof(smsBuf))
		{		
			*u8pt2 = 0;
			flagSmsReadFinish = 1;
			return 0;
		}
        
		*u8pt2 = c;
		u8pt2++;
	}
	else
	{
		switch(comma)
		{
			case 0:
				break;
			case 1:
				if((u8pt - smsSender) >= sizeof(smsSender))
				{
					smsSender[0] = 0;
					return 0;
				}
				if(((c >= '0') && (c <= '9')) || (c == '+'))
				{
					*u8pt = c;
					u8pt++;
					*u8pt = 0;
				}
				break;
			default:
				break;
		}
	}
    
	if(c == '\n')
	{
		getSmsDataFlag = 1;
	}
    
	return 0xff;
}

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
