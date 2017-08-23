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
#ifndef SENSOR_H__
#define SENSOR_H__

#include "defines.h"

/* Private define ------------------------------------------------------------*/
#define WIND_SPEED_RCC  RCC_AHB1Periph_GPIOC
#define WIND_SPEED_Port GPIOC
#define WIND_SPEED_Pin  GPIO_Pin_4

#define WIND_DIR_RCC    RCC_AHB1Periph_GPIOA
#define WIND_DIR_Port   GPIOA
#define WIND_DIR_Pin    GPIO_Pin_2

#define RAINSUM_RCC     RCC_AHB1Periph_GPIOC
#define RAINSUM_Port    GPIOC
#define RAINSUM_Pin     GPIO_Pin_5

#define SEN_POWER_RCC   RCC_AHB1Periph_GPIOE
#define SEN_POWER_Port  GPIOE
#define SEN_POWER_Pin   GPIO_Pin_2

#define REL0_RCC   RCC_AHB1Periph_GPIOE
#define REL0_Port  GPIOE
#define REL0_Pin   GPIO_Pin_5

#define REL1_RCC   RCC_AHB1Periph_GPIOE
#define REL1_Port  GPIOE
#define REL1_Pin   GPIO_Pin_4

#define REL2_RCC   RCC_AHB1Periph_GPIOE
#define REL2_Port  GPIOE
#define REL2_Pin   GPIO_Pin_3

#define REL0_ON     GPIO_WriteBit(REL0_Port, REL0_Pin, Bit_SET)
#define REL0_OFF    GPIO_WriteBit(REL0_Port, REL0_Pin, Bit_RESET)
#define REL0_STT    GPIO_ReadOutputDataBit(REL0_Port, REL0_Pin)

#define REL1_ON     GPIO_WriteBit(REL1_Port, REL1_Pin, Bit_SET)
#define REL1_OFF    GPIO_WriteBit(REL1_Port, REL1_Pin, Bit_RESET)
#define REL1_STT    GPIO_ReadOutputDataBit(REL1_Port, REL1_Pin)

#define REL2_ON     GPIO_WriteBit(REL2_Port, REL2_Pin, Bit_SET)
#define REL2_OFF    GPIO_WriteBit(REL2_Port, REL2_Pin, Bit_RESET)
#define REL2_STT    GPIO_ReadOutputDataBit(REL2_Port, REL2_Pin)

#define SPOW_EN     GPIO_WriteBit(SEN_POWER_Port, SEN_POWER_Pin, Bit_SET)
#define SPOW_DIS    GPIO_WriteBit(SEN_POWER_Port, SEN_POWER_Pin, Bit_RESET)

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern uint8_t start_sensor_flag;

extern float temperature;
extern float humidity;
extern float temperature1;
extern float humidity1;
extern int32_t pa;
extern int32_t lux;
extern int rainPercent;
extern int rainPanelTemp;
extern int windDir;
extern int windSpeed;
extern int rainSum;
extern float voltage;
extern int windspeed_cnt;
extern float fwindSpeed;
extern int latch_windspeed_cnt;
extern int windDir_ADC;
extern uint16_t windDir_disp;
extern int rainsum_cnt;
extern int _tmp[3];

/* Private function prototypes -----------------------------------------------*/
void Sensor_Init(void);
void Sensor_Update(void);
void OutPut_Init(void);

// Rain sensor
void RainSensor_Init(void);
uint8_t RainSensor_Read(int *rainLevel, int *panelTemp);

// Wind
void WindSpeed_Init(void);
void WindSpeed_Sampling(void);
void Read_WindSpeed(void);

void WindDir_Sampling(void);
void WindDir_Init(void);
void Read_WindDir(int *winddir);
int ADC_2_Degree(uint16_t val);

// Rain Sum
void RainSum_Init(void);
void Read_RainSum(int *rainsum);

// Temp & Humid
void TempHud_Init(void);
void Read_Temperature(void);
void Read_Humidity(void);

// Pressure & Light
void PressLight_Init(void);
void Read_Pressure(void);
void Read_Light(void);

#endif

/************************ END OF FILE ****/
