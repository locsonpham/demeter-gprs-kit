#include "rs485_camera.h"

#include "defines.h"
#include "uart.h"
#include "ringbuf.h"
#include "sys_time.h"
#include "systick.h"
#include "system_config.h"

#define TICK_SECOND     1000
#define CAM_TIME_OUT	TICK_SECOND * 15
#define CAM_INTERVAL	TICK_SECOND * sysCfg.cameraInterval * 60
#define CAM_DELAYED_START	TICK_SECOND * 10
#define CAM_MAX_GET_DATA_RETRY		3
#define CAM_MAX_SNAPSHOT_RETRY		3


uint8_t uartCAM_RxBuff[400];
RINGBUF uartCAM_RxRingBuff;

typedef struct{
	DATE_TIME time;
	uint32_t size;
	uint16_t packetCount;
}SNAPSHOT_INFO;

SNAPSHOT_INFO ssInfo[4];

uint8_t cmdSetCompression[] = {'U', 'Q', 
						0,		/* camera ID */
						200,	/* compression value, higher value, lower quality */
						'#'};
						
uint8_t cmdSnapshot[] = {'U', 'H', 
						0, 		/* camera ID */
						'3',	/* resolution = 640x480 */
						0, 2, 	/* packet size = 512 */
						'#'};

uint8_t cmdGetPacket[] = {'U', 'E', 
						0, 		/* camera ID */
						1, 0, 	/* packet ID */
						'#'};

uint8_t cmdSetBaudRate[] = {'U', 'I', 
							0, 		/* camera ID */
						'4', 	/* baud 115200 */
						'#'};
						
uint8_t cmdChangeId[] = {'U', 'D',
					0xff,			/* current ID */
					0,			/* new ID */
					'#'};
volatile enum{
	WAIT_HEADER,
	GET_CMD,
	GET_CAM_ID,
	GET_CMD_DATA,
	GET_IMG_PKG_ID0,
	GET_IMG_PKG_ID1,
	GET_IMG_DATA_LEN0,
	GET_IMG_DATA_LEN1,
	GET_IMG_DATA,
	GET_IMG_CRC0,
	GET_IMG_CRC1
}camState;

volatile enum{
	INITIAL,
	TAKE_SNAPSHOT,
	WAIT_RESPONSE,
	GET_PACKET,
	WAIT_PACKET,
	GOT_PACKET,
	WAIT_ENQUEUED,
	NEXT_CAMERA,
}camManState;


uint8_t cameraShotFlag = 0;
uint8_t camCurrentId = 0;
uint8_t camRespondedCmd = 0;
uint8_t camRespondedId = 0;
//uint8_t camData[CAMERA_DATA_SIZE];
uint16_t camRespondedDataLen,camRespondedCrc;
uint16_t camSnapshotMap;
uint8_t camStopFlag = 0;

volatile uint16_t camRespondedPacketId;
volatile uint16_t camRespondedPacketLen;
volatile uint8_t camAcked;
volatile uint8_t camGetDataRetryCnt;
volatile uint32_t camTick;
volatile uint32_t camIntervalTick;

// static void CAMSendCmd(uint8_t *buf, uint16_t len);
static void ProcessCmd(void);

CAMERA_PAKET_TYPE packetWriteJpegFile;
CAMERA_PAKET_TYPE packetReadJpegFile;

/**
  * @brief  Config UART for Camera Communication
  * @param         
  * @param
  * @retval
  */
void UART_CAM_Config(void)
{
    UART_Config(CAM_COMM, 115200);
    
    /* Ring Buff */
	RINGBUF_Init(&uartCAM_RxRingBuff, uartCAM_RxBuff, sizeof(uartCAM_RxBuff));
}

/**
  * @brief Camera UART Handler
  * @param         
  * @param
  * @retval
  */
void UART_CAM_Handler(void)
{
    uint8_t data;
	
	/* USART Receive Interrupt */
	if (USART_GetITStatus(CAM_COMM, USART_IT_RXNE) != RESET)
	{
		// Read one byte from the receive data register
		data = USART_ReceiveData(CAM_COMM);
        //u_send(SIM_COMM, data);
        
        RINGBUF_Put(&uartCAM_RxRingBuff, data);
        
        // CAM Input
        CAM_Input(data);
	}
}

