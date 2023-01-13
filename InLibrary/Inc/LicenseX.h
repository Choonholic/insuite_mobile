// LicenseX.h

#pragma once

#include "InAPI.h"

#ifndef ILM_UNKNOWN
#define ILM_UNKNOWN		0x00
#define ILM_INVALID		0x01
#define ILM_ACTIVATED	0x02
#define ILM_TRIAL		0x04
#define ILM_EXPIRED		0x08
#define ILM_RESERVED	0x10
#endif

BOOL	INAPI_API	WriteActivateBinaryX(LPCTSTR lpszFileName, DWORD dwVersion, SYSTEMTIME stDate, BOOL bIgnore = FALSE);
BOOL	INAPI_API	WriteActivateBinaryX(LPCTSTR lpszFileName, DWORD dwVersion, DWORD dwAdd);

int		INAPI_API	ReadActivateStatusX(LPCTSTR lpszFileName, DWORD dwVersion, ULONGLONG& ullPeriod);
DWORD	INAPI_API	CheckActivateVersionX(LPCTSTR lpszFileName);

BOOL	INAPI_API	IsValidMailX(LPCTSTR lpszMail);

DWORD	INAPI_API	CalculateSerialNumberX(LPCTSTR lpszMail, WORD wProductID, WORD wVersion);
DWORD	INAPI_API	MakeVersionNumberX(WORD wProductID, WORD wVersion);

BOOL	INAPI_API	CheckBundleX(LPCTSTR lpszPart, int nShift);
