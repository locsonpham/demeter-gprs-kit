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
#include "com.h"

#include "uart.h"
#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 uint16_t myRegBuf[myRegHolding];
 Timeout_Type mbTimeout;
 
 uint8_t rxBuffer[64];
 uint8_t rxCounter = 0;
 uint8_t slaveADDR = 0;
 
 uint8_t resFlag = 0;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Modbus Init
  * @param  None
  * @retval None
  */
void myMB_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // Config UART
    UART_Config(myMB_UART, baud);
    
    RCC_AHB1PeriphClockCmd(RW_RCC, ENABLE);

    /* This sequence sets up the TX and RX pins
    * so they work correctly with the USART1 peripheral
    */
    GPIO_InitStruct.GPIO_Pin = RW_Pin; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(RW_Port, &GPIO_InitStruct);
    
    RW_L;
    
    InitTimeout(&mbTimeout, myMB_SILENT);
}

void myMB_Handler(void)
{
    /* USART Receive Interrupt */
	if (USART_GetITStatus(myMB_UART, USART_IT_RXNE) != RESET)
	{
        uint8_t data;
        
		// Read one byte from the receive data register
		data = USART_ReceiveData(myMB_UART);
        
        rxBuffer[rxCounter++] = data;
        
        InitTimeout(&mbTimeout, myMB_SILENT); // reset silent
	}
}

uint8_t sendRequest(uint8_t addr, uint8_t func, uint8_t size, uint8_t startaddr)
{
    uint8_t message[64];
    uint8_t checksum = 0;
    uint8_t i = 0;
    
    slaveADDR = addr;
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
    RW_H;
    for (i = 0; i < 5; i++)
    {
        u_send(myMB_UART, message[i]);
    }
    RW_L;
    
    InitTimeout(&mbTimeout, 0);
    resFlag = 0;
    
    Timeout_Type timeout;
    InitTimeout(&timeout, 1000);
    
    while (resFlag == 0)
    {
        if (CheckTimeout(&timeout) == TIMEOUT)
        {
            INFO("Timeout roi");
            return 0;
        }
    }
    
    return 1;
}

void responseMessage(uint8_t addr, uint8_t func, uint8_t size, uint8_t startReg, uint8_t* buf)
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
    RW_H;
    for (i = 0; i < size + 5; i++)
    {
        u_send(myMB_UART, message[i]);
    }
    RW_L;
}

void myMB_Process(void) 
{
    // MASTER
    if (CheckTimeout(&mbTimeout) == TIMEOUT)
    {
        if (rxCounter >= 5)
        {
            uint8_t checksum = 0;
            
            // checksum
            for (int i = 0; i < rxCounter - 1; i++)
            {
                checksum += rxBuffer[i];
            }
            
            if (checksum == rxBuffer[rxCounter - 1])    // valid frame
            {
                if (rxBuffer[0] == slaveADDR) // new slave
                {
                    //INFO("res mess\n");
                    
                    uint8_t func = rxBuffer[1];
                    uint8_t size = rxBuffer[2];
                    uint8_t startReg = rxBuffer[3];
                    uint8_t buffer[64] = {0};
                    
                    switch (func)
                    {
                        case FUNC_READ:
                        {
                            for (uint8_t i = 0; i < (size / 2); i++)
                            {
                                myRegBuf[startReg + i] = (uint16_t)rxBuffer[4 + 2*i] << 8;
                                myRegBuf[startReg + i] |= rxBuffer[4 + 2*i + 1];
                            }
                            
                            resFlag = 1;
                            break;
                        }
                        
                        case FUNC_WRITE: // response WRITE
                        {
                            //INFO("Write FUNC\n");
                            for (uint8_t i = 0; i < size; i++)
                            {
                                myRegBuf[i] = (uint16_t)rxBuffer[4 + 2*i] << 8;
                                myRegBuf[i] |= rxBuffer[4 + 2*i + 1];
                            }
                            
                            // send back message
                            responseMessage(myMB_ADDR, func, size * 2, startReg, buffer);
                            
                            break;
                        }
                        
                        default: break;
                    }
                }
            }
        }
        
        rxCounter = 0;
        InitTimeout(&mbTimeout, myMB_SILENT); // reset timeout
    }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
