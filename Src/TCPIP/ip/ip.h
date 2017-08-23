#ifndef __IP_H
#define __IP_H
#include "main.h"
#define IP_HEADER_LEN   20
// Vi tri cac truong trong goi Ethernet
#define IP_P                           (ETH_HEADER_LEN+0)
#define IP_HEADER_LEN_VER_P    (ETH_HEADER_LEN+0)
#define IP_TOS_P                     (ETH_HEADER_LEN+1)
#define IP_TOTLEN_H_P             (ETH_HEADER_LEN+2)
#define IP_TOTLEN_L_P             (ETH_HEADER_LEN+3)
#define IP_IDEN_H_P               (ETH_HEADER_LEN+4)
#define IP_IDEN_L_P               (ETH_HEADER_LEN+5)
#define IP_FLAGS_P                   (ETH_HEADER_LEN+6)         
#define IP_FRAG_H_P               (ETH_HEADER_LEN+6)
#define IP_FRAG_L_P                  (ETH_HEADER_LEN+7)
#define IP_TTL_P                     (ETH_HEADER_LEN+8)
#define IP_PROTO_P                   (ETH_HEADER_LEN+9)
#define IP_CHECKSUM_P             (ETH_HEADER_LEN+10)
#define IP_SRC_P                      (ETH_HEADER_LEN+12)
#define IP_DST_P                      (ETH_HEADER_LEN+16)
//Gia tri truong Protocol 
#define IP_PROTO_ICMP_V 1
#define IP_PROTO_TCP_V 6
#define IP_PROTO_UDP_V 17

uint8_t PacketIsIp(uint8_t *EthFrame,uint16_t len);
uint16_t CheckSum(uint8_t *EthFrame, uint16_t len,uint8_t type);
void IP_FillCheckSum(uint8_t *EthFrame);
void IP_MakeHeader(uint8_t *EthFrame);
void IP_ProcessDataIn(uint16_t len,uint8_t *EthFrame);
#endif
