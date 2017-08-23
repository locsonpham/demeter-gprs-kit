#ifndef 	__ARP_H
#define 	__ARP_H
#include "main.h"
//Gia tri cac truong trong khung ARP
#define ETH_ARP_HW_TYPE_H_V	0x00
#define ETH_ARP_HW_TYPE_L_V	0x01
#define ETH_ARP_PRO_TYPE_H_V  0x08
#define ETH_ARP_PRO_TYPE_L_V  0x00
#define ETH_ARP_OPCODE_REPLY_H_V 0x0
#define ETH_ARP_OPCODE_REPLY_L_V 0x02
#define ETH_ARP_OPCODE_REQUEST_H_V 0x0
#define ETH_ARP_OPCODE_REQUEST_L_V 0x01
//Vi tri cac truong trong khung Ethernet
#define ETH_ARP_HW_TYPE_H_P		(ETH_HEADER_LEN+0)
#define ETH_ARP_HW_TYPE_L_P		(ETH_HEADER_LEN+1)
#define ETH_ARP_PRO_TYPE_H_P	(ETH_HEADER_LEN+2)
#define ETH_ARP_PRO_TYPE_L_P	(ETH_HEADER_LEN+3)
#define ETH_ARP_HLEN_P				(ETH_HEADER_LEN+4)
#define ETH_ARP_PLEN_P				(ETH_HEADER_LEN+5)
#define ETH_ARP_OPCODE_H_P 		(ETH_HEADER_LEN+6)
#define ETH_ARP_OPCODE_L_P 		(ETH_HEADER_LEN+7)
#define ETH_ARP_SRC_MAC_P 		(ETH_HEADER_LEN+8)
#define ETH_ARP_SRC_IP_P 			(ETH_HEADER_LEN+14)
#define ETH_ARP_DST_MAC_P 		(ETH_HEADER_LEN+18)
#define ETH_ARP_DST_IP_P 			(ETH_HEADER_LEN+24)
uint8_t PacketIsArp(uint8_t *EthFrame,unsigned  int len);
void ARP_ProcessDataIn(uint8_t* EthFrame);
#endif
