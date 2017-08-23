/**
  ******************************************************************************
  * @file    uart.c 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief	Module UART
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "uart.h"
#include "interrupt.h"

/* Private macro -------------------------------------------------------------*/
volatile uart_param uart1, uart3, uart5;

/* Private variables ---------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/

void UART_Config(USART_TypeDef *u, uint32_t baud)
{
    if (u == USART1) UART1_Init(baud);
    else if (u == USART2) UART2_Init(baud);
    else if (u == USART3) UART3_Init(baud);
    else if (u == UART4) UART4_Init(baud);
    else if (u == UART5) UART5_Init(baud);
}

/*******************************************************************************
Function name: UART1_Init
Decription: None
Input: None
Output: None
*******************************************************************************/
void UART1_Init(uint32_t baud_rate)
{
    /* This is a concept that has to do with the libraries provided by ST
    * to make development easier the have made up something similar to
    * classes, called TypeDefs, which actually just define the common
    * parameters that every peripheral needs to work correctly
    *
    * They make our life easier because we don't have to mess around with
    * the low level stuff of setting bits in the correct registers
    */
    GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
    USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
    NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

    /* enable APB2 peripheral clock for USART1
    * note that only USART1 and USART6 are connected to APB2
    * the other USARTs are connected to APB1
    */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* enable the peripheral clock for the pins used by
    * USART1, PA9 for TX and PA10 for RX
    */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* This sequence sets up the TX and RX pins
    * so they work correctly with the USART1 peripheral
    */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // Pins 9 (TX) and 10 (RX) are used
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                         // the pins are configured as alternate function so the USART peripheral has access to them
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                // this defines the IO speed and has nothing to do with the baudrate!
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                        // this defines the output type as push pull mode (as opposed to open drain)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;                        // this activates the pullup resistors on the IO pins
    GPIO_Init(GPIOB, &GPIO_InitStruct);                                        // now all the values are passed to the GPIO_Init() function which sets the GPIO registers

    /* The RX and TX pins are now connected to their AF
    * so that the USART1 can take over control of the
    * pins
    */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); //
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    /* Now the USART_InitStruct is used to define the
    * properties of USART1
    */
    USART_InitStruct.USART_BaudRate = baud_rate;                                // the baudrate is set to the value we passed into this init function
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
    USART_InitStruct.USART_StopBits = USART_StopBits_1;                // we want 1 stop bit (standard)
    USART_InitStruct.USART_Parity = USART_Parity_No;                // we don't want a parity bit (standard)
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
    USART_Init(USART1, &USART_InitStruct);                                        // again all the properties are passed to the USART_Init function which takes care of all the bit setting

    /* Enable the USART1 global Interrupt */
    NVIC_SetPriority(USART1_IRQn, UART1_IRQ_Group);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_IRQ_Priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

    /* Here the USART1 receive interrupt is enabled
    * and the interrupt controller is configured
    * to jump to the USART1_IRQHandler() function
    * if the USART1 receive interrupt occurs
    */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

//     NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                 // we want to configure the USART1 interrupts
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                 // this sets the subpriority inside the group
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                         // the USART1 interrupts are globally enabled
//     NVIC_Init(&NVIC_InitStructure);                                                         // the properties are passed to the NVIC_Init function which takes care of the low level stuff        

    // finally this enables the complete USART1 peripheral
    USART_Cmd(USART1, ENABLE);
}

/*******************************************************************************
Function name: UART2_Init
Decription: None
Input: None
Output: None
*******************************************************************************/
void UART2_Init(uint32_t baud_rate)
{
    GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
    USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
    NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

    /* enable APB2 peripheral clock for USART1
    * note that only USART1 and USART6 are connected to APB2
    * the other USARTs are connected to APB1
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* enable the peripheral clock for the pins used by
    * USART2, PA2 for TX and PA3 for RX
    */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* This sequence sets up the TX and RX pins
    * so they work correctly with the USART1 peripheral
    */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_5; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
	
    /* The RX and TX pins are now connected to their AF
    * so that the USART1 can take over control of the
    * pins
    */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

    /* Now the USART_InitStruct is used to define the
    * properties of USART5
    */
    USART_InitStruct.USART_BaudRate = baud_rate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStruct);


    /* Here the USART1 receive interrupt is enabled
    * and the interrupt controller is configured
    * to jump to the USART1_IRQHandler() function
    * if the USART1 receive interrupt occurs
    */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // finally this enables the complete USART1 peripheral
    USART_Cmd(USART2, ENABLE);
}

