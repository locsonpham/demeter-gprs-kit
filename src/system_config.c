#include "system_config.h"

#include "systick.h"
#include "SPIFlash.h"
#include "db.h"
#include "flash.h"
#include "defines.h"

CONFIG_POOL sysCfg;

void CFG_Load(void)
{
    uint32_t saveFlag = 0,u32Temp = 0;
    int16_t i;
    uint8_t *u8Pt;

    //FlashInit();
//     DrvFMC_EnableISP();
//     DrvFMC_EnableConfigUpdate();
//     DrvFMC_BootSelect (E_FMC_APROM);
//     DrvFMC_EnablePowerSaving ( );
//     DrvFMC_EnableLowSpeedMode ( );
//     DrvFMC_EnablePowerSaving ( );

//     DrvFMC_WriteConfig (  0xF89FFF7E , 0x0001F000);

//     memcpy(&sysCfg, (void*)CONFIG_AREA_START, sizeof(CONFIG_POOL));
    SPIFlashReadArray(CONFIG_ADDR, (uint8_t*)&sysCfg, sizeof(CONFIG_POOL));

    u8Pt = (uint8_t *)&sysCfg;
    
    u32Temp = 0;
    for(i = 0; i < sizeof(CONFIG_POOL)-sizeof(sysCfg.crc); i++)
    {
        u32Temp += u8Pt[i];
    }
    if(u32Temp != sysCfg.crc)
    {
        INFO("FLASH->WRONG CRC\n");
        memset((void*)&sysCfg, 0xFF, sizeof sysCfg);
        saveFlag = 1;
    }
    else
    {        
        INFO("FLASH->OK CRC\n");
        return;
    }
    if((uint8_t)sysCfg.imei[0] == 0xFF)
    {
        saveFlag = 1;
        sysCfg.imei[0] = 0;
    }

    if((uint8_t)sysCfg.id[0] == 0xFF)
    {
        saveFlag = 1;
        sysCfg.id[0] = 'D';
        sysCfg.id[1] = 'A';
        sysCfg.id[2] = 'T';
        sysCfg.id[3] = 'A';
        sysCfg.id[4] = 'L';
        sysCfg.id[5] = 'O';
        sysCfg.id[6] = 'G';
        sysCfg.id[7] = 0;
    }

    // load default SMS config password
    if((uint8_t)sysCfg.smsPwd[0] == 0xFF || sysCfg.smsPwd[0] == 0)
    {
        saveFlag = 1;
        strcpy((char *)sysCfg.smsPwd, DEFAULT_SMS_PWD);
    }

    if((uint8_t)sysCfg.whiteCallerNo[0] == 0xFF)
    {
        saveFlag = 1;
        strcpy((char *)sysCfg.whiteCallerNo, DEFAULT_BOSS_NUMBER);
    }

    // load default GPRS config
    if((uint8_t)sysCfg.gprsApn[0] == 0xFF || sysCfg.gprsApn[0] == 0)
    {
        strcpy((char *)sysCfg.gprsApn, DEFAULT_GPSR_APN);
        strcpy((char *)sysCfg.gprsUsr, DEFAULT_GPRS_USR);
        strcpy((char *)sysCfg.gprsPwd, DEFAULT_GPRS_PWD);
        saveFlag = 1;
    }

    //////////////////////////////////////////////
    ///////////////////// Default server 0 IP
    if((sysCfg.DServer0IP[0]==0xFFFF || sysCfg.DServer0IP[1]==0xFFFF))
    {
        ((uint8_t*)(sysCfg.DServer0IP))[0] = DEFAULT_DSERVER0_IP0;
        ((uint8_t*)(sysCfg.DServer0IP))[1] = DEFAULT_DSERVER0_IP1;
        ((uint8_t*)(sysCfg.DServer0IP))[2] = DEFAULT_DSERVER0_IP2;
        ((uint8_t*)(sysCfg.DServer0IP))[3] = DEFAULT_DSERVER0_IP3;

        sysCfg.DServer0Port = DEFAULT_DSERVER0_PORT;
        saveFlag = 1;
    }

    // load default data server domain
    if((uint8_t)sysCfg.DServer0Name[0] == 0xFF)
    {
        strcpy((char *)sysCfg.DServer0Name, DEFAULT_DSERVER0_NAME);
        saveFlag = 1;
    }
    
    // load default use IP or Domain
    if(sysCfg.DServer0UseIP == 0xFF)
    {
        sysCfg.DServer0UseIP = DEFAULT_DSERVER0_USEIP;
        saveFlag = 1;
    }
    
    ///////////////////// Default server 1 IP
    if((sysCfg.DServer1IP[0]==0xFFFF || sysCfg.DServer1IP[1]==0xFFFF))
    {
        ((uint8_t*)(sysCfg.DServer1IP))[0] = DEFAULT_DSERVER1_IP0;
        ((uint8_t*)(sysCfg.DServer1IP))[1] = DEFAULT_DSERVER1_IP1;
        ((uint8_t*)(sysCfg.DServer1IP))[2] = DEFAULT_DSERVER1_IP2;
        ((uint8_t*)(sysCfg.DServer1IP))[3] = DEFAULT_DSERVER1_IP3;

        sysCfg.DServer1Port = DEFAULT_DSERVER1_PORT;
        saveFlag = 1;
    }

    // load default data server domain
    if((uint8_t)sysCfg.DServer1Name[0] == 0xFF)
    {
        strcpy((char *)sysCfg.DServer1Name, DEFAULT_DSERVER1_NAME);
        saveFlag = 1;
    }
    
    // load default use IP or Domain
    if(sysCfg.DServer1UseIP == 0xFF)
    {
        sysCfg.DServer1UseIP = DEFAULT_DSERVER1_USEIP;
        saveFlag = 1;
    }
    
    ///////////////////// Default server 2 IP
    if((sysCfg.DServer2IP[0]==0xFFFF || sysCfg.DServer2IP[1]==0xFFFF))
    {
        ((uint8_t*)(sysCfg.DServer2IP))[0] = DEFAULT_DSERVER2_IP0;
        ((uint8_t*)(sysCfg.DServer2IP))[1] = DEFAULT_DSERVER2_IP1;
        ((uint8_t*)(sysCfg.DServer2IP))[2] = DEFAULT_DSERVER2_IP2;
        ((uint8_t*)(sysCfg.DServer2IP))[3] = DEFAULT_DSERVER2_IP3;

        sysCfg.DServer2Port = DEFAULT_DSERVER2_PORT;
        saveFlag = 1;
    }

    // load default data server domain
    if((uint8_t)sysCfg.DServer2Name[0] == 0xFF)
    {
        strcpy((char *)sysCfg.DServer2Name, DEFAULT_DSERVER2_NAME);
        saveFlag = 1;
    }
    
    // load default use IP or Domain
    if(sysCfg.DServer2UseIP == 0xFF)
    {
        sysCfg.DServer2UseIP = DEFAULT_DSERVER2_USEIP;
        saveFlag = 1;
    }
    
    ///////////////////// Default Image Server 0 IP
    if((sysCfg.IServer0IP[0]==0xFFFF || sysCfg.IServer0IP[1]==0xFFFF))
    {
        ((uint8_t*)(sysCfg.IServer0IP))[0] = DEFAULT_ISERVER0_IP0;
        ((uint8_t*)(sysCfg.IServer0IP))[1] = DEFAULT_ISERVER0_IP1;
        ((uint8_t*)(sysCfg.IServer0IP))[2] = DEFAULT_ISERVER0_IP2;
        ((uint8_t*)(sysCfg.IServer0IP))[3] = DEFAULT_ISERVER0_IP3;

        sysCfg.IServer0Port = DEFAULT_ISERVER0_PORT;
        saveFlag = 1;
    }

    // load default image server domain
    if((uint8_t)sysCfg.IServer0Name[0] == 0xFF)
    {
        strcpy((char *)sysCfg.IServer0Name, DEFAULT_ISERVER0_NAME);
        saveFlag = 1;
    }
    
    // load default use IP or Domain
    if(sysCfg.IServer0UseIP == 0xFF)
    {
        sysCfg.IServer0UseIP = DEFAULT_ISERVER0_USEIP;
        saveFlag = 1;
    }
    
    ///////////////////// Default Image Server 1 IP
    if((sysCfg.IServer1IP[0]==0xFFFF || sysCfg.IServer1IP[1]==0xFFFF))
    {
        ((uint8_t*)(sysCfg.IServer1IP))[0] = DEFAULT_ISERVER1_IP0;
        ((uint8_t*)(sysCfg.IServer1IP))[1] = DEFAULT_ISERVER1_IP1;
        ((uint8_t*)(sysCfg.IServer1IP))[2] = DEFAULT_ISERVER1_IP2;
        ((uint8_t*)(sysCfg.IServer1IP))[3] = DEFAULT_ISERVER1_IP3;

        sysCfg.IServer1Port = DEFAULT_ISERVER1_PORT;
        saveFlag = 1;
    }

    // load default image server domain
    if((uint8_t)sysCfg.IServer1Name[0] == 0xFF)
    {
        strcpy((char *)sysCfg.IServer1Name, DEFAULT_ISERVER1_NAME);
        saveFlag = 1;
    }
    
    // load default use IP or Domain
    if(sysCfg.IServer1UseIP == 0xFF)
    {
        sysCfg.IServer1UseIP = DEFAULT_ISERVER1_USEIP;
        saveFlag = 1;
    }

    if(sysCfg.numCameras == 0xFF)
    {
        saveFlag = 1;
        sysCfg.numCameras = 1;
    }
    
    if(sysCfg.cameraCompression < 100 || sysCfg.cameraCompression > 250)
    {
        sysCfg.cameraCompression = 100;
        saveFlag = 1;
    }

    if(sysCfg.cameraWorkingStartTime >= 24)
    {
        sysCfg.cameraWorkingStartTime = 0;
        saveFlag = 1;
    }

    if(sysCfg.cameraWorkingStopTime > 24)
    {
        sysCfg.cameraWorkingStopTime = 24;
        saveFlag = 1;
    }

    if(sysCfg.cameraInterval == 0 || sysCfg.cameraInterval == 0xFFFF)
    {
        saveFlag = 1;
        sysCfg.cameraInterval = 30;
    }

    
    if(sysCfg.prepaidAccount == 0xFFFFFFFF)
    {
        saveFlag = 1;
        sysCfg.prepaidAccount = 0;
    }

    if(sysCfg.lastError == 0xFFFF)
    {
        sysCfg.lastError = 0;
    }

    sysCfg.testMode = 0;

    if(sysCfg.language == 0xFFFFFFFF)
    {
        sysCfg.language = 2;
        saveFlag = 1;
    }

    if(sysCfg.sleepTimer == 0xFFFFFFFF)
    {
        sysCfg.sleepTimer = DEFAULT_SLEEP_TIMER;
        saveFlag = 1;
    }

    if(sysCfg.accountAlarmCheck >= 24)
    {
        sysCfg.accountAlarmCheck = 6;//6h AM
        saveFlag = 1;
    }

    if((uint8_t)sysCfg.upgradeTimeStamp[0] == 0xFF)
    {
        sysCfg.upgradeTimeStamp[0] = 0;
        saveFlag = 1;
    }

    if(saveFlag)
    {
        INFO("Save new setting\n");
        CFG_Save();
    }
}

