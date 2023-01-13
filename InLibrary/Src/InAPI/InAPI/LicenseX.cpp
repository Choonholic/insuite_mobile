// LicenseX.cpp

#include "stdafx.h"
#include "LicenseX.h"
#include "TimeX.h"
#include "DeviceInfoX.h"
#include "SystemX.h"
#include "ConvertersX.h"
#include "StringX.h"

#define KEYCODE	0x1167
#define BUFSIZE	80
#define DEVSIZE	52

void BuildActivateInfoX(LPBYTE lpszInfo, int nSize, DWORD dwVersion, BOOL bDate, SYSTEMTIME stDate, DWORD dwAdd)
{
	char	szBuffer[BUFSIZE + 1];
	char	szDevice[DEVSIZE + 1];
	char	szConv[9];

	// Initializing
	memset(szBuffer, 0, (BUFSIZE + 1) * sizeof(char));
	memset(szDevice, 0, (DEVSIZE + 1) * sizeof(char));
	GetDeviceUniqueIDA(szDevice, DEVSIZE + 1);

	// Mapping Device ID
	memcpy_s(szBuffer +  8, 7, szDevice +  0, 7);
	memcpy_s(szBuffer + 19, 8, szDevice +  7, 8);
	memcpy_s(szBuffer + 31, 7, szDevice + 15, 7);
	memcpy_s(szBuffer + 40, 8, szDevice + 22, 8);
	memcpy_s(szBuffer + 50, 7, szDevice + 30, 7);
	memcpy_s(szBuffer + 61, 8, szDevice + 37, 8);
	memcpy_s(szBuffer + 73, 7, szDevice + 45, 7);

	// Mapping Version
	sprintf(szConv, "%02X", LOBYTE(HIWORD(dwVersion)));
	memcpy_s(szBuffer + 17, 2, szConv, 2);
	sprintf(szConv, "%02X", HIBYTE(HIWORD(dwVersion)));
	memcpy_s(szBuffer + 29, 2, szConv, 2);
	sprintf(szConv, "%02X", HIBYTE(LOWORD(dwVersion)));
	memcpy_s(szBuffer + 59, 2, szConv, 2);
	sprintf(szConv, "%02X", LOBYTE(LOWORD(dwVersion)));
	memcpy_s(szBuffer + 71, 2, szConv, 2);

	if (bDate)
	{
		// Mapping Date
		sprintf(szConv, "%02X", stDate.wMonth);
		memcpy_s(szBuffer + 15, 2, szConv, 2);
		sprintf(szConv, "%02X", stDate.wDayOfWeek);
		memcpy_s(szBuffer + 38, 2, szConv, 2);
		sprintf(szConv, "%02X", stDate.wDay);
		memcpy_s(szBuffer + 69, 2, szConv, 2);
		sprintf(szConv, "%04X", stDate.wYear);
		memcpy_s(szBuffer + 27, 2, szConv + 0, 2);
		memcpy_s(szBuffer + 48, 2, szConv + 2, 2);

		// Store Detector
		memcpy_s(szBuffer + 57, 2, "9F", 2);
	}
	else
	{
		// Mapping Additional Information
		sprintf(szConv, "%02X", HIBYTE(LOWORD(dwAdd)));
		memcpy_s(szBuffer + 15, 2, szConv, 2);
		sprintf(szConv, "%02X", LOBYTE(HIWORD(dwAdd)));
		memcpy_s(szBuffer + 38, 2, szConv, 2);
		sprintf(szConv, "%02X", HIBYTE(HIWORD(dwAdd)));
		memcpy_s(szBuffer + 69, 2, szConv, 2);
		sprintf(szConv, "%02X", LOBYTE(LOWORD(dwAdd)));
		memcpy_s(szBuffer + 27, 2, szConv, 2);
		memcpy_s(szBuffer + 48, 2, "64", 2);

		// Store Detector
		memcpy_s(szBuffer + 57, 2, "F3", 2);
	}

	// Calculate Header
	DWORD	dwHeader	= 0;

	for (int i = 8; i < BUFSIZE; i++)
	{
		dwHeader	+= szBuffer[i];
	}

	dwHeader	*= KEYCODE;

	sprintf(szConv, "%08X", dwHeader);
	memcpy_s(szBuffer, BUFSIZE + 1, szConv, 8);
	strcpy_s(reinterpret_cast<LPSTR>(lpszInfo), nSize, szBuffer);
}

BOOL WriteActivateBinaryX(LPCTSTR lpszFileName, DWORD dwVersion, SYSTEMTIME stDate, BOOL bIgnore)
{
	char	szBuffer[BUFSIZE + 1];

	// Check Version
	if ((bIgnore == FALSE) && (dwVersion != CheckActivateVersionX(lpszFileName)))
	{
		bIgnore	= TRUE;
	}

	// Initializing
	memset(szBuffer, 0, (BUFSIZE + 1) * sizeof(char));

	// Build Infomation
	BuildActivateInfoX(reinterpret_cast<LPBYTE>(szBuffer), BUFSIZE + 1, dwVersion, TRUE, stDate, 0);

	// Write Binary
	HANDLE	hFile	= CreateFile(lpszFileName, GENERIC_WRITE, 0, 0, (bIgnore ? CREATE_ALWAYS : CREATE_NEW), FILE_ATTRIBUTE_HIDDEN, NULL);
	DWORD	dwSize	= 0;

	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, szBuffer, BUFSIZE * sizeof(char), &dwSize, NULL);
		CloseHandle(hFile);
		return TRUE;
	}

	return FALSE;
}

BOOL WriteActivateBinaryX(LPCTSTR lpszFileName, DWORD dwVersion, DWORD dwAdd)
{
	char		szBuffer[BUFSIZE + 1];
	SYSTEMTIME	stDate;

	// Initializing
	memset(szBuffer, 0, (BUFSIZE + 1) * sizeof(char));
	memset(&stDate, 0, sizeof(SYSTEMTIME));

	// Build Infomation
	BuildActivateInfoX(reinterpret_cast<LPBYTE>(szBuffer), BUFSIZE + 1, dwVersion, FALSE, stDate, dwAdd);

	// Write Binary
	HANDLE	hFile	= CreateFile(lpszFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
	DWORD	dwSize	= 0;

	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, szBuffer, BUFSIZE * sizeof(char), &dwSize, NULL);
		CloseHandle(hFile);
		return TRUE;
	}

	return FALSE;
}

int ReadActivateStatusX(LPCTSTR lpszFileName, DWORD dwVersion, ULONGLONG& ullPeriod)
{
	// Read Binary
	HANDLE	hFile;
	DWORD	dwSize;
	char	szBuffer[BUFSIZE + 1];
	char	szConv[9];

	hFile	= CreateFile(lpszFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return ILM_INVALID;
	}

	memset(szBuffer, 0, (BUFSIZE + 1) * sizeof(char));
	ReadFile(hFile, szBuffer, BUFSIZE * sizeof(char), &dwSize, NULL);
	CloseHandle(hFile);

	if (dwSize != (BUFSIZE * sizeof(char)))
	{
		return ILM_INVALID;
	}

	// Check Header
	DWORD	dwHeader	= 0;
	DWORD	dwValue		= 0;

	memcpy_s(szConv, 9, szBuffer, 8);

	szConv[8]	= NULL;

	HexAStringToDWORD(szConv, dwHeader);

	for (int i = 8; i < BUFSIZE; i++)
	{
		dwValue	+= szBuffer[i];
	}

	dwValue	*= KEYCODE;

	if (dwValue != dwHeader)
	{
		return ILM_INVALID;
	}

	// Build Version
	DWORD	dwCurrentVersion	= 0;

	memcpy_s(szConv + 0, 2, szBuffer + 29, 2);
	memcpy_s(szConv + 2, 2, szBuffer + 17, 2);
	memcpy_s(szConv + 4, 2, szBuffer + 59, 2);
	memcpy_s(szConv + 6, 2, szBuffer + 71, 2);

	szConv[8]	= NULL;

	HexAStringToDWORD(szConv, dwCurrentVersion);

	if (dwVersion != dwCurrentVersion)
	{
		return ILM_INVALID;
	}

	// Build Device ID
	char	szDevice[DEVSIZE + 1];
	char	szDevID[DEVSIZE + 1];

	memcpy_s(szDevice +  0, 7, szBuffer +  8, 7);
	memcpy_s(szDevice +  7, 8, szBuffer + 19, 8);
	memcpy_s(szDevice + 15, 7, szBuffer + 31, 7);
	memcpy_s(szDevice + 22, 8, szBuffer + 40, 8);
	memcpy_s(szDevice + 30, 7, szBuffer + 50, 7);
	memcpy_s(szDevice + 37, 8, szBuffer + 61, 8);
	memcpy_s(szDevice + 45, 7, szBuffer + 73, 7);

	szDevice[DEVSIZE]	= NULL;

	if (GetDeviceUniqueIDA(szDevID, DEVSIZE + 1))
	{
		if (memcmp(szDevice, szDevID, DEVSIZE * sizeof(char)))
		{
			return ILM_INVALID;
		}
	}

	if (!strncmp(szBuffer + 57, "F3", 2))
	{
		// Activated
		return ILM_ACTIVATED;
	}

	if (strncmp(szBuffer + 57, "9F", 2))
	{
		return ILM_INVALID;
	}

	// Build Date
	SYSTEMTIME	stStart;
	SYSTEMTIME	stToday;

	memset(&stStart, 0, sizeof(SYSTEMTIME));
	memset(szConv, 0, 9 * sizeof(char));

	memcpy_s(szConv, 9, szBuffer + 15, 2);
	HexAStringToDWORD(szConv, dwValue);

	stStart.wMonth	= static_cast<WORD>(dwValue);

	memcpy_s(szConv, 9, szBuffer + 69, 2);
	HexAStringToDWORD(szConv, dwValue);

	stStart.wDay	= static_cast<WORD>(dwValue);

	memcpy_s(szConv, 9, szBuffer + 38, 2);
	HexAStringToDWORD(szConv, dwValue);

	stStart.wDayOfWeek	= static_cast<WORD>(dwValue);

	memcpy_s(szConv + 0, 2, szBuffer + 27, 2);
	memcpy_s(szConv + 2, 2, szBuffer + 48, 2);
	HexAStringToDWORD(szConv, dwValue);

	stStart.wYear	= static_cast<WORD>(dwValue);

	AddSystemTime(&stStart, ullPeriod);
	GetLocalTime(&stToday);

	if (CompareSystemTime(&stToday, &stStart) < 0)
	{
		GetSystemTimeSpan(&ullPeriod, &stStart, &stToday);
		return ILM_TRIAL;
	}

	return ILM_EXPIRED;
}

