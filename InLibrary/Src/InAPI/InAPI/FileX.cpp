// FileX.cpp

#include "stdafx.h"
#include "FileX.h"

DWORD CheckUnicodeBOM(HANDLE hFile)
{
	WORD	wBOM	= 0;
	DWORD	dwRead	= 0;
	DWORD	dwCurrent;

	dwCurrent	= SetFilePointer(hFile, 0, NULL, FILE_CURRENT);

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, &wBOM, sizeof(WORD), &dwRead, NULL);

	if (dwRead != sizeof(WORD))
	{
		return BOM_FILEERROR;
	}

	if ((wBOM == 0xFFFE) || (wBOM == 0xFEFF))
	{
		return static_cast<DWORD>(wBOM);
	}

	SetFilePointer(hFile, dwCurrent, NULL, FILE_BEGIN);
	return BOM_NOUNICODE;
}

#define MAX_LENGTH	4096

BOOL ReadStringXW(HANDLE hFile, LPTSTR lpszString, int nSize, BOOL bIncludeCRLF)
{
	TCHAR	szBuffer[2];
	DWORD	dwSize	= 0;
	int		nRead	= 0;
	int		nMaxLen	= min(MAX_LENGTH, nSize) - 1;

	memset(lpszString, 0, nSize * sizeof(TCHAR));

	while (nRead < nMaxLen)
	{
		dwSize	= 0;

		ReadFile(hFile, szBuffer, sizeof(TCHAR), &dwSize, NULL);

		if (dwSize == sizeof(TCHAR))
		{
			if ((szBuffer[0] == _T('\r')) || (szBuffer[0] == _T('\n')))
			{
				if (szBuffer[0] == _T('\r'))
				{
					if (bIncludeCRLF)
					{
						lpszString[nRead++] = szBuffer[0];
					}

					ReadFile(hFile, szBuffer, sizeof(TCHAR), &dwSize, NULL);
				}

				if (bIncludeCRLF)
				{
					lpszString[nRead++] = szBuffer[0];
				}

				break;
			}

			lpszString[nRead++] = szBuffer[0];
		}
		else
		{
			break;
		}
	}

	return static_cast<BOOL>(_tcslen(lpszString) > 0);
}

BOOL ReadStringXA(HANDLE hFile, LPTSTR lpszString, int nSize, BOOL bIncludeCRLF)
{
	TCHAR	szWide[MAX_LENGTH];
	BYTE	szMBCS[MAX_LENGTH];
	BYTE	szBuffer[2];
	DWORD	dwSize	= 0;
	int		nRead	= 0;
	int		nMaxLen	= min(MAX_LENGTH, nSize) - 1;

	memset(szWide, 0, sizeof(TCHAR) * MAX_LENGTH);
	memset(szMBCS, 0, sizeof(BYTE) * MAX_LENGTH);

	while (nRead < nMaxLen)
	{
		dwSize	= 0;

		ReadFile(hFile, szBuffer, sizeof(BYTE), &dwSize, NULL);

		if (dwSize == sizeof(BYTE))
		{
			if ((szBuffer[0] == '\r') || (szBuffer[0] == '\n'))
			{
				if (szBuffer[0] == '\r')
				{
					if (bIncludeCRLF)
					{
						szMBCS[nRead++] = static_cast<BYTE>(szBuffer[0]);
					}

					ReadFile(hFile, szBuffer, sizeof(BYTE), &dwSize, NULL);
				}

				if (bIncludeCRLF)
				{
					szMBCS[nRead++] = (BYTE)szBuffer[0];
				}

				break;
			}

			szMBCS[nRead++] = (BYTE)szBuffer[0];
		}
		else
		{
			break;
		}
	}

#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, reinterpret_cast<LPCSTR>(szMBCS), -1, static_cast<LPWSTR>(szWide), MAX_LENGTH);
	_tcscpy_s(lpszString, nSize, szWide);
#else
	_tcscpy_s(lpszString, nSize, szMBCS);
#endif

	return static_cast<BOOL>(_tcslen(lpszString) > 0);
}
