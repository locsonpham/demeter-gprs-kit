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
#include "sensor.h"

#include "adc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t start_sensor_flag = 0;
int windspeed_cnt = 0;
int latch_windspeed_cnt = 0;
int rainsum_cnt = 0;

// Temperature & Humidity from SHT1x
float temperature = 0;
float humidity = 0;
float temperature1 = 0;
float humidity1 = 0;
// BMP180
int32_t pa = 0;
// BH1750
int32_t lux = 0;
// Rain Sensor
int rainPercent = 0;
int rainPanelTemp = 0;
// Wind
int windDir = 0;
uint16_t windDir_disp = 0;  // display on plc
int windDir_ADC = 0;
int windSpeed = 0;
float fwindSpeed = 0;
// Rain Sum
int rainSum = 0;

// Voltage
float voltage = 0;



/* Private function prototypes -----------------------------------------------*/
void Sensor_PowerPinInit(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  * @param  
  * @param  
  * @retval 
  */
void Sensor_Init(void)
{
    //Sensor_PowerPinInit();
    OutPut_Init();
    
    // Init ADC
    ADC_Config();
    
    // WindSpeed
    WindSpeed_Init();
    
    // RainSum
    RainSum_Init();
}

// Rain sensor
void RainSensor_Init(void)
{
}

// DATA: 55,23\n
// percent: 55
// temperature: 23 *C
uint8_t RainSensor_Read(int *rainLevel, int *panelTemp)
{
    RS485_FlushBuffer();
    RS485_Send("!");
    if(!RINGBUF_Check(&rs485RingBuff, "DATA: ", 1000))
    {
        uint8_t i = 0;
        uint8_t c;
        uint8_t buf[10];
        int data[2] = {0};
        
        delay_ms(100);
        
		while(RINGBUF_Get(&rs485RingBuff, &c) == 0)
		{
				if((c == '\r') || (c == '\n')) break;
            
				buf[i] = c;
				i++;
				if(i >= sizeof(buf)) 
				{
					break;		
				}
		}
		buf[i] = '\0';
		sscanf((char *)buf,"%d,%d",&data[0], &data[1]);
        
        if ((data[0] >= 0) && (data[0] <= 100)) {
            *rainLevel = data[0];
        }
        else {
            return 0xff;
        }
        
        if ((data[1] >= 0) && (data[1] <= 100)) {
            *panelTemp = data[1];
        }
        else {
            return 0xff;
        }
        
        return 0;
    }
    
    *rainLevel = 0;
    *panelTemp = 0;
    return 1;
}

uint8_t TempHud_Read(float *_temp, float *_humid, int *_light, int *_pa)
{
    RS485_FlushBuffer();
    RS485_Send("@");
    if(!RINGBUF_Check(&rs485RingBuff, "DATA: ", 1000))
    {
        uint8_t i = 0;
        uint8_t c;
        uint8_t buf[64];
        int data[4] = {0};
        
        delay_ms(100);
        
		while(RINGBUF_Get(&rs485RingBuff, &c) == 0)
		{
				if((c == '\r') || (c == '\n')) break;
            
				buf[i] = c;
				i++;
				if(i >= sizeof(buf)) 
				{
					break;		
				}
		}
		buf[i] = '\0';
		sscanf((char *)buf,"%d,%d,%d,%d",&data[0], &data[1], &data[2], &data[3]);
        
        if ((data[0] >= 0) && (data[0] <= 1000)) {
            *_temp = (float)data[0] / 10;
        }
        else {
            return 0xff;
        }
        
        if ((data[1] >= 0) && (data[1] <= 1000)) {
            *_humid = (float)data[1] / 10;
        }
        else {
            return 0xff;
        }
        
        if (data[2] >= 0)
        {
            if (data[2] == 0) data[2]++;
            
            *_light = data[2];
        }
        
        if (data[3] >= 0)
        {
            *_pa = data[3];
        }
        
        return 0;
    }
    
    *_temp = 0;
    *_humid = 0;
    *_light = 0;
    *_pa = 0;
    
    return 1;
}

/**
  * @brief  Configures EXTI Line (connected to Pxx pin) in interrupt mode
  * @param  None
  * @retval None
  */
// Wind use EXTI
// PIN: PC4
void WindSpeed_Init(void)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    
    /* Enable GPIOx clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure Pxx pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Connect EXTI Line0 to Pxx pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);

    /* Configure EXTI Linex */
    EXTI_InitStructure.EXTI_Line = EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
/**
  * @brief  This function handles External line 4 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        /* Clear the EXTI line 0 pending bit */
        EXTI_ClearITPendingBit(EXTI_Line4);
        
        windspeed_cnt++;
    }
}

