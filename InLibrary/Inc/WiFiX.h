// WiFiX.h

#pragma once

#include "InAPI.h"

// Legends
// 0 ... 1, 2 Mbps
// 1 ... 1, 2, 5.5, 11 Mbps
// 2 ... 1, 2, 5.5, 11, 22 Mbps
// 3 ... Up to 18Mbps
// 4 ... Up to 24Mbps
// 5 ... Up to 36Mbps
// 6 ... Up to 48Mbps
// 7 ... Up to 54Mbps
// 8 ... All

DWORD	INAPI_API	GetDot11SupportedRate(void);
void	INAPI_API	SetDot11SupportedRate(DWORD dwRate = 8);

#ifndef WIFI_ERROR
#define WIFI_ERROR				0x00
#define WIFI_PRESENT			0x01
#define WIFI_POWERON			0x02
#define WIFI_NETWORKSAVAILABLE	0x04
#define WIFI_CONNECTING			0x08
#define WIFI_CONNECTED			0x10
#endif

DWORD	INAPI_API	GetWiFiStatus(void);
