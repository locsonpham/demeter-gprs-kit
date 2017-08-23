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
#include "modem.h"

#include "defines.h"
#include "includes.h"

#include "systick.h"
#include "at_command_parser.h"
#include "system_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint8_t *modemOk = "OK";
const uint8_t *modemError = "ERROR";

uint8_t modemId[16];
volatile uint8_t modemRinging = 0;
volatile uint8_t modemStarted = 0;
uint8_t csqBuff[16];
uint8_t csqValue = 0;
uint8_t gsmSignal = 0;
uint32_t batteryPercent = 100;

// Status Variables
// GSM Status
uint8_t modemInitFlag = 0;
volatile uint8_t modemStatus;

// GPRS Status
uint8_t gprsInitFlag = 0;
volatile uint8_t gprsStatus = 0;
uint8_t modemIP[4] = {0};

// CellLocation Status
uint8_t cellInitFlag = 0;
volatile uint8_t cellStatus = 0;

uint8_t simInserted = 0;

cellLocateType cellLocate;

/* Private function prototypes -----------------------------------------------*/
void MODEM_FlushBuffer(void);
uint8_t MODEM_CheckSIM(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Init GSM Module
  * @param  
  * @param  
  * @retval return 0 if OK (0 means no error)
  */
uint8_t MODEM_Init(void)
{
    uint8_t tryAgain = 3;
    
    // Init UART
    SIM900_Init(9600);
    
    while (tryAgain)
    {
        MODEM_FlushBuffer();
        
        POWKEY_On();
        
        if(MODEM_CheckResponse("NORMAL POWER DOWN", TIME_SEC(3)) == 0)
        {
            MODEM_Info("\r\nGSM->OFF NOW...\r\n");
            tryAgain--;
            continue;
        }
        
        MODEM_Info("\r\nGSM->TURNING ON...\r\n");
           
        if(MODEM_SendCommand(modemOk,modemError,3000,3,"AT\r") == 0) break;
        
        tryAgain--;
    }

    if (tryAgain == 0)
    {
        MODEM_Info("\r\nGSM-> Init FAIL\r\n");
        return 0xff;
    }
	
    MODEM_Info("\r\nGSM-> GSM Module READY...\r\n");
	
	// Clear all configs
	MODEM_Info("\r\nGSM->CONFIGS CLEAR\r\n");
	if(MODEM_SendCommand(modemOk,modemError,3000,0,"ATZ\r"))	return 0xff;
	
	// Read modem IMEI
	GetIMEI(modemId, sizeof modemId);
	MODEM_Info("GSM-> IMEI: %s\n", modemId);

	if(strlen((char *)modemId) == 0) return 0xff;
    
    MODEM_Info("\r\nGSM-> CHECK SIM CARD...\r\n");
    if(MODEM_CheckSIM()) return 0xff;

	if(sysCfg.imei[0] == '\0')
    {
		if(strcmp((char *)sysCfg.imei, (char *)modemId))
		{
			strcpy((char *)sysCfg.imei, (char *)modemId);
			if(sysCfg.id[0] == '\0')
				strcpy((char *)sysCfg.id, (char *)modemId);
			
			// Save IMEI if avaiable
			#ifdef USE_EXT_FLASH
			CFG_Save();
			#endif
		}
    }

	// Check battery
	MODEM_GetCBC();
    
	// Check Signal
    MODEM_GetCSQ();

	// Set SMS format to text mode
	MODEM_Info("\r\nGSM->SMS SET TEXT MODE\r\n");
	if(MODEM_SendCommand(modemOk,modemError,3000,0,"AT+CMGF=1\r"))	return MODEM_SIM_CARD_ERROR;
    
    if(MODEM_SendCommand(modemOk,modemError,3000,0,"AT+CLIP=1\r"))	return 0xff;
	
// 	// Set SMS DISPLAY  mode
// 	MODEM_Info("\r\nGSM->SET SMS DISPLAYMODE\r\n");
// 	if(MODEM_SendCommand(modemOk,modemError,3000,0,"AT+CNMI=2,1,0,0,0\r"))	return 0xff;
	
	modemStatus = 1;
 	return MODEM_INIT_OK;
}

/**
  * @brief  turn MODEM into Sleep Mode use CMD
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_SleepCMD(void)
{
    SIM900_Init(9600);
    COMM_Puts("AT+CPOWD=1\r");
    delay_ms(3000);
    return 0;
}

/**
  * @brief  turn MODEM into Sleep Mode use KEY Pin
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_Sleep(void)
{
    uint8_t tryAgain = 5;
    
    // Init UART
    SIM900_Init(9600);
    
    while (tryAgain)
    {
        MODEM_FlushBuffer();
        
        POWKEY_On();
        
        if(MODEM_CheckResponse("NORMAL POWER DOWN", TIME_SEC(3)) == 0)
        {
            MODEM_Info("\r\nGSM->OFF NOW...\r\n");
            break;
        }
        
        tryAgain--;
    }
}

/**
  * @brief  init cellular location
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_CellInit(void)
{
    // Set bearer parameter
    if(MODEM_SendCommand(modemOk, modemError, 5000, 1,"AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r") == 0xff) return 0xff;
    delay_ms(500);
    // Set apn
    if(MODEM_SendCommand(modemOk,modemError, 5000, 1,"AT+SAPBR=3,1,\"APN\",\"internet\"\r") == 0xff) return 0xff;
    delay_ms(500);
//     if(MODEM_SendCommand(modemOk,modemError, 5000, 1,"AT+SAPBR=3,1,\"USER\",\"mms\"\r") == 0xff) return 0xff;
//     delay_ms(500);
//     if(MODEM_SendCommand(modemOk,modemError, 5000, 1,"AT+SAPBR=3,1,\"PWD\",\"mms\"\r") == 0xff) return 0xff;
//     delay_ms(500);
    // Activate bearer context
    if(MODEM_SendCommand(modemOk,modemError, 10000, 1,"AT+SAPBR=1,1\r") == 0xff) return 0xff;
    delay_ms(500);
    // Get IP context
    if(MODEM_SendCommand(modemOk,modemError, 5000, 1,"AT+SAPBR=2,1\r") == 0xff) return 0xff;
    delay_ms(500);
    
	cellStatus = 1;
    return 0;
}

/**
  * @brief  send CMD to get datetime and location
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_GetCellLoc(void)
{
    if(MODEM_SendCommand(modemOk,modemError, 10000, 1,"AT+CIPGSMLOC=1,1\r") == 0xff) return 0xff;
    return 0;
}

/**
  * @brief  parse data string to get datetime and cellular location 
  * @param  
  * @param  
  * @retval 
  */
uint8_t ParserCellLocate(void)
{
    if (cellLocationFlag)
    {
        cellLocationFlag = 0;
        
        double lat, lon;
        DATE_TIME time;
        
        MODEM_Info("\nGet CellLoc\n");
        
        if (CellLoc_Parser(&lat, &lon, &sysTime) == 0)
        {
            if (lat == 0.0 && lon == 0.0)
            {
                MODEM_Info("GPS INVALID\n");
            }
            else
            {
                // Update time for GSM
                cellLocate.lat = (double)lat;
                cellLocate.lon = (double)lon;
                
//               char buff[64];
//               sprintf(buff, "AT+CCLK=\"%02d/%02d/%02d,%02d:%02d:%02d+28\"\r", cellLocate.time.year%100, cellLocate.time.month, cellLocate.time.mday, cellLocate.time.hour, cellLocate.time.min, cellLocate.time.sec);
//               COMM_Puts(buff);
//               delay_ms(500);
                
                MODEM_Info("\ncellLat: %f, cellLon: %f\n", (float)cellLocate.lat, (float)cellLocate.lon);
            }
        }
        else
        {
        }
    }
}

/**
  * @brief  deinit cellLocate
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_DeActiveBearer(void)
{
    if(MODEM_SendCommand(modemOk,modemError, 5000, 1,"AT+SAPBR=0,1\r") == 0xff) return 0xff;
	
	cellStatus = 0;
    return 0;
}

/**
  * @brief set GPRS Internet Profile
  * @param         
  * @param
  * @retval
  */
uint8_t InternetSetProfiles(uint8_t *apn,uint8_t *usr,uint8_t *pwd)
{	
	uint8_t buf[32];

	delay_ms(100);
    
	MODEM_FlushBuffer();
    sprintf((char*)buf, "AT+CSTT=\"%s\",\"%s\",\"%s\"\r", apn, usr, pwd); 
	COMM_Puts(buf);
	if(MODEM_CheckResponse("OK", 1000))
    {
		MODEM_Info("\r\nGSM-> AT+CSTT FAILS\r\n");
		return 0xff;
	}
    
	return 0;
}

/**
  * @brief  enable GPRS
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_EnableGPRS(void)
{
	delay_ms(100);
    
	MODEM_FlushBuffer();
    
	MODEM_Info("\r\nGSM->ENABLE GPRS FUNCTION\r\n");
	COMM_Puts("AT+CGATT=1\r");
    
	if(MODEM_CheckResponse("OK", 10000))
    {
		MODEM_Info("\r\nGSM-> ENABLE GPRS FUNCTION FAILS\r\n");
		return 0xff;
	}
    
    // Deactivate GPRS PDP Context
    MODEM_Info("\r\nGSM->RESET IP\r\n");
	COMM_Puts("AT+CIPSHUT\r");
	if(MODEM_CheckResponse("SHUT OK", 10000))
    {
		MODEM_Info("\r\nGSM->RESET IP FAIL\r\n");
		return 0xff;
	}
    
    // Setup CIPMUX
    MODEM_Info("\r\nGSM->SET MUL CONNECTION CIPMUX=1\r\n");
	COMM_Puts("AT+CIPMUX=1\r");
	if(MODEM_CheckResponse("OK", 10000))
    {
		MODEM_Info("\r\nGSM->SET MUX FAIL\r\n");
		return 0xff;
	}
    
    // Setup Internet Profile
    InternetSetProfiles("internet", "mms", "mms");
    
    // Bring up wireless
    MODEM_Info("\r\nGSM->BRING UP WIRELESS\r\n");
	COMM_Puts("AT+CIICR\r");
	if(MODEM_CheckResponse("OK", 10000))
    {
		MODEM_Info("\r\nGSM->BRING UP FAIL\r\n");
		return 0xff;
	}
    
    delay_ms(TIME_SEC(5)); // delay time GPRS ready connect
    
    // Get Current IP
    MODEM_Info("\r\nGSM->LOCAL IP\r\n");
	
	// Try to get LOCAL IP
	uint8_t ipTry = 3;
	
	while (ipTry--)
	{
        COMM_Puts("AT+CIFSR\r");
        
		if(MODEM_CheckResponse("\r", 5000))
		{
			MODEM_Info("\r\nGSM->LOCAL IP FAIL\r\n");
			return 0xff;
		}
		else
		{
			break;
		}
		
		delay_ms(1000);
	}
	
	gprsStatus = 1;	// init OK
    
	return 0;
}

/**
  * @brief  Disable GPRS
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_DisableGPRS(void)
{
	delay_ms(100);
    
    MODEM_Info("\r\nGSM->RESET IP\r\n");
	COMM_Puts("AT+CIPSHUT\r");
	if(MODEM_CheckResponse("SHUT OK", 5000))
    {
		MODEM_Info("\r\nGSM->RESET IP FAIL\r\n");
		return 0xff;
	}
    
    delay_ms(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->DISABLE GPRS FUNTION\r\n");
	COMM_Puts("AT+CGATT=0\r");
	if(MODEM_CheckResponse("OK", 5000))
    {
		MODEM_Info("\r\nGSM-> DISABLE GPRS FAIL\r\n");
		return 0xff;
	}
    
	gprsStatus = 0;
	return 0;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_CreateSocket(uint8_t socket)
{
	return 0xff;
}

/**
  * @brief  Close socket
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_CloseSocket(uint8_t socket)
{
	COMPARE_TYPE cmp1,cmp2;
	Timeout_Type tOut;
	uint8_t c;
	uint8_t buf[32];
    
	delay_ms(100);
    
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->CLOSE SOCKET:%d\r\n", socket);
	sprintf((char *)buf, "AT+CIPCLOSE=%d\r", socket);
	COMM_Puts(buf);
    
	InitFindData(&cmp1,"CLOSE OK");
	InitFindData(&cmp2,"ERROR");
	InitTimeout(&tOut, 10000);
    
	while(CheckTimeout(&tOut))
	{
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp1,c) == 0)
			{
				MODEM_Info("\r\nGSM->CLOSE SOCKET OK\r\n");
                socketMask[socket] = 0xff;
                tcpSocketStatus[socket] = SOCKET_CLOSE;
				return 0;
			}
            
			if(FindData(&cmp2,c) == 0)
			{
				break;
			}
		}
	}
    
	MODEM_Info("\r\nGSM->CLOSE SOCKET FAILS\r\n");
	return 0xff;
}

/**
  * @brief  connect to socket
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_ConnectSocket(uint8_t socket, uint8_t *addr, uint16_t port, uint8_t useIp)
{
	COMPARE_TYPE cmp1, cmp2, cmp3, cmp4;
	Timeout_Type tOut;
	uint8_t c, i;
	uint8_t buf[32];
    
	delay_ms(100);
	MODEM_FlushBuffer();

	delay_ms(500);
	if(useIp == 0)
	{
        MODEM_Info("\r\GSM->CONNECT SOCKET:%d:%s:%d\n", socket, addr, port);
        sprintf((char *)buf, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d\r", socket, addr, port);
	}
    else
    {
        MODEM_Info("\r\nGSM->CONNECT SOCKET:%d:%d.%d.%d.%d:%d\r\n", socket, addr[0], addr[1], addr[2], addr[3], port);
        sprintf((char *)buf, "AT+CIPSTART=%d,\"TCP\",\"%d.%d.%d.%d\",%d\r", socket, addr[0], addr[1], addr[2], addr[3], port);
    }
    
	COMM_Puts(buf);
    
	InitFindData(&cmp1, "CONNECT OK");
	InitFindData(&cmp2, "CONNECT FAIL");
    InitFindData(&cmp3, "ALREADY CONNECT");
    InitFindData(&cmp4, "ERROR");
    
	InitTimeout(&tOut,TIME_SEC(10));
	while(CheckTimeout(&tOut))
	{
		if(RINGBUF_Get(&commBuf, &c) == 0)
		{
			if(FindData(&cmp1, c) == 0)
			{
				MODEM_Info("\r\nGSM->CONNECT SOCKET OK\r\n");
                socketMask[socket] = socket;
                tcpSocketStatus[socket] = SOCKET_OPEN;
				return 0;
			}
            
            if (FindData(&cmp4, c) == 0)
            {
                break;
            }
            
            if(FindData(&cmp3,c) == 0)
			{
				MODEM_Info("\r\nGSM->ALREADY CONNECT SOCKET\r\n");
                socketMask[socket] = socket;
                tcpSocketStatus[socket] = SOCKET_OPEN;
				return 0;
			}
            
			if(FindData(&cmp2,c) == 0)
			{
				break;
			}
		}
	}
    
    socketMask[socket] = 0xff;
    tcpSocketStatus[socket] = SOCKET_CLOSE;
	MODEM_Info("\r\nGSM->CONNECT SOCKET FAILS\r\n");
    
	return 0xff;
}

/**
  * @brief  send data via socket connection
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_GprsSendData(uint8_t socket, uint8_t *data, uint32_t length)
{
	COMPARE_TYPE cmp1;
	Timeout_Type tOut;
	uint8_t buf[32], c;
	uint32_t i;
	uint32_t tryNum = 5;
    
    // Flush data in buffer
	MODEM_FlushBuffer();
    
    // prepare buf to send CMD
    // example: AT+CIPSEND=0,14
    // -- write 14 byte to socket 0
	sprintf((char *)buf, "AT+CIPSEND=%d,%d\r", socket, length);
	
	for(;;)
	{	
        // Write CMD "Write socket"
		COMM_Puts(buf);
        
        // init time out
		InitTimeout(&tOut, 5000);
        /// init find data ">"
		InitFindData(&cmp1,">");
        
        // this code use "while -  continue loop"
        // example
        // while (test condition) {
        //      { stament
        //          if (test expression)
        //          { continue; // return to while loop
        // in this case is for (;;;)
        /////////////////////////////////////////////////
		while(CheckTimeout(&tOut))
		{
			if(RINGBUF_Get(&commBuf,&c) == 0)
			{
				if(FindData(&cmp1,c) == 0)  // find character ">"
				{
					break;
				}
			}
		}
        
		if(CheckTimeout(&tOut) == TIMEOUT)
		{
			tryNum --;
			if(tryNum == 0) 
			{
				tcpSocketStatus[socket] = SOCKET_ERROR;
				return 0xff;
			}
			continue;   // loop back to beginning of loop
		}
        
        // Write data to socket
		i = length;
		for(i = 0; i < length; i++)
		{
			COMM_Putc(data[i]);
		}
        
        // waiting for OK reply from GSM
		InitTimeout(&tOut, 5000);    // wait 5s
		InitFindData(&cmp1,"OK");
		
		while(CheckTimeout(&tOut))
		{
			if(RINGBUF_Get(&commBuf,&c) == 0)
			{
				if(FindData(&cmp1,c) == 0)
				{
					MODEM_Info("\r\nGSM->%s OK\r\n",buf);
					delay_ms(100);
					return 0;
				}
			}
		}
        
		MODEM_Info("\r\nGSM->%s FAILS\r\n",buf);
		tryNum --;
		if(tryNum == 0) 
		{
			tcpSocketStatus[socket] = SOCKET_ERROR;
			return 0xff;
		}
		
		delay_ms(100);
	}
}

/**
  * @brief  check ACK/UNACK data
  * @param  
  * @param  
  * @retval 
  */
uint16_t MODEM_CheckGPRSDataOut(uint8_t socket)
{
	COMPARE_TYPE cmp1,cmp2;
	Timeout_Type tOut;
	uint8_t buf[32], c;
    
	delay_ms(100);
	MODEM_FlushBuffer();
	MODEM_Info("\r\nGSM->CHECK OUTCOMING UNACK SOCKET:%d\r\n",socket);
	sprintf((char *)buf, "AT+CIPACK=%d\r", socketMask[socket]);
	COMM_Puts(buf);
    
	InitFindData(&cmp1,"OK");
	InitFindData(&cmp2,"ERROR");
	InitTimeout(&tOut, 1000);
	while(CheckTimeout(&tOut))
	{
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp1,c) == 0)
			{
				MODEM_Info("\r\nGPRS->UNACK:%d Bytes\r\n", GPRS_dataUnackLength[socket]);
                return 0;
				//return GPRS_dataUnackLength[socket];
			}
            
			if(FindData(&cmp2,c) == 0)
			{
				MODEM_Info("\r\nGPRS->ERROR\r\n");
				//tcpSocketStatus[socket] = SOCKET_CLOSE;
				break;
			}
		}
	}
    
	//num of data will check in at_command_parser.c 
	// return GPRS_dataUnackLength
	MODEM_Info("\r\nGPRS->UNACK:%d Bytes\r\n", GPRS_dataUnackLength[socket]);
	return 0xffff;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
