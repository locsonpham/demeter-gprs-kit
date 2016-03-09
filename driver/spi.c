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
#include "spi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief SPI Config for all SPIx Periph
  * @param         
  * @param
  * @retval
  */
void SPI_Config(SPI_TypeDef* comm)
{
    if      (comm == SPI1)  SPI1_Config();
    else if (comm == SPI2)  SPI2_Config();
}

/**
  * @brief SPI1 Config
  * @param         
  * @param
  * @retval
  */
void SPI1_Config(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	
    /* GPIOA and AFIO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1, ENABLE);

    /* CONFIG PA5,PA7 AS SCK AND MOSI PINS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* CONFIG PA6 AS MISO PINS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* SPI1 Config ----------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI1_MODE;
    SPI_InitStructure.SPI_DataSize = SPI1_DATASIZE;
    SPI_InitStructure.SPI_CPOL = SPI1_CPOL;
    SPI_InitStructure.SPI_CPHA = SPI1_CPHA;
    SPI_InitStructure.SPI_NSS = SPI1_NSS;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI1_PRESCALER;
    SPI_InitStructure.SPI_FirstBit = SPI1_FIRSTBIT;
    SPI_Init(SPI1, &SPI_InitStructure);

    /* Enable SPI1 */
    SPI_Cmd(SPI1, ENABLE);
}

/**
  * @brief SPI2 Config
  * @param         
  * @param
  * @retval
  */
void SPI2_Config()
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	
    /* GPIOB and AFIO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	/* CONFIG PB13, PB15 AS SCK AND MOSI PINS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* CONFIG PB14 AS MISO PINS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* SPI2 Config ----------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI2_MODE;
    SPI_InitStructure.SPI_DataSize = SPI2_DATASIZE;
    SPI_InitStructure.SPI_CPOL = SPI2_CPOL;
    SPI_InitStructure.SPI_CPHA = SPI2_CPHA;
    SPI_InitStructure.SPI_NSS = SPI2_NSS;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI2_PRESCALER;
    SPI_InitStructure.SPI_FirstBit = SPI2_FIRSTBIT;
    SPI_Init(SPI2, &SPI_InitStructure);
 
    SPI_Cmd(SPI2, ENABLE);
}


/**
  * @brief Send and receive data via SPI
  * @param         
  * @param
  * @retval
  */
uint8_t SPI_SendByte(SPI_TypeDef* comm, unsigned char data)
{
	while(SPI_I2S_GetFlagStatus(comm, SPI_I2S_FLAG_TXE) == RESET);

	SPI_I2S_SendData(comm, data);

	while(SPI_I2S_GetFlagStatus(comm, SPI_I2S_FLAG_RXNE) == RESET);
    
	return SPI_I2S_ReceiveData(comm);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