uint32_t CAM_crc(uint8_t *buff, uint32_t length)
{
	uint32_t i;
	uint32_t crc = 0;
	for(i = 0;i < length; i++)
	{
		crc += buff[i];
	}
	return crc;
}

uint16_t CAM_crc16(uint8_t *buff, uint32_t length)
{
	uint32_t i;
	uint16_t crc = 0;
	for(i = 0;i < length; i++)
	{
		crc += buff[i];
	}
	return crc;
}

void CAM_Init(void)
{
    uint8_t i = 0;
    
    // Init RS485 DIR
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RS485_DIR_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = RS485_DIR_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RS485_DIR_Port, &GPIO_InitStructure);
    
    RS485_DIR_IN;   // Default Receiver
    
	camCurrentId = 0;
	
// 	#if CONFIG_USE_ID_IN_PACKETS == 1
// 	strcpy((char *)packetWriteJpegFile.id, (char *)sysCfg.id);
// 	#endif
	
	cmdSnapshot[0] = 'U';
	cmdSnapshot[1] = 'H';
	cmdSnapshot[3] = '3';   // 640x480
	cmdSnapshot[4] = 0;
	cmdSnapshot[5] = 2;     // 512 Package Size
	cmdSnapshot[6] = '#';
	
	cmdGetPacket[0] = 'U';  // Package ID
	cmdGetPacket[1] = 'E';
	cmdGetPacket[3] = 1;
	cmdGetPacket[4] = 0;
	cmdGetPacket[5] = '#';
	
	cmdChangeId[0] = 'U';
	cmdChangeId[1] = 'D';
	cmdChangeId[2] = 0xff;
	cmdChangeId[3] = 0;
	cmdChangeId[4] = '#';
    
    UART_CAM_Config(); // UART to RS485
    
// 	CAMSendCmd(cmdChangeId, sizeof cmdChangeId);
// 	CAMSendCmd(cmdChangeId, sizeof cmdChangeId);
// 	delay_ms(1000);
// 	CAMSendCmd(cmdSetBaudRate, sizeof cmdSetBaudRate);
// 	CAMSendCmd(cmdSetBaudRate, sizeof cmdSetBaudRate);
// 	delay_ms(1000);
// 	CAMSendCmd(cmdChangeId, sizeof cmdChangeId);
// 	CAMSendCmd(cmdChangeId, sizeof cmdChangeId);
// 	delay_ms(1000);
    
// 	cmdSetCompression[0] = 'U';
// 	cmdSetCompression[1] = 'Q';
// 	cmdSetCompression[3] = sysCfg.cameraCompression;
// 	cmdSetCompression[4] = '#';
// 	
// 	for(i=0;i<sysCfg.numCameras;i++)
// 	{
// 		cmdSetCompression[2] = i;
// 		CAMSendCmd(cmdSetCompression, sizeof cmdSetCompression);
// 		delay_ms(1000);
// 	}

	camTick = GetTick();
    
    delay_ms(3000);
}

uint8_t CAM_TakeSnapshot(uint16_t cameras)
{
	if(camManState != INITIAL) return 0xff;
	
	INFO("CAM: Snapshotting\n");
	memset(ssInfo, 0, sizeof ssInfo);
	camSnapshotMap = cameras;
	
	packetWriteJpegFile.size = sizeof(CAMERA_PAKET_TYPE);
	packetWriteJpegFile.type = PACKET_TYPE_WRITE_FILE;
	
	packetWriteJpegFile.year = sysTime.year;
	packetWriteJpegFile.month = sysTime.month;
	packetWriteJpegFile.mday = sysTime.mday;
	packetWriteJpegFile.hour = sysTime.hour;
	packetWriteJpegFile.min = sysTime.min;
	packetWriteJpegFile.sec = sysTime.sec;
	
	memcpy(packetWriteJpegFile.id,sysCfg.id,sizeof(packetWriteJpegFile.id));
	
//     packetWriteJpegFile.lat = modemInfo.lat;
//     packetWriteJpegFile.lon = modemInfo.lon;
    
	packetWriteJpegFile.cameraNumber = camCurrentId;
	
	packetWriteJpegFile.iSize = CAMERA_DATA_SIZE + 43;
	packetWriteJpegFile.iType = PACKET_TYPE_IMAGE_FILE;
	packetWriteJpegFile.fileSize = 0;			// file size
	packetWriteJpegFile.offset = 0;				// offset of the chunk
	packetWriteJpegFile.len = 0;				// size of the chunk
	
	camManState = TAKE_SNAPSHOT;
	return 0;
}

