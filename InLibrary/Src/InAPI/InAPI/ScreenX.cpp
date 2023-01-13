// ScreenX.cpp

#include "StdAfx.h"
#include "ScreenX.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#define TOUCH_ROOTKEY		HKEY_LOCAL_MACHINE
#define TOUCH_SUBKEY		_T("HARDWARE\\DEVICEMAP\\TOUCH")
#define TOUCH_MAXCALERROR	_T("MaxCalError")

DWORD GetErrorAnalysisSensitivity(void)
{
	DWORD	dwMaxError	= 7;

	RegistryGetDWORD(TOUCH_ROOTKEY, TOUCH_SUBKEY, TOUCH_MAXCALERROR, &dwMaxError);
	return dwMaxError;
}

void SetErrorAnalysisSensitivity(DWORD dwMaxError)
{
	if ((dwMaxError < 1) || (dwMaxError > 20))
	{
		dwMaxError	= 7;
	}

	RegistrySetDWORD(TOUCH_ROOTKEY, TOUCH_SUBKEY, TOUCH_MAXCALERROR, dwMaxError);
}
