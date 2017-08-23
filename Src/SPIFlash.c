/*********************************************************************
 *
 *  SPI Flash Memory Driver
 *  - Tested with SST 25VF016B 
 *  - Expected compatibility with other SST (Microchip) SST25 series 
 *    devices
 *
 *********************************************************************
 * FileName:        SPIFlash.c
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 * Author               		Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * E. Wood              		3/20/08 Original
 * Dave Collier/H. Schlunder	6/09/10	Update for SST25VF010A
********************************************************************/

/**
  ******************************************************************************
  * @file    
  * @author Edited - lspham
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "spiflash.h"

#include "spi.h"
#include "defines.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define READ				0x03    // SPI Flash opcode: Read up up to 25MHz
#define READ_FAST			0x0B    // SPI Flash opcode: Read up to 50MHz with 1 dummy byte
#define ERASE_4K			0x20    // SPI Flash opcode: 4KByte sector erase
#define ERASE_32K			0x52    // SPI Flash opcode: 32KByte block erase
#define ERASE_SECTOR		0xD8    // SPI Flash opcode: 64KByte block erase
#define ERASE_ALL			0x60    // SPI Flash opcode: Entire chip erase
#define WRITE				0x02    // SPI Flash opcode: Write one byte (or a page of up to 256 bytes, depending on device)
#define WRITE_uint16_t_STREAM	0xAD    // SPI Flash opcode: Write continuous stream of 16-bit words (AAI mode); available on SST25VF016B (but not on SST25VF010A)
#define WRITE_uint8_t_STREAM	0xAF    // SPI Flash opcode: Write continuous stream of bytes (AAI mode); available on SST25VF010A (but not on SST25VF016B)
#define RDSR				0x05    // SPI Flash opcode: Read Status Register
#define EWSR				0x50    // SPI Flash opcode: Enable Write Status Register
#define WRSR				0x01    // SPI Flash opcode: Write Status Register
#define WREN				0x06    // SPI Flash opcode: Write Enable
#define WRDI				0x04    // SPI Flash opcode: Write Disable / End AAI mode
#define RDID				0x90    // SPI Flash opcode: Read ID
#define JEDEC_ID			0x9F    // SPI Flash opcode: Read JEDEC ID
#define EBSY				0x70    // SPI Flash opcode: Enable write BUSY status on SO pin
#define DBSY				0x80    // SPI Flash opcode: Disable write BUSY status on SO pin

#define BUSY    0x01    // Mask for Status Register BUSY (Internal Write Operaiton in Progress status) bit
#define WEL     0x02    // Mask for Status Register WEL (Write Enable status) bit
#define BP0     0x04    // Mask for Status Register BP0 (Block Protect 0) bit
#define BP1     0x08    // Mask for Status Register BP1 (Block Protect 1) bit
#define BP2     0x10    // Mask for Status Register BP2 (Block Protect 2) bit
#define BP3     0x20    // Mask for Status Register BP3 (Block Protect 3) bit
#define AAI     0x40    // Mask for Status Register AAI (Auto Address Increment Programming status) bit
#define BPL     0x80    // Mask for Status Register BPL (BPx block protect bit read-only protect) bit

#define SPI_FLASH_SECTOR_MASK 0x00000FFF

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Internal pointer to address being written
static uint32_t dwWriteAddr;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

// SPI Flash device capabilities
static union
{
	unsigned char v;
	struct
	{
		unsigned char bWriteWordStream : 1;	// Supports AAI Word opcode (0xAD)
		unsigned char bWriteByteStream : 1;	// Supports AAI Byte opcode (0xAF)
		unsigned char bPageProgram : 1;		// Supports Byte program opcode with up to 256 bytes/page (0x02)
		unsigned char filler : 5;
	} bits;
} deviceCaps;


static uint8_t _SendCmd(uint8_t cmd);
static uint8_t _WaitWhileBusy(void);
static uint8_t _GetStatus(void);

void SST_SPI_Config(void)
{
	SPI_Config(SST_SPI);
}

uint8_t SST_WriteByte(uint32_t u8Data)
{
    return SPI_SendByte(SST_SPI, u8Data);
}


/*****************************************************************************
  Function:
    void SPIFlashInit(void)

  Description:
    Initializes SPI Flash module.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is only called once during the lifetime of the application.

  Internal:
    This function sends WRDI to clear any pending write operation, and also
    clears the software write-protect on all memory locations.
  ***************************************************************************/
void SPIFlashInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
    uint8_t Dummy;
    
    // Init CS Pin
    RCC_AHB1PeriphClockCmd(SST_CS_RCC, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = SST_CS_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;                     
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SST_CS_Port, &GPIO_InitStruct);
    
    // DeSelect SST
    SST_CS_DeAssert();
	
	// Config SPI
	SST_SPI_Config();

	// Read Device ID code to determine supported device capabilities/instructions
	{
	    // Activate chip select
	    SST_CS_Assert();
	
	    // Send instruction
		Dummy = SST_WriteByte(RDID);
		
		// Send 3 byte address (0x000000), discard Manufacture ID, get Device ID
	    for(int i = 0; i < 5; i++)
	    {
			Dummy = SST_WriteByte(0x00);
		}
        
        INFO("\nType: %x\n", Dummy);

	    // Deactivate chip select
	    SST_CS_DeAssert();
		
		// Decode Device Capabilities Flags from Device ID
		deviceCaps.v = 0x00;
		switch(Dummy)
		{
			case 0x43:	// SST25LF020(A)	(2 Mbit)	0xAF, 14us, AAI Byte
			case 0x48:	// SST25VF512(A)	(512 Kbit)	0xAF, 14us, AAI Byte
			case 0x49:	// SST25VF010A		(1 Mbit)	0xAF, 14us, AAI Byte
				deviceCaps.bits.bWriteByteStream = 1;
				break;
				
			case 0x4B:	// SST25VF064C		(64 Mbit)	0x02, 1.5ms/256 byte page, no AAI
				deviceCaps.bits.bPageProgram = 1;
				break;

			//case 0x01:	// SST25WF512		(512 Kbit)	0xAD, 50us, AAI Word
			//case 0x02:	// SST25WF010		(1 Mbit)	0xAD, 50us, AAI Word
			//case 0x03:	// SST25WF020		(2 Mbit)	0xAD, 50us, AAI Word
			//case 0x04:	// SST25WF040		(4 Mbit)	0xAD, 50us, AAI Word
			//case 0x05:	// SST25WF080		(8 Mbit)	0xAD, 14us, AAI Word
			//case 0x41:	// SST25VF016B		(16 Mbit)	0xAD,  7us, AAI Word
			//case 0x4A:	// SST25VF032B		(32 Mbit)	0xAD,  7us, AAI Word
			//case 0x8C:	// SST25VF020B		(2 Mbit)	0xAD,  7us, AAI Word
			//case 0x8D:	// SST25VF040B		(4 Mbit)	0xAD,  7us, AAI Word
			//case 0x8E:	// SST25VF080B		(8 Mbit)	0xAD,  7us, AAI Word				
			// Assume AAI Word programming is supported for the above commented 
			// devices and unknown devices.
			default:	
				deviceCaps.bits.bWriteWordStream = 1;
		}
	}


    // Clear any pre-existing AAI write mode
    // This may occur if the PIC is reset during a write, but the Flash is
    // not tied to the same hardware reset.
    _SendCmd(WRDI);

    // Execute Enable-Write-Status-Register (EWSR) instruction
    _SendCmd(EWSR);

    // Clear Write-Protect on all memory locations
    SST_CS_Assert();
	Dummy = SST_WriteByte(WRSR);
    Dummy = SST_WriteByte(0x00); // Clear all block protect bits
    SST_CS_DeAssert();
}


/*****************************************************************************
  Function:
    void SPIFlashReadArray(uint32_t dwAddress, uint8_t *vData, uint16_t wLength)

  Description:
    Reads an array of bytes from the SPI Flash module.

  Precondition:
    SPIFlashInit has been called, and the chip is not busy (should be
    handled elsewhere automatically.)

  Parameters:
    dwAddress - Address from which to read
    vData - Where to store data that has been read
    wLength - Length of data to read

  Returns:
    None
  ***************************************************************************/