void CAM_Manage(void)
{	
	DATE_TIME t;
    
	if(camStopFlag == 0)
    {
        switch(camManState)
        {
            case INITIAL:
                break;
                
            case TAKE_SNAPSHOT:
            {
                if(camCurrentId >= sysCfg.numCameras)
                {
                    camManState = INITIAL;
                    break;
                }
                
                if(!((1<<camCurrentId) & camSnapshotMap))
                {
                    camManState = WAIT_RESPONSE;
                    break;
                }
                
                cmdSnapshot[2] = camCurrentId;
                CAMSendCmd(cmdSnapshot, sizeof cmdSnapshot);
                CAMSendCmd(cmdSnapshot, sizeof cmdSnapshot);
                camTick = GetTick();
                camState = WAIT_HEADER;
                camManState = WAIT_RESPONSE;
            } break;
            
            case WAIT_RESPONSE:
            {
                if(GetTick() - camTick >= TICK_SECOND)
                {
                    if(++camCurrentId >= sysCfg.numCameras)
                    {
                        camCurrentId = 0;
                        cmdGetPacket[3] = 1;
                        camManState = GET_PACKET;
                    }
                    else
                    {
                        camManState = TAKE_SNAPSHOT;
                    }
                }
            } break;
                
            case GET_PACKET:
            {
                if(ssInfo[camCurrentId].size == 0)
                {
                    camManState = NEXT_CAMERA;
                    break;
                }
                
                cmdGetPacket[2] = camCurrentId;
                CAMSendCmd(cmdGetPacket, sizeof cmdGetPacket);
                camTick = GetTick();
                camManState = WAIT_PACKET;
                
            } break;
            
            case WAIT_PACKET:
            {
                if(GetTick() - camTick > CAM_TIME_OUT)
                {
                    if(++camGetDataRetryCnt >= CAM_MAX_GET_DATA_RETRY)
                    {
                        camManState = NEXT_CAMERA;
                    }
                    else
                    {
                        INFO("CAM: GET_PACKET timeout %d\n", camGetDataRetryCnt);
                        camManState = GET_PACKET;
                    }
                }
            } break;
                
            case GOT_PACKET:
            {
                t.year = (int16_t)packetWriteJpegFile.year + 2000;
                t.month = packetWriteJpegFile.month;
                t.mday = packetWriteJpegFile.mday;
                t.hour = packetWriteJpegFile.hour;
                t.min = packetWriteJpegFile.min;
                t.sec = packetWriteJpegFile.sec;
                
                //if(DB_ImageCreateHierarchy(t) == 0)
                {
                    if(packetWriteJpegFile.offset <= 512)
                    {
                        packetWriteJpegFile.crc = CAM_crc((uint8_t *)&packetWriteJpegFile,packetWriteJpegFile.size - 4);
                        //DB_SaveImageInfo(t,(uint8_t *)&packetWriteJpegFile,packetWriteJpegFile.size);
                    }
                    //DB_SaveImageData(t,packetWriteJpegFile.data,packetWriteJpegFile.len, packetWriteJpegFile.offset);
                }
                camGetDataRetryCnt = 0;
                cmdGetPacket[3]++;
                camManState = WAIT_ENQUEUED;
            } break;
                
            case WAIT_ENQUEUED:
            {
                if(camRespondedPacketId >= ssInfo[camCurrentId].packetCount)
                {
                    t.year = (int16_t)packetWriteJpegFile.year + 2000;
                    t.month = packetWriteJpegFile.month;
                    t.mday = packetWriteJpegFile.mday;
                    t.hour = packetWriteJpegFile.hour;
                    t.min = packetWriteJpegFile.min;
                    t.sec = packetWriteJpegFile.sec;
    // 				if(DB_ImageCreateHierarchy(t) == 0)
    // 				{
    // 					DB_SaveImageFileName(&t);
    // 				}
                    camManState = NEXT_CAMERA;
                }
                else
                {
                    camManState = GET_PACKET;
                }
            } break;
                
            case NEXT_CAMERA:
            {
                if(++camCurrentId >= sysCfg.numCameras)
                {
                    camCurrentId = 0;
                    camManState = INITIAL;
                }
                else
                {
                    cmdGetPacket[2] = camCurrentId;
                    cmdGetPacket[3] = 1;
                    camManState = GET_PACKET;
                }
                camGetDataRetryCnt = 0;
            } break;
                
            default: break;
        }
    }
}

