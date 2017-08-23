/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN, 2007           */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "stm32f4xx.h"

#include "systick.h"

#define USE_SPI3

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD9	(0x40+9)	/* SEND_CSD */
#define CMD10	(0x40+10)	/* SEND_CID */
#define CMD12	(0x40+12)	/* STOP_TRANSMISSION */
#define ACMD13	(0xC0+13)	/* SD_STATUS (SDC) */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD18	(0x40+18)	/* READ_MULTIPLE_BLOCK */
#define CMD23	(0x40+23)	/* SET_BLOCK_COUNT (MMC) */
#define ACMD23	(0xC0+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD25	(0x40+25)	/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */

/* DEFINE THE MARCRO FOR ARM*/

#ifdef USE_SPI1
//     #define SPI_SD                   		SPI1

//     #define GPIO_SD_PWR						GPIOC
//     #define GPIO_Pin_SD_PWR     			GPIO_Pin_6
//     #define RCC_APB2Periph_GPIO_SD_PWR 	    RCC_APB2Periph_GPIOC

//     #define GPIO_CS                  		GPIOA
//     #define GPIO_Pin_CS              		GPIO_Pin_4
//     #define RCC_APB2Periph_GPIO_CS   		RCC_APB2Periph_GPIOA

//     #define GPIO_SPI_SD              		GPIOA
//     #define GPIO_Pin_SPI_SD_SCK      		GPIO_Pin_5
//     #define GPIO_Pin_SPI_SD_MISO     		GPIO_Pin_6
//     #define GPIO_Pin_SPI_SD_MOSI     		GPIO_Pin_7
//     #define RCC_APB2Periph_GPIO_SPI   		RCC_APB2Periph_GPIOA
//     #define RCC_APBPeriph_SPI_SD     		RCC_APB2Periph_SPI1
//     #define RCC_SD_CLOCK_INIT				RCC_APB2PeriphClockCmd
//     /* - for SPI1 and full-speed APB2: 72MHz/4 */
//     #define SPI_BaudRateHighSpeed_SPI_SD	SPI_BaudRatePrescaler_4
//     #define SPI_BaudRateLowSpeed_SPI_SD  	SPI_BaudRatePrescaler_256
#endif

#ifdef USE_SPI2
//     #define SPI_SD                   		SPI2

//     #define GPIO_SD_PWR						GPIOA
//     #define GPIO_Pin_SD_PWR     			GPIO_Pin_4
//     #define RCC_APB2Periph_GPIO_SD_PWR 	    RCC_APB2Periph_GPIOA

//     #define GPIO_CS                  		GPIOB
//     #define GPIO_Pin_CS              		GPIO_Pin_12
//     #define RCC_APB2Periph_GPIO_CS   		RCC_APB2Periph_GPIOB

//     #define GPIO_SPI_SD              		GPIOB
//     #define GPIO_Pin_SPI_SD_SCK      		GPIO_Pin_13
//     #define GPIO_Pin_SPI_SD_MISO     		GPIO_Pin_14
//     #define GPIO_Pin_SPI_SD_MOSI     		GPIO_Pin_15
//     #define RCC_APB2Periph_GPIO_SPI   		RCC_APB2Periph_GPIOB
//     #define RCC_APBPeriph_SPI_SD     		RCC_APB1Periph_SPI2
//     #define RCC_SD_CLOCK_INIT				RCC_APB1PeriphClockCmd
//     /* - for SPI2 and full-speed APB1: 36MHz/4 */
//     #define SPI_BaudRateHighSpeed_SPI_SD	SPI_BaudRatePrescaler_4
//     #define SPI_BaudRateLowSpeed_SPI_SD  	SPI_BaudRatePrescaler_256
#endif

