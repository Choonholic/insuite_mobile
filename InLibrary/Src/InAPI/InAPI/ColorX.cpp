// ColorX.cpp

#include "stdafx.h"
#include "ColorX.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#define REG_KEY_COLOR		_T("SOFTWARE\\Microsoft\\Color")
#define REG_NAME_DEFSHCOLOR	_T("DefSHColor")
#define REG_NAME_SHCOLOR	_T("SHColor")
#define REG_NAME_BASEHUE	_T("BaseHue")
#define REG_SIZE_SHCOLOR	41

DWORD GetShellBaseHue(void)
{
	DWORD	dwHue	= 0xFFFFFFFF;

	RegistryGetDWORD(HKEY_LOCAL_MACHINE, REG_KEY_COLOR, REG_NAME_BASEHUE, &dwHue);
	return dwHue;
}

DWORD GetUserShellColor(int nIndex)
{
	HKEY		hKey	= 0;
	COLORREF	cr		= 0xFFFFFFFF;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_COLOR, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		TCHAR	szName[32];
		BYTE	buffer[4];
		DWORD	dwType	= REG_BINARY;
		DWORD	dwSize	= sizeof(buffer);

		wsprintf(szName, _T("%d"), nIndex);

		if ((RegQueryValueEx(hKey, szName, 0, &dwType, static_cast<LPBYTE>(buffer), &dwSize) == ERROR_SUCCESS) && (dwType == REG_BINARY) && (dwSize == 4))
		{
			cr	= RGB(buffer[0], buffer[1], buffer[2]);
		}

		RegCloseKey(hKey);

		if (cr == 0xFFFFFFFF)
		{
			cr	= static_cast<COLORREF>(GetShellColor(nIndex));
		}
	}

	return static_cast<DWORD>(cr);
}

DWORD GetShellColor(int nIndex)
{
	HKEY		hKey	= 0;
	COLORREF	cr		= 0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_COLOR, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		COLORREF	crTable[REG_SIZE_SHCOLOR];
		DWORD		dwType	= REG_BINARY;
		DWORD		dwSize	= sizeof(crTable);

		if ((RegQueryValueEx(hKey, REG_NAME_SHCOLOR, 0, &dwType, reinterpret_cast<LPBYTE>(&crTable), &dwSize) == ERROR_SUCCESS) && (dwType == REG_BINARY))
		{
			if (nIndex < static_cast<int>(dwSize / sizeof(COLORREF)))
			{
				cr	= crTable[nIndex];
			}
		}

		RegCloseKey(hKey);
	}

	return static_cast<DWORD>(cr);
}

DWORD GetDefShellColor(int nIndex)
{
	HKEY		hKey	= 0;
	COLORREF	cr		= 0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_COLOR, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		COLORREF	crTable[REG_SIZE_SHCOLOR];
		DWORD		dwType	= REG_BINARY;
		DWORD		dwSize	= sizeof(crTable);

		if ((RegQueryValueEx(hKey, REG_NAME_DEFSHCOLOR, 0, &dwType, reinterpret_cast<LPBYTE>(&crTable), &dwSize) == ERROR_SUCCESS) && (dwType == REG_BINARY))
		{
			if (nIndex < static_cast<int>(dwSize / sizeof(COLORREF)))
			{
				cr	= crTable[nIndex];
			}
		}

		RegCloseKey(hKey);
	}

	return static_cast<DWORD>(cr);
}

BOOL SetUserShellColor(int nIndex, DWORD dwColor)
{
	BOOL		bResult	= FALSE;
	HKEY		hKey	= 0;
	COLORREF	cr		= 0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_COLOR, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		TCHAR	szName[32];

		wsprintf(szName, _T("%d"), nIndex);

		if (RegSetValueEx(hKey, szName, 0, REG_BINARY, reinterpret_cast<LPBYTE>(&dwColor), sizeof(DWORD)) == ERROR_SUCCESS)
		{
			bResult	= TRUE;
		}

		RegCloseKey(hKey);
	}

	return bResult;
}

