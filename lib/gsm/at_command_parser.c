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

#define smsScanf	sscanf

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

#define CMD_COUNT   (sizeof (AT_ProcessCmd) / sizeof (AT_ProcessCmd[0]))
#define AT_CMD_PARSER_SIZE	16

const uint8_t terminateStr[7] = "\r\nOK\r\n";

uint8_t cellLocationBuf[128];   // not use in SIM900 (16/7/15 now accept)
uint8_t cellLocationFlag = 0;   // not use in SIM900

char GetCallerNumber(char c);
char GetCallerNumber2(char c);
char RingCallProcess(char c);
char SMS_NewMessage(char c);
char SMS_Process(char c);
char GPRS_GetLength(char c);
char GPRS_GetData(char c);
char CLOSE_TCP_Process(char c);
char Parse(char *buff,uint8_t *smsSendBuff,uint32_t smsLenBuf);
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
char CellID_Process(char c);
char CellID_DeActive(char c);
char GetModemClock(char c);
char Socket0Manage(char c);
char Socket1Manage(char c);
char Socket2Manage(char c);
char Socket3Manage(char c);
char Socket4Manage(char c);
char Socket5Manage(char c);
char Socket6Manage(char c);
char Socket7Manage(char c);

static const SCMD AT_ProcessCmd[] = {
	"+CLIP: \"", GetCallerNumber,
//     "+CLIP:+", GetCallerNumber2,
 	"RING" , RingCallProcess,
    "+CMGL: ", SMS_Process,
	"+CMTI:", SMS_NewMessage,
	//"+RECEIVE,", GPRS_GetLength, // get data length then get data
	"+RECEIVE,", GPRS_GetData,  // get data from gprs
	//"+UUSOCL: ", GPRS_CloseSocket1,
	"+CIPSEND: ", GPRS_CloseSocket,
	"+USOCTL: ",GPRS_SocketControl,
    "+PDP: DEACT", GPRS_DeActive,
	"+CMGR: ", SMS_ReadMsg,
	//"+USOCR: ", GPRS_CreateSocket,
	"BUSY", CallBusy,
	"NO ANSWER", CallBusy,
	"NO CARRIER", CallBusy,
	"+CMTI: \"SM\",", SMS_Process,
	"+CMTI: \"ME\",", SMS_Process,
	//"+UDNSRN: \"",GPRS_GetDNS,
	//"+UULOC: ",CellLocateParser,
	"+CBC: ", GetBatteryStatus,
    
    // Cell ID (only for SIM900)
    "+CIPGSMLOC: ", CellID_Process,
    "+SAPBR 1: DEACT", CellID_DeActive,
    
    // Time SIM900
    "+CCLK: ", GetModemClock,
    "0, CLOSED", Socket0Manage,
    "1, CLOSED", Socket1Manage,
    "2, CLOSED", Socket2Manage,
    "3, CLOSED", Socket3Manage,
    "4, CLOSED", Socket4Manage,
    "5, CLOSED", Socket5Manage,
    "6, CLOSED", Socket6Manage,
    "7, CLOSED", Socket7Manage
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

char CellID_Process(char c)
{
    cellLocationBuf[AT_cmdRecvLength] = c;
	if(c == '\r')
	{
		cellLocationBuf[AT_cmdRecvLength + 1] = 0;
		cellLocationFlag = 1;
		return 0;
	}
    return 0xff;
}

/*
+CIPGSMLOC: 0,106.728612,10.830515,2015/07/16,06:28:29
*/
uint8_t CellID_Parser(float *lat, float *lon, DATE_TIME *gsmtime)
{
    char* str;
    char* sbuf[10];
    uint8_t seg_cnt = 0;
    uint8_t i = 0;
    
    uint8_t length = sizeof(cellLocationBuf);
    
    if (length < 5) // no data (+CIPGSMLOC: 601\r)
    {
        cellIDActive = 0;
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

char CellID_DeActive(char c)
{
    cellIDActive = 0;
    return 0;
}

char GetModemClock(char c)
{
    return 0;
}

char Socket0Manage(char c)
{
    socketMask[0] = 0xff;
    tcpSocketStatus[0] = SOCKET_CLOSE;
}

char Socket1Manage(char c)
{
    socketMask[1] = 0xff;
    tcpSocketStatus[1] = SOCKET_CLOSE;
}

char Socket2Manage(char c)
{
    socketMask[2] = 0xff;
    tcpSocketStatus[2] = SOCKET_CLOSE;
}

char Socket3Manage(char c)
{
    socketMask[3] = 0xff;
    tcpSocketStatus[3] = SOCKET_CLOSE;
}

char Socket4Manage(char c)
{
    socketMask[4] = 0xff;
    tcpSocketStatus[4] = SOCKET_CLOSE;
}

char Socket5Manage(char c)
{
    socketMask[5] = 0xff;
    tcpSocketStatus[5] = SOCKET_CLOSE;
}

char Socket6Manage(char c)
{
    socketMask[6] = 0xff;
    tcpSocketStatus[6] = SOCKET_CLOSE;
}

char Socket7Manage(char c)
{
    socketMask[7] = 0xff;
    tcpSocketStatus[7] = SOCKET_CLOSE;
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

char GPRS_DeActive(char c)
{
    gprsInitFlag = 1;
    gprsStatus = 0;
    return 0;
}

void AT_CommandCtl(void)
{	
	static uint32_t timeout = 0;
	uint8_t i;
    
	if(AT_cmdState == AT_CMD_FINISH) timeout = 0;
	else
	{
		timeout++;
		if(timeout >= 3000)
		{
			AT_cmdState = AT_CMD_FINISH;
			for(i = 0; i < CMD_COUNT;i++)
			{
				cmdCnt[i] = 0;
			}
		}
	}
}

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
			for(i = 0; i < CMD_COUNT;i++)
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
/*+CMGR: "REC READ","+841645715282","","12/07/26,20:50:07+28"
"thienhailue"
*/

uint8_t flagSmsReadFinish = 0;
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

// untested
void SMS_Manage(uint8_t *buff, uint32_t lengBuf)
{
	Timeout_Type t;
	uint8_t tmpBuf[32],c;	
	uint16_t smsLoc, len, i, res;
    
	// read the newly received SMS
	INFO("\r\nSMS->CHECK ALL\r\n");
	COMM_Puts("AT+CMGL=\"ALL\"\r");
	delay_ms(1000);
    
	while(RINGBUF_GetFill(&smsUnreadRingBuff) >=2)
	{
        INFO("Yolo :))\n");
		//watchdogFeed[WTD_MAIN_LOOP] = 0;
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
            NVIC_SystemReset();
        }
			
	}
}


uint16_t CMD_CfgParse(char *buff, uint8_t *smsSendBuff, uint32_t smsLenBuf, uint16_t *dataOutLen, uint8_t pwdCheck)
{
	char *pt,*u8pt,tempBuff0[32],tempBuff1[16],tempBuff2[16],tempBuff3[16],tempBuff4[16],flagCfgSave = 0;
	uint32_t i,t1,t2,t3,t4,t5,t6,len,error,flagResetDevice = 0;
	DATE_TIME t;
	uint8_t cmdOkFlag = 0;
	
	len = 0;
	
	//check password
	if(pwdCheck)
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
    
	//check monney
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
	//chagre monney
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
// 	
// 	//change password
// 	pt = strstr(buff,"DMCFG,0,");
// 	if(pt != NULL)
// 	{
// 		// compare with saved password here
// 		smsScanf(pt,"DMCFG,0,%s",tempBuff0);
// 		memcpy((char *)sysCfg.smsPwd,tempBuff0,sizeof(sysCfg.smsPwd));
// 		INFO("\n\rSMS->NEW PASSWORD:%s\n\r",sysCfg.smsPwd);
// 		len += sprintf((char *)&smsSendBuff[len], "npwr,%s\n", sysCfg.smsPwd);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 	}
//     
// 	//change gprs password
// 	pt = strstr(buff,"DMCFG,1,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,1,%s",tempBuff0);
// 		memcpy((char *)sysCfg.gprsPwd,tempBuff0,sizeof(sysCfg.gprsPwd));
// 		INFO("\n\rSMS->NEW GPRS PASSWORD:%s\n\r",sysCfg.gprsPwd);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,1,%s\n", sysCfg.gprsPwd);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 	}
// 	
// 	//change gprs user
// 	pt = strstr(buff,"DMCFG,2,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,2,%s",tempBuff0);
// 		memcpy((char *)sysCfg.gprsUsr,tempBuff0,sizeof(sysCfg.gprsUsr));
// 		INFO("\n\rSMS->NEW GPRS USER:%s\n\r",sysCfg.gprsUsr);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,2,%s\n", sysCfg.gprsUsr);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 	}
// 	
// 	//change gprs apn
// 	pt = strstr(buff,"DMCFG,3,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,3,%s",tempBuff0);
// 		memcpy((char *)sysCfg.gprsApn,tempBuff0,sizeof(sysCfg.gprsApn));
// 		INFO("\n\rSMS->NEW GPRS APN:%s\n\r",sysCfg.gprsApn);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,3,%s\n", sysCfg.gprsApn);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 	}
// 	
// 	//change Server Name
// 	pt = strstr(buff,"DMCFG,4,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,4,%s",tempBuff0);
// 		memcpy((char *)sysCfg.priDserverName,tempBuff0,sizeof(sysCfg.priDserverName));
// 		sysCfg.dServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS DATA SERVER NAME:%s\n\r",sysCfg.priDserverName);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,4,%s\n", sysCfg.priDserverName);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[0] = SOCKET_CLOSE;
// 	}

// 	//change Server IP
// 	pt = strstr(buff,"DMCFG,5,");
// 	if(pt != NULL)
// 	{
// 		u8pt = (char *)sysCfg.priDserverIp;
// 		smsScanf(pt,"DMCFG,5,%d.%d.%d.%d:%s",&t1,&t2,&t3,&t4,tempBuff1);
// 		sysCfg.priDserverPort = atoi((char *)tempBuff1);
// 		u8pt[0] = t1;
// 		u8pt[1] = t2;
// 		u8pt[2] = t3;
// 		u8pt[3] = t4;		
// 		sysCfg.dServerUseIp = 1;
// 		INFO("\n\rSMS->NEW GPRS DATA SERVER IP:%d.%d.%d.%d:%d\n\r",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.priDserverPort);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,5,%d.%d.%d.%d:%d\n",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.priDserverPort);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[0] = SOCKET_CLOSE;
// 	}
// 	
// 	//change Server use IP
// 	pt = strstr(buff,"DMCFG,6,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,6,%s",tempBuff0);
// 		if(tempBuff0[0] == '1')
// 			sysCfg.dServerUseIp = 1;
// 		else
// 			sysCfg.dServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS DATA SERVER IP:%s\n\r",sysCfg.dServerUseIp);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,6,%d\n", sysCfg.dServerUseIp);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[0] = SOCKET_CLOSE;
// 	}
// 	
// 	//change Server Name
// 	pt = strstr(buff,"DMCFG,8,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,8,%s",tempBuff0);
// 		memcpy((char *)sysCfg.priFserverName,tempBuff0,sizeof(sysCfg.priFserverName));
// 		sysCfg.fServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS FIRMWARE SERVER NAME:%s\n\r",sysCfg.priFserverName);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,8,%s\n", sysCfg.priFserverName);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[1] = SOCKET_CLOSE;
// 	}
//     
// 	//change Server IP
// 	pt = strstr(buff,"DMCFG,9,");
// 	if(pt != NULL)
// 	{
// 		u8pt = (char *)sysCfg.priFserverIp;
// 		smsScanf(pt,"DMCFG,9,%d.%d.%d.%d:%s",&t1,&t2,&t3,&t4,tempBuff1);
// 		sysCfg.priFserverPort = atoi((char *)tempBuff1);
// 		u8pt[0] = t1;
// 		u8pt[1] = t2;
// 		u8pt[2] = t3;
// 		u8pt[3] = t4;		
// 		sysCfg.fServerUseIp = 1;
// 		INFO("\n\rSMS->NEW GPRS FIRMWARE IP:%d.%d.%d.%d:%d\n\r",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.priFserverPort);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,9,%d.%d.%d.%d:%d\n",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.priFserverPort);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[1] = SOCKET_CLOSE;
// 	}
// 	
// 	//change Server use IP
// 	pt = strstr(buff,"DMCFG,10,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,10,%s",tempBuff0);
// 		if(tempBuff0[0] == '1')
// 			sysCfg.dServerUseIp = 1;
// 		else
// 			sysCfg.fServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS FIRMWARE SERVER IP:%s\n\r",sysCfg.fServerUseIp);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,10,%d\n", sysCfg.fServerUseIp);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[1] = SOCKET_CLOSE;
// 	}

// 	
// 	//change interval time run
// 	pt = strstr(buff,"DMCFG,16,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,16,%[^/]/%s",tempBuff0,tempBuff1);
// 		sysCfg.normalReportInterval = atoi((char *)tempBuff0);
// 		sysCfg.savingReportInterval = atoi((char *)tempBuff1);
// 		INFO("\n\rSMS->CHANGE INTERVAL TIME RUN:%d/%d\n\r",sysCfg.normalReportInterval,sysCfg.savingReportInterval);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,16,%d/%d\n", sysCfg.normalReportInterval,sysCfg.savingReportInterval);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 	}
// 	
// 	//change Server Name
// 	pt = strstr(buff,"DMCFG,26,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,26,%s",tempBuff0);
// 		memcpy((char *)sysCfg.priIserverName,tempBuff0,sizeof(sysCfg.priIserverName));
// 		sysCfg.iServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS IMAGE SERVER NAME:%s\n\r",sysCfg.priIserverName);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,8,%s\n", sysCfg.priIserverName);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[2] = SOCKET_CLOSE;
// 	}
// 	//change Server IP
// 	pt = strstr(buff,"DMCFG,27,");
// 	if(pt != NULL)
// 	{
// 		u8pt = (char *)sysCfg.priIserverIp;
// 		smsScanf(pt,"DMCFG,27,%d.%d.%d.%d:%s",&t1,&t2,&t3,&t4,tempBuff1);
// 		sysCfg.priIserverPort = atoi((char *)tempBuff1);
// 		u8pt[0] = t1;
// 		u8pt[1] = t2;
// 		u8pt[2] = t3;
// 		u8pt[3] = t4;		
// 		sysCfg.iServerUseIp = 1;
// 		INFO("\n\rSMS->NEW GPRS IMAGE IP:%d.%d.%d.%d:%d\n\r",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.priIserverPort);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,27,%d.%d.%d.%d:%d\n",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.priIserverPort);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[2] = SOCKET_CLOSE;
// 	}
// 	
// 	//change Server use IP
// 	pt = strstr(buff,"DMCFG,28,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,28,%s",tempBuff0);
// 		if(tempBuff0[0] == '1')
// 			sysCfg.iServerUseIp = 1;
// 		else
// 			sysCfg.iServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS IMAGE SERVER IP:%s\n\r",sysCfg.iServerUseIp);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,28,%d\n", sysCfg.iServerUseIp);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[2] = SOCKET_CLOSE;
// 	}
// 	
	//reset device
	flagResetDevice = 0;
	pt = strstr(buff,"DMCFG,RESET");
	if(pt != NULL)
	{
		flagResetDevice = 1;
		INFO("\n\rSMS->RESET DEVICE\\n\r");
		len += sprintf((char *)&smsSendBuff[len], "DEVICE RESETING...\n");
		cmdOkFlag = 1;
	}
// 	
// 	//change Server Name
// 	pt = strstr(buff,"DMCFG,41,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,41,%s",tempBuff0);
// 		memcpy((char *)sysCfg.secDserverName,tempBuff0,sizeof(sysCfg.secDserverName));
// 		sysCfg.dServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS DATA SERVER NAME:%s\n\r",sysCfg.secDserverName);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,41,%s\n", sysCfg.secDserverName);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[0] = SOCKET_CLOSE;
// 	}

// 	//change Server IP
// 	pt = strstr(buff,"DMCFG,42,");
// 	if(pt != NULL)
// 	{
// 		u8pt = (char *)sysCfg.secDserverIp;
// 		smsScanf(pt,"DMCFG,42,%d.%d.%d.%d:%s",&t1,&t2,&t3,&t4,tempBuff1);
// 		sysCfg.secDserverPort = atoi((char *)tempBuff1);
// 		u8pt[0] = t1;
// 		u8pt[1] = t2;
// 		u8pt[2] = t3;
// 		u8pt[3] = t4;		
// 		sysCfg.dServerUseIp = 1;
// 		INFO("\n\rSMS->NEW GPRS DATA SERVER IP:%d.%d.%d.%d:%d\n\r",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.secDserverPort);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,42,%d.%d.%d.%d:%d\n",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.secDserverPort);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[0] = SOCKET_CLOSE;
// 	}
// 	

// 	//change Server Name
// 	pt = strstr(buff,"DMCFG,43,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,43,%s",tempBuff0);
// 		memcpy((char *)sysCfg.secFserverName,tempBuff0,sizeof(sysCfg.secFserverName));
// 		sysCfg.fServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS FIRMWARE SERVER NAME:%s\n\r",sysCfg.secFserverName);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,43,%s\n", sysCfg.secFserverName);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[1] = SOCKET_CLOSE;
// 	}
//     
// 	//change Server IP
// 	pt = strstr(buff,"DMCFG,44,");
// 	if(pt != NULL)
// 	{
// 		u8pt = (char *)sysCfg.secFserverIp;
// 		smsScanf(pt,"DMCFG,44,%d.%d.%d.%d:%s",&t1,&t2,&t3,&t4,tempBuff1);
// 		sysCfg.secFserverPort = atoi((char *)tempBuff1);
// 		u8pt[0] = t1;
// 		u8pt[1] = t2;
// 		u8pt[2] = t3;
// 		u8pt[3] = t4;		
// 		sysCfg.fServerUseIp = 1;
// 		INFO("\n\rSMS->NEW GPRS FIRMWARE IP:%d.%d.%d.%d:%d\n\r",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.secFserverPort);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,44,%d.%d.%d.%d:%d\n",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.secFserverPort);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[1] = SOCKET_CLOSE;
// 	}
// 	
// 	//change Server Name
// 	pt = strstr(buff,"DMCFG,45,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,45,%s",tempBuff0);
// 		memcpy((char *)sysCfg.secIserverName,tempBuff0,sizeof(sysCfg.secIserverName));
// 		sysCfg.iServerUseIp = 0;
// 		INFO("\n\rSMS->NEW GPRS FIRMWARE SERVER NAME:%s\n\r",sysCfg.secIserverName);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,45,%s\n", sysCfg.secIserverName);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[2] = SOCKET_CLOSE;
// 	}
//     
// 	//change Server IP
// 	pt = strstr(buff,"DMCFG,46,");
// 	if(pt != NULL)
// 	{
// 		u8pt = (char *)sysCfg.secIserverIp;
// 		smsScanf(pt,"DMCFG,46,%d.%d.%d.%d:%s",&t1,&t2,&t3,&t4,tempBuff1);
// 		sysCfg.secFserverPort = atoi((char *)tempBuff1);
// 		u8pt[0] = t1;
// 		u8pt[1] = t2;
// 		u8pt[2] = t3;
// 		u8pt[3] = t4;		
// 		sysCfg.iServerUseIp = 1;
// 		INFO("\n\rSMS->NEW GPRS FIRMWARE IP:%d.%d.%d.%d:%d\n\r",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.secIserverPort);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,46,%d.%d.%d.%d:%d\n",u8pt[0],u8pt[1],u8pt[2],u8pt[3],sysCfg.secIserverPort);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 		tcpSocketStatus[2] = SOCKET_CLOSE;
// 	}
//     
// 	//get firmware version
// 	pt = strstr(buff,"DMCFG,47");
// 	if(pt != NULL)
// 	{
// 		flagResetDevice = 1;
// 		INFO("\n\rSMS->RESET DEVICE\\n\r");
// 		len += sprintf((char *)&smsSendBuff[len], "VERSION:%s\n",FIRMWARE_VERSION);
// 		cmdOkFlag = 1;
// 	}
// 	
// 	//CHANGE PHONE BOSS
// 	pt = strstr(buff,"DMCFG,60,");
// 	if(pt != NULL)
// 	{
// 		smsScanf(pt,"DMCFG,60,%s",tempBuff0);
// 		memcpy((char *)sysCfg.whiteCallerNo,tempBuff0,sizeof(sysCfg.whiteCallerNo));
// 		INFO("\n\rSMS->BOSS PHONE:%s\n\r",sysCfg.whiteCallerNo);
// 		len += sprintf((char *)&smsSendBuff[len], "DMCFG,60,%s\n", sysCfg.whiteCallerNo);
// 		flagCfgSave = 1;
// 		cmdOkFlag = 1;
// 	}
// 	
	if(len >= smsLenBuf)	len = smsLenBuf;
	
	smsSendBuff[len] = 0;
	*dataOutLen = len;
	if(flagCfgSave)
	{
		CFG_Save();
	}
    
	if(flagResetDevice)
	{
		return 0xa5;
	}
	return 0;
}

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
