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
#include "i2c.h"

#include "systick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief
  * @param         
  * @param
  * @retval
  */
 /**
* @brief  Initializes the I2C peripheral
* @param  None
* @return None
*/
void I2C1_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	
	/* Enable I2C1 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	/* Enable GPIO Clock */
    RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Reset sEE_I2C IP */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* Release reset signal of sEE_I2C IP */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    
    /* GPIO Configure */
    /* CONFIG PB8 AND PB9 AS SCL AND SDA PINS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
 	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	
	// DeInit I2C1
    I2C_DeInit(I2C1);
    
    /* I2C1 configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = baud;
	
    /* I2C1 Peripheral Enable */
    I2C_Cmd(I2C1, ENABLE);
	
	/* Apply I2C1 configuration*/
    I2C_Init(I2C1, &I2C_InitStructure);
}

/******************* (C) COPYRIGHT 2015 UFOTech *****END OF FILE****/
