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
#include "sht1x.h"

#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_TypeDef* _datPort;
GPIO_TypeDef* _clkPort;
uint16_t _datPin, _clkPin;
uint8_t SHT_Status = 0;

#define SCK_MODE_OUT    pinMode(_clkPort, _clkPin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz)
#define SCK_H   GPIO_WriteBit(_clkPort, _clkPin, Bit_SET)
#define SCK_L   GPIO_WriteBit(_clkPort, _clkPin, Bit_RESET)

#define SDA_MODE_OUT    pinMode(_datPort, _datPin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz)
#define SDA_H   GPIO_WriteBit(_datPort, _datPin, Bit_SET)
#define SDA_L   GPIO_WriteBit(_datPort, _datPin, Bit_RESET)

#define SDA_MODE_IN     pinMode(_datPort, _datPin, GPIO_Mode_IPU, GPIO_Speed_50MHz)
#define SDA_IN  GPIO_ReadInputDataBit(_datPort, _datPin)

/* Private function prototypes -----------------------------------------------*/
float SHT_readTemperatureRaw(void);
int SHT_shiftIn(int _numBits);
int SHT_shiftOut(int command);
void SHT_sendCommandSHT(int _command);
void SHT_waitForResultSHT(void);
int SHT_getData16SHT(void);
void SHT_skipCrcSHT(void);
void SHT_delay_us(uint32_t time);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
  
