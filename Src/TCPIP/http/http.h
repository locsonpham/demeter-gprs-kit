#ifndef __HTTP_H
#define __HTTP_H
#include "main.h"
void HTTP_ProcessDataIn(uint8_t *EthFrame,uint16_t data_point, uint16_t Len);
void HTTP_SendData(uint8_t *EthFrame);
uint16_t find_string(uint8_t str1,uint8_t *str2,uint16_t len);
#endif 