#ifdef USE_SPI3
    #define SPI_SD                   		SPI3
    
    #define RCC_APBPeriph_SDClockCmd        RCC_APB1PeriphClockCmd
    #define RCC_APBPeriph_SPIClockCmd       RCC_APB1PeriphClockCmd
    
    #define GPIO_SD_PWR						GPIOC
    #define GPIO_Pin_SD_PWR     			GPIO_Pin_9
    #define RCC_APBPeriph_GPIO_SD_PWR 	    RCC_AHB1Periph_GPIOC

    #define GPIO_CS                  		GPIOC
    #define GPIO_Pin_CS              		GPIO_Pin_8
    #define RCC_APBPeriph_GPIO_CS   		RCC_AHB1Periph_GPIOC
    

    #define GPIO_SPI_SD              		GPIOC
    #define GPIO_Pin_SPI_SD_SCK      		GPIO_Pin_10
    #define GPIO_Pin_SPI_SD_MISO     		GPIO_Pin_11
    #define GPIO_Pin_SPI_SD_MOSI     		GPIO_Pin_12
    #define RCC_APBPeriph_GPIO_SPI   		RCC_AHB1Periph_GPIOC
    
    #define RCC_APBPeriph_SPI_SD     		RCC_APB1Periph_SPI3
    /* - for SPI3 and full-speed APB1: 36MHz/4 */
    #define SPI_BaudRateHighSpeed_SPI_SD	SPI_BaudRatePrescaler_32
    #define SPI_BaudRateLowSpeed_SPI_SD  	SPI_BaudRatePrescaler_256
#endif



/* Port Controls  (Platform dependent) */
#define SELECT()			GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS)
#define DESELECT()			GPIO_SetBits(GPIO_CS, GPIO_Pin_CS)
#define SD_PWR_ON()     	GPIO_SetBits(GPIO_SD_PWR, GPIO_Pin_SD_PWR)			/* SD power on */
#define SD_PWR_OFF()     	GPIO_ResetBits(GPIO_SD_PWR, GPIO_Pin_SD_PWR)		/* SD power off */

#define SOCKPORT	1			/* Socket contact port */
#define SOCKWP		0			/* Write protect switch (PB5) */
#define SOCKINS		0			/* Card detect switch (PB4) */
/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/
void delay_sd(uint32_t time)
{
	delay_us(time);
}


static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

#ifdef	SUPPORT_TIMEOUT
static volatile
BYTE Timer1, Timer2;	/* 100Hz decrement timer */
#endif

static
BYTE CardType;			/* Card type flags */

enum speed_setting { INTERFACE_SLOW, INTERFACE_FAST };

static void interface_speed( enum speed_setting speed )
{
	DWORD tmp;
	tmp = SPI_SD->CR1;
	if ( speed == INTERFACE_SLOW ) {
		/* Set slow clock (100k-400k) */
		tmp = ( tmp | SPI_BaudRateLowSpeed_SPI_SD );
	} else {
		/* Set fast clock (depends on the CSD) */
		tmp = ( tmp & ~SPI_BaudRateLowSpeed_SPI_SD ) | SPI_BaudRateHighSpeed_SPI_SD;
	}
	SPI_SD->CR1 = tmp;
}

BYTE sd_raw_rw_spi(BYTE b_data)
{
	BYTE Data = 0;

	  /* Wait until the transmit buffer is empty */
	  while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE) == RESET);
	  /* Send the byte */
	  SPI_I2S_SendData(SPI_SD, b_data);

	  /* Wait until a data is received */
	  while (SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_RXNE) == RESET);
	  /* Get the received data */
	  Data = SPI_I2S_ReceiveData(SPI_SD);

	  /* Return the shifted data */
	  return Data;
}

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

#define xmit_spi(dat) 		sd_raw_rw_spi(dat)		//SPDR=(dat); loop_until_bit_is_set(SPSR,SPIF)


/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static
BYTE rcvr_spi (void)
{
	return sd_raw_rw_spi(0xFF);
}