void CAM_Stop(void)
{
	camStopFlag = 1;
}

void CAM_Start(void)
{
	camStopFlag = 0;
}

void CAM_ChangeId(uint8_t id, uint8_t newId)
{
	INFO("CAM: Change ID from %d to %d\n", id, newId);
	cmdChangeId[2] = id;
	cmdChangeId[3] = newId;
	CAMSendCmd(cmdChangeId, sizeof cmdChangeId);
	delay_ms(1000);
	CAMSendCmd(cmdChangeId, sizeof cmdChangeId);
	delay_ms(1000);
}
void CAM_Input(uint8_t c)
{
	static uint16_t CamCrc = 0;
    
	if(camStopFlag == 0)    // cam is running
    {
        switch(camState)
        {
            case WAIT_HEADER:
            {
                if(c == 'U')
                {
                    CamCrc = c;
                    camState = GET_CMD;
                }
            } break;
                
            case GET_CMD:
            {
                CamCrc += c;
                camRespondedCmd = c;
                camRespondedDataLen = 0;
                camState = GET_CAM_ID;
            } break;
                
            case GET_CAM_ID:
            {
                CamCrc += c;
                camRespondedId = c;
                
                // image data
                if(camRespondedCmd == 'F')
                {
                    camState = GET_IMG_PKG_ID0;
                }
                else
                {
                    camRespondedDataLen = 0;
                    camState = GET_CMD_DATA;
                }
            } break;
                
            case GET_CMD_DATA:
            {
                if(c == '#')
                {
                    ProcessCmd();
                    camState = WAIT_HEADER;
                }
                else
                {
                    packetWriteJpegFile.data[camRespondedDataLen++] = c;
                }
            } break;
                
            case GET_IMG_PKG_ID0:
            {
                CamCrc += c;
                camRespondedPacketId = c;
                camState = GET_IMG_PKG_ID1;
            } break;
                
            case GET_IMG_PKG_ID1:
            {
                CamCrc += c;
                camRespondedPacketId |= ((uint16_t)c)<<8;
                camState = GET_IMG_DATA_LEN0;
            } break;
                
            case GET_IMG_DATA_LEN0:
            {
                CamCrc += c;
                camRespondedPacketLen = c;
                camState = GET_IMG_DATA_LEN1;
            } break;
                
            case GET_IMG_DATA_LEN1:
            {
                CamCrc += c;
                camRespondedPacketLen |= ((uint16_t)c)<<8;
                camRespondedPacketLen += 2;
                camRespondedDataLen = 0;
                camState = GET_IMG_DATA;
            } break;
                
            case GET_IMG_DATA:
            {
                if((camRespondedDataLen < CAMERA_DATA_SIZE))
                {
                    packetWriteJpegFile.data[camRespondedDataLen] = c;
                    CamCrc += c;
                }
                camRespondedDataLen++;
                if(camRespondedDataLen >= camRespondedPacketLen - 2)
                {
                    camState = GET_IMG_CRC0;
                }
            } break;
                
            case GET_IMG_CRC0:
            {
                camRespondedCrc = c;
                camState = GET_IMG_CRC1;
            } break;
            
            case GET_IMG_CRC1:
            {
                camRespondedCrc |= ((uint16_t)c)<<8;
                //if(CamCrc == camRespondedCrc)
                {
                    // prepair the packet
                    CamCrc = 0;
                    packetWriteJpegFile.cameraNumber = camRespondedId;
                    packetWriteJpegFile.len = camRespondedDataLen;
                    packetWriteJpegFile.offset = (uint32_t)(camRespondedPacketId - 1)*CAMERA_DATA_SIZE;
                    packetWriteJpegFile.fileSize = ssInfo[camRespondedId].size;
                    
                    packetWriteJpegFile.year = ssInfo[camRespondedId].time.year - 2000;
                    packetWriteJpegFile.month = ssInfo[camRespondedId].time.month;
                    packetWriteJpegFile.mday = ssInfo[camRespondedId].time.mday;
                    packetWriteJpegFile.hour = ssInfo[camRespondedId].time.hour;
                    packetWriteJpegFile.min = ssInfo[camRespondedId].time.min;
                    packetWriteJpegFile.sec = ssInfo[camRespondedId].time.sec;
                    
                    //memcpy(packetWriteJpegFile.data, camData, camRespondedDataLen);
                    
                    INFO("CAM: Got packet from CAM %d, ID: %d / %d, len:%d\n", camRespondedId, camRespondedPacketId, ssInfo[camCurrentId].packetCount, camRespondedDataLen);
                }
            //	else
                //	camManState = GOT_PACKET;
                camManState = GOT_PACKET;
                camState = WAIT_HEADER;
            } break;
            
            default:
            {
                camManState = GOT_PACKET;
                camState = WAIT_HEADER;
            } break;
        }
    }
}

