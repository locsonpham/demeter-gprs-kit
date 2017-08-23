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
uint8_t BH1750_write8(uint8_t data);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief
  * @param         
  * @param
  * @retval
  */
void BH1750_Begin(uint8_t mode) 
{
    #ifdef BH1750_POWER_CONTROL
    pinMode(BH1750_Port, BH1750_Pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    BH1750_OFF;
    delay_ms(100);
    BH1750_EN;
    delay_ms(500);
    #endif
    
    // Start BH
    if (BH1750_write8(BH1750_POWER_ON) == Success)
    {
        BH_Info("\nBH OK\n");
    }
    else
    {
        BH_Info("\nError\n");
    }
    delay_ms(100);
    
    // Config mode
    BH1750_Configure(mode);
    delay_ms(100);
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
                    BH_Info("Apply Mode OK\n");
            }
            else
            {
                    BH_Info("Apply Mode Error\n");
            }
            delay_ms(10);
            break;
        default:
            // Invalid measurement mode
            BH_Info("Invalid measurement mode");
            break;
    }
}


uint8_t BH1750_ReadLightLevel(int32_t* level) 
{
    uint8_t data[2];
    uint8_t *pBuffer;
    uint8_t Nbyte = 2;
    uint16_t lux;
	float flevel = 0;
    
    pBuffer = data;
    
    /* While the bus is busy */
    Timed_I2C(I2C_GetFlagStatus(BH1750_I2C, I2C_FLAG_BUSY));

    /* Enable I2C1 acknowledgement if it is already disabled by other function */
    I2C_AcknowledgeConfig(BH1750_I2C, ENABLE);

    /*----- Reception Phase -----*/

    /* Send START condition a second time */
    I2C_GenerateSTART(BH1750_I2C, ENABLE);

    /* Test on EV5 and clear it */
    Timed_I2C(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send slave address for read */
    I2C_Send7bitAddress(BH1750_I2C, BH1750_I2CADDR, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    Timed_I2C(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    /* While there is data to be read */
    while(Nbyte)
    {
        if(Nbyte == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(BH1750_I2C, DISABLE);

            /* Send STOP condition */
            I2C_GenerateSTOP(BH1750_I2C, ENABLE);
            Timed_I2C(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
        }

        /* Test on EV7 and clear it */
        Timed_I2C(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED));

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
    lux = (data[0] << 8) | data[1];
    flevel = (float)lux/1.2; // convert to real lux
    
	*level = (uint16_t)flevel; // update result
    // return Success
    return 0;
	
	// return Error
	errReturn:
    *level = -1;
	return 1;
}

uint8_t BH1750_write8(uint8_t data)
{
    I2C_AcknowledgeConfig(BH1750_I2C, ENABLE);
    
    I2C_GenerateSTART(BH1750_I2C, ENABLE);
	Timed_I2C(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_MODE_SELECT));
    
	I2C_Send7bitAddress(BH1750_I2C, BH1750_I2CADDR, I2C_Direction_Transmitter); // Send slave address
	Timed_I2C(!I2C_CheckEvent(BH1750_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    
	I2C_SendData(BH1750_I2C, data); // Send register value
	Timed_I2C(!I2C_CheckEvent(BH1750_I2C,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(BH1750_I2C,ENABLE);
    
	// return Success
    return 1;
	
	// return Error
	errReturn:
	return 0;
}


/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