void SPIFlashReadArray(uint32_t dwAddress, uint8_t *vData, uint16_t wLength)
{
    volatile uint8_t Dummy;

    // Ignore operations when the destination is NULL or nothing to read
    if(wLength == 0)
        return;

    // Activate chip select
    SST_CS_Assert();

    // Send READ opcode
    Dummy = SST_WriteByte(READ);

    // Send address
	SST_WriteByte(dwAddress >> 16);
	SST_WriteByte(dwAddress >> 8);
	SST_WriteByte(dwAddress);

    // Read data
    while(wLength--)
    {
        *vData++ = SST_WriteByte(0x00);
    }

    // Deactivate chip select
    SST_CS_DeAssert();
}

/*****************************************************************************
  Function:
    void SPIFlashBeginWrite(uint32_t dwAddr)

  Summary:
    Prepares the SPI Flash module for writing.

  Description:
    Prepares the SPI Flash module for writing.  Subsequent calls to
    SPIFlashWrite or SPIFlashWriteArray will begin at this location and
    continue sequentially.

    SPI Flash

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    dwAddr - Address where the writing will begin

  Returns:
    None

  Remarks:
    Flash parts have large sector sizes, and can only erase entire sectors
    at once.  The SST parts for which this library was written have sectors
    that are 4kB in size.  Your application must ensure that writes begin on
    a sector boundary so that the SPIFlashWrite functions will erase the
    sector before attempting to write.  Entire sectors need not be written
    at once, so applications can begin writing to the front of a sector,
    perform other tasks, then later call SPIFlashBeginWrite and point to an
    address in this sector that has not yet been programmed.  However, care
    must taken to ensure that writes are not attempted on addresses that are
    not in the erased state.  The chip will provide no indication that the
    write has failed, and will silently ignore the command.
  ***************************************************************************/
void SPIFlashBeginWrite(uint32_t dwAddr)
{
    dwWriteAddr = dwAddr;
}