/* Alternative macro to receive data fast */
#define rcvr_spi_m(dst)	*(dst) = sd_raw_rw_spi(0xFF)			//SPDR=0xFF; loop_until_bit_is_set(SPSR,SPIF); *(dst)=SPDR

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
BYTE wait_ready (void)
{
	BYTE res;

#ifdef SUPPORT_TIMEOUT
	Timer2 = 50;	/* Wait for ready in timeout of 500ms */
#endif

	rcvr_spi();
	do
		res = rcvr_spi();
#ifdef	SUPPORT_TIMEOUT
	while ((res != 0xFF) && Timer2);
#else
	while ((res != 0xFF));
#endif
	return res;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void release_spi (void)
{
	DESELECT();
	rcvr_spi();
}

/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static
void power_on (void)
{

#ifdef SUPPORT_TIMEOUT
	for (Timer1 = 3; Timer1; );	/* Wait for 30ms */
#endif

	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;
	volatile BYTE dummyread;

	/* Enable the SPI clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    
    /* Connect SPI pins to AF5 */  
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);    
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* SPI  MISO pin configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);  

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure CS pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure power pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	 /* power on sd card */
  	SD_PWR_ON();
	delay_sd(10);

	DESELECT();

	/* SPI Config */
	SPI_I2S_DeInit(SPI_SD);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
// 	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
// 	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRateLowSpeed_SPI_SD;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_SD, &SPI_InitStructure);

	SPI_CalculateCRC(SPI_SD, DISABLE);

	/* SPI enable */
	SPI_Cmd(SPI_SD, ENABLE);

	/* drain SPI */
	while(SPI_I2S_GetFlagStatus(SPI_SD, SPI_I2S_FLAG_TXE) == RESET) { ; }
	dummyread = SPI_I2S_ReceiveData(SPI_SD);
}

static
void power_off (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	if (!(Stat & STA_NOINIT)) {
		SELECT();
		wait_ready();
		release_spi();
	}

	SPI_Cmd(SPI_SD, DISABLE);
	SPI_I2S_DeInit(SPI_SD);

	RCC_APBPeriph_SPIClockCmd(RCC_APBPeriph_SPI_SD, DISABLE);

	/* All SPI-Pins to input with weak internal pull-downs */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_SPI_SD_SCK | GPIO_Pin_SPI_SD_MISO | GPIO_Pin_SPI_SD_MOSI;  
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_Init(GPIO_SPI_SD, &GPIO_InitStructure);

	Stat |= STA_NOINIT;		/* Set STA_NOINIT */
}