uint8_t VoiceSetup(void)
{
// 	DelayMs(500);
// 	COMM_Puts("AT+USPM=0,3,0,0\r");
// 	if(MODEM_CheckResponse("OK",3000))
// 		return 0xff;
// 	DelayMs(500);
// 	COMM_Puts("AT+UMGC=0,12,12000\r");
// 	if(MODEM_CheckResponse("OK",3000))
// 		return 0xff;
// 	DelayMs(500);
// 	COMM_Puts("AT+CRSL=3\r");
// 	if(MODEM_CheckResponse("OK",3000))
// 		return 0xff;
// 	DelayMs(500);
// 	COMM_Puts("AT+USGC=3,0,0,16384,24576,16384\r");
// 	if(MODEM_CheckResponse("OK",3000))
// 		return 0xff;
// 	DelayMs(500);
// 	return 0;
}

/**
  * @brief  calling proccess
  * @param  callAction: 1:Answer | 0:Hang Up
  * @param  
  * @retval 
  */
uint8_t CallingProcess(uint8_t callAction)
{
	if(callAction)  // true
	{
// 		if(VoiceSetup())    // setup voice OK -> return 0
// 			return 0xff;
		COMM_Puts("ATA\r"); // Start Answer ( AT ANSWER)
		if(MODEM_CheckResponse("OK", 3000))
			return 0xff;
	}
	else
	{
		COMM_Puts("ATH\r"); // Hold (AT HOLD)
		if(MODEM_CheckResponse("OK", 3000))
			return 0xff;
	}
	return 0;
}

