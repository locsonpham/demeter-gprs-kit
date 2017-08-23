#include "ip.h"
uint8_t PacketIsIp(uint8_t *EthFrame,uint16_t len)
   {
   uint8_t i=0;
   //eth+ip+udp header is 42
   if (len<42)
      {
       return(0);
      }
   if(EthFrame[ETH_TYPE_H_P]!=ETHTYPE_IP_H_V || EthFrame[ETH_TYPE_L_P]!=ETHTYPE_IP_L_V)
      {
       return(0);
      }
   if (EthFrame[IP_HEADER_LEN_VER_P]!=0x45)
      {
       // must be IP V4 and 20 byte header
       return(0);
      }
   while(i<4)
      {
       if(EthFrame[IP_DST_P+i]!=MyIpAddr[i])
         {
           return(0);
          }
       i++;
      }
   return(1);
}
uint16_t CheckSum(uint8_t *EthFrame, uint16_t len,uint8_t type)
{
   // type 0   =   ip 
   //      1   =   udp
   //      2   =   tcp
   uint32_t sum = 0;
   //uint16_t temp;
   //if(type==0){
   //        // do not add anything
   //}
   if(type==1)
      {
      sum+=IP_PROTO_UDP_V; // protocol udp
      // the length here is the length of udp (data+header len)
      // =length given to this function - (IP.scr+IP.dst length)
      sum+=len-8; // = real tcp len
      }
   if(type==2)
      {
      sum+=IP_PROTO_TCP_V; 
      // the length here is the length of tcp (data+header len)
      // =length given to this function - (IP.scr+IP.dst length)
      sum+=len-8; // = real tcp len
      }
   // build the sum of 16bit words
   while(len >1)
      {
      sum += 0xFFFF & (((uint16_t)*EthFrame)<<8|*(EthFrame+1));
      /*temp=0;
      temp=*EthFrame;
      temp=temp<<8;
      temp=temp|*(EthFrame+1);
      sum=sum+temp;*/
      EthFrame  +=2;
      len-=2;
      }
   // if there is a byte left then add it (padded with zero)
   if (len)
      {
      sum += ((uint16_t)(0xFF &*EthFrame))<<8;
       /*temp=0;
       temp= *EthFrame;
       temp=temp<<8;
       sum+=temp;*/
      }
   // now calculate the sum over the bytes in the sum
   // until the result is only 16bit long
   while (sum>>16)
      {
      sum = (sum & 0xFFFF)+(sum >> 16); 
      }
   // build 1's complement:
   return( (uint16_t) sum ^ 0xFFFF);
   }
void IP_FillCheckSum(uint8_t *EthFrame)
   {
   uint16_t ck;
   // clear the 2 byte checksum
   EthFrame[IP_CHECKSUM_P]=0;
   EthFrame[IP_CHECKSUM_P+1]=0;
   EthFrame[IP_FLAGS_P]=0x40; // don't fragment
   EthFrame[IP_FLAGS_P+1]=0;  // fragement offset
   EthFrame[IP_TTL_P]=64; // Time To Live
   // calculate the checksum:
   ck=CheckSum(&EthFrame[IP_P], IP_HEADER_LEN,0);
   EthFrame[IP_CHECKSUM_P]=ck>>8;
   EthFrame[IP_CHECKSUM_P+1]=ck& 0xff;
   }
void IP_MakeHeader(uint8_t *EthFrame)
   {
   uint8_t i=0;
   for(i=0;i<4;i++)
      {
        EthFrame[IP_DST_P+i]=EthFrame[IP_SRC_P+i];
        EthFrame[IP_SRC_P+i]=MyIpAddr[i];
      }
   IP_FillCheckSum(EthFrame);
   }
void IP_ProcessDataIn(uint16_t len,uint8_t *EthFrame)
{ 
    #ifdef IP_VIEW
    printf("Dia chi IP nguon %3u.%3u.%3u.%3u\r\n",EthFrame[IP_SRC_P],EthFrame[IP_SRC_P+1],EthFrame[IP_SRC_P+2],EthFrame[IP_SRC_P+3]);
   #endif
    if(PacketIsIcmp(EthFrame)==1)
           {
             #ifdef IP_VIEW   
         printf("Goi ICMP\r\n");  
            #endif
         ICMP_ProcessDataIn(EthFrame,len);
         return;
         }
      if (PacketIsTcp(EthFrame)==1)
         {
            
                #ifdef IP_VIEW
                printf("Goi TCP\r\n");  
                #endif
                 TCP_ProcessDataIn(EthFrame);
            return; 
         }
}

