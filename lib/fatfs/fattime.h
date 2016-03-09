#ifndef FATTIME_H
#define FATTIME_H

#include "integer.h"

typedef struct {
	WORD	year;	/* 2000..2099 */
	BYTE	month;	/* 1..12 */
	BYTE	mday;	/* 1.. 31 */
	BYTE	wday;	/* 1..7 */
	BYTE	hour;	/* 0..23 */
	BYTE	min;	/* 0..59 */
	BYTE	sec;	/* 0..59 */
} RTC_T;

BOOL gettime (RTC_T*);					/* Get time */
BOOL settime (const RTC_T*);				/* Set time */
DWORD get_fattime (void);

#endif // FATTIME_H
