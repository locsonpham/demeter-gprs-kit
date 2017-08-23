/**
  ******************************************************************************
  * @file    flash.c 
  * @author  Nguyen Van Hoa Vu - Pham Son Loc - Robot Club BK HCM
  * @version
  * @date
  * @brief	Main flash body
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t flash_data[FLASH_BUFFER] = {0};

uint32_t uwStartSector = 0;
uint32_t uwEndSector = 0;
uint32_t uwAddress = 0;
uint32_t uwSectorCounter = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
Function name: Flash_Init
Decription:
*******************************************************************************/
void Flash_Write(void)
{
    /* Unlock the Flash *********************************************************/
  /* Enable the flash control register access */
  FLASH_Unlock();
    
  /* Erase the user Flash area ************************************************/
  /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

  /* Get the number of the start and end sectors */
  uwStartSector = GetSector(FLASH_USER_START_ADDR);
  uwEndSector = GetSector(FLASH_USER_END_ADDR);

  /* Strat the erase operation */
  uwSectorCounter = uwStartSector;
  while (uwSectorCounter <= uwEndSector) 
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (FLASH_EraseSector(uwSectorCounter, VoltageRange_3) != FLASH_COMPLETE)
    { 
      /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
      while (1)
      {
      }
    }
    /* jump to the next sector */
    if (uwSectorCounter == FLASH_Sector_11)
    {
      uwSectorCounter += 40;
    } 
    else 
    {
      uwSectorCounter += 8;
    }
  }
  
  /* Program the user Flash area word by word ********************************/
  /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */

  uwAddress = FLASH_USER_START_ADDR;
  int usAddress = FLASH_USER_START_ADDR + sizeof(FLASH_BUFFER); // end address - mean User Stop Write Address
  int i = 0;

  while (uwAddress < usAddress)
  {
    for (i = 0; i < FLASH_BUFFER; i++)
    {
        if (FLASH_ProgramByte(uwAddress, flash_data[i]) == FLASH_COMPLETE)
        {
            uwAddress = uwAddress++;
        }
        else
        { 
            /* Error occurred while writing data in Flash memory. 
            User can add here some code to deal with this error */
            while (1)
            {
            }
        }
    }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) */
  FLASH_Lock(); 
}

/*******************************************************************************
Function name: Flash_Read
Decription:
*******************************************************************************/
void Flash_Read(void)
{
	uint16_t i = 0;
    
    /* Check the correctness of written data */
    uwAddress = FLASH_USER_START_ADDR;

    while((uwAddress < FLASH_USER_START_ADDR + FLASH_BUFFER))
    {	  
        flash_data[i]=(*(__IO uint8_t*) uwAddress);
        uwAddress++;
        i++;
    }
}

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  
#if defined (USE_STM324xG_EVAL)
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }

#else /* USE_STM324x7I_EVAL */  
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_Sector_11;  
  }

  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_Sector_12;  
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_Sector_13;  
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_Sector_14;  
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_Sector_15;  
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_Sector_16;  
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_Sector_17;  
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_Sector_18;  
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_Sector_19;  
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_Sector_20;  
  } 
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_Sector_21;  
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_Sector_22;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_Sector_23;  
  }
#endif /* USE_STM324x7I_EVAL */
  return sector;
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/
