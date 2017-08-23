#include "ethernet.h"
void ETH_Init(uint8_t * MacAddr)
{
   ENC28J60_Init(MacAddr);
}
void ETH_SendFrame(uint16_t len,uint8_t *EthFrame)
{
    #ifdef ETHERNET_VIEW
    printf("Gui goi Ethernet co do dai %lu\r\n",len);
    printf("Dia chi Mac Dich: %2x:%2x:%2x:%2x:%2x:%2x\r\n",EthFrame[ETH_DST_MAC],EthFrame[ETH_DST_MAC+1],EthFrame[ETH_DST_MAC+2],EthFrame[ETH_DST_MAC+3],EthFrame[ETH_DST_MAC+4],EthFrame[ETH_DST_MAC+5]);
    #endif
   ENC28J60_PacketSend(len,EthFrame);
}
uint16_t ETH_GetFrame(uint16_t maxlen, uint8_t *EthFrame)
{
   return ENC28J60_PacketReceive(maxlen,EthFrame);
}
/*void ETH_SetMac(uint8_t * MacAddr)
{
   ENC28J60_Write(MAADR5, MacAddr[0]);   
   ENC28J60_Write(MAADR4, MacAddr[1]);
   ENC28J60_Write(MAADR3, MacAddr[2]);
   ENC28J60_Write(MAADR2, MacAddr[3]);
   ENC28J60_Write(MAADR1, MacAddr[4]);
   ENC28J60_Write(MAADR0, MacAddr[5]);
}*/
void ETH_GetMac(uint8_t *MyMacAddr)
{
   MyMacAddr[0]=ENC28J60_Read(MAADR5);
   MyMacAddr[1]=ENC28J60_Read(MAADR4);
   MyMacAddr[2]=ENC28J60_Read(MAADR3);
   MyMacAddr[3]=ENC28J60_Read(MAADR2);
   MyMacAddr[4]=ENC28J60_Read(MAADR1);
   MyMacAddr[5]=ENC28J60_Read(MAADR0);
}
void ETH_MakeHeader(uint8_t *EthFrame)
{
   uint8_t i=0;
   //Chep dia chi Mac nguon thanh dia chi dich va dien dia chi cua no thanh dia chi nguon
   for(i=0;i<6;i++)
      {
        EthFrame[ETH_DST_MAC +i]=EthFrame[ETH_SRC_MAC +i];
        EthFrame[ETH_SRC_MAC +i]=MyMacAddr[i];
      }
}
void ETH_ProcessDataIn( uint16_t len,uint8_t *EthFrame)
{
   //Neu day la goi tin ARP
   if(PacketIsArp(EthFrame,len)==1)
   {
      #ifdef ETHERNET_VIEW
        printf("Goi ARP\r\n");    
        #endif
      ARP_ProcessDataIn(EthFrame);
      return;
   }
   //Neu day la goi tin IP
  else if(PacketIsIp(EthFrame,len)==1)
   {  
        #ifdef ETHERNET_VIEW
      printf("Goi IP\r\n"); 
        #endif
      IP_ProcessDataIn(len,EthFrame); 
     return;      
   }
   //printf("Unknow\n");
}