/*******************************************************************************
Function name: UART3_Init
Decription: None
Input: None
Output: None
*******************************************************************************/
void UART3_Init(uint32_t baud_rate)
{
    /* This is a concept that has to do with the libraries provided by ST
    * to make development easier the have made up something similar to
    * classes, called TypeDefs, which actually just define the common
    * parameters that every peripheral needs to work correctly
    *
    * They make our life easier because we don't have to mess around with
    * the low level stuff of setting bits in the correct registers
    */
    GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
    USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
    NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

    /* enable APB2 peripheral clock for USART1
    * note that only USART1 and USART6 are connected to APB2
    * the other USARTs are connected to APB1
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* enable the peripheral clock for the pins used by
    * USART3, PC10 for TX and PC11 for RX
    */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* This sequence sets up the TX and RX pins
    * so they work correctly with the USART1 peripheral
    */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; // Pins 6 (TX) and 7 (RX) are used
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;                         // the pins are configured as alternate function so the USART peripheral has access to them
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                // this defines the IO speed and has nothing to do with the baudrate!
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;                        // this defines the output type as push pull mode (as opposed to open drain)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;                        // this activates the pullup resistors on the IO pins
    GPIO_Init(GPIOB, &GPIO_InitStruct);                                        // now all the values are passed to the GPIO_Init() function which sets the GPIO registers

    /* The RX and TX pins are now connected to their AF
    * so that the USART1 can take over control of the
    * pins
    */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

    /* Now the USART_InitStruct is used to define the
    * properties of USART3
    */
    USART_InitStruct.USART_BaudRate = baud_rate;                                // the baudrate is set to the value we passed into this init function
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;// we want the data frame size to be 8 bits (standard)
    USART_InitStruct.USART_StopBits = USART_StopBits_1;                // we want 1 stop bit (standard)
    USART_InitStruct.USART_Parity = USART_Parity_No;                // we don't want a parity bit (standard)
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // we don't want flow control (standard)
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; // we want to enable the transmitter and the receiver
    USART_Init(USART3, &USART_InitStruct);                                        // again all the properties are passed to the USART_Init function which takes care of all the bit setting


    /* Here the USART1 receive interrupt is enabled
    * and the interrupt controller is configured
    * to jump to the USART1_IRQHandler() function
    * if the USART1 receive interrupt occurs
    */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;                 // we want to configure the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// this sets the priority group of the USART1 interrupts
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                 // this sets the subpriority inside the group
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                         // the USART1 interrupts are globally enabled
    NVIC_Init(&NVIC_InitStructure);                                                         // the properties are passed to the NVIC_Init function which takes care of the low level stuff        

    // finally this enables the complete USART1 peripheral
    USART_Cmd(USART3, ENABLE);
	
	uart3.uartx = USART3;
	uart3.RxPackedIncomeFlag = 0;
	uart3.RxCounter = 0;
	uart3.Length = 0;
}

