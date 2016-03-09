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
#include "stm32f10x.h"

#include "systick.h"
#include "flash.h"


/* Private define ------------------------------------------------------------*/

// Default Setting
#define FIRMWARE_VERSION    "FAGV1.2"   // update rf
#define HARDWARE_VERSION    "HAGV1.1"

// GSM & GPRS
#define DEFAULT_BOSS_NUMBER	    "01655236473"
#define DEFAULT_SMS_PWD			"12345678"
#define DEFAULT_GPSR_APN		"internet"
#define DEFAULT_GPRS_USR		"mms"
#define DEFAULT_GPRS_PWD		"mms"

// DATA SERVER 0
#define DEFAULT_DSERVER0_NAME	"api.demeter.vn"
#define DEFAULT_DSERVER0_IP0    128
#define DEFAULT_DSERVER0_IP1	199
#define DEFAULT_DSERVER0_IP2	87
#define DEFAULT_DSERVER0_IP3	131
#define DEFAULT_DSERVER0_PORT	3000
#define DEFAULT_DSERVER0_USEIP	0

// DATA SERVER 1
#define DEFAULT_DSERVER1_NAME	"api.thingspeak.com"
#define DEFAULT_DSERVER1_IP0	184
#define DEFAULT_DSERVER1_IP1	106
#define DEFAULT_DSERVER1_IP2	153
#define DEFAULT_DSERVER1_IP3	149
#define DEFAULT_DSERVER1_PORT	80
#define DEFAULT_DSERVER1_USEIP	0

// DATA SERVER 2
#define DEFAULT_DSERVER2_NAME	"1945.vn"
#define DEFAULT_DSERVER2_IP0	112
#define DEFAULT_DSERVER2_IP1	78
#define DEFAULT_DSERVER2_IP2	4
#define DEFAULT_DSERVER2_IP3	219
#define DEFAULT_DSERVER2_PORT	4050
#define DEFAULT_DSERVER2_USEIP	1


// IMAGE SERVER
#define DEFAULT_ISERVER0_NAME	"api.demeter.vn"
#define DEFAULT_ISERVER0_IP0	128
#define DEFAULT_ISERVER0_IP1	199
#define DEFAULT_ISERVER0_IP2	87
#define DEFAULT_ISERVER0_IP3	131
#define DEFAULT_ISERVER0_PORT	3002
#define DEFAULT_ISERVER0_USEIP	0

// IMAGE SERVER
#define DEFAULT_ISERVER1_NAME	"1945.vn"
#define DEFAULT_ISERVER1_IP0	112
#define DEFAULT_ISERVER1_IP1	78
#define DEFAULT_ISERVER1_IP2	4
#define DEFAULT_ISERVER1_IP3	219
#define DEFAULT_ISERVER1_PORT	3002
#define DEFAULT_ISERVER1_USEIP	1

#define CONFIG_SIZE_GPRS_APN		16
#define CONFIG_SIZE_GPRS_USR		16
#define CONFIG_SIZE_GPRS_PWD		16
#define CONFIG_SIZE_SERVER_NAME		31
#define CONFIG_SIZE_SMS_PWD			16
#define CONFIG_SIZE_PHONE_NUMBER	16

#define CONFIG_SIZE_IP				32

#define SOCKET_NUM       2
#define DSERVER1_SOCKET  0
#define DSERVER2_SOCKET  1
#define FSERVER_SOCKET   2
#define ISERVER_SOCKET   3

#define DEFAULT_NORMAL_REPORT_INTERVAL	    TIME_SEC(600)   // 10 mins
#define DEFAULT_SAVING_REPORT_INTERVAL		TIME_SEC(1800)	// 30 mins
#define DEFAULT_SLEEP_TIMER		60

#define TIME_INITGPRS       TIME_SEC(20)
#define TIME_RECONSERVER    TIME_SEC(30)

#define TIME_GETCELLID   TIME_SEC(600)  // 10 mins

#define TIME_2SLEEP     TIME_SEC(300)  // 5 mins
#define TIME_SAVINGMODE TIME_SEC(3600)  // 1 hour

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
    
	// Data Server 0
	uint16_t DServer0IP[2];		                    /**< ip addr */
	uint8_t  DServer0Name[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t DServer0Port;	                        /**< port */	
    uint8_t  DServer0UseIP;
	
    // Data Server 0
	uint16_t DServer1IP[2];		                    /**< ip addr */
	uint8_t  DServer1Name[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t DServer1Port;	                        /**< port */	
    uint8_t  DServer1UseIP;
    
    // Data Server 0
	uint16_t DServer2IP[2];		                    /**< ip addr */
	uint8_t  DServer2Name[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t DServer2Port;	                        /**< port */
    uint8_t  DServer2UseIP;
    
    // Image Server 0
	uint16_t IServer0IP[2];		                    /**< ip addr */
	uint8_t  IServer0Name[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t IServer0Port;	                        /**< port */
    uint8_t  IServer0UseIP;

    // Image Server 1
	uint16_t IServer1IP[2];		                    /**< ip addr */
	uint8_t  IServer1Name[CONFIG_SIZE_SERVER_NAME];	/**< domain name */
	uint16_t IServer1Port;	                        /**< port */
    uint8_t  IServer1UseIP;
	
	
	uint8_t numCameras;				/**< number of supported cameras */
	uint8_t cameraCompression;		/**< compression value */
	uint8_t cameraWorkingStartTime;
	uint8_t cameraWorkingStopTime;
	uint16_t cameraInterval;
	uint16_t enableWarning;
									
	int32_t prepaidAccount;
    
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
void CFG_Load(void);
void CFG_Save(void);

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
