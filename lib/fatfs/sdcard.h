/*-----------------------------------------------------------------------/
/  Low level sdcard interface modlue include file   (C)ChaN, 2013          /
/-----------------------------------------------------------------------*/

#ifndef _SDCARD_DEFINED
#define _SDCARD_DEFINED

#include "diskio.h"

DSTATUS MMC_disk_initialize (void);
DSTATUS MMC_disk_status (void);
DRESULT MMC_disk_read (BYTE *buff, DWORD sector, BYTE count);
DRESULT MMC_disk_write (const BYTE *buff, DWORD sector, BYTE count);
DRESULT MMC_disk_ioctl (BYTE ctrl, void *buff);
DSTATUS disk_initialize1 (
	BYTE drv		/* Physical drive nmuber (0) */
);

#endif
