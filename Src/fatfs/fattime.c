#include "integer.h"
#include "fattime.h"

BOOL gettime (RTC_T *rtc)
{
	rtc->sec = 10;			/*0-59*/
	rtc->min = 50;			/*0-59*/
	rtc->hour = 10;			/*0-23*/
	rtc->wday = 2;			/*1-7*/
	rtc->mday = 30;			/*1-31*/
	rtc->month = 11;			/*1-12*/
	rtc->year = 2009;			/*2009>*/
	return TRUE;
}

BOOL settime (const RTC_T *rtc)
{
	rtc = rtc;
	return TRUE;
}

DWORD get_fattime (void)
{
	RTC_T rtc;
	
	/* Get local time */
	gettime(&rtc);

	/* Pack date and time into a DWORD variable */
	return	  ((DWORD)(rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.mday << 16)
			| ((DWORD)rtc.hour << 11)
			| ((DWORD)rtc.min << 5)
			| ((DWORD)rtc.sec >> 1);
}

