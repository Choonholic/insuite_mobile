// ThemeX.cpp

#include "stdafx.h"
#include "ThemeX.h"
#include "ShellX.h"

BOOL SetWallpaper(LPCTSTR szFilename)
{
	HKEY	hKey;
	TCHAR	szValue[MAX_PATH + 1];

	if (CopyFile(szFilename, _T("\\Windows\\tdycust.gif"), FALSE) == FALSE)
	{
		return FALSE;
	}

	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Today"), 0, 0, &hKey) == ERROR_SUCCESS)
	{
		_tcscpy_s(szValue, MAX_PATH + 1, _T("\\windows\\tdycust.gif"));
		RegSetValueEx(hKey, _T("WallFile"), 0, REG_SZ, reinterpret_cast<LPBYTE>(szValue), (_tcslen(szValue) + 1) * sizeof(TCHAR));
		_tcscpy_s(szValue, MAX_PATH + 1, _T("Wallpaper"));
		RegSetValueEx(hKey, _T("Wall"), 0, REG_SZ, reinterpret_cast<LPBYTE>(szValue), (_tcslen(szValue) + 1) * sizeof(TCHAR));
		_tcscpy_s(szValue, MAX_PATH + 1, _T("tdycust.gif"));
		RegSetValueEx(hKey, _T("Watermark"), 0, REG_SZ, reinterpret_cast<LPBYTE>(szValue), (_tcslen(szValue) + 1) * sizeof(TCHAR));
		RegCloseKey(hKey);
		::SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0xF2, 0);
		return TRUE;
	}

	return FALSE;
}

BOOL GetWallpaper(LPTSTR szFilename, int nLen)
{
	int		nWidth	= GetSystemMetrics(SM_CXSCREEN);
	int		nHeight	= GetSystemMetrics(SM_CYSCREEN);

	_tcscpy_s(szFilename, nLen, _T(""));

	// User Wallpaper
	if (IsFileExist(_T("\\Windows\\tdycust.gif")))
	{
		_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdycust.gif"));
		return TRUE;
	}

	// Size-dependent
	if ((nWidth == 240) && (nHeight == 320))
	{
		// 96 DPI / Portrait Mode
		if (IsFileExist(_T("\\Windows\\tdywater_240_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_240_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_240_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_240_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_240_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_240_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_240_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_240_320.gif"));
			return TRUE;
		}
	}
	else if ((nWidth == 320) && (nHeight == 240))
	{
		// 96 DPI / Landscape Mode
		if (IsFileExist(_T("\\Windows\\tdywater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_320_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_320_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_320_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_320_240.gif"));
			return TRUE;
		}
	}
	else if ((nWidth == 240) && (nHeight == 240))
	{
		// 96 DPI / Square Mode
		if (IsFileExist(_T("\\Windows\\tdywater_240_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_240_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_240_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_240_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_240_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_240_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_240_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_240_240.gif"));
			return TRUE;
		}
	}
	else if ((nWidth == 480) && (nHeight == 640))
	{
		// 192 DPI / Portrait Mode
		if (IsFileExist(_T("\\Windows\\tdywater_480_640.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_480_640.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_480_640.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_480_640.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_480_640.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_480_640.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_480_640.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_480_640.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\tdywater_240_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_240_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_240_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_240_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_240_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_240_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_240_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_240_320.gif"));
			return TRUE;
		}
	}
	else if ((nWidth == 640) && (nHeight == 480))
	{
		// 192 DPI / Landscape Mode
		if (IsFileExist(_T("\\Windows\\tdywater_640_480.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_640_480.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_640_480.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_640_480.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_640_480.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_640_480.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_640_480.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_640_480.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\tdywater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_320_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_320_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_320_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_320_240.gif"));
			return TRUE;
		}
	}
	else if ((nWidth == 480) && (nHeight == 480))
	{
		// 192 DPI / Square Mode
		if (IsFileExist(_T("\\Windows\\tdywater_480_480.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_480_480.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_480_480.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_480_480.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_480_480.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_480_480.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_480_480.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_480_480.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\tdywater_240_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_240_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_320_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_240_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_240_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_240_240.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_240_240.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_240_240.gif"));
			return TRUE;
		}
	}
	else if ((nWidth == 320) && (nHeight == 320))
	{
		// 128 DPI / Square Mode
		if (IsFileExist(_T("\\Windows\\tdywater_320_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater_320_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\stwater_320_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater_320_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_tdywater_320_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater_320_320.gif"));
			return TRUE;
		}

		if (IsFileExist(_T("\\Windows\\Default_stwater_320_320.gif")))
		{
			_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater_320_320.gif"));
			return TRUE;
		}
	}

	// Theme
	if (IsFileExist(_T("\\Windows\\tdywater.gif")))
	{
		_tcscpy_s(szFilename, nLen, _T("\\Windows\\tdywater.gif"));
		return TRUE;
	}

	if (IsFileExist(_T("\\Windows\\stwater.gif")))
	{
		_tcscpy_s(szFilename, nLen, _T("\\Windows\\stwater.gif"));
		return TRUE;
	}

	if (IsFileExist(_T("\\Windows\\Default_tdywater.gif")))
	{
		_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_tdywater.gif"));
		return TRUE;
	}

	if (IsFileExist(_T("\\Windows\\Default_stwater.gif")))
	{
		_tcscpy_s(szFilename, nLen, _T("\\Windows\\Default_stwater.gif"));
		return TRUE;
	}

	return FALSE;
}

BOOL ResetWallpaper(void)
{
	HKEY	hKey;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Today"), 0, 0, &hKey) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, _T("WallFile"), 0, REG_SZ, NULL, 0);
		RegSetValueEx(hKey, _T("Wall"), 0, REG_SZ, NULL, 0);
		RegDeleteValue(hKey, _T("Watermark"));
		RegCloseKey(hKey);
		::SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0xF2, 0);
		return TRUE;
	}

	return FALSE;
}