uint8_t CAM_SetPacketOffset(uint8_t packet)
{
	if(camManState == WAIT_ENQUEUED)
	{
		cmdGetPacket[3] = packet;
		camManState = GET_PACKET;
		return 0;
	}
	return 0xff;
}

uint8_t CAM_GetPacketCnt(void)
{
	return ssInfo[camCurrentId].packetCount;
}

uint8_t CAM_CheckEndPacket(void)
{
	if(camManState == WAIT_ENQUEUED)
		if(camRespondedPacketId == ssInfo[camCurrentId].packetCount)
			return 0;
	return 0xff;
}

uint8_t CAM_SetNextCamId(void)
{
	if(camManState == WAIT_ENQUEUED)
	{
		camManState = NEXT_CAMERA;
		return 0;
	}
	return 0xff;
}

uint8_t CAM_NextPacket(void)
{
	if(camManState == WAIT_ENQUEUED)
	{
		if(camRespondedPacketId >= ssInfo[camCurrentId].packetCount)
				camManState = NEXT_CAMERA;
		else
				camManState = GET_PACKET;
		return 0;
	}
	return 0xff;
}

uint8_t CAM_BackPacket(void)
{
	if(camManState == WAIT_ENQUEUED)
	{
		cmdGetPacket[3]--;
		camManState = GET_PACKET;
		return (cmdGetPacket[3]);
	}
	return 0xff;
}

uint8_t CAM_CheckDataIsReady(void)
{
	if(camManState == WAIT_ENQUEUED)
	{
			return 1;
	}
	return 0;
}

uint8_t* CAM_GetPacket(void)
{
	if(camManState == WAIT_ENQUEUED)
	{
		if(packetWriteJpegFile.size > sizeof(CAMERA_PAKET_TYPE))
		{
			NVIC_SystemReset();
		}
		return (uint8_t*)&packetWriteJpegFile;
	}
	return NULL;
}

void CAMSendCmd(uint8_t *buf, uint16_t len)
{
	camAcked = 0;
	RS485_DIR_OUT;
	while(len--){
		u_send(CAM_COMM, *buf++);
	}
	RS485_DIR_IN;
}

static void ProcessCmd()
{
	switch(camRespondedCmd)
    {
		case 'R':   // snapshot info
        {
			memcpy(&(ssInfo[camRespondedId].size), packetWriteJpegFile.data, 4);
			memcpy(&(ssInfo[camRespondedId].packetCount), packetWriteJpegFile.data + 4, 2);
			//ssInfo[camRespondedId].packetCount = *((uint16_t*)(packetWriteJpegFile.data + 4));
			
		//	TRACKER_Pause();
			memcpy(&(ssInfo[camRespondedId].time), &sysTime, sizeof sysTime);
			//TRACKER_Resume();
			
			INFO("CAM: id=%d, snapshot size=%lu, packets=%d\n", camRespondedId, ssInfo[camRespondedId].size, ssInfo[camRespondedId].packetCount);
        } break;
			
		case 'C':
        {
			CAMSendCmd((uint8_t*)&camRespondedId, 1);
        } break;
			
		case 'H':
		case 'E':
		case 'D':
		case 'Q':
        {
			camAcked = 1;
        } break;
        
		case '?':
        {
			camAcked = 0xff;
        } break;
        
        default: break;
	}
}
