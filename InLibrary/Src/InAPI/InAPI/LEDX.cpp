// LEDX.cpp

#include "stdafx.h"
#include "LEDX.h"
#include "nled.h"

UINT GetLEDCount(void)
{
	UINT			uiCount	= 0;
	NLED_COUNT_INFO	nci;

	if (NLedGetDeviceInfo(NLED_COUNT_INFO_ID, (PVOID)&nci))
	{
		uiCount	= static_cast<UINT>(nci.cLeds);
	}

	return uiCount;
}

BOOL SetLEDStatus(UINT uiLed, INT iStatus)
{
	NLED_SETTINGS_INFO	nsi;

	nsi.LedNum		= uiLed;
	nsi.OffOnBlink	= iStatus;

	return NLedSetDevice(NLED_SETTINGS_INFO_ID, &nsi);
}
