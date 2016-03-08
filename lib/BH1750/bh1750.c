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
#include "bh1750.h"

#include "systick.h"
#include "i2c.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
I2C_Status BH1750_write8(uint8_t data);

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
void BH1750_Begin(uint8_t mode) {
    
    pinMode(BH1750_Port, BH1750_Pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    
    //while (1)
    {
    BH1750_OFF;
    delay_ms(100);
    BH1750_EN;
    delay_ms(500);
    }
    
    // Start BH
    if (BH1750_write8(BH1750_POWER_ON) == Success)
    {
        INFO("\nBH OK\n");
    }
    else
    {
        INFO("\nError\n");
    }
    delay_ms(500);
    
    // Config mode
    BH1750_Configure(mode);
    delay_ms(500);
}


void BH1750_Configure(uint8_t mode) {

    switch (mode) {
        case BH1750_CONTINUOUS_HIGH_RES_MODE:
        case BH1750_CONTINUOUS_HIGH_RES_MODE_2:
        case BH1750_CONTINUOUS_LOW_RES_MODE:
        case BH1750_ONE_TIME_HIGH_RES_MODE:
        case BH1750_ONE_TIME_HIGH_RES_MODE_2:
        case BH1750_ONE_TIME_LOW_RES_MODE:
            // apply a valid mode change
            if (BH1750_write8(mode) == Success)
            {
                #if BH1750_DEBUG == 1
                    printf("Apply Mode OK\n");
                #endif
            }
            else
            {
                #if BH1750_DEBUG == 1
                    printf("Apply Mode Error\n");
                #endif
            }
            delay_ms(10);
            break;
        default:
            // Invalid measurement mode
            #if BH1750_DEBUG == 1
                printf("Invalid measurement mode");
            #endif
            break;
    }
}


I2C_Status BH1750_ReadLightLevel(uint16_t* level) {
    
    uint8_t data[2];
    uint8_t *pBuffer;
    uint8_t Nbyte = 2;
    uint16_t lux;
    
    pBuffer = data;
    
    /* While the bus is busy */
    Timed(I2C_GetFlagStatus(BH1750_I2C, I2C_FLAG_BUSY));

    /* Enable I2C1 acknowledgement if it is already disabled by other function */
    I2C_AcknowledgeConfig(BH1750_I2C, ENABLE);

    /*----- Reception Phase -----*/

    /* Send START condition a second time */
    I2C_GenerateSTART(BH1750_I2C, ENABLE);

    /* Test on EV5 and clear it */
    Timed(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send slave address for read */
    I2C_Send7bitAddress(BH1750_I2C, BH1750_I2CADDR, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    Timed(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* While there is data to be read */
    while(Nbyte)
    {
        if(Nbyte == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(BH1750_I2C, DISABLE);

            /* Send STOP condition */
            I2C_GenerateSTOP(BH1750_I2C, ENABLE);
            Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
        }

        /* Test on EV7 and clear it */
        Timed(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED));

        /* Read a byte from the MPU6050 */
        *pBuffer = I2C_ReceiveData(BH1750_I2C);

        /* Point to the next location where the byte read will be saved */
        pBuffer++;

        /* Decrement the read bytes counter */
        Nbyte--;
    }
    
    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(BH1750_I2C, ENABLE);

    // Calc lux
    *level = (data[0] << 8) | data[1];
  
    #if BH1750_DEBUG == 1
    //printf("Raw light level: %d\n", lux);
    #endif

    *level = *level/1.2; // convert to lux

    #if BH1750_DEBUG == 1
    //printf("Lux: %d\n", lux);
    #endif
  
    // return Success
    return Success;

    // return Error
    errReturn:
    return Error;
}

I2C_Status BH1750_write8(uint8_t data)
{
    Timeout_Type timeout;
    InitTimeout(&timeout, BH1750_TIMEOUT);
    
    I2C_AcknowledgeConfig(BH1750_I2C, ENABLE);
    
    I2C_GenerateSTART(BH1750_I2C, ENABLE);
    InitTimeout(&timeout, BH1750_TIMEOUT);
	while (!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_MODE_SELECT)) // Wait for EV5
    {
 		if(CheckTimeout(&timeout) == TIMEOUT) { INFO("1"); return Error; }
    }
    
	I2C_Send7bitAddress(BH1750_I2C, BH1750_I2CADDR, I2C_Direction_Transmitter); // Send slave address
    InitTimeout(&timeout, BH1750_TIMEOUT);
	while (!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) // Wait for EV6
    {
        if(CheckTimeout(&timeout) == TIMEOUT) { INFO("2"); return Error; }
    }
    
	I2C_SendData(BH1750_I2C, data); // Send register value
    InitTimeout(&timeout, BH1750_TIMEOUT);
	while (!I2C_CheckEvent(BH1750_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED)) // Wait for EV8
    {
        if(CheckTimeout(&timeout) == TIMEOUT) { INFO("3"); return Error; }
    }
	I2C_GenerateSTOP(BH1750_I2C,ENABLE);
    
//     /* 1. Check if bus is busy *************************************/
//     InitTimeout(&timeout, BH1750_TIMEOUT);
//     while(I2C_GetFlagStatus(BH1750_I2C, I2C_FLAG_BUSY))
// 	{
// 		if(CheckTimeout(&timeout) == TIMEOUT) return Error;
// 	}
//     
//     /* 2. Send START condition *************************************/
//     I2C_GenerateSTART(BH1750_I2C, ENABLE);

//     /* Test on EV5 and clear it */
//     InitTimeout(&timeout, BH1750_TIMEOUT);
//     while(!I2C_GetFlagStatus(BH1750_I2C, I2C_FLAG_SB))
//     {
//         if (CheckTimeout(&timeout) == TIMEOUT) return Error;
//     }

//     /* 3. Send device address for write ***************************/
//     I2C_Send7bitAddress(BH1750_I2C, BH1750_I2CADDR, I2C_Direction_Transmitter);

//     /* Test on EV6 and clear it */
//     InitTimeout(&timeout, BH1750_TIMEOUT);
//     while(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
//     {
//         if (CheckTimeout(&timeout) == TIMEOUT) return Error;
//     }

//     /* Send the data to write to */
//     I2C_SendData(BH1750_I2C, data);

//     /* Test on EV8 and clear it */
//     InitTimeout(&timeout, BH1750_TIMEOUT);
//     while((!I2C_GetFlagStatus(BH1750_I2C, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(BH1750_I2C,I2C_FLAG_BTF)))
//     {
//         if (CheckTimeout(&timeout) == TIMEOUT) return Error;
//     }
//     
//     /* Send STOP condition */
//     I2C_GenerateSTOP(BH1750_I2C, ENABLE);
//     
//     /* Wait to make sure that STOP control bit has been cleared */
// 	InitTimeout(&timeout, BH1750_TIMEOUT);
// 	while(I2C_ReadRegister(BH1750_I2C, I2C_Register_CR1) & I2C_CR1_STOP)
// 	{
// 		if (CheckTimeout(&timeout) == TIMEOUT) return Error;
// 	}
    
    return Success;
}


/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/