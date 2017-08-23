#include "stm32f4xx.h"

#include "db.h"


void Tracking_Init(void);
// uint16_t AddTrackerPacket(MSG_STATUS_RECORD *logRecordSend, char *data, uint32_t *len);
uint32_t CheckTrackerMsgIsReady(void);
uint32_t GetLastTrackerMsg(char *buff,uint32_t *len);
uint32_t GetTrackerMsg(char *buff,uint32_t *len);
void addLogPackage(void);
