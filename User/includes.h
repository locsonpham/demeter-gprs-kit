/**
  ******************************************************************************
  * @file    includes.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__INCLUDES_H
#define __INCLUDES_H

/* Private define ------------------------------------------------------------*/
#define DEBUG_VIA_UART

#ifdef DEBUG_VIA_UART
#define INFO(...)		printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

// uncomment use SD Card or RS485
//#define USE_SD
//#define USE_RS485
//#define USE_ETHERNET

#define USE_HMI
#define USE_GSM
#define USE_GPRS


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

// TIME
#include "sys_time.h"
#include "m_time.h"

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "integer.h" // in fatfs for port in RS485

#include <stdbool.h>
typedef   bool BOOL;
#ifndef FALSE
#define FALSE false
#define TRUE true
#endif

// User header file
#include "main.h"
#include "system_config.h"
#include "interrupt.h"

// Peripheral
#include "systick.h"
#include "uart.h"
#include "spi.h"
#include "output.h"
#include "input.h"
#include "adc.h"
#include "flash.h"

// Header comm
#include "ringbuf.h"

#include "rf433_com.h"
#include "rs485_com.h"

// GSM
#include "sim900.h"
#include "modem.h"
#include "at_command_parser.h"

// // ETHERNET
// #include "EtherShield.h"

// MODBUS
#include "rs232_com.h"
#include "mb.h"
#include "mbport.h"
#include "mbhandler.h"

// SD
#include "ff.h"
#include "sdcard.h"

// Temp Hud Sensor




typedef uint8_t byte;

/* Extern variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
