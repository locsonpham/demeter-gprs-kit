#include "tracking.h"

#include "sys_time.h"

__IO uint16_t CCR1_Val = 50000;
__IO uint16_t CCR2_Val = 27309;
__IO uint16_t CCR3_Val = 13654;
__IO uint16_t CCR4_Val = 6826;
uint16_t PrescalerValue = 0;
uint16_t capture = 0;

TRACKER_MSG_TYPE trackerMsg;
MSG_STATUS_RECORD	logRecord, logRecordTemp;

uint8_t serverSendDataFlag = 0;
uint8_t flagSendStatus = 0;
uint8_t sendRainSum = 0;

/**
  * @brief  Configure the TIM IRQ Handler.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* Enable the TIM3 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xff;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0xff;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Tracking_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    TIM_Config();
    
    /* -----------------------------------------------------------------------
    TIM3 Configuration: Output Compare Timing Mode:

    In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM3CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM3CLK = HCLK / 2 = SystemCoreClock /2
          
    To get TIM3 counter clock at 6 MHz, the prescaler is computed as follows:
       Prescaler = (TIM3CLK / TIM3 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /6 MHz) - 1
                                              
    CC1 update rate = TIM3 counter clock / CCR1_Val = 146.48 Hz
    ==> Toggling frequency = 73.24 Hz

    C2 update rate = TIM3 counter clock / CCR2_Val = 219.7 Hz
    ==> Toggling frequency = 109.8 Hz

    CC3 update rate = TIM3 counter clock / CCR3_Val = 439.4 Hz
    ==> Toggling frequency = 219.7 Hz

    CC4 update rate = TIM3 counter clock / CCR4_Val = 878.9 Hz
    ==> Toggling frequency = 439.4 Hz

    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
    ----------------------------------------------------------------------- */   


    /* Compute the prescaler value */
    PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 10000) - 1;

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);

    /* Output Compare Timing Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM3, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);

    /* TIM Interrupts enable */
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    
    /* Enable the TIM3 global Interrupt */
    NVIC_SetPriority(TIM3_IRQn, TIM3_IRQ_Group);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM3_IRQ_Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);
}

void addLogPackage(void)
{
    /* LOGGER ---------------------------------------------------------------------*/

    if (sysTime.year == 1970) return;

    ////////////////////////////////////////////////////////////
    // ASCII protocol
    ///////////////////////////////////////////////////////////
    static uint16_t rpSampleCount = 0;
    uint32_t u32Temp;
    
    rpSampleCount++;
    
    if((rpSampleCount % 10) == 0)   // 10s
    {
        serverSendDataFlag = 1;
    }

    if(serverSendDataFlag || flagSendStatus)
    {
        serverSendDataFlag = 0;
        
        flagSendStatus = 0;
        logRecord.currentTime = sysTime;
        logRecord.temp = temperature;
        logRecord.humid = humidity;
        logRecord.temp1 = temperature1;
        logRecord.humid1 = humidity1;
        logRecord.lux = lux;
        logRecord.pa = pa;
        logRecord.rainPercent = rainPercent;
        logRecord.rainPanelTemp = rainPanelTemp;
        
        if (rpSampleCount % 60 == 0)
        {
            logRecord.rainSum = rainSum;
            sendRainSum = 1;
        }
        else
        {
            logRecord.rainSum = 0;
        }
        
        logRecord.windSpeed = windSpeed;
        logRecord.windDir = windDir;
        logRecord.voltage = voltage;
        
        logRecord.out0 = REL0_STT;
        logRecord.out1 = REL1_STT;
        logRecord.out2 = REL2_STT;
        
        // Save LOG to Flash
        logRecord.serverSent = 0;
        if(trackerMsg.newMsgIsEmpty)
        {
            trackerMsg.newMsgIsEmpty = 0;
            trackerMsg.newMsg = logRecord;
            logRecord.serverSent = 1;
        }
        
        logRecordTemp = logRecord;
    }
}

