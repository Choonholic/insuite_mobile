// ConvertersX.cpp

#include "stdafx.h"
#include "ConvertersX.h"

#ifndef MAX_LENGTH_SERIES
#define MAX_LENGTH_SERIES
#define HEX_DWORD_MAX_LENGTH	8
#define DEC_DWORD_MAX_LENGTH	10
#endif

BOOL DecWStringToDWORD(LPCWSTR lpszValue, DWORD& dwValue)
{
	int			nLength		= wcslen(lpszValue);
	int			nValue		= 0;
	ULONGLONG	ullValue	= 0;

	dwValue	= 0;

	if (nLength > DEC_DWORD_MAX_LENGTH)
	{
		return FALSE;
	}

	for (int i = 0; i < nLength; i++)
	{
		if ((lpszValue[i] >= L'0') && (lpszValue[i] <= L'9'))
		{
			nValue	= lpszValue[i] - L'0';
		}
		else
		{
			return FALSE;
		}

		ullValue	= ((ullValue * 10) + nValue);
	}

	dwValue	= static_cast<DWORD>(ullValue & 0xFFFFFFFF);

	return TRUE;
}

BOOL HexWStringToDWORD(LPCWSTR lpszValue, DWORD& dwValue)
{
	int			nLength		= wcslen(lpszValue);
	int			nValue		= 0;
	ULONGLONG	ullValue	= 0;

	dwValue	= 0;

	if (nLength > HEX_DWORD_MAX_LENGTH)
	{
		return FALSE;
	}

	for (int i = 0; i < nLength; i++)
	{
		if ((lpszValue[i] >= L'0') && (lpszValue[i] <= L'9'))
		{
			nValue	= lpszValue[i] - L'0';
		}
		else if ((lpszValue[i] >= L'A') && (lpszValue[i] <= L'F'))
		{
			nValue	= lpszValue[i] - L'A' + 0x0A;
		}
		else if ((lpszValue[i] >= L'a') && (lpszValue[i] <= L'f'))
		{
			nValue	= lpszValue[i] - L'a' + 0x0A;
		}
		else
		{
			return FALSE;
		}

		ullValue	= ((ullValue << 4) + nValue);
	}

	dwValue	= static_cast<DWORD>(ullValue & 0xFFFFFFFF);

	return TRUE;
}

BOOL DecAStringToDWORD(LPCSTR lpszValue, DWORD& dwValue)
{
	int			nLength		= strlen(lpszValue);
	int			nValue		= 0;
	ULONGLONG	ullValue	= 0;

	dwValue	= 0;

	if (nLength > DEC_DWORD_MAX_LENGTH)
	{
		return FALSE;
	}

	for (int i = 0; i < nLength; i++)
	{
		if ((lpszValue[i] >= '0') && (lpszValue[i] <= '9'))
		{
			nValue	= lpszValue[i] - '0';
		}
		else
		{
			return FALSE;
		}

		ullValue	= ((ullValue * 10) + nValue);
	}

	dwValue	= static_cast<DWORD>(ullValue & 0xFFFFFFFF);

	return TRUE;
}

BOOL HexAStringToDWORD(LPCSTR lpszValue, DWORD& dwValue)
{
	int			nLength		= strlen(lpszValue);
	int			nValue		= 0;
	ULONGLONG	ullValue	= 0;

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

		ullValue	= ((ullValue << 4) + nValue);
	}

	dwValue	= static_cast<DWORD>(ullValue & 0xFFFFFFFF);

	return TRUE;
}

BOOL DWORDToDecWString(DWORD dwValue, LPWSTR lpszValue, LPDWORD lpcbValue)
{
	WCHAR	szConvert[16];

	wsprintf(szConvert, L"%ld", dwValue);

	if (static_cast<DWORD>(wcslen(szConvert) + 1) <= (*lpcbValue))
	{
		wcscpy_s(lpszValue, *lpcbValue, szConvert);

		*lpcbValue	= static_cast<DWORD>(wcslen(szConvert) + 1);

		return TRUE;
	}

	return FALSE;
}

BOOL DWORDToHexWString(DWORD dwValue, LPWSTR lpszValue, LPDWORD lpcbValue, BOOL bUpperCase)
{
	WCHAR	szConvert[16];

	wsprintf(szConvert, (bUpperCase ? L"%lX" : L"%lx"), dwValue);

	if (static_cast<DWORD>(wcslen(szConvert) + 1) <= (*lpcbValue))
	{
		wcscpy_s(lpszValue, *lpcbValue, szConvert);

		*lpcbValue	= static_cast<DWORD>(wcslen(szConvert) + 1);

		return TRUE;
	}

	return FALSE;
}

BOOL DWORDToDecAString(DWORD dwValue, LPSTR lpszValue, LPDWORD lpcbValue)
{
	char	szConvert[16];

	sprintf(szConvert, "%ld", dwValue);

	if (static_cast<DWORD>(strlen(szConvert) + 1) <= (*lpcbValue))
	{
		strcpy_s(lpszValue, *lpcbValue, szConvert);

		*lpcbValue	= static_cast<DWORD>(strlen(szConvert) + 1);

		return TRUE;
	}

	return FALSE;
}

BOOL DWORDToHexAString(DWORD dwValue, LPSTR lpszValue, LPDWORD lpcbValue, BOOL bUpperCase)
{
	char	szConvert[16];

	sprintf(szConvert, (bUpperCase ? "%lX" : "%lx"), dwValue);

	if (static_cast<DWORD>(strlen(szConvert) + 1) <= (*lpcbValue))
	{
		strcpy_s(lpszValue, *lpcbValue, szConvert);

		*lpcbValue	= static_cast<DWORD>(strlen(szConvert) + 1);

		return TRUE;
	}

	return FALSE;
}

BYTE PercentageToBYTE(int nPercentage)
{
	return static_cast<BYTE>((nPercentage * 0xFF) / 100);
}

int BYTEToPercentage(BYTE nByte)
{
	return static_cast<int>((nByte * 100) / 0xFF);
}
