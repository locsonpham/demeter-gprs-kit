/**
  ******************************************************************************
  * @file    
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "comms.h"

#include "uart.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 uint16_t myRegBuf1[myRegHolding1];
 Timeout_Type mbTimeout1;
 
 uint8_t rxBuffer1[64];
 uint8_t rxCounter1 = 0;
 uint8_t slaveADDR1 = 0;
 
 uint8_t resFlag1 = 0;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Modbus Init
  * @param  None
  * @retval None
  */
void myMB_Init1(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Config UART
    UART_Config(myMB_UART1, baud);
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    /* This sequence sets up the TX and RX pins
    * so they work correctly with the USART1 peripheral
    */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    GPIO_WriteBit(GPIOE, GPIO_Pin_13, Bit_RESET);
    GPIO_WriteBit(GPIOE, GPIO_Pin_15, Bit_RESET);
    
    InitTimeout(&mbTimeout1, myMB_SILENT1);
}

void myMB_Handler1(void)
{
    /* USART Receive Interrupt */
	if (USART_GetITStatus(myMB_UART1, USART_IT_RXNE) != RESET)
	{
        uint8_t data;
        
		// Read one byte from the receive data register
		data = USART_ReceiveData(myMB_UART1);
        
        rxBuffer1[rxCounter1++] = data;
        //u_send(USART1, data);
        
        InitTimeout(&mbTimeout1, myMB_SILENT1); // reset silent
	}
}

uint8_t sendRequest1(uint8_t addr, uint8_t func, uint8_t size, uint8_t startaddr)
{
    uint8_t message[64];
    uint8_t checksum = 0;
    uint8_t i = 0;
    
    slaveADDR1 = addr;
    // address
    message[0] = addr;
    // function
    message[1] = func;
    // size
    message[2] = size;
    // start address
    message[3] = startaddr;
    
    // calc checksum
    for (i = 0; i < 4; i++)
    {
        checksum += message[i];
    }
    
    // add checksum
    message[4] = checksum;
    
    // send data
    for (i = 0; i < 5; i++)
    {
        u_send(myMB_UART1, message[i]);
    }
    
    InitTimeout(&mbTimeout1, 0);
    resFlag1 = 0;
    
    Timeout_Type timeout;
    InitTimeout(&timeout, 1000);
    
    while (resFlag1 == 0)
    {
        if (CheckTimeout(&timeout) == TIMEOUT)
        {
            INFO("Timeout roi");
            return 0;
        }
    }
    
    return 1;
}

void responseMessage1(uint8_t addr, uint8_t func, uint8_t size, uint8_t startReg, uint8_t* buf)
{
    uint8_t message[64];
    uint8_t checksum = 0;
    uint8_t i = 0;
    
    // address
    message[0] = addr;
    // function
    message[1] = func;
    // size
    message[2] = size;
    // start reg
    message[3] = startReg;
    
    for (i = 0; i < size; i++)
    {
        message[i + 4] = *(buf++);
    }
    
    // calc checksum
    for (i = 0; i < size + 4; i++)
    {
        checksum += message[i];
    }
    
    // add checksum
    message[size + 4] = checksum;
    
    // send data
    for (i = 0; i < size + 5; i++)
    {
        u_send(myMB_UART1, message[i]);
    }
}

void myMB_Process1(void) 
{
    // SLAVE
    // Check Timeout
    if (CheckTimeout(&mbTimeout1) == TIMEOUT)
    {
        if (rxCounter1 >= 5)
        {
            uint8_t checksum = 0;
            
            // checksum
            for (int i = 0; i < rxCounter1 - 1; i++)
            {
                checksum += rxBuffer1[i];
            }
            
            if (checksum == rxBuffer1[rxCounter1 - 1])    // valid frame
            {
                if (rxBuffer1[0] == myMB_ADDR1) // valid address
                {
                    //INFO("Valid address\n");
                    uint8_t func = rxBuffer1[1];
                    uint8_t size = rxBuffer1[2];
                    uint8_t startReg = rxBuffer1[3];
                    uint8_t buffer[64] = {0};
                    
                    switch (func)
                    {
                        case FUNC_READ1: // response READ
                        {
                            //INFO("Read FUNC\n");
                            for (uint8_t i = 0; i < size; i++)
                            {
                                buffer[2*i] = (uint8_t)(myRegBuf1[i] >> 8);
                                buffer[2*i + 1] = (uint8_t)myRegBuf1[i];
                            }
                            
                            // send back message
                            responseMessage1(myMB_ADDR1, func, size * 2, startReg, buffer);
                            
                            break;
                        }
                        
                        case FUNC_WRITE1: // response WRITE
                        {
                            //INFO("Write FUNC\n");
                            for (uint8_t i = 0; i < size; i++)
                            {
                                myRegBuf1[i] = (uint16_t)rxBuffer1[4 + 2*i] << 8;
                                myRegBuf1[i] |= rxBuffer1[4 + 2*i + 1];
                            }
                            
                            // send back message
                            responseMessage1(myMB_ADDR1, func, size * 2, startReg, buffer);
                            
                            break;
                        }
                        
                        default: break;
                    }
                }
            }
        }
        
        rxCounter1 = 0;
        InitTimeout(&mbTimeout1, myMB_SILENT1); // reset timeout
    }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