void MODEM_SetDateTime()
{
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
void MODEM_Hangup(void)
{
	delay_ms(100);
	COMM_Puts("ATH\r");
	MODEM_CheckResponse("OK", 2000);
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
void MODEM_DataMode(void)
{	
	delay_ms(100);
	COMM_Puts("ATO\r");
	MODEM_CheckResponse("CONNECT", 10000);
	MODEM_FlushBuffer();
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_GetCBC(void)
{
    MODEM_SendCommand(modemOk, modemError, 1000, 0, "AT+CBC\r"); //Batery check status
	MODEM_Info("GSM->Battery Status:%d%\n", batteryPercent);
    return 0;
}

/* Check signal quality
The following mapping of “signal” to <rssi> exists:
“signal”    <rssi>
0           < 4 or 99 (<= -106 dBm or unknown)
1           < 10 (<= -96 dBm)
2           < 16 (<= -84 dBm)
3           < 22 (<= -72 dBm)
4           < 28 (<= -60 dBm)
5           >=28 (> -60 dBm)
*/
uint8_t MODEM_GetCSQ(void)
{
	uint8_t i,c;
	float fRssi, fBer;
	delay_ms(100);
	MODEM_FlushBuffer();
	COMM_Puts("AT+CSQ\r");	// cancel calling if busy.
	if(!MODEM_CheckResponse("+CSQ: ", 2000))
	{
		i = 0;
		while(RINGBUF_Get(&commBuf,&c) == 0)
		{
				if((c == '\r') || (c == '\n') || (c == ',')) break;
            
				csqBuff[i] = c;
				i++;
				if(i >= sizeof(csqBuff)) 
				{
					break;		
				}
		}
		csqBuff[i] = '\0';
		sscanf((char *)csqBuff,"%d,%d",&fRssi, &fBer);
		csqValue = (uint8_t)fRssi;
		if((csqValue < 4) || (csqValue == 99))
			gsmSignal = 0;
		else if(csqValue < 10)
			gsmSignal = 1;
		else if(csqValue < 16)
			gsmSignal = 2;
		else if(csqValue < 22)
			gsmSignal = 3;
		else if(csqValue < 28)
			gsmSignal = 4;
		else if(csqValue < 51)
			gsmSignal = 5;
		else 
			gsmSignal = 0;
        
        MODEM_Info("GSM->Signal Strength:%d%\n", gsmSignal);
        
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
uint8_t SendSMS(const uint8_t *recipient, const uint8_t *data)
{
	uint8_t buf[32];
	uint8_t i;
    
	delay_ms(100);
	MODEM_FlushBuffer();
    
	MODEM_Info("\r\nGSM->SMS SEND TO %s...\r\n", recipient);
	MODEM_Info("\r\nGSM->DATA: %s\r\n", data);
	sprintf((char *)buf, "AT+CMGS=\"%s\"\r", recipient);
    
	COMM_Puts(buf);
    
	if(MODEM_CheckResponse("> ", TIME_SEC(20)))
	{
		MODEM_Info("\r\nGSM->SMS SEND FAILS\r\n");
		return 0xff;
	}
	
	for(i = 0; i < 160; i++)
	{
		if(data[i] == '\0')
			break;

		COMM_Putc(data[i]);
	}
    
	COMM_Putc(0x1A);
	
	if(MODEM_CheckResponse("OK", 10000)){
		MODEM_Info("\r\nGSM->SMS SEND FAILS\r\n");
		return 0xff;
	}
	
	MODEM_Info("\r\n");
	MODEM_Info("\r\nSMS SEND OK\r\n");
	
	return 0;
}

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
uint8_t MODEM_SendSMS(const uint8_t *recipient, uint8_t *data)
{
    data[160] = 0;
    //MODEM_Info("\r\nGSM->SMS:%s\r\n",data);
    if(SendSMS(recipient, data)) return 0xff;
	return 0;
}

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
uint8_t MODEM_CheckResponse(uint8_t *str, uint32_t t)
{
	COMPARE_TYPE cmp;
	uint8_t c;
	InitFindData(&cmp, str);
	while(t--)
	{
		delay_ms(1);
		if(RINGBUF_Get(&commBuf,&c) == 0)
		{
			if(FindData(&cmp,c) == 0)
			{
				return 0;
			}
		}
	}
	return 0xff;
}

/**
  * @brief wait seconds
  * @param         
  * @param
  * @retval
  */
uint8_t MODEM_Wait(const uint8_t *res, uint32_t time)
{
	uint8_t c, i=0;
	time *= 1000;
	while(time--){
		delay_ms(1);
		while(RINGBUF_Get(&commBuf, &c)==0){
			if(c==res[i]){
				if(res[++i]==0)return 1;
			}else if(c==res[0]) i = 1;
			else i = 0;
		}
	}
	
	return 0;
}

uint8_t MODEM_Gets(uint8_t *buf, uint16_t bufSize, uint8_t match)
{
	uint8_t c;
	uint16_t i = 0;
	
	if(bufSize <= 1 || buf == NULL) return 0;
	
	while(RINGBUF_Get(&commBuf, &c)==0){
		if(c == match) break;
		if(buf != NULL)
			buf[i++] = c;
		if(i>=bufSize-1) break;
	}
	
	buf[i] = 0;
	
	return i;
}

/**
  * @brief Get Operator Name
  * @param         
  * @param
  * @retval
  */
uint8_t MODEM_GetOperatorName(uint8_t *buf, uint16_t bufSize)
{
	uint8_t c;
	
	COMM_Puts("AT+COPS?\r");
	if(MODEM_CheckResponse("+COPS: 0,0,\"", 5000) == 0) return 0xff;
	delay_ms(1000);
	
	while(RINGBUF_Get(&commBuf, &c)==0){
		if(c == '\"' || bufSize-- == 1){
			*buf = 0;
			// buffer should be empty
			MODEM_CheckResponse("OK", 1000);
			return 0;
		}
		*buf++ = c;
	}
	
	return 0xff;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
uint8_t MODEM_CheckSIM(void)
{
    COMM_Puts("AT+CPIN?\r");
    
    if (MODEM_CheckResponse("OK", 5000) == 0)   // SIM Inserted
    {
        simInserted = 1;
        return 0;
    }
	
	simInserted = 0;
	return 0xff;
}

uint32_t MODEM_CheckAccount(void)
{
	uint8_t c, state=0;
	uint8_t buf[16], *pbuf = buf;
	
	COMM_Puts("ATD*101#\r");
    
    // Waiting for data
    // If timeout return 0
    // else continue
	if(!MODEM_CheckResponse("+CUSD:", 5000) == 0) return 0xff;
	delay_ms(1000);
	
	while(RINGBUF_Get(&commBuf, &c)==0)
    {
		switch(state)
        {
			case 0:
				if(c == 'a') state ++;
				break;
			case 1:
				if(c == ' ') state ++;
				break;
			case 2:
				if(c == ' ')
                {
					*pbuf = 0;
					// buffer should be empty
					MODEM_CheckResponse("OK", 1000);
					return atol((char *)buf);
				}
				*pbuf++ = c;
				break;
		}
	}
	
	return 0;
}

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
static void GetIMEI(uint8_t *buf, uint16_t bufSize)
{
	MODEM_FlushBuffer();
	
	COMM_Puts("AT+CGSN\r");
	MODEM_CheckResponse("\n", 1000);
	delay_ms(100);
	
	MODEM_Gets(buf, bufSize, '\r');
	
	MODEM_CheckResponse("OK", 1000);
}

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
uint8_t MODEM_SendCommand(const uint8_t *resOk, const uint8_t *resFails, uint32_t timeout, uint8_t tryAgainNum, const uint8_t *format, ...)
{
	static  uint8_t  buffer[256];
	COMPARE_TYPE cmp1,cmp2;
	Timeout_Type tOut;
	uint8_t c;
    
    va_list     vArgs;
	va_start(vArgs, format);
	vsprintf((char *)&buffer[0], (char const *)format, vArgs);
    
	if(tryAgainNum == 0) tryAgainNum = 1;
    
	while(tryAgainNum)
	{
		delay_ms(100);
		InitFindData(&cmp1,(uint8_t *)resOk);
		InitFindData(&cmp2,(uint8_t *)resFails);
		InitTimeout(&tOut,timeout);
        
		MODEM_FlushBuffer();
		MODEM_Info("\r\nGSM->%s\r\n", buffer);
        
        // Send data
		COMM_Puts(buffer);
        
		while(CheckTimeout(&tOut))
		{
			if(RINGBUF_Get(&commBuf,&c) == 0)
			{
				if(FindData(&cmp1,c) == 0)
				{
					MODEM_Info("\r\nGSM->%s OK\r\n",buffer);
					return 0;
				}
				if(FindData(&cmp2,c) == 0)
				{
					break;
				}
			}
		}
		tryAgainNum--;
	}
    
	MODEM_Info("\r\nGSM->%s FAILS\r\n",buffer);
	return 0xff;
}

void MODEM_FlushBuffer(void)
{
	uint8_t c;
	while(RINGBUF_Get(&commBuf, &c)==0);
}

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