/*****************************************************************************
  Function:
    void SPIFlashWrite(uint8_t vData)

  Summary:
    Writes a byte to the SPI Flash part.

  Description:
    This function writes a byte to the SPI Flash part.  If the current
    address pointer indicates the beginning of a 4kB sector, the entire
    sector will first be erased to allow writes to proceed.  If the current
    address pointer indicates elsewhere, it will be assumed that the sector
    has already been erased.  If this is not true, the chip will silently
    ignore the write command.

  Precondition:
    SPIFlashInit and SPIFlashBeginWrite have been called, and the current
    address is either the front of a 4kB sector or has already been erased.

  Parameters:
    vData - The byte to write to the next memory location.

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPIFlashWrite(uint8_t vData)
{
    volatile uint8_t Dummy;

    // If address is a boundary, erase a sector first
    if((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0u)
        SPIFlashEraseSector(dwWriteAddr);

    // Enable writing
    _SendCmd(WREN);

    // Activate the chip select
    SST_CS_Assert();

    // Issue WRITE command with address
    Dummy = SST_WriteByte(WRITE);
	SST_WriteByte(dwWriteAddr >> 16);
	SST_WriteByte(dwWriteAddr >> 8);
	SST_WriteByte(dwWriteAddr);

    // Write the byte
	Dummy = SST_WriteByte(vData);
    dwWriteAddr++;

    // Deactivate chip select and wait for write to complete
    SST_CS_DeAssert();
    _WaitWhileBusy();
}

/*****************************************************************************
  Function:
    void SPIFlashWriteArray(uint8_t* vData, uint16_t wLen)

  Summary:
    Writes an array of bytes to the SPI Flash part.

  Description:
    This function writes an array of bytes to the SPI Flash part.  When the
    address pointer crosses a sector boundary (and has more data to write),
    the next sector will automatically be erased.  If the current address
    pointer indicates an address that is not a sector boundary and is not
    already erased, the chip will silently ignore the write command until the
    next sector boundary is crossed.

  Precondition:
    SPIFlashInit and SPIFlashBeginWrite have been called, and the current
    address is either the front of a sector or has already been erased.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPIFlashWriteArray(uint8_t* vData, uint16_t wLen)
{
    volatile uint8_t Dummy;
    BOOL isStarted;
    uint8_t vOpcode;
    uint8_t i;

	// Do nothing if no data to process
	if(wLen == 0u)
		return;

    // If starting at an odd address, write a single byte
    if((dwWriteAddr & 0x01) && wLen)
    {
        SPIFlashWrite(*vData);
        vData++;
        wLen--;
    }

	// Assume we are using AAI Word program mode unless changed later
	vOpcode = WRITE_uint16_t_STREAM;	

    isStarted = FALSE;

    // Loop over all remaining uint16_ts
    while(wLen > 1)
    {
        // Don't do anything until chip is ready
        _WaitWhileBusy();

        // If address is a sector boundary
        if((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0)
            SPIFlashEraseSector(dwWriteAddr);

        // If not yet started, initiate AAI mode
        if(!isStarted)
        {
            // Enable writing
            _SendCmd(WREN);

			// Select appropriate programming opcode.  The WRITE_uint16_t_STREAM 
			// mode is the default if neither of these flags are set.
	        if(deviceCaps.bits.bWriteByteStream)
	            vOpcode = WRITE_uint8_t_STREAM;
			else if(deviceCaps.bits.bPageProgram)
			{
				// Note: Writing one byte at a time is extremely slow (ex: ~667 
				// bytes/second write speed on SST SST25VF064C).  You can 
				// improve this by over a couple of orders of magnitude by 
				// writing a function to write full pages of up to 256 bytes at 
				// a time.  This is implemented this way only because I don't 
				// have an SST25VF064C handy to test with right now. -HS
				while(wLen--)
			        SPIFlashWrite(*vData++);
				return;
			}

            // Activate the chip select
            SST_CS_Assert();

            // Issue WRITE_xxx_STREAM command with address
			Dummy = SST_WriteByte(vOpcode);
            SST_WriteByte(dwWriteAddr >> 16);
            SST_WriteByte(dwWriteAddr >> 8);
            SST_WriteByte(dwWriteAddr);

            isStarted = TRUE;
        }
        // Otherwise, just write the AAI command again
        else
        {
            // Assert the chip select pin
            SST_CS_Assert();

            // Issue the WRITE_STREAM command for continuation
            Dummy = SST_WriteByte(vOpcode);
        }

        // Write a byte or two
        for(i = 0; i <= deviceCaps.bits.bWriteWordStream; i++)
        {
	        Dummy = SST_WriteByte(*(vData++));
            wLen--;
            dwWriteAddr++;
		}

        // Release the chip select to begin the write
        SST_CS_DeAssert();

        // If a boundary was reached, end the write
        if((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0)
        {
            _WaitWhileBusy();
            _SendCmd(WRDI);
            isStarted = FALSE;
        }
    }

    // Wait for write to complete, then exit AAI mode
    _WaitWhileBusy();
    _SendCmd(WRDI);

    // If a byte remains, write the odd address
    if(wLen)
        SPIFlashWrite(*vData);
}


/*****************************************************************************
  Function:
    void SPIFlashEraseSector(uint32_t dwAddr)

  Summary:
    Erases a sector.

  Description:
    This function erases a sector in the Flash part.  It is called
    internally by the SPIFlashWrite functions whenever a write is attempted
    on the first byte in a sector.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    dwAddr - The address of the sector to be erased.

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPIFlashEraseSector(uint32_t dwAddr)
{
    volatile uint8_t Dummy;

    // Enable writing
    _SendCmd(WREN);

    // Activate the chip select
    SST_CS_Assert();
    Dummy = SST_WriteByte(ERASE_4K);
    SST_WriteByte(dwAddr >> 16);
	SST_WriteByte(dwAddr >> 8);
	SST_WriteByte(dwAddr);

    // Deactivate chip select to perform the erase
    SST_CS_DeAssert();

    // Wait for erase to complete
    _WaitWhileBusy();
}

void SPIFlashEraseAll(void)
{
	volatile uint8_t Dummy;

    // Enable writing
    _SendCmd(WREN);

    // Activate the chip select
    SST_CS_Assert();
    Dummy = SST_WriteByte(ERASE_ALL);

    // Deactivate chip select to perform the erase
    SST_CS_DeAssert();

    // Wait for erase to complete
    _WaitWhileBusy();
}


/*****************************************************************************
  Function:
    static void _SendCmd(uint8_t cmd)

  Summary:
    Sends a single-byte command to the SPI Flash part.

  Description:
    This function sends a single-byte command to the SPI Flash part.  It is
    used for commands such as WREN, WRDI, and EWSR that must have the chip
    select activated, then deactivated immediately after the command is
    transmitted.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    cmd - The single-byte command code to send

  Returns:
    None
  ***************************************************************************/