void SHT_InitPin(GPIO_TypeDef* dataPort, uint16_t dataPin, GPIO_TypeDef* clkPort, uint16_t clkPin)
{
	_datPort = dataPort;
	_datPin = dataPin;
	_clkPort = clkPort;
	_clkPin = clkPin;
    
    SCK_MODE_OUT;
    SDA_MODE_OUT;
    
    SCK_L;
    SDA_H;
    
    #ifdef USE_TEMP_EN
    pinMode(TEMP_EN_Port, TEMP_EN_Pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    TEMP_EN;
    #endif
    
    delay_ms(100);
}

void SHT_delay_us(uint32_t time)
{
	time *= 14;	// 72 Mhz/5 = 14; 5 for while loop
	while (time--);
}


/* ================  Public methods ================ */

/**
 * Reads the current temperature in degrees Celsius
 */
float SHT_readTemperatureC()
{
    int _val;                // Raw value returned from sensor
    float _temperature;      // Temperature derived from raw value

    // Conversion coefficients from SHT15 datasheet
    const float D1 = -39.66;  // for 14 Bit @ 3.3V
    const float D2 =   0.01; // for 14 Bit DEGC

    // Fetch raw value
    _val = SHT_readTemperatureRaw();

    if (SHT_Status == 1) return -1;  // Error

    // Convert raw value to degrees Celsius
    _temperature = (_val * D2) + D1;

    return (_temperature);
}

/**
 * Reads the current temperature in degrees Fahrenheit
 */
float SHT_readTemperatureF()
{
  int _val;                 // Raw value returned from sensor
  float _temperature;       // Temperature derived from raw value

  // Conversion coefficients from SHT15 datasheet
  const float D1 = -39.6;   // for 14 Bit @ 5V
  const float D2 =   0.018; // for 14 Bit DEGF

  // Fetch raw value
  _val = SHT_readTemperatureRaw();

  // Convert raw value to degrees Fahrenheit
  _temperature = (_val * D2) + D1;

  return (_temperature);
}

/**
 * Reads current temperature-corrected relative humidity
 */
float SHT_readHumidity()
{
    int _val;                    // Raw humidity value returned from sensor
    float _linearHumidity;       // Humidity with linear correction applied
    float _correctedHumidity;    // Temperature-corrected humidity
    float _temperature;          // Raw temperature value

    // Conversion coefficients from SHT15 datasheet
    const float C1 = -4.0;       // for 12 Bit
    const float C2 =  0.0405;    // for 12 Bit
    const float C3 = -0.0000028; // for 12 Bit
    const float T1 =  0.01;      // for 14 Bit @ 5V
    const float T2 =  0.00008;   // for 14 Bit @ 5V

    // Command to send to the SHT1x to request humidity
    int _gHumidCmd = 0x05;

    // Fetch the value from the sensor
    SHT_sendCommandSHT(_gHumidCmd);
    SHT_waitForResultSHT();
    _val = SHT_getData16SHT();
    SHT_skipCrcSHT();

    // Apply linear conversion to raw value
    _linearHumidity = C1 + C2 * _val + C3 * _val * _val;

    // Get current temperature for humidity correction
    _temperature = SHT_readTemperatureC();

    // Correct humidity value for current temperature
    _correctedHumidity = (_temperature - 25.0 ) * (T1 + T2 * _val) + _linearHumidity;
    
    if (SHT_Status == 1) return -1;

    return (_correctedHumidity);
}


/* ================  Private methods ================ */

/**
 * Reads the current raw temperature value
 */
float SHT_readTemperatureRaw()
{
    int _val;

    // Command to send to the SHT1x to request Temperature
    int _gTempCmd  = 0x03;

    SHT_sendCommandSHT(_gTempCmd);
    SHT_waitForResultSHT();
    
    if (SHT_Status == 1) return 0;  // Sensor Error
    
    _val = SHT_getData16SHT();
    SHT_skipCrcSHT();

    return (_val);
}

/**
 */
int SHT_shiftIn(int _numBits)
{
    int ret = 0;
    int i;
    
    SDA_MODE_IN;

    for (i=0; i<_numBits; ++i)
    {
        SCK_H;
        __nop();
        ret = ret*2 + SDA_IN;
        __nop(); __nop();
        SCK_L;
        __nop();
    }

    return(ret);
}

int SHT_shiftOut(int command)
{
    SDA_MODE_OUT;
    
    for (uint8_t i=0; i<8; i++)
    {
        if (((uint8_t)command & 0x80) != 0) SDA_H;
        else SDA_L;
        __nop();
        SCK_H;
        __nop(); __nop(); __nop();
        SCK_L;
        __nop();
        
        command = command * 2;
    }
    
    return 0;
}

/**
 */
void SHT_sendCommandSHT(int _command)
{
    int ack;

    // Config datPin as output
    pinMode(_datPort, _datPin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    pinMode(_clkPort, _clkPin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    SHT_delay_us(5);
    
//     // Reset Connection
//     GPIO_WriteBit(_datPort, _datPin, Bit_SET);
//     for (uint8_t i = 0; i < 9; i++)
//     {
//         GPIO_WriteBit(_clkPort, _clkPin, Bit_SET);
//         SHT_delay_us(5);
//         GPIO_WriteBit(_clkPort, _clkPin, Bit_RESET);
//         SHT_delay_us(5);
//     }

    // Transmission Start
    SDA_H;
    SCK_L;
    __nop();
    SCK_H;
    __nop();
    SDA_L;
    __nop();
    SCK_L;
    __nop(); __nop(); __nop();
    SCK_H;
    __nop();
    SDA_H;
    __nop();
    SCK_L;
    

    // The command (3 msb are address and must be 000, and last 5 bits are command)
    SHT_shiftOut(_command);
    
    // Verify get correct ACK
    SDA_MODE_IN;
    __nop();
    SCK_H;
    __nop(); __nop(); __nop();
    
    if (SDA_IN == Bit_RESET)
    {
        SHT_Printf("1. Send CMD OK\n");
        SHT_Status = 0; // Sensor OK
    }
    else
    {
        SHT_Printf("1. Send CMD Error\n");
        SHT_Status = 1; // Sensor Error
    }
    
    SCK_L;
    __nop(); __nop(); __nop();
    if (SDA_IN == Bit_SET)
    {
        SHT_Printf("2. Start measurement\n");
    }
}

/**
 */
void SHT_waitForResultSHT()
{
    int ack; 
    
    // Wait for SHTx completion of measurement
    SDA_MODE_IN;
    __nop();
    uint8_t timeout = 100;
    while (timeout--)
    {
        if (SDA_IN == Bit_RESET) 
        {
            SHT_Printf("3. Data is ready\n");
            SHT_Status = 0; // Sensor OK
            break;
        }
        delay_ms(10);
    }
    
    if (SDA_IN == Bit_SET || timeout == 0)
    {
        SHT_Printf("3. Timeout Error\n");
        SHT_Status = 1; // Sensor Error
    }
}

/**
 */
int SHT_getData16SHT()
{
    int val;

    // Get the most significant bits
    SDA_MODE_IN;
    val = SHT_shiftIn(8);
    val *= 256;

    // Send the required ack
    SDA_MODE_OUT;
    SDA_L;
    __nop();
    SCK_H;
    __nop(); __nop(); __nop();
    SCK_L;
    __nop();

    // Get the least significant bits
    SDA_MODE_IN;
    val |= SHT_shiftIn(8);

    return val;
}

/**
 */
void SHT_skipCrcSHT()
{
    // Skip acknowledge to end trans (no CRC)
    SDA_MODE_OUT;

    SDA_H;
    __nop();
    SCK_H;
    __nop(); __nop();
    SCK_L;
    __nop();
}


/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
