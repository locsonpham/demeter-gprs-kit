#ifndef __ICMP_H
#define __ICMP_H
#include "main.h"
//Gia tri truong Type ICMP
#define ICMP_TYPE_ECHOREPLY_V 0
#define ICMP_TYPE_ECHOREQUEST_V 8
//Vi tri
#define ICMP_TYPE_P  (ETH_HEADER_LEN+IP_HEADER_LEN)
#define ICMP_CODE_P	 (ETH_HEADER_LEN+IP_HEADER_LEN+1)
#define ICMP_CHECKSUM_P (ETH_HEADER_LEN+IP_HEADER_LEN+2)
#define ICMP_OTHER			(ETH_HEADER_LEN+IP_HEADER_LEN+4)
uint8_t PacketIsIcmp(uint8_t *EthFrame);
void ICMP_ProcessDataIn(uint8_t *EthFrame,uint16_t len);
#endif
