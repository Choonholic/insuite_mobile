// DeviceInfoX.h

#pragma once

#include "InAPI.h"

BOOL	INAPI_API	GetMachineNameX(LPTSTR lpszName, int nSize);
BOOL	INAPI_API	GetPlatformTypeX(LPTSTR lpszType, int nSize);
BOOL	INAPI_API	GetVersionX(LPDWORD lpdwMajor, LPDWORD lpdwMinor, LPDWORD lpdwBuild = NULL, LPTSTR lpszCSD = NULL, int nCSDSize = 0);

BOOL	INAPI_API	GetDeviceIDX(LPTSTR lpszID, int nSize);
BOOL	INAPI_API	GetDescriptionX(LPTSTR lpszDesc, int nSize);

BOOL	INAPI_API	SetDeviceIDX(LPCTSTR lpszID);
BOOL	INAPI_API	SetDescriptionX(LPCTSTR lpszDesc);