/*******************************************************************************
Function name: UART4_Init
Decription: None
Input: None
Output: None
*******************************************************************************/
void UART4_Init(uint32_t baud_rate)
{
    GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
    USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
    NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

    /* enable APB2 peripheral clock for USART1
    * note that only USART1 and USART6 are connected to APB2
    * the other USARTs are connected to APB1
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    /* enable the peripheral clock for the pins used by
    * USART4, PA0 for TX and PA1 for RX
    */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* This sequence sets up the TX and RX pins
    * so they work correctly with the USART1 peripheral
    */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
	
    /* The RX and TX pins are now connected to their AF
    * so that the USART1 can take over control of the
    * pins
    */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4);

    /* Now the USART_InitStruct is used to define the
    * properties of USART5
    */
    USART_InitStruct.USART_BaudRate = baud_rate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(UART4, &USART_InitStruct);


    /* Here the USART1 receive interrupt is enabled
    * and the interrupt controller is configured
    * to jump to the USART1_IRQHandler() function
    * if the USART1 receive interrupt occurs
    */
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // finally this enables the complete USART1 peripheral
    USART_Cmd(UART4, ENABLE);
}

/*******************************************************************************
Function name: UART5_Init
Decription: None
Input: None
Output: None
*******************************************************************************/
void UART5_Init(uint32_t baud_rate)
{
    GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
    USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
    NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

    /* enable APB2 peripheral clock for USART1
    * note that only USART1 and USART6 are connected to APB2
    * the other USARTs are connected to APB1
    */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

    /* enable the peripheral clock for the pins used by
    * USART5, PC12 for TX and PD2 for RX
    */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);

    /* This sequence sets up the TX and RX pins
    * so they work correctly with the USART1 peripheral
    */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
    /* The RX and TX pins are now connected to their AF
    * so that the USART1 can take over control of the
    * pins
    */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);

    /* Now the USART_InitStruct is used to define the
    * properties of USART5
    */
    USART_InitStruct.USART_BaudRate = baud_rate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(UART5, &USART_InitStruct);


    /* Here the USART1 receive interrupt is enabled
    * and the interrupt controller is configured
    * to jump to the USART1_IRQHandler() function
    * if the USART1 receive interrupt occurs
    */
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // finally this enables the complete USART1 peripheral
    USART_Cmd(UART5, ENABLE);
}

void UART6_Init(uint32_t baud_rate)
{
    GPIO_InitTypeDef GPIO_InitStruct; // this is for the GPIO pins used as TX and RX
    USART_InitTypeDef USART_InitStruct; // this is for the USART1 initilization
    NVIC_InitTypeDef NVIC_InitStructure; // this is used to configure the NVIC (nested vector interrupt controller)

    /* enable APB2 peripheral clock for USART1
    * note that only USART1 and USART6 are connected to APB2
    * the other USARTs are connected to APB1
    */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    /* enable the peripheral clock for the pins used by
    * USART5, PC12 for TX and PD2 for RX
    */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD, ENABLE);

    /* This sequence sets up the TX and RX pins
    * so they work correctly with the USART1 peripheral
    */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12; 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
    /* The RX and TX pins are now connected to their AF
    * so that the USART1 can take over control of the
    * pins
    */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);

    /* Now the USART_InitStruct is used to define the
    * properties of USART5
    */
    USART_InitStruct.USART_BaudRate = baud_rate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(UART5, &USART_InitStruct);


    /* Here the USART1 receive interrupt is enabled
    * and the interrupt controller is configured
    * to jump to the USART1_IRQHandler() function
    * if the USART1 receive interrupt occurs
    */
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // enable the USART1 receive interrupt

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // finally this enables the complete USART1 peripheral
    USART_Cmd(UART5, ENABLE);
}

/*******************************************************************************
Function name: UART_Send
Decription:	Send byte via USART Module
Input: None
Output: None
*******************************************************************************/
uint8_t u_send(USART_TypeDef *uart, uint8_t data)
{
	USART_SendData(uart, data);
        
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(uart, USART_FLAG_TC) == RESET);
    
    return data;
}

// UART send string
void u_printf(USART_TypeDef *uart, char* s)
{
	while (*s)
	{
		u_send(uart, *s);
		s++;
	}
}

/*******************************************************************************
Function name: PUTCHAR_PROTOTYPE
Decription:	Retargets the C library printf function to the USART.
Input: None
Output: None
*******************************************************************************/
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(USART_DEBUG, (uint8_t) ch);

    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART_DEBUG, USART_FLAG_TC) == RESET)
    {}

    return ch;
}