int ADC_2_Degree(uint16_t val)
{
    float _tmp = 0;
    
    _tmp = (float)val / 4095 * 360;
    
    if (_tmp < 0) _tmp += 360;
    if (_tmp > 360) _tmp -= 360;
    
    return (int)_tmp;
}

int _tmp[3] = {0, 0, 0};
void WindDir_Sampling(void)
{
    //Wind Dir
    int angle = 0;
    int _dir = 0;
    
    angle = ADC_2_Degree(ADC_Val[2]);
    
    // Convert to Direction
    if(angle < 22) 
        _dir = 0;
    else if (angle < 67) 
        _dir = 1;
    else if (angle < 112) 
        _dir = 2; 
    else if (angle < 157) 
        _dir = 3; 
    else if (angle < 212) 
        _dir = 4; 
    else if (angle < 247) 
        _dir = 5; 
    else if (angle < 292) 
        _dir = 6; 
    else if (angle < 337) 
        _dir = 7; 
    else
        _dir = 0;
    
    // deadband at 207 degree => _dir = 4
    _tmp[2] = _tmp[1];
    _tmp[1] = _tmp[0];
    
    if (sys_delay == 0)
    {
        if (_tmp[0] == 0 && _dir ==4) _dir = 0;
        if (_tmp[0] == 0 && _dir ==5) _dir = 0;
    }
    
    _tmp[0] = _dir;

    
    if ((_tmp[2] == _tmp[1]) && (_tmp[1] == _tmp[0]))
        windDir = _tmp[0];
}

void WindSpeed_Sampling(void)   // pulses per 1s
{
    static uint8_t cnt = 0;
    static uint8_t sample_period = 5;
    
    cnt++;
    
    if (cnt >= sample_period)
    {
        float tmp = 0;
        // V = P*(2.25/T) | P: pulses, T: sampling period, V: mile/h => V (km/h) = V(mile/h) * 1.60934
        tmp = (float)windspeed_cnt * 2.25 * 1.60934 / sample_period;
        
        latch_windspeed_cnt = windspeed_cnt; // for debug
        fwindSpeed = tmp; // for debug
        windSpeed = (uint32_t)tmp;
        windspeed_cnt = 0;
        cnt = 0;
    }
}

/**
  * @brief  This function handles External line 4 interrupt request.
  * @param  None
  * @retval None
  */
void WindDir_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Config as INPUT
    RCC_AHB1PeriphClockCmd(WIND_DIR_RCC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = WIND_SPEED_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(WIND_SPEED_Port, &GPIO_InitStruct);
}


void Read_WindDir(int *winddir)
{
    int dir = 0;
    float temp = ADC_Med[2];
    *winddir = temp;
}

/**
  * @brief  Configures EXTI Line (connected to Pxx pin) in interrupt mode
  * @param  None
  * @retval None
  */
// RAINSUM use EXTI
// PIN: PC5
void RainSum_Init(void)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    
    /* Enable GPIOx clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure Pxx pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Connect EXTI Line0 to Pxx pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);

    /* Configure EXTI Linex */
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Linex Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  This function handles External line 4 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        /* Clear the EXTI line x pending bit */
        EXTI_ClearITPendingBit(EXTI_Line5);
        
        rainSum++;
    }
}

