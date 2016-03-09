#ifndef __RS485_CAMERA_H
#define __RS485_CAMERA_H

#include "typedef.h"
#include "db.h"
#include "packets.h"

// --> RS485 HALF DUPLEX Pin Config
#define CAM_COMM            USART2
#define UART_CAM_Handler    USART2_IRQHandler

#define RS485_DIR_RCC   RCC_APB2Periph_GPIOB
#define RS485_DIR_Pin   GPIO_Pin_14
#define RS485_DIR_Port  GPIOB

#define RS485_DIR_OUT   GPIO_SetBits(RS485_DIR_Port, RS485_DIR_Pin)
#define RS485_DIR_IN    GPIO_ResetBits(RS485_DIR_Port, RS485_DIR_Pin)
// <-- END Pin Config

#define CAMERA_DATA_SIZE	        512
#define CONFIG_USE_GPS_IN_CAMERA	1
#define CONFIG_USE_ID_IN_PACKETS	1
#define PACKET_TYPE_WRITE_FILE	    0xF2
#define PACKET_TYPE_IMAGE_FILE	    0xF3

typedef struct __attribute__((packed)){
	uint16_t size;				// = sizeof (PACKET_WRITE_FILE)
	uint8_t type;				// = 0xF2
    
	float lat;			// (-180 - +180)
	float lon;			// (-90 - +90)
	
	uint16_t iSize;				// = CAMERA_DATA_SIZE + 40 
	uint8_t iType;				// = 0xF3
	uint8_t id[17];
    
	// time stamp
	uint8_t year;				// year since 2000
	uint8_t month;				// 1 - 12
	uint8_t mday;				// 1 - 31
	uint8_t hour;				// 0 - 23
	uint8_t min;				// 0 - 59
	uint8_t sec;					// 0 - 59
	
	uint8_t cameraNumber;
	
	uint32_t fileSize;			// file size
	uint32_t offset;		    // offset of the chunk
	uint32_t len;				// size of the chunk
	uint8_t data[CAMERA_DATA_SIZE];	// data chunk
	uint32_t crc;
} CAMERA_PAKET_TYPE;


extern uint8_t cameraShotFlag;
extern CAMERA_PAKET_TYPE packetWriteJpegFile;
extern CAMERA_PAKET_TYPE packetReadJpegFile;
uint32_t CAM_crc(uint8_t *buff, uint32_t length);
void CAM_Init(void);
void CAM_Input(uint8_t c);
void CAM_Manage(void);
void CAMSendCmd(uint8_t *buf, uint16_t len);
uint8_t CAM_CheckDataIsReady(void);
uint8_t CAM_SetPacketOffset(uint8_t packet);
uint8_t CAM_CheckEndPacket(void);
uint8_t CAM_GetPacketCnt(void);
uint8_t CAM_NextPacket(void);
uint8_t CAM_SetNextCamId(void);
uint8_t CAM_BackPacket(void);
uint8_t* CAM_GetPacket(void);
void CAM_ChangeId(uint8_t id, uint8_t newId);
uint8_t CAM_TakeSnapshot(uint16_t cameras);
void CAM_Stop(void);
void CAM_Start(void);

#endif