/**
  * @brief  This function handles TIM3 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
    {
        static uint8_t i = 0;
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

        capture = TIM_GetCapture1(TIM3);
        TIM_SetCompare1(TIM3, capture + CCR1_Val);
    }
}

// Use for Demeter
uint16_t AddTrackerPacket(MSG_STATUS_RECORD *logRecordSend, char *data, uint32_t *len)
{
	char *buff = data;
	char tmpBuf[32];
	DATE_TIME time;
	buff[0] = 0;
	data[0] = 0;
    
    if (logRecordSend->currentTime.year == 0) return 0xff;
    
    TIME_Copy(&time, &logRecordSend->currentTime);
    
	time = sysTime;
	//TIME_AddSec(&time, -3600*7);
	//device ID
	sprintf(tmpBuf, "{\"dev\":\"%s\",", modemId);
	strcat(buff, tmpBuf);
	// Timestamp
    int32_t timestamp = TIME_GetSec(&time);
	sprintf(tmpBuf, "\"tim\":%d,", timestamp);
	strcat(buff, tmpBuf);

	// Temperature
    sprintf(tmpBuf, "\"temperature\":%0.1f,", logRecordSend->temp);
    strcat(buff, tmpBuf);
	// Humid
    sprintf(tmpBuf, "\"humidity\":%0.1f,", logRecordSend->humid);
    strcat(buff, tmpBuf);
    // Temperature
    sprintf(tmpBuf, "\"temperature1\":%0.1f,", logRecordSend->temp1);
    strcat(buff, tmpBuf);
	// Humid
    sprintf(tmpBuf, "\"humidity1\":%0.1f,", logRecordSend->humid1);
    strcat(buff, tmpBuf);
    // Lux
    sprintf(tmpBuf, "\"lux\":%d,", logRecordSend->lux);
    strcat(buff, tmpBuf);
    // pa
    sprintf(tmpBuf, "\"pa\":%d,", logRecordSend->pa);
    strcat(buff, tmpBuf);
    // rain sensity
    sprintf(tmpBuf, "\"rainsensity\":%d,", logRecordSend->rainPercent);
    strcat(buff, tmpBuf);
    // rain panel temperature
    sprintf(tmpBuf, "\"rainpanel\":%d,", logRecordSend->rainPanelTemp);
    strcat(buff, tmpBuf);
    // rain summary
    if (sendRainSum)
    {
        sendRainSum = 0;
        
        sprintf(tmpBuf, "\"rainsum\":%d,", logRecordSend->rainSum);
        strcat(buff, tmpBuf);
    }
    // wind speed
    sprintf(tmpBuf, "\"windspd\":%d,", logRecordSend->windSpeed);
    strcat(buff, tmpBuf);
    // wind dir
    sprintf(tmpBuf, "\"winddir\":%d,", logRecordSend->windDir);
    strcat(buff, tmpBuf);
    // rel0
    sprintf(tmpBuf, "\"rel0\":%d,", logRecordSend->out0);
    strcat(buff, tmpBuf);
    // rel1
    sprintf(tmpBuf, "\"rel1\":%d,", logRecordSend->out1);
    strcat(buff, tmpBuf);
    // rel2
    sprintf(tmpBuf, "\"rel2\":%d,", logRecordSend->out2);
    strcat(buff, tmpBuf);
    // voltage
    sprintf(tmpBuf, "\"volt\":%0.1f,", logRecordSend->voltage);
    strcat(buff, tmpBuf);
    
	//firmware
	sprintf(tmpBuf, "\"fwr\":\"%s\"","0.0.1");
	strcat(buff, tmpBuf);

	strcat(buff, "}");
	*len = strlen(buff);

	return 0;
}

// Check if there is packet to send
uint32_t CheckTrackerMsgIsReady(void)
{
	if((!trackerMsg.oldMsgIsEmpty || !trackerMsg.newMsgIsEmpty))
		return 1;
    
	return 0;
}

// Get last tracking message
uint32_t GetLastTrackerMsg(char *buff,uint32_t *len)
{
	if(!trackerMsg.lastMsgIsEmpty)
	{
		AddTrackerPacket(&trackerMsg.lastMsg, buff, len);
		trackerMsg.lastMsgIsEmpty = 1;
// 		PrintTrackerInfo(&trackerMsg.lastMsg);
		return 0;
	}
    
	*len = 0;
	return 1;
}

// Get tracking message
uint32_t GetTrackerMsg(char *buff,uint32_t *len)
{
	if((!trackerMsg.oldMsgIsEmpty || !trackerMsg.newMsgIsEmpty))
	{
        INFO("Message Avaiable\n");
		if(!trackerMsg.newMsgIsEmpty)   // new message income
		{
            //INFO("NEW Message Avaiable\n");
            AddTrackerPacket(&logRecordTemp, buff, len);
 			//AddTrackerPacket(&trackerMsg.newMsg, buff, len);
//             INFO("\nNEW LOG DATA\n");
//             INFO("Date: %02d/%02d/%02d\n", logRecordTemp.currentTime.mday, logRecordTemp.currentTime.month, logRecordTemp.currentTime.year);
//             INFO("Time:  %02d:%02d:%02d\n", logRecordTemp.currentTime.hour, logRecordTemp.currentTime.min, logRecordTemp.currentTime.sec);
//             INFO("lat: %f\nlon: %f\nhdop: %f\nspeed: %f\ncrc: %d\n\n", logRecordTemp.lat, logRecordTemp.lon, logRecordTemp.hdop, logRecordTemp.speed, logRecordTemp.crc);
//             INFO("\nNEW LOG DATA\n");
//             INFO("Date: %02d/%02d/%02d\n", trackerMsg.newMsg.currentTime.mday, trackerMsg.newMsg.currentTime.month, trackerMsg.newMsg.currentTime.year);
//             INFO("Time:  %02d:%02d:%02d\n", trackerMsg.newMsg.currentTime.hour, trackerMsg.newMsg.currentTime.min, trackerMsg.newMsg.currentTime.sec);
//             INFO("lat: %f\nlon: %f\nhdop: %f\nspeed: %f\ncrc: %d\n\n", trackerMsg.newMsg.lat, trackerMsg.newMsg.lon, trackerMsg.newMsg.hdop, trackerMsg.newMsg.speed, trackerMsg.newMsg.crc);
			trackerMsg.lastMsg = trackerMsg.newMsg;
			trackerMsg.newMsgIsEmpty = 1;
			trackerMsg.lastMsgIsEmpty = 0;
		}
		else    // get older message not send yet
		{
            //INFO("OLD Message Avaiable\n");
			//AddTrackerPacket(&trackerMsg.oldMsg, buff, len);
//             INFO("\nOLD LOG DATA\n");
//             INFO("Date: %02d/%02d/%02d\n", trackerMsg.oldMsg.currentTime.mday, trackerMsg.oldMsg.currentTime.month, trackerMsg.oldMsg.currentTime.year);
//             INFO("Time:  %02d:%02d:%02d\n", trackerMsg.oldMsg.currentTime.hour, trackerMsg.oldMsg.currentTime.min, trackerMsg.oldMsg.currentTime.sec);
//             INFO("lat: %f\nlon: %f\nhdop: %f\nspeed: %f\ncrc: %d\n\n", trackerMsg.oldMsg.lat, trackerMsg.oldMsg.lon, trackerMsg.oldMsg.hdop, trackerMsg.oldMsg.speed, trackerMsg.oldMsg.crc);
			trackerMsg.lastMsg = trackerMsg.oldMsg;
			trackerMsg.oldMsgIsEmpty = 1;
			trackerMsg.lastMsgIsEmpty = 0;
		}
		return 0;
	}
    
    // no message available
	*len = 0;
	return 1;
}