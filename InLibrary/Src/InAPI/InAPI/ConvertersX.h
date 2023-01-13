// ConvertersX.h

#pragma once

#include "InAPI.h"

#ifdef _UNICODE
#define DecStringToDWORD	DecWStringToDWORD
#define HexStringToDWORD	HexWStringToDWORD
#define DWORDToDecString	DWORDToDecWString
#define DWORDToHexString	DWORDToHexWString
#else
#define DecStringToDWORD	DecAStringToDWORD
#define HexStringToDWORD	HexAStringToDWORD
#define DWORDToDecString	DWORDToDecAString
#define DWORDToHexString	DWORDToHexAString
#endif

// String To DWord
BOOL	INAPI_API	DecWStringToDWORD(LPCWSTR lpszValue, DWORD& dwValue);
BOOL	INAPI_API	HexWStringToDWORD(LPCWSTR lpszValue, DWORD& dwValue);
BOOL	INAPI_API	DecAStringToDWORD(LPCSTR lpszValue, DWORD& dwValue);
BOOL	INAPI_API	HexAStringToDWORD(LPCSTR lpszValue, DWORD& dwValue);

// DWord To String
BOOL	INAPI_API	DWORDToDecWString(DWORD dwValue, LPWSTR lpszValue, LPDWORD lpcbValue);
BOOL	INAPI_API	DWORDToHexWString(DWORD dwValue, LPWSTR lpszValue, LPDWORD lpcbValue, BOOL bUpperCase = TRUE);
BOOL	INAPI_API	DWORDToDecAString(DWORD dwValue, LPSTR lpszValue, LPDWORD lpcbValue);
BOOL	INAPI_API	DWORDToHexAString(DWORD dwValue, LPSTR lpszValue, LPDWORD lpcbValue, BOOL bUpperCase = TRUE);

// Percentage To Data
BYTE	INAPI_API	PercentageToBYTE(int nPercentage);
int		INAPI_API	BYTEToPercentage(BYTE nByte);
