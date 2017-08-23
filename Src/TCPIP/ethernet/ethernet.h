#ifndef __ETHERNET_H
#define __ETHERNET_H
#include "main.h"
#define ETH_HEADER_LEN   14
// Gia tri truong Type Ethenet
#define ETHTYPE_ARP_H_V 0x08
#define ETHTYPE_ARP_L_V 0x06
#define ETHTYPE_IP_H_V  0x08
#define ETHTYPE_IP_L_V  0x00
//Vi tri truong type trong khung Ethenet
#define ETH_TYPE_H_P 12
#define ETH_TYPE_L_P 13
//Vi tri truong dia chi dich trong khung Ethernet
#define ETH_DST_MAC 0
//Vi tri truong dia chi nguon trong khung Ethernet
#define ETH_SRC_MAC 6
void ETH_Init(uint8_t * MacAddr);
void ETH_SendFrame(uint16_t len,uint8_t *EthFrame);
uint16_t ETH_GetFrame(uint16_t maxlen, uint8_t *EthFrame);
//void ETH_SetMac(uint8_t * MacAddr);
void ETH_GetMac(uint8_t *MyMacAddr);
void ETH_MakeHeader(uint8_t *EthFrame);
void ETH_ProcessDataIn( uint16_t len,uint8_t *EthFrame);
//void ETH_ProcessDataOut(uint16_t len,uint8_t *EthFrame);
#endif
