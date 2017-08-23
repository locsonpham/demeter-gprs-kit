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

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void SPI_Config(SPI_TypeDef* comm)
{
    if (comm == SPI1) SPI1_Config();
    if (comm == SPI2) SPI2_Config();
}

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
void SPI1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    /* Connect SPI pins to AF5 */  
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);    
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI  MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* SPI GPIO Configuration --------------------------------------------------*/

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//     SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
    
	SPI_Cmd(SPI1, ENABLE);
}

void SPI2_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* Connect SPI pins to AF5 */  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);    
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* SPI  MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* SPI GPIO Configuration --------------------------------------------------*/

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//     SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
    
	SPI_Cmd(SPI2, ENABLE);
}

/*******************************************************************************
Function name:
Decription:
Input: None
Output: None
*******************************************************************************/
uint8_t SPI_SendByte(SPI_TypeDef* comm, unsigned char data)
{
	while(SPI_I2S_GetFlagStatus(comm, SPI_I2S_FLAG_TXE) == RESET);

	SPI_I2S_SendData(comm, data);

	while(SPI_I2S_GetFlagStatus(comm, SPI_I2S_FLAG_RXNE) == RESET);
    
	return SPI_I2S_ReceiveData(comm);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
