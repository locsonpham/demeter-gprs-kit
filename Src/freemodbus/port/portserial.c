/*
 * FreeModbus Libary: ATMega168 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *   - Initial version and ATmega168 support
 * Modfications Copyright (C) 2006 Tran Minh Hoang:
 *   - ATmega8, ATmega16, ATmega32 support
 *   - RS485 support for DS75176
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.6 2006/09/17 16:45:53 wolti Exp $
 */

#include "stm32f4xx_conf.h"

#include "port.h"
#include "interrupt.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
    {
        USART_ITConfig( USART6, USART_IT_RXNE, ENABLE );
    }
    else
    {
        USART_ITConfig( USART6, USART_IT_RXNE, DISABLE );
    }

    if ( xTxEnable )
    {
        USART_ITConfig( USART6, USART_IT_TXE, ENABLE );

#ifdef RTS_ENABLE
        RTS_HIGH;
#endif
    }
    else
    {
        USART_ITConfig( USART6, USART_IT_TXE, DISABLE );
    }
}

BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
        eMBParity eParity )
{
    NVIC_InitTypeDef        NVIC_InitStructure;
    GPIO_InitTypeDef        GPIO_InitStructure;
    USART_InitTypeDef       USART_InitStructure;
    USART_ClockInitTypeDef  USART_ClockInitStructure;

    /* prevent compiler warning. */
    (void) ucPORT;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART6, ENABLE );
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE );

    // И вот еще функция, которой не было при работе с STM32F10x,
    // но которую нужно вызывать при использовании STM32F4xx
    GPIO_PinAFConfig( GPIOC, GPIO_PinSource6, GPIO_AF_USART6 );
    GPIO_PinAFConfig( GPIOC, GPIO_PinSource7, GPIO_AF_USART6 );


    GPIO_StructInit( &GPIO_InitStructure );

    //Настраиваем UARTx Tx (PC.6) как выход
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    //Настраиваем UARTx Rx (PC.7) как выход
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7    ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

//    // Настройка UART4_RTS
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init( GPIOA, &GPIO_InitStructure );
//    GPIO_WriteBit( GPIOA, GPIO_Pin_1, Bit_RESET );


    USART_DeInit( USART6 );

    USART_ClockStructInit( &USART_ClockInitStructure );

    USART_ClockInit( USART6, &USART_ClockInitStructure );

    // настройка скорости обмена
    USART_InitStructure.USART_BaudRate = (uint32_t)ulBaudRate;

    if( ucDataBits == 9 )
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
    else
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    // выбор количества стоп-битов
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
//    if( comm_settings->stop_bits == 2 )
//        USART_InitStructure.USART_StopBits = USART_StopBits_2;
//    else
//        USART_InitStructure.USART_StopBits = USART_StopBits_1;

    // выбор паритета. По умолчанию - отсутствует
    switch( eParity )
    {
    case MB_PAR_NONE:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    case MB_PAR_ODD:
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
        break;
    case MB_PAR_EVEN:
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        break;
    default:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    };

    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    //Инициализация параметров UARTx
    USART_Init( USART6, &USART_InitStructure );

//    USART_ITConfig( UART4,  USART_IT_RXNE, ENABLE );
//    USART_ITConfig( UART4,  USART_IT_TXE, ENABLE );
    
    NVIC_SetPriority(USART6_IRQn, UART6_IRQ_Group);
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART6_IRQ_Priority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

//     NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;           // канал
//     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   // приоритет
//     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          // приоритет субгруппы
//     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             // включаем канал
//     NVIC_Init(&NVIC_InitStructure);                             // инициализируем

    // разрешаем работу UART4
    USART_Cmd( USART6, ENABLE );

    vMBPortSerialEnable( TRUE, TRUE );

#ifdef RTS_ENABLE
    RTS_INIT;
#endif
    return TRUE;
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    USART_SendData( USART6, (uint16_t) ucByte );
    return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = (CHAR) USART_ReceiveData( USART6 );
    return TRUE;
}

void USART6_IRQHandler( void )
{
    if ( USART_GetITStatus( USART6, USART_IT_RXNE ) != RESET )
    {
        USART_ClearITPendingBit( USART6, USART_IT_RXNE );
        pxMBFrameCBByteReceived();

    }
    if ( USART_GetITStatus( USART6, USART_IT_TXE ) != RESET )
    {
        USART_ClearITPendingBit( USART6, USART_IT_TXE );
        pxMBFrameCBTransmitterEmpty();
    }
}

//SIGNAL( SIG_USART_DATA )
//{
//    pxMBFrameCBTransmitterEmpty(  );
//}
//
//SIGNAL( SIG_USART_RECV )
//{
//
//}

#ifdef RTS_ENABLE
SIGNAL( SIG_UART_TRANS )
{
    RTS_LOW;
}
#endif