void Sensor_PowerPinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Config as INPUT
    RCC_AHB1PeriphClockCmd(SEN_POWER_RCC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = SEN_POWER_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SEN_POWER_Port, &GPIO_InitStruct);
    
    SPOW_EN;
}

void OutPut_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Config as INPUT
    RCC_AHB1PeriphClockCmd(REL0_RCC | REL0_RCC | REL0_RCC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = REL0_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(REL0_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = REL1_Pin;
    GPIO_Init(REL1_Port, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = REL2_Pin;
    GPIO_Init(REL2_Port, &GPIO_InitStruct);
}

void Sensor_Reset(void)
{
    SPOW_DIS;
    delay_ms(1000);
    SPOW_EN;
    delay_ms(2000);
}

/**
  * @brief  This function Init SHT1x
  * @param  None
  * @retval None
  */
void TempHud_Init(void)
{
    // Because SHT1x and BMP180 use the same I2C Bus
    I2C_DeInit(I2C1);
    
    SHT_InitPin(GPIOB, GPIO_Pin_9, GPIOB, GPIO_Pin_8);
}

void TempHud1_Init(void)
{
    // Because SHT1x and BMP180 use the same I2C Bus
    I2C_DeInit(I2C1);
    
    SHT_InitPin(GPIOE, GPIO_Pin_12, GPIOE, GPIO_Pin_11);
//     SHT_InitPin(GPIOB, GPIO_Pin_9, GPIOB, GPIO_Pin_8);
}

// Pressure & Light
void PressLight_Init(void)
{
    BMP180_Init(100000);
    BH1750_Begin(BH1750_CONTINUOUS_HIGH_RES_MODE);
}

void Read_Voltage(float *volt)
{
    *volt = (float)ADC_Med[1] * (11.71 / 1291) + 0.3; // 1291 1.053 11.71 0.3
}

void Sensor_Update(void)
{
    static uint8_t err_check = 0;
    int sen_tick = 0;
    
    //----------------------------------------------------------------
    // Read Voltage
    Read_Voltage(&voltage);
    
    //----------------------------------------------------------------
    // Start read SHT1x
    TempHud_Init();
    
    // Read Temperature
    if (SHT_readTemperatureC(&temperature))
    {
        INFO("SHT Temp Error ...\n");
        err_check++;
        temperature = 0;
    }
    else 
    {
        if (temperature > 100 || temperature < 0)
        {
            INFO("SHT Temp Error ...\n");
            err_check++;
            temperature = 0;
        }            
    } 
    
    // Read Humidity
    if (SHT_readHumidity(&humidity))
    {
        INFO("SHT Humid Error ...\n");
        err_check++;
        humidity = 0;
    }
    else
    {
        if (humidity > 100 || humidity < 0)
        {
            INFO("SHT Humid Error ...\n");
            err_check++;
            humidity = 0;
        }
    }

    
    //----------------------------------------------------------------
    // Start BMP & BH
    PressLight_Init();
    delay_ms(100);
    
//     // read air pressure
//     if (BMP180_Read(&pa))
//     {
//         INFO("BMP180 Error\n");
//         err_check++;
//         pa = 0;
//     }
    
    // read light ambient
    int32_t _temp;
    if (BH1750_ReadLightLevel(&_temp) == 0)
    {
        lux = (int32_t)(_temp);
        if (lux == 0) lux = 0;
    }
    else
    {
        INFO("BH1750 Error\n");
        err_check++;
        lux = 0;
    }
    
    if (err_check >= 5)
    {
        INFO("Reset Sensor ... \n\n");
        err_check = 0;
        Sensor_Reset();
    }
    
    //----------------------------------------------------------------
    // WindSpeed
    // value is stored in windSpeed
    
    //----------------------------------------------------------------
    // Wind Dir
    // value is stored in windDir
    
    //----------------------------------------------------------------
    // Rain Sum
    // Auto increase in EXTI
    
}

/************************ END OF FILE ****/
