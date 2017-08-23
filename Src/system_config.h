/**
  ******************************************************************************
  * @file    spi.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef	__SYSTEM_CONFIG_H
#define __SYSTEM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Private define ------------------------------------------------------------*/

// Default Setting
#define FIRMWARE_VERSION    "1.0"

// GSM & GPRS
#define DEFAULT_BOSS_NUMBER	    "01655236473"
#define DEFAULT_SMS_PWD			"12345678"
#define DEFAULT_GPSR_APN		"internet"
#define DEFAULT_GPRS_USR		"mms"
#define DEFAULT_GPRS_PWD		"mms"

// DATA SERVER
#define DEFAULT_DSERVER_NAME	""
#define DEFAULT_DSERVER_IP0		128
#define DEFAULT_DSERVER_IP1		199
#define DEFAULT_DSERVER_IP2		107
#define DEFAULT_DSERVER_IP3		77
#define DEFAULT_DSERVER_PORT	3000
#define DEFAULT_DSERVER_USEIP	0

// FIRMWARE SERVER
#define DEFAULT_FSERVER_NAME	""
#define DEFAULT_FSERVER_IP0		183
#define DEFAULT_FSERVER_IP1		80
#define DEFAULT_FSERVER_IP2		144
#define DEFAULT_FSERVER_IP3		205
#define DEFAULT_FSERVER_PORT	5000
#define DEFAULT_FSERVER_USEIP	1

#define CONFIG_SIZE_GPRS_APN		16
#define CONFIG_SIZE_GPRS_USR		16
#define CONFIG_SIZE_GPRS_PWD		16
#define CONFIG_SIZE_SERVER_NAME		31
#define CONFIG_SIZE_SMS_PWD			16
#define CONFIG_SIZE_VIN				20
#define CONFIG_SIZE_PLATE_NO		12
#define CONFIG_SIZE_DRIVER_NAME		32
#define CONFIG_SIZE_LICENSE_NO		12
#define CONFIG_SIZE_ISSUE_DATE		11
#define CONFIG_SIZE_EXPIRY_DATE		11
#define CONFIG_SIZE_PHONE_NUMBER	16

#define CONFIG_SIZE_IP				32

#define SOCKET_NUM  1

#define SMS_HEADER  GOGO

#define DSERVER1_SOCKET  0
#define DSERVER2_SOCKET  1
#define FSERVER_SOCKET   2

#define DATA_SERVER_TIME_MAX_RESEND		    600   // 10 mins
#define FIRMWARE_SERVER_TIME_MAX_RESEND		600   // 10 mins
#define IMAGE_SERVER_TIME_MAX_RESEND		600   // 10 mins

#define DEFAULT_NORMAL_REPORT_INTERVAL	    TIME_SEC(60)   // 10s
#define DEFAULT_STOP_REPORT_INTERVAL		10	//  5mins
#define DEFAULT_SLEEP_TIMER		60

#define TIME_INITGPRS       TIME_SEC(10)
#define TIME_RECONSERVER    TIME_SEC(10)

#define TIME_GETCELLINFO   TIME_SEC(300)  // 10 mins

#define TIME_2SLEEP     TIME_SEC(900)  // 15 mins
#define TIME_SAVINGMODE TIME_SEC(3600)  // 1 hour

#define CONFIG_SPEED_STOP   5

#define TIME_INTO_PROTECTMODE   TIME_SEC(300)   // 3 mins

