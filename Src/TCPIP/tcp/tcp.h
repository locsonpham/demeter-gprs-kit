#ifndef __TCP_H
#define __TCP_H
#include "main.h"
#define TCP_HEADER_P     (ETH_HEADER_LEN+IP_HEADER_LEN)
#define TCP_SRC_PORT_H_P (ETH_HEADER_LEN+IP_HEADER_LEN)
#define TCP_SRC_PORT_L_P (ETH_HEADER_LEN+IP_HEADER_LEN+1)
#define TCP_DST_PORT_H_P (ETH_HEADER_LEN+IP_HEADER_LEN+2)
#define TCP_DST_PORT_L_P (ETH_HEADER_LEN+IP_HEADER_LEN+3)
// the tcp seq number is 4 bytes 0x26-0x29
#define TCP_SEQ_H_P        (ETH_HEADER_LEN+IP_HEADER_LEN+4)
#define TCP_SEQACK_H_P      (ETH_HEADER_LEN+IP_HEADER_LEN+8)
// flags: SYN=2
#define TCP_FLAGS_P 0x2f
#define TCP_FLAGS_SYN_V 2
#define TCP_FLAGS_FIN_V 1
#define TCP_FLAGS_PUSH_V 8
#define TCP_FLAGS_SYNACK_V 0x12
#define TCP_FLAGS_ACK_V 0x10
#define TCP_FLAGS_PSHACK_V 0x18
//  plain len without the options:
#define TCP_HEADER_LEN_PLAIN 20
#define TCP_HEADER_LEN_P 0x2e
#define TCP_WINDOW_H_P 0x30
#define TCP_WINDOW_L_P 0x31
#define TCP_CHECKSUM_H_P 0x32
#define TCP_CHECKSUM_L_P 0x33
#define TCP_OPTIONS_P 0x36
#define TCP_DATA_P    0x36
uint8_t PacketIsTcp(uint8_t * EthFrame);
uint8_t PacketIsTcpSyn(uint8_t *EthFrame);
void TCP_ProcessDataIn(uint8_t *EthFrame);
void TCP_MakeHead(uint8_t *EthFrame,uint16_t len,uint8_t mss,uint8_t cp_seq);
void TCP_GetLenDataHeader(uint8_t *EthFrame);
uint16_t TCP_GetDataPointer(void);
void TCP_SendSyn( uint8_t *EthFrame);
void TCP_SendAck( uint8_t *EthFrame);
void TCP_SendData(uint8_t *EthFrame, uint16_t Len,uint8_t Fin);
#endif