static
int chk_power(void)		/* Socket power state: 0=off, 1=on */
{
	// CHECK POWER
	return  1;
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
BOOL rcvr_datablock (
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
)
{
	BYTE token;

#ifdef	SUPPORT_TIMEOUT
	Timer1 = 20;
#endif

	do {							/* Wait for data packet in timeout of 200ms */
		token = rcvr_spi();
#ifdef SUPPORT_TIMEOUT
	} while ((token == 0xFF) && Timer1);
#else
	} while ((token == 0xFF));
#endif

	if(token != 0xFE) return FALSE;	/* If not valid data token, retutn with error */

	do {							/* Receive the data block into buffer */
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
	} while (btr -= 4);
	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	return TRUE;					/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
static
BOOL xmit_datablock (
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data/Stop token */
)
{
	BYTE resp, wc;


	if (wait_ready() != 0xFF) return FALSE;

	xmit_spi(token);					/* Xmit data token */
	if (token != 0xFD) {	/* Is data token */
		wc = 0;
		do {							/* Xmit the 512 byte data block to MMC */
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (--wc);
		xmit_spi(0xFF);					/* CRC (Dummy) */
		xmit_spi(0xFF);
		resp = rcvr_spi();				/* Reveive data response */
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return FALSE;
	}

	return TRUE;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	DESELECT();
	SELECT();
	if (wait_ready() != 0xFF) return 0xFF;

	/* Send command packet */
	xmit_spi(cmd);						/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	BYTE n, cmd, ty, ocr[4];


	if (drv) return STA_NOINIT;			/* Supports only single drive */
	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

	power_on();							/* Force socket power on */

	interface_speed( INTERFACE_SLOW );
	
	for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks */
	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */

#ifdef	SUPPORT_TIMEOUT
		Timer1 = 100;						/* Initialization timeout of 1000 msec */
#endif

		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDHC */
			for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
#ifdef	SUPPORT_TIMEOUT
				while (Timer1 && send_cmd(ACMD41, 1UL << 30));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
#else
				while (send_cmd(ACMD41, 1UL << 30));				/* Wait for leaving idle state (ACMD41 with HCS bit) */
#endif

#ifdef	SUPPORT_TIMEOUT
				if (Timer1 && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
#else
				if (send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
#endif
					for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		} else {							/* SDSC or MMC */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
#ifdef	SUPPORT_TIMEOUT
			while (Timer1 && send_cmd(cmd, 0));			/* Wait for leaving idle state */
#else
			while (send_cmd(cmd, 0));			/* Wait for leaving idle state */
#endif

#ifdef 	SUPPORT_TIMEOUT
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
#else
			if (send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
#endif
				ty = 0;
		}
	}
	CardType = ty;
	release_spi();

	if (ty) {			/* Initialization succeded */
		Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */

		interface_speed( INTERFACE_FAST);
	} else {			/* Initialization failed */
		power_off();
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512))
			count = 0;
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	release_spi();

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	release_spi();

	return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY == 0 */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL != 0
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	WORD csize;


	if (drv) return RES_PARERR;

	res = RES_ERROR;

	if (ctrl == CTRL_POWER) {
		switch (*ptr) {
		case 0:		/* Sub control code == 0 (POWER_OFF) */
			if (chk_power())
				power_off();		/* Power off */
			res = RES_OK;
			break;
		case 1:		/* Sub control code == 1 (POWER_ON) */
			power_on();				/* Power on */
			res = RES_OK;
			break;
		case 2:		/* Sub control code == 2 (POWER_GET) */
			*(ptr+1) = (BYTE)chk_power();
			res = RES_OK;
			break;
		default :
			res = RES_PARERR;
		}
	}
	else {
		if (Stat & STA_NOINIT) return RES_NOTRDY;

		switch (ctrl) {
		case CTRL_SYNC :		/* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
			SELECT();
			if (wait_ready() == 0xFF)
				res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)csize << 10;
				} else {					/* SDC ver 1.XX or MMC*/
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			if (CardType & CT_SD2) {	/* SDC ver 2.00 */
				if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16)) {				/* Read partial block */
						for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {					/* SDC ver 1.XX or MMC */
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
					if (CardType & CT_SD1) {	/* SDC ver 1.XX */
						*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {					/* MMC */
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
			if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
			if (send_cmd(CMD10, 0) == 0		/* READ_CID */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 4; n; n--) *ptr++ = rcvr_spi();
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
			if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(ptr, 64))
					res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
		}

		release_spi();
	}

	return res;
}
#endif /* _USE_IOCTL != 0 */


/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void disk_timerproc (void)
{
	static BYTE pv;
	BYTE n, s;

#ifdef	SUPPORT_TIMEOUT
	n = Timer1;						/* 100Hz decrement timer */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;
#endif

	n = pv;
	pv = SOCKPORT & (SOCKWP | SOCKINS);	/* Sample socket switch */

	if (n == pv) {					/* Have contacts stabled? */
		s = Stat;

		if (pv & SOCKWP)			/* WP is H (write protected) */
			s |= STA_PROTECT;
		else						/* WP is L (write enabled) */
			s &= ~STA_PROTECT;

		if (pv & SOCKINS)			/* INS = H (Socket empty) */
			s |= (STA_NODISK | STA_NOINIT);
		else						/* INS = L (Card inserted) */
			s &= ~STA_NODISK;

		Stat = s;
	}
}