DWORD CheckActivateVersionX(LPCTSTR lpszFileName)
{
	// Read Binary
	HANDLE	hFile;
	DWORD	dwSize;
	char	szBuffer[BUFSIZE + 1];

	hFile	= CreateFile(lpszFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	memset(szBuffer, 0, (BUFSIZE + 1) * sizeof(char));
	ReadFile(hFile, szBuffer, BUFSIZE * sizeof(char), &dwSize, NULL);
	CloseHandle(hFile);

	if (dwSize != (BUFSIZE * sizeof(char)))
	{
		return 0;
	}

	// Check Header
	DWORD	dwHeader	= 0;
	DWORD	dwValue		= 0;
	char	szConv[9];

	memcpy_s(szConv, 9, szBuffer, 8);

	szConv[8]	= NULL;

	HexAStringToDWORD(szConv, dwHeader);

	for (int i = 8; i < BUFSIZE; i++)
	{
		dwValue	+= szBuffer[i];
	}

	dwValue	*= KEYCODE;

	if (dwValue != dwHeader)
	{
		return 0;
	}

	// Build Version
	DWORD	dwVersion	= 0;

	memcpy_s(szConv + 0, 2, szBuffer + 29, 2);
	memcpy_s(szConv + 2, 2, szBuffer + 17, 2);
	memcpy_s(szConv + 4, 2, szBuffer + 59, 2);
	memcpy_s(szConv + 6, 2, szBuffer + 71, 2);

	szConv[8]	= NULL;

	HexAStringToDWORD(szConv, dwVersion);
	return dwVersion;
}

BOOL IsValidMailX(LPCTSTR lpszMail)
{
	int	nLength	= _tcslen(lpszMail);

	if (nLength < 5)
	{
		return FALSE;
	}

	if ((FindX(lpszMail, _T('@')) < 1) || (FindX(lpszMail, _T('@')) > (nLength - 4)))
	{
		return FALSE;
	}

	if (FindX(lpszMail, _T('.')) > (nLength - 2))
	{
		return FALSE;
	}

	return TRUE;
}

#define LEFTKEY		0x0116
#define RIGHTKEY	0x0607
#define ENDKEY		0x6926

DWORD CalculateSerialNumberX(LPCTSTR lpszMail, WORD wProductID, WORD wVersion)
{
	DWORD	dwKey;
	TCHAR	szLeft[256];
	TCHAR	szRight[256];
	DWORD	dwLeft;
	DWORD	dwRight;
	int		nLeftLen;
	int		nRightLen;
	int		i;
	int		nMul;
	TCHAR	ch;
	DWORD	dwIndex;
	DWORD	dwSerial;

	LeftX(szLeft, 256, lpszMail, FindX(lpszMail, _T('@')));
	MidX(szRight, 256, lpszMail, FindX(lpszMail, _T('@')) + 1);

	dwKey		= static_cast<DWORD>(wProductID + wVersion);
	dwLeft		= 0;
	dwRight		= 0;
	nLeftLen	= _tcslen(szLeft);
	nRightLen	= _tcslen(szRight);

	for (i = 1, nMul = nLeftLen; i < nLeftLen; i += 2, nMul--)
	{
		ch		= szLeft[i];
		dwIndex	= ch * nMul;
		dwLeft	+= dwIndex;
	}

	for (i = 0, nMul = 1; i < nLeftLen; i += 2, nMul++)
	{
		ch		= szLeft[i];
		dwIndex	= ch * nMul;
		dwLeft	-= dwIndex;
	}

	dwLeft	+= LEFTKEY;
	dwLeft	*= dwKey;

	for (i = 0, nMul = nRightLen; i < nRightLen; i += 2, nMul--)
	{
		ch		= szRight[i];
		dwIndex	= ch * nMul;
		dwRight	+= dwIndex;
	}

	for (i = 1, nMul = 1; i < nRightLen; i += 2, nMul++)
	{
		ch		= szRight[i];
		dwIndex	= ch * nMul;
		dwRight	-= dwIndex;
	}

	dwRight	+= RIGHTKEY;
	dwRight	*= dwKey;

	dwSerial	= dwLeft + dwRight;
	dwLeft		= ((((dwSerial >> 24) & 0xFF) << 8) + (dwSerial & 0xFF));
	dwRight		= (((dwSerial >> 16) & 0xFF) + (((dwSerial >> 8) & 0xFF) << 8));
	dwLeft		= ((dwLeft * ENDKEY) & 0x0000FFFF);
	dwRight		= ((dwRight * ENDKEY) & 0x0000FFFF);
	dwSerial	= MAKELONG(dwRight, dwLeft);

	return dwSerial;
}

DWORD MakeVersionNumberX(WORD wProductID, WORD wVersion)
{
	return static_cast<DWORD>(MAKELONG(wVersion, wProductID));
}

#define MAX_PARTLEN	128

BOOL CheckBundleX(LPCTSTR lpszPart, int nShift)
{
	TCHAR	szPart[MAX_PARTLEN + 1];
	TCHAR	szMachine[MAX_PARTLEN + 1];
	int		i;

	if (!GetMachineNameX(szMachine, MAX_PARTLEN + 1))
	{
		return FALSE;
	}

	if (_tcslen(lpszPart) > MAX_PARTLEN)
	{
		return FALSE;
	}

	memset(szPart, 0, sizeof(TCHAR) * (MAX_PARTLEN + 1));

	for (i = 0; i < (int)_tcslen(lpszPart); i++)
	{
		szPart[i]	= lpszPart[i] - nShift;
	}

	return (static_cast<BOOL>(FindX(szMachine, szPart) != -1));
}
