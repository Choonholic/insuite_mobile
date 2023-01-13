// Utilities.cpp

#include "stdafx.h"
#include "Utilities.h"
#include <winioctl.h>

extern "C"	__declspec(dllimport)	BOOL	KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);

#ifndef IOCTL_HAL_GET_DEVICEID
#define IOCTL_HAL_GET_DEVICEID	CTL_CODE(FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif	// IOCTL_HAL_GET_DEVICEID

#pragma comment(lib, "ceshell.lib")

INT GetSpecialDirectoryEx(INT nFolderID, LPTSTR lpszDir, int nSize)
{
	int				rc;
	LPITEMIDLIST	pidl;
	BOOL			fUseIMalloc	= TRUE;
	LPMALLOC		lpMalloc	= NULL;

	if (nSize < MAX_PATH)
	{
		return E_FAIL;
	}

	rc	= SHGetMalloc(&lpMalloc);

	if (rc == E_NOTIMPL)
	{
		fUseIMalloc	= FALSE;
	}
	else if (rc != NOERROR)
	{
		return rc;
	}

	rc	= SHGetSpecialFolderLocation(NULL, nFolderID, &pidl);

	if (rc == NOERROR)
	{
		if (SHGetPathFromIDList(pidl, lpszDir))
		{
			rc	= E_FAIL;
		}

		if (fUseIMalloc)
		{
			lpMalloc->Free(pidl);
		}
	}

	if (fUseIMalloc)
	{
		lpMalloc->Release();
	}

	return rc;
}

int CombineFileName(LPTSTR lpszFullPath, int nSize, LPCTSTR lpszFolder, LPCTSTR lpszFileName)
{
	if (!_tcslen(lpszFolder))
	{
		_tcscpy_s(lpszFullPath, nSize, _T("\\"));
	}
	else if (lpszFolder[0] != _T('\\'))
	{
		_tcscpy_s(lpszFullPath, nSize, _T("\\"));
		_tcscat_s(lpszFullPath, nSize, lpszFolder);
	}
	else
	{
		_tcscpy_s(lpszFullPath, nSize, lpszFolder);
	}

	if (lpszFullPath[_tcslen(lpszFullPath) - 1] != _T('\\'))
	{
		_tcscat_s(lpszFullPath, nSize, _T("\\"));
	}

	_tcscat_s(lpszFullPath, nSize, lpszFileName);
	return (_tcslen(lpszFullPath));
}

int ExtendSubPath(LPTSTR lpszFullPath, int nSize, LPCTSTR lpszBasePath, LPCTSTR lpszSubPath, BOOL bAddSeparator)
{
	int	nLength;

	if (!_tcslen(lpszBasePath))
	{
		_tcscpy_s(lpszFullPath, nSize, _T("\\"));
	}
	else if (lpszBasePath[0] != _T('\\'))
	{
		_tcscpy_s(lpszFullPath, nSize, _T("\\"));
		_tcscat_s(lpszFullPath, nSize, lpszBasePath);
	}
	else
	{
		_tcscpy_s(lpszFullPath, nSize, lpszBasePath);
	}

	nLength	= _tcslen(lpszFullPath);

	if (lpszFullPath[nLength - 1] == _T('\\'))
	{
		lpszFullPath[nLength - 1] = NULL;
	}

	if (lpszSubPath[0] != _T('\\'))
	{
		_tcscat_s(lpszFullPath, nSize, _T("\\"));
	}

	_tcscat_s(lpszFullPath, nSize, lpszSubPath);

	nLength	= _tcslen(lpszFullPath);

	if (lpszFullPath[nLength - 1] == _T('\\'))
	{
		lpszFullPath[nLength - 1] = NULL;
	}

	if (bAddSeparator)
	{
		_tcscat_s(lpszFullPath, nSize, _T("\\"));
	}

	return (_tcslen(lpszFullPath));
}

#define HEX_DWORD_MAX_LENGTH	8

BOOL HexAStringToDWORD(LPCSTR lpszValue, DWORD& dwValue)
{
	int	nLength	= strlen(lpszValue);
	int	nValue;

	dwValue	= 0;

	if (nLength > HEX_DWORD_MAX_LENGTH)
	{
		return FALSE;
	}

	for (int i = 0; i < nLength; i++)
	{
		if ((lpszValue[i] >= '0') && (lpszValue[i] <= '9'))
		{
			nValue	= lpszValue[i] - '0';
		}
		else if ((lpszValue[i] >= 'A') && (lpszValue[i] <= 'F'))
		{
			nValue	= lpszValue[i] - 'A' + 0x0A;
		}
		else if ((lpszValue[i] >= 'a') && (lpszValue[i] <= 'f'))
		{
			nValue	= lpszValue[i] - 'a' + 0x0A;
		}
		else
		{
			return FALSE;
		}

		dwValue	= static_cast<DWORD>((dwValue << 4) + static_cast<DWORD>(nValue));
	}

	return TRUE;
}

#define DEVID_LENGTH	26
#define BUFID_LENGTH	(DEVID_LENGTH * 2)

BOOL GetDeviceUniqueIDA(LPSTR lpBuffer, int nSize)
{
	const int	nBuffSize	= 4096;
	BYTE		arrOutBuff[nBuffSize];
	DWORD		dwOutBytes;

	if (nSize < (BUFID_LENGTH + 1))
	{
		return FALSE;
	}

	memset(lpBuffer, 0, (sizeof(BYTE) * nSize));

	if (KernelIoControl(IOCTL_HAL_GET_DEVICEID, 0, 0, arrOutBuff, nBuffSize, &dwOutBytes))
	{
		char	cData[3];
		int		nStart	= 0;

		if (dwOutBytes < DEVID_LENGTH)
		{
			return FALSE;
		}

		if (dwOutBytes > DEVID_LENGTH)
		{
			nStart	= (dwOutBytes - DEVID_LENGTH) / 2;
		}

		for (int i = nStart; i < (nStart + DEVID_LENGTH); i++)
		{
			sprintf(cData, "%02X", arrOutBuff[i]);

			lpBuffer[(i - nStart) * 2 + 0]	= cData[0];
			lpBuffer[(i - nStart) * 2 + 1]	= cData[1];
		}

		return TRUE;
	}

	return FALSE;
}

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

DWORD MakeVersionNumberX(WORD wProductID, WORD wVersion)
{
	return static_cast<DWORD>(MAKELONG(wVersion, wProductID));
}

int FindX(LPCTSTR lpszString, LPCTSTR lpszSub)
{
	ASSERT(lpszString != NULL);

	int	nLength	= _tcslen(lpszString);

	LPTSTR	lpszFound	= _tcsstr(lpszString, lpszSub);
	int		nPos;

	if (lpszFound == NULL)
	{
		nPos	= -1;
	}
	else
	{
		nPos	= lpszFound - lpszString;
	}

	return nPos;
}

BOOL GetMachineNameX(LPTSTR lpszName, int nSize)
{
	return (SystemParametersInfo(SPI_GETOEMINFO, sizeof(TCHAR) * nSize, lpszName, 0));
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