typedef struct __attribute__((packed))
{
	int8_t imei[18];
	
	int8_t id[18];
	
	int8_t smsPwd[CONFIG_SIZE_SMS_PWD];					/**< SMS config password */
	
	int8_t whiteCallerNo[CONFIG_SIZE_PHONE_NUMBER];		/**< */
	
	// GPRS config
	int8_t gprsApn[CONFIG_SIZE_GPRS_APN];
	int8_t gprsUsr[CONFIG_SIZE_GPRS_USR];
	int8_t gprsPwd[CONFIG_SIZE_GPRS_PWD];
	uint16_t runReportInterval;
	uint16_t stopReportInterval;
    
	// current server config
	uint8_t curDServer;
	// primary server config
	uint16_t priDserverIp[2];		/**< ip addr */
	uint8_t  priDserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t priDserverPort;	/**< port */	
    
	// secondary server config
	uint16_t secDserverIp[2];		/**< ip addr */
	uint8_t  secDserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t secDserverPort;	/**< port */	
	uint8_t  dServerUseIp;	/**<>**/
    
	// current firmware server config
	uint8_t curFServer;
	// primary server config
	uint16_t priFserverIp[2];		/**< ip addr */
	uint8_t  priFserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t priFserverPort;	/**< port */	
    
	// secondary server config
	uint16_t secFserverIp[2];		/**< ip addr */
	uint8_t  secFserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t secFserverPort;	/**< port */	
	uint8_t  fServerUseIp;	/**<>**/
	
	uint8_t curInfoServer;
	// primary server config
	uint16_t priInfoServerIp[2];		/**< ip addr */
	uint8_t  priInfoServerName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t priInfoServerPort;	/**< port */	
    
	// secondary server config
	uint16_t secInfoServerIp[2];		/**< ip addr */
	uint8_t  secInfoServerName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t secInfoServerPort;	/**< port */	
	uint8_t  infoServerUseIp;	/**<>**/
	
//  current driver info
	uint8_t driverIndex;
	//DRIVER_INFO driverList[CONFIG_MAX_DRIVERS];
	// vehicle info
	int8_t plateNo[CONFIG_SIZE_PLATE_NO];
	int8_t vin[CONFIG_SIZE_VIN];
	uint16_t speedSensorRatio;							/**< pulses/Km/h */
	uint16_t useGpsSpeed;
	
	uint16_t a1Mode;					/**< raw mode = 0, percentage mode = 1 */
	uint16_t a1LowerBound;			/**< lower bound value for percentage mode */
	uint16_t a1UpperBound;			/**< upper bound value for percentage mode */
	
	uint16_t a2Mode;
	uint16_t a2LowerBound;
	uint16_t a2UpperBound;
	
	// current image server config
	uint8_t curIServer;
	// primary image server config
	uint16_t priIserverIp[2];		/**< ip addr */
	uint8_t  priIserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t priIserverPort;	/**< port */	
	// secondary image server config
	uint16_t secIserverIp[2];		/**< ip addr */
	uint8_t  secIserverName[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t secIserverPort;	/**< port */	
	uint8_t  iServerUseIp;	/**<>**/
	
	
	uint8_t numCameras;				/**< number of supported cameras */
	uint8_t cameraCompression;		/**< compression value */
	uint8_t cameraWorkingStartTime;
	uint8_t cameraWorkingStopTime;
	uint16_t cameraInterval;
	uint16_t enableWarning;
	uint16_t cameraEvents;			/**< camera capture events map
									bit 0-1: DIN1 assert/deassert event
									bit 2-3: DIN2 assert/deassert event
									bit 4-5: DIN3 assert/deassert event
									bit 6-7: DIN4 assert/deassert event
									bit 8: periodically
									bit 9-15: reserved */
	uint16_t speedLimit;									/**< vehicle speed limit */
	uint32_t drivingTimeLimit;
	uint32_t totalDrivingTimeLimit;
	uint32_t baseMileage;	
									
	int32_t prepaidAccount;
	uint16_t featureSet;									/**< feature set mapping 
														bit0: Backup battery support
														bit1: RS-232 camera support
														bit2: RS-485 camera support
														bit3: ECU status report support
														bit4: RFID card reader support 
														bit5: Beep warning enable
														bit6: Clear log
														bit7: message fist in fist out(FIFO)
														bit8: message send status disable 
														bit9: DOOR IO logic invert
														bit10: ACC IO logic invert
														bit11: SOS IO logic invert
														bit12: AIRCON IO logic invert
														*/
	uint16_t lastError;
	uint16_t testMode;
	
	uint32_t language;			
	uint32_t dummy[3];
	uint32_t sleepTimer;
	
	uint8_t accountAlarmCheck;
	
	uint8_t upgradeTimeStamp[20];	/**< firmware upgrade timestamp */
	uint8_t infoString[200];
	uint32_t crc;
} CONFIG_POOL;

extern CONFIG_POOL sysCfg;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
