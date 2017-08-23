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
#include "mbhandler.h"

#include "defines.h"
#include "sensor.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

 __IO USHORT   usRegHoldingStart = REG_HOLDING_START;
 USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

/* Private function prototypes -----------------------------------------------*/
void relay_update(void);
void digital_input_update(void);
void environment_update(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Relay control
  * @param  None
  * @retval None
  */
void relay_update()
{
    if (usRegHoldingBuf[13]) REL0_ON; else REL0_OFF;
    if (usRegHoldingBuf[14]) REL1_ON; else REL1_OFF;
    if (usRegHoldingBuf[15]) REL2_ON; else REL2_OFF;
}

/**
  * @brief  Update digital
  * @param  None
  * @retval None
  */
void digital_input_update()
{
}

/**
  * @brief  Update environment
  * @param  None
  * @retval None
  */
void environment_update()
{
}

void systemUpdate(void)
{
    
}

void user_update(void)
{
    
}

void timeUpdate(void)
{
    
}

/**
  * @brief  Modbus Routine
  * @param  None
  * @retval None
  */
void mbPolling(void)
{
    (void) eMBPoll();
    
    // Update Modbus
    usRegHoldingBuf[0] = (uint16_t)(temperature * 10);
    usRegHoldingBuf[1] = (uint16_t)(humidity * 10);
    usRegHoldingBuf[2] = (uint16_t)(lux);
    usRegHoldingBuf[3] = (uint16_t)(rainSum);
    usRegHoldingBuf[4] = (uint16_t)(windSpeed);
    usRegHoldingBuf[5] = (uint16_t)(windDir);
    // [6] reset rainSum
    usRegHoldingBuf[7] = (uint16_t)(voltage * 10);
    // [8] REL0
    // [9] REL1
    // [10] REL2
    
    // rest rainSum
    if (usRegHoldingBuf[6] != 0)
    {
        rainSum = 0;
        usRegHoldingBuf[6] = 0;
    }
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs )
{
//     eMBErrorCode eStatus = MB_ENOERR;
//     int iRegIndex;

//     if ( ( usAddress >= REG_INPUT_START ) &&
//          ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
//     {
//         iRegIndex = (int) ( usAddress - usRegInputStart );
//         while ( usNRegs > 0 )
//         {
//             *pucRegBuffer++ =
//                     (unsigned char) ( usRegInputBuf[ iRegIndex ] >> 8 );
//             *pucRegBuffer++ = (unsigned char) ( usRegInputBuf[ iRegIndex ]
//                     & 0xFF );
//             iRegIndex++;
//             usNRegs--;
//         }
//     }
//     else
//     {
//         eStatus = MB_ENOREG;
//     }

//     return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNCoils, eMBRegisterMode eMode )
{
//     eMBErrorCode eStatus = MB_ENOERR;
//     short iNCoils = (short) usNCoils;
//     unsigned short usBitOffset;

// //     / * Check if we have registers mapped at this block. * /
//     if ((usAddress >= REG_COILS_START) 
//         && (usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE))
//     {
//         usBitOffset = (unsigned short) (usAddress - REG_COILS_START);
//         switch (eMode)
//         {
// //                 / * Read current values ??and pass to protocol stack. * /
//             case MB_REG_READ:
//                 while (iNCoils > 0)
//                 {
//                     *pucRegBuffer ++=
//                         xMBUtilGetBits (pucCoilBuf, usBitOffset,
//                                         (unsigned char) (iNCoils > 8 ? 8 : iNCoils));
//                     iNCoils -= 8;
//                     usBitOffset += 8;
//                 }
//                 break;

// //                 / * Update current register values. * /
//             case MB_REG_WRITE:
//                 while (iNCoils > 0)
//                 {
//                     xMBUtilSetBits (pucCoilBuf, usBitOffset,
//                                     (unsigned char) (iNCoils > 8 ? 8 : iNCoils),
//                                     *pucRegBuffer ++);
//                     iNCoils -= 8;
//                 }
//                 break;
//         }

//     }
//     else
//     {
//         eStatus = MB_ENOREG;
//     }
//     return eStatus;
}

// eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
// {
//     eMBErrorCode    eStatus = MB_ENOERR;
//     int             iRegIndex , iRegBitIndex , iNReg;
//     iNReg =  usNCoils / 8 + 1; 

//     if( ( usAddress >= COIL_START ) &&
//         ( usAddress + usNCoils <= COIL_START + COIL_NCOILS ) )
//     {
//         iRegIndex    = ( int )( usAddress - usCoilStart ) / 8 ;    
// 		iRegBitIndex = ( int )( usAddress - usCoilStart ) % 8 ;	
//         switch ( eMode )
//         {
//             /* Pass current coil values to the protocol stack. */
//         case MB_REG_READ:
//             while( iNReg > 0 )
//             {
// 				*pucRegBuffer++ = xMBUtilGetBits(&pucCoilBuf[iRegIndex++] , iRegBitIndex , 8);
//                 iNReg --;
//             }
// 			pucRegBuffer --;
// 			usNCoils = usNCoils % 8;                        
// 			*pucRegBuffer = *pucRegBuffer <<(8 - usNCoils);
// 			*pucRegBuffer = *pucRegBuffer >>(8 - usNCoils);
//             break;

//             /* Update current coil values with new values from the
//              * protocol stack. */
//         case MB_REG_WRITE:
//             while(iNReg > 1)								
//             {
// 				xMBUtilSetBits(&pucCoilBuf[iRegIndex++] , iRegBitIndex  , 8 , *pucRegBuffer++);
//                 iNReg--;
//             }
// 			usNCoils = usNCoils % 8;                            
// 			if (usNCoils != 0)                                  //xMBUtilSetBits
// 			{
// 				xMBUtilSetBits(&pucCoilBuf[iRegIndex++], iRegBitIndex, usNCoils,
// 						*pucRegBuffer++);
// 			}
// 			break;
//         }
//     }
//     else
//     {
//         eStatus = MB_ENOREG;
//     }
//     return eStatus;
// }

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNDiscrete )
{
    return MB_ENOREG;
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/