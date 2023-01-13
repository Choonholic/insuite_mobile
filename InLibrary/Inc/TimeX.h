// TimeX.h

#pragma once

#include "InAPI.h"

// Time Compare
LONG	INAPI_API	CompareSystemTime(const LPSYSTEMTIME lpSystemTime1, const LPSYSTEMTIME lpSystemTime2);

// Time Calculators
#ifndef OS_100NS
#define OS_100NS	(ULONGLONG)(10000000UL)
#define OM_100NS	(ULONGLONG)((ULONGLONG)(10000000UL) * (ULONGLONG)(60UL))
#define OH_100NS	(ULONGLONG)((ULONGLONG)(10000000UL) * (ULONGLONG)(3600UL))
#define OD_100NS	(ULONGLONG)((ULONGLONG)(10000000UL) * (ULONGLONG)(86400UL))
#endif

LONG	INAPI_API	GetSystemTimeSpan(ULONGLONG* ullSpan, const LPSYSTEMTIME lpSystemTime1, const LPSYSTEMTIME lpSystemTime2);
void	INAPI_API	AddSystemTime(LPSYSTEMTIME lpstDst, const LPSYSTEMTIME lpstSrc, ULONGLONG ullSpan);
void	INAPI_API	AddSystemTime(LPSYSTEMTIME lpSystemTime, ULONGLONG ullSpan);
void	INAPI_API	SubSystemTime(LPSYSTEMTIME lpstDst, const LPSYSTEMTIME lpstSrc, ULONGLONG ullSpan);
void	INAPI_API	SubSystemTime(LPSYSTEMTIME lpSystemTime, ULONGLONG ullSpan);