void CFG_Save(void)
{
    int16_t i;
	uint8_t *ptr;
    uint32_t u32Temp;
	uint8_t *u8Pt;
    
 	__disable_irq();	
	
	u8Pt = (uint8_t *)&sysCfg;
	u32Temp = 0;
	for(i = 0;i < sizeof(CONFIG_POOL)-sizeof(sysCfg.crc);i++)
	{
		u32Temp += u8Pt[i];
	}
	sysCfg.crc = u32Temp;
	ptr = (uint8_t *)&sysCfg;
    
    SPIFlashBeginWrite(CONFIG_ADDR);
    SPIFlashWriteArray(ptr, sizeof(CONFIG_POOL));
	
 	__enable_irq();
    
// 	int16_t i;
// 	uint32_t addr = CONFIG_AREA_START;
// 	uint32_t *ptr,u32Temp;
// 	uint8_t *u8Pt;
//  	__disable_irq();	
// 	
// 	u8Pt = (uint8_t *)&sysCfg;
// 	u32Temp = 0;
// 	for(i = 0;i < sizeof(CONFIG_POOL)-sizeof(sysCfg.crc);i++)
// 	{
// 		u32Temp += u8Pt[i];
// 	}
// 	sysCfg.crc = u32Temp;

// 	addr = CONFIG_AREA_START;
// 	ptr = (uint32_t *)&sysCfg;
//     
//     Flash_Write(addr, ptr, sizeof(sysCfg));
// 	
//  	__enable_irq();
}
