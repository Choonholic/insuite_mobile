// TimeX.cpp

#include "stdafx.h"
#include "TimeX.h"

LONG CompareSystemTime(const LPSYSTEMTIME lpSystemTime1, const LPSYSTEMTIME lpSystemTime2)
{
	FILETIME	ftTime1;
	FILETIME	ftTime2;

	SystemTimeToFileTime(lpSystemTime1, &ftTime1);
	SystemTimeToFileTime(lpSystemTime2, &ftTime2);
	return (CompareFileTime(&ftTime1, &ftTime2));
}

LONG GetSystemTimeSpan(ULONGLONG* ullSpan, const LPSYSTEMTIME lpSystemTime1, const LPSYSTEMTIME lpSystemTime2)
{
	FILETIME		ftTime1;
	FILETIME		ftTime2;
	ULARGE_INTEGER	uiTime1;
	ULARGE_INTEGER	uiTime2;

	SystemTimeToFileTime(lpSystemTime1, &ftTime1);
	SystemTimeToFileTime(lpSystemTime2, &ftTime2);

	uiTime1.LowPart		= ftTime1.dwLowDateTime;
	uiTime1.HighPart	= ftTime1.dwHighDateTime;
	uiTime2.LowPart		= ftTime2.dwLowDateTime;
	uiTime2.HighPart	= ftTime2.dwHighDateTime;

	LONG	lResult		= CompareFileTime(&ftTime1, &ftTime2);

	if (lResult < 0)
	{
		*ullSpan	= uiTime2.QuadPart - uiTime1.QuadPart;
	}
	else
	{
		*ullSpan	= uiTime1.QuadPart - uiTime2.QuadPart;
	}

	return lResult;
}

void AddSystemTime(LPSYSTEMTIME lpstDst, const LPSYSTEMTIME lpstSrc, ULONGLONG ullSpan)
{
	FILETIME		ftTimeSrc;
	FILETIME		ftTimeDst;
	ULARGE_INTEGER	uiTime;

	SystemTimeToFileTime(lpstSrc, &ftTimeSrc);

	uiTime.LowPart	= ftTimeSrc.dwLowDateTime;
	uiTime.HighPart	= ftTimeSrc.dwHighDateTime;
	uiTime.QuadPart	+= ullSpan;

	ftTimeDst.dwLowDateTime		= uiTime.LowPart;
	ftTimeDst.dwHighDateTime	= uiTime.HighPart;

	FileTimeToSystemTime(&ftTimeDst, lpstDst);
}

void AddSystemTime(LPSYSTEMTIME lpSystemTime, ULONGLONG ullSpan)
{
	FILETIME		ftTimeSrc;
	FILETIME		ftTimeDst;
	ULARGE_INTEGER	uiTime;

	SystemTimeToFileTime(lpSystemTime, &ftTimeSrc);

	uiTime.LowPart	= ftTimeSrc.dwLowDateTime;
	uiTime.HighPart	= ftTimeSrc.dwHighDateTime;
	uiTime.QuadPart	+= ullSpan;

	ftTimeDst.dwLowDateTime		= uiTime.LowPart;
	ftTimeDst.dwHighDateTime	= uiTime.HighPart;

	FileTimeToSystemTime(&ftTimeDst, lpSystemTime);
}

void SubSystemTime(LPSYSTEMTIME lpstDst, const LPSYSTEMTIME lpstSrc, ULONGLONG ullSpan)
{
	FILETIME		ftTimeSrc;
	FILETIME		ftTimeDst;
	ULARGE_INTEGER	uiTime;

	SystemTimeToFileTime(lpstSrc, &ftTimeSrc);

	uiTime.LowPart	= ftTimeSrc.dwLowDateTime;
	uiTime.HighPart	= ftTimeSrc.dwHighDateTime;
	uiTime.QuadPart	-= ullSpan;

	ftTimeDst.dwLowDateTime		= uiTime.LowPart;
	ftTimeDst.dwHighDateTime	= uiTime.HighPart;

	FileTimeToSystemTime(&ftTimeDst, lpstDst);
}

void SubSystemTime(LPSYSTEMTIME lpSystemTime, ULONGLONG ullSpan)
{
	FILETIME		ftTimeSrc;
	FILETIME		ftTimeDst;
	ULARGE_INTEGER	uiTime;

	SystemTimeToFileTime(lpSystemTime, &ftTimeSrc);

	uiTime.LowPart	= ftTimeSrc.dwLowDateTime;
	uiTime.HighPart	= ftTimeSrc.dwHighDateTime;
	uiTime.QuadPart	-= ullSpan;

	ftTimeDst.dwLowDateTime		= uiTime.LowPart;
	ftTimeDst.dwHighDateTime	= uiTime.HighPart;

	FileTimeToSystemTime(&ftTimeDst, lpSystemTime);
}