static uint8_t _SendCmd(uint8_t cmd)
{
    // Activate chip select
    SST_CS_Assert();
	
	cmd = SST_WriteByte(cmd);

    // Deactivate chip select
    SST_CS_DeAssert();
    
    return cmd;
}


/*****************************************************************************
  Function:
    static void _WaitWhileBusy(void)

  Summary:
    Waits for the SPI Flash part to indicate it is idle.

  Description:
    This function waits for the SPI Flash part to indicate it is idle.  It is
    used in the programming functions to wait for operations to complete.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
static uint8_t _WaitWhileBusy(void)
{
    volatile uint8_t Dummy;
    uint32_t timeout = 1000000;

    while((_GetStatus() & BUSY) && timeout)
	{
		timeout--;
	}
    
    return 0;
}

/*****************************************************************************
  Function:
    static void _GetStatus()

  Summary:
    Reads the status register of the part.

  Description:
    This function reads the status register of the part.  It was written
    for debugging purposes, and is not needed for normal operation.  Place
    a breakpoint at the last instruction and check the "status" variable to
    see the result.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
static uint8_t _GetStatus()
{
    uint8_t res;
	SST_CS_Assert();       // assert CS
	SST_WriteByte(0x05);       // READ status command
	res = SST_WriteByte(0xFF); // Read back status register content
	SST_CS_DeAssert();     // de-assert cs
	return res;
}

uint8_t SST25_Write(uint32_t addr, uint8_t *buf, uint32_t count)
{
    SPIFlashBeginWrite(addr);
    SPIFlashWriteArray(buf, count);
}

uint8_t SST25_Read(uint32_t addr, uint8_t *buf, uint32_t count)
{
    SPIFlashReadArray(addr, buf, count);
}

uint8_t SST25_Erase(uint32_t addr, enum SST25_ERASE_MODE mode)
{
	switch(mode)
    {
        case all:
        {
            volatile uint8_t Dummy;

            // Enable writing
            _SendCmd(WREN);

            // Activate the chip select
            SST_CS_Assert();
            Dummy = SST_WriteByte(ERASE_ALL);

            // Deactivate chip select to perform the erase
            SST_CS_DeAssert();

            // Wait for erase to complete
            _WaitWhileBusy();
        } break;

        case block4k:
        {
            volatile uint8_t Dummy;

            // Enable writing
            _SendCmd(WREN);

            // Activate the chip select
            SST_CS_Assert();
            Dummy = SST_WriteByte(ERASE_4K);
            SST_WriteByte(addr >> 16);
            SST_WriteByte(addr >> 8);
            SST_WriteByte(addr);

            // Deactivate chip select to perform the erase
            SST_CS_DeAssert();

            // Wait for erase to complete
            _WaitWhileBusy();
        } break;

        case block32k:
        {
            volatile uint8_t Dummy;

            // Enable writing
            _SendCmd(WREN);

            // Activate the chip select
            SST_CS_Assert();
            Dummy = SST_WriteByte(ERASE_32K);
            SST_WriteByte(addr >> 16);
            SST_WriteByte(addr >> 8);
            SST_WriteByte(addr);

            // Deactivate chip select to perform the erase
            SST_CS_DeAssert();

            // Wait for erase to complete
            _WaitWhileBusy();
        } break;

        case block64k:
        {
            volatile uint8_t Dummy;

            // Enable writing
            _SendCmd(WREN);

            // Activate the chip select
            SST_CS_Assert();
            Dummy = SST_WriteByte(ERASE_SECTOR);
            SST_WriteByte(addr >> 16);
            SST_WriteByte(addr >> 8);
            SST_WriteByte(addr);

            // Deactivate chip select to perform the erase
            SST_CS_DeAssert();

            // Wait for erase to complete
            _WaitWhileBusy();
        } break;
        
        default: break;
    }

	return 0;
}
