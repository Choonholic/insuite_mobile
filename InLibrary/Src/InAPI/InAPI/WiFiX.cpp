// WiFiX.cpp

#include "StdAfx.h"
#include "WiFiX.h"
#include "snapi.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#define WIFI_ROOTKEY	HKEY_LOCAL_MACHINE
#define WIFI_SUBKEY		_T("Comm\\TNETWLN1\\Parms")
#define WIFI_VALUE		_T("Dot11SupportedRateMaskG")

DWORD GetDot11SupportedRate(void)
{
	DWORD	dwRate	= 8;

	RegistryGetDWORD(WIFI_ROOTKEY, WIFI_SUBKEY, WIFI_VALUE, &dwRate);
	return dwRate;
}

void SetDot11SupportedRate(DWORD dwRate)
{
	if (dwRate > 8)
	{
		dwRate	= 8;
	}

	RegistrySetDWORD(WIFI_ROOTKEY, WIFI_SUBKEY, WIFI_VALUE, dwRate);
}

DWORD GetWiFiStatus(void)
{
#if (_WIN32_WCE >= 0x0502)
	DWORD	dwData	= 0;
	HRESULT	hr;

	hr	= RegistryGetDWORD(SN_WIFISTATECONNECTED_ROOT, SN_WIFISTATECONNECTED_PATH, SN_WIFISTATECONNECTED_VALUE, &dwData);

	if (hr == S_OK)
	{
		return dwData;
	}
#endif

	return 0;
}
