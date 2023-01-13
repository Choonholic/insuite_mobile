// SystemX.h

#pragma once

#include "InAPI.h"

// Reset Flags
#if (_WIN32_WCE >= 0x0502)
#ifndef EWX_DEFER
#define EWX_DEFER	0x0100	// Reboot will be deferred until it is safe
#endif
#ifndef EWX_PROMPT
#define EWX_PROMPT	0x0200	// User will be prompted before a reboot
#endif
#else
#ifndef EWX_DEFER
#define EWX_DEFER	0x0000	// Not Available
#endif
#ifndef EWX_PROMPT
#define EWX_PROMPT	0x0000	// Not Available
#endif
#endif

// Power Manipulations
BOOL	INAPI_API	Reset(UINT uExFlags = 0);
BOOL	INAPI_API	FullReset(void);
BOOL	INAPI_API	Suspend(void);
BOOL	INAPI_API	Shutdown(void);

// Device Lock
BOOL	INAPI_API	DeviceLock(void);

// Unique Device ID
#ifdef _UNICODE
#define GetDeviceUniqueID	GetDeviceUniqueIDW
#else
#define GetDeviceUniqueID	GetDeviceUniqueIDA
#endif

// Device ID
BOOL	INAPI_API	GetDeviceUniqueIDW(LPWSTR lpBuffer, int nSize);
BOOL	INAPI_API	GetDeviceUniqueIDA(LPSTR lpBuffer, int nSize);