BOOL SetShellColor(int nIndex, DWORD dwColor)
{
	BOOL		bResult	= FALSE;
	HKEY		hKey	= 0;
	COLORREF	cr		= 0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_COLOR, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		COLORREF	crTable[REG_SIZE_SHCOLOR];
		DWORD		dwType	= REG_BINARY;
		DWORD		dwSize	= sizeof(crTable);

		if ((RegQueryValueEx(hKey, REG_NAME_SHCOLOR, 0, &dwType, reinterpret_cast<LPBYTE>(&crTable), &dwSize) == ERROR_SUCCESS) && (dwType == REG_BINARY))
		{
			if (nIndex < static_cast<int>(dwSize / sizeof(COLORREF)))
			{
				crTable[nIndex]	= static_cast<COLORREF>(dwColor);

				if (RegSetValueEx(hKey, REG_NAME_SHCOLOR, 0, REG_BINARY, reinterpret_cast<LPBYTE>(&crTable), sizeof(crTable)) == ERROR_SUCCESS)
				{
					bResult	= TRUE;
				}
			}
		}

		RegCloseKey(hKey);
	}

	return bResult;
}

#define REG_KEY_GWE			_T("System\\GWE")
#define REG_VAL_DEFSYSCOLOR	_T("DefSysColor")
#define REG_VAL_SYSCOLOR	_T("SysColor")
#define REG_IDX_SYSCOLOR	29

DWORD GetSystemColor(int nIndex)
{
	HKEY		hKey	= 0;
	COLORREF	cr		= 0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_GWE, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		COLORREF	crTable[REG_IDX_SYSCOLOR];
		DWORD		dwType	= REG_BINARY;
		DWORD		dwSize	= sizeof(crTable);

		if ((RegQueryValueEx(hKey, REG_VAL_SYSCOLOR, 0, &dwType, reinterpret_cast<LPBYTE>(&crTable), &dwSize) == ERROR_SUCCESS) && (dwType == REG_BINARY))
		{
			if (nIndex < static_cast<int>(dwSize / sizeof(COLORREF)))
			{
				cr	= crTable[nIndex];
			}
		}

		RegCloseKey(hKey);
	}

	return static_cast<DWORD>(cr);
}

DWORD GetDefSystemColor(int nIndex)
{
	HKEY		hKey	= 0;
	COLORREF	cr		= 0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_GWE, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		COLORREF	crTable[REG_IDX_SYSCOLOR];
		DWORD		dwType	= REG_BINARY;
		DWORD		dwSize	= sizeof(crTable);

		if ((RegQueryValueEx(hKey, REG_VAL_DEFSYSCOLOR, 0, &dwType, reinterpret_cast<LPBYTE>(&crTable), &dwSize) == ERROR_SUCCESS) && (dwType == REG_BINARY))
		{
			if (nIndex < static_cast<int>(dwSize / sizeof(COLORREF)))
			{
				cr	= crTable[nIndex];
			}
		}

		RegCloseKey(hKey);
	}

	return static_cast<DWORD>(cr);
}

BOOL SetSystemColor(int nIndex, DWORD dwColor)
{
	BOOL		bResult	= FALSE;
	HKEY		hKey	= 0;
	COLORREF	cr		= 0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_GWE, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		COLORREF	crTable[REG_IDX_SYSCOLOR];
		DWORD		dwType	= REG_BINARY;
		DWORD		dwSize	= sizeof(crTable);

		if ((RegQueryValueEx(hKey, REG_VAL_SYSCOLOR, 0, &dwType, reinterpret_cast<LPBYTE>(&crTable), &dwSize) == ERROR_SUCCESS) && (dwType == REG_BINARY))
		{
			if (nIndex < static_cast<int>(dwSize / sizeof(COLORREF)))
			{
				crTable[nIndex]	= static_cast<COLORREF>(dwColor);

				if (RegSetValueEx(hKey, REG_VAL_SYSCOLOR, 0, REG_BINARY, reinterpret_cast<LPBYTE>(&crTable), sizeof(crTable)) == ERROR_SUCCESS)
				{
					bResult	= TRUE;
				}
			}
		}

		RegCloseKey(hKey);
	}

	return bResult;
}
