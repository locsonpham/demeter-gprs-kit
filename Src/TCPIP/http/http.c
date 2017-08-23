#include "http.h"
#include "TCPIP/web/web.h"
uint16_t find_string(uint8_t *str1,uint8_t *str2,uint16_t len)
{
    {
    uint16_t i=0,j=0;
    while(i<len)
    {
        if(str2[i++] != str1[j++])
        {
            j = 0;
        }
        if(str1[j] == 0)
        {
            return(i-j);
        }
        if(str2[i] == 0){
            return(len);
      }
   }
   return(len);
   }
}
void HTTP_SendData(uint8_t *EthFrame)
{
    uint16_t len=0; 
    #ifdef HTTP_VIEW
    printf("Du Lieu Gui: \r\n"); 
    #endif
    while(web[len])
    {
        EthFrame[TCP_DATA_P+len]=web[len];
        #ifdef HTTP_VIEW
        putchar(web[len]);  
        #endif
        len++;
    }
    /*while(len<size)
    {
        EthFrame[TCP_DATA_P+len]=web[len];
        #ifdef HTTP_VIEW
        putchar(web[len]);  
        #endif
        len++;  
    }*/
    TCP_SendData(EthFrame,len,1); 
    printf("\r\n");
}
void HTTP_ProcessDataIn(uint8_t *EthFrame,uint16_t data_point, uint16_t Len)
{
            uint16_t cmd;  
            #ifdef HTTP_VIEW
            uint16_t k;
            //uint8_t* Data_Http;
            #endif
            uint8_t str[20];
            sprintf(str,"LED1=");
            cmd=find_string(str,&EthFrame[data_point],Len);
            if(cmd!=Len)
            {
               if(EthFrame[data_point+cmd+5]=='1')
                  {
                        LED1=0;
                  }
               if(EthFrame[data_point+cmd+5]=='0')
                  {
                        LED1=1;
                  }
            } 
            sprintf(str,"LED2");
            cmd=find_string(str,&EthFrame[data_point],Len);
            if(cmd!=len)
            {
               if(EthFrame[data_point+cmd+5]=='1')LED2=0;
               if(EthFrame[data_point+cmd+5]=='0')LED2=1;
            }
            sprintf(str,"LED3=");
            cmd=find_string(str,&EthFrame[data_point],Len);
            if(cmd!=len)
            {
               if(EthFrame[data_point+cmd+5]=='1')LED3=0;
               if(EthFrame[data_point+cmd+5]=='0')LED3=1;
            } 
            #ifdef HTTP_VIEW
            //Data_Http=(uint8_t*)&(EthFrame[data_point]);
            printf("Du Lieu Nhan: \r\n");      
            for(k=0;k<len;k++)
            {
                  putchar(EthFrame[data_point+k]);
            } 
            #endif
            HTTP_SendData(EthFrame);
}
