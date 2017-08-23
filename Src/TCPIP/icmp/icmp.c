#include "icmp.h"
uint8_t PacketIsIcmp(uint8_t * EthFrame)
{
	if(EthFrame[IP_PROTO_P]==IP_PROTO_ICMP_V && EthFrame[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
		return 1;
	else
		return 0;
}
void ICMP_ProcessDataIn(uint8_t *EthFrame,uint16_t len)
{
		 ETH_MakeHeader(EthFrame);
		 IP_MakeHeader(EthFrame);
		 //Doi ban tin request thanh ban tin Echo, thay doi lai checksum
		 	EthFrame[ICMP_TYPE_P]=ICMP_TYPE_ECHOREPLY_V;	  
			if (EthFrame[ICMP_CHECKSUM_P] > (0xff-0x08))
				{
				EthFrame[ICMP_CHECKSUM_P+1]++;
				}
			EthFrame[ICMP_CHECKSUM_P]+=0x08;
		ETH_SendFrame(len,EthFrame);
}

