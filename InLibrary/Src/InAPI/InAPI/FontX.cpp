// FontX.cpp

#include "stdafx.h"
#include "FontX.h"
#include "ShellX.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#ifndef HKLM_FONTPATH
#define HKLM_FONTPATH	_T("SOFTWARE\\Microsoft\\FontPath")
#define HKLM_MICROSOFT	_T("SOFTWARE\\Microsoft")
#endif

#define DISABLE_MODIFY_SYSTEM_FONT

BOOL GetFontPath(LPTSTR lpszPath, int nSize)
{
	TCHAR	szValue[MAX_PATH];
	DWORD	dwSize;

	memset(szValue, 0, sizeof(TCHAR) * MAX_PATH);
	RegistryGetString(HKEY_LOCAL_MACHINE, HKLM_FONTPATH, _T("FontPath"), szValue, MAX_PATH);

	if (_tcslen(szValue) < 1)
	{
		GetSpecialDirectoryEx(CSIDL_WINDOWS, szValue, MAX_PATH);
	}

	dwSize	= _tcslen(szValue);

	if (szValue[dwSize - 1] == _T('\\'))
	{
		szValue[dwSize - 1]	= NULL;
	}

	_tcscpy_s(lpszPath, nSize, szValue);
	return TRUE;
}

BOOL SetFontPath(LPCTSTR lpszPath)
{
	TCHAR		szPath[MAX_PATH + 1];
	int			nLength;

	if (lpszPath == NULL)
	{
		return (RegDeleteKey(HKEY_LOCAL_MACHINE, HKLM_FONTPATH) == ERROR_SUCCESS);
	}

	_tcscpy_s(szPath, MAX_PATH + 1, lpszPath);

	nLength	= _tcslen(szPath);

	if (szPath[nLength - 1] == _T('\\'))
	{
		szPath[nLength - 1]	= NULL;
	}

	return (RegistrySetString(HKEY_LOCAL_MACHINE, HKLM_FONTPATH, _T("FontPath"), szPath) == ERROR_SUCCESS);
}

HFONT CreateFixedFont(LPCTSTR lpszFontFace, int nDoublePoint, LONG lWeight)
{
	LOGFONT		lf;
	HFONT		hFont;
	HFONT		hFont96DPI;
	HFONT		hOldFont;
	TEXTMETRIC	tm;
	HFONT		hSysFont = reinterpret_cast<HFONT>(GetStockObject(SYSTEM_FONT));

	GetObject(hSysFont, sizeof(LOGFONT), &lf);

	if (lpszFontFace != NULL)
	{
		_tcscpy_s(lf.lfFaceName, LF_FACESIZE, lpszFontFace);
	}

	lf.lfWeight	= lWeight;

	if (nDoublePoint > 0)
	{
		lf.lfHeight	= -1 * (nDoublePoint * 96 / 72) / 2;
	}
	else if (nDoublePoint < 0)
	{
		lf.lfHeight	= nDoublePoint;
	}

	HDC	hDC	= GetDC(NULL);

	hFont96DPI	= CreateFontIndirect(&lf);
	hOldFont	= reinterpret_cast<HFONT>(SelectObject(hDC, hFont96DPI));

	GetTextMetrics(hDC, &tm);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont96DPI);
	ReleaseDC(NULL, hDC);

	lf.lfHeight	= DRA::SCALEY(tm.tmHeight);
	hFont		= CreateFontIndirect(&lf);

	return hFont;
}

HFONT CreateUserFont(LPCTSTR lpszFontFace, LONG lWeight)
{
	LOGFONT	lf;
	HFONT	hFont;
	HFONT	hSysFont	= reinterpret_cast<HFONT>(GetStockObject(SYSTEM_FONT));
	int		iFontPoint;

	GetObject(hSysFont, sizeof(LOGFONT), &lf);

	if (lpszFontFace != NULL)
	{
		_tcscpy_s(lf.lfFaceName, LF_FACESIZE, lpszFontFace);
	}

	lf.lfWeight	= lWeight;

	SHGetUIMetrics(SHUIM_FONTSIZE_POINT, &iFontPoint, sizeof(iFontPoint), NULL);

	lf.lfHeight	= PtsToHt(iFontPoint);
	hFont		= CreateFontIndirect(&lf);

	return hFont;
}

HFONT CreateUserFont(LPLOGFONT lpLogFont)
{
	LOGFONT	lf;
	HFONT	hFont;
	int		iFontPoint;

	memcpy_s(&lf, sizeof(LOGFONT), lpLogFont, sizeof(LOGFONT));
	SHGetUIMetrics(SHUIM_FONTSIZE_POINT, &iFontPoint, sizeof(iFontPoint), NULL);

	lf.lfHeight	= PtsToHt(iFontPoint);
	hFont		= CreateFontIndirect(&lf);

	return hFont;
}

void ReleaseFont(HFONT hFont)
{
	if (hFont != NULL)
	{
		DeleteObject(hFont);

		hFont	= NULL;
	}
}

void GetFixedLogFont(LPLOGFONT lpLogFont, LPCTSTR lpszFontFace, int nDoublePoint, LONG lWeight)
{
	ASSERT(lpLogFont != NULL);

	HFONT		hFont96DPI;
	HFONT		hOldFont;
	TEXTMETRIC	tm;
	HFONT		hSysFont = reinterpret_cast<HFONT>(GetStockObject(SYSTEM_FONT));

	GetObject(hSysFont, sizeof(LOGFONT), lpLogFont);

	if (lpszFontFace != NULL)
	{
		_tcscpy_s(lpLogFont->lfFaceName, LF_FACESIZE, lpszFontFace);
	}

	lpLogFont->lfWeight	= lWeight;

	if (nDoublePoint > 0)
	{
		lpLogFont->lfHeight	= -1 * (nDoublePoint * 96 / 72) / 2;
	}
	else if (nDoublePoint < 0)
	{
		lpLogFont->lfHeight	= nDoublePoint;
	}

	HDC	hDC	= GetDC(NULL);

	hFont96DPI	= CreateFontIndirect(lpLogFont);
	hOldFont	= reinterpret_cast<HFONT>(SelectObject(hDC, hFont96DPI));

	GetTextMetrics(hDC, &tm);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont96DPI);
	ReleaseDC(NULL, hDC);

	lpLogFont->lfHeight	= DRA::SCALEY(tm.tmHeight);
}

void GetUserLogFont(LPLOGFONT lpLogFont, LPCTSTR lpszFontFace, LONG lWeight)
{
	ASSERT(lpLogFont != NULL);

	HFONT	hSysFont	= reinterpret_cast<HFONT>(GetStockObject(SYSTEM_FONT));
	int		iFontPoint;

	GetObject(hSysFont, sizeof(LOGFONT), lpLogFont);

	if (lpszFontFace != NULL)
	{
		_tcscpy_s(lpLogFont->lfFaceName, LF_FACESIZE, lpszFontFace);
	}

	SHGetUIMetrics(SHUIM_FONTSIZE_POINT, &iFontPoint, sizeof(iFontPoint), NULL);

	lpLogFont->lfWeight	= lWeight;
	lpLogFont->lfHeight	= PtsToHt(iFontPoint);
}

void GetUserLogFont(LPLOGFONT lpUserLogFont, LPLOGFONT lpLogFont)
{
	ASSERT(lpUserLogFont != NULL);
	ASSERT(lpLogFont != NULL);

	int	iFontPoint;

	memcpy_s(lpUserLogFont, sizeof(LOGFONT), lpLogFont, sizeof(LOGFONT));
	SHGetUIMetrics(SHUIM_FONTSIZE_POINT, &iFontPoint, sizeof(iFontPoint), NULL);

	lpUserLogFont->lfHeight	= PtsToHt(iFontPoint);
}

#define KEY_SYSFONT		_T("SYSTEM\\GDI\\SYSFNT")
#define KEY_POPFONT		_T("SYSTEM\\GWE\\Menu\\PopFnt")
#define KEY_BARFONT		_T("SYSTEM\\GWE\\Menu\\BarFnt")
#define KEY_OOMFONT		_T("SYSTEM\\GWE\\OOMFnt")
#define KEY_BUTTON		_T("SYSTEM\\GWE\\Button")

BOOL GetDefaultFont(LPLOGFONT lpLogFont)
{
	HFONT	hSysFont	= reinterpret_cast<HFONT>(GetStockObject(SYSTEM_FONT));

	GetObject(hSysFont, sizeof(LOGFONT), lpLogFont);

	lpLogFont->lfHeight	= GetDefaultFontHeight();

	return TRUE;
}

BOOL SetDefaultFont(LPLOGFONT lpLogFont, BOOL bHtInPts)
{
#ifdef DISABLE_MODIFY_SYSTEM_FONT
	return FALSE;
#else
	return TRUE;
#endif
}

BOOL GetTaskBarFont(LPLOGFONT lpLogFont)
{
	HFONT	hSysFont	= reinterpret_cast<HFONT>(GetStockObject(SYSTEM_FONT));

	GetObject(hSysFont, sizeof(LOGFONT), lpLogFont);

	lpLogFont->lfHeight	= GetDefaultFontHeight();
	lpLogFont->lfWeight	= FW_BOLD;

	return TRUE;
}

BOOL SetTaskBarFont(LPLOGFONT lpLogFont, BOOL bHtInPts)
{
#ifdef DISABLE_MODIFY_SYSTEM_FONT
	return FALSE;
#else
	return TRUE;
#endif
}

BOOL GetSystemFont(LPLOGFONT lpLogFont)
{
	DWORD	dwValue;
	BOOL	bHtInPts	= FALSE;
	TCHAR	szFaceName[LF_FACESIZE];

	ZeroMemory(lpLogFont, sizeof(LOGFONT));

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("CS"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfCharSet	= static_cast<BYTE>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("Wt"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfWeight		= static_cast<LONG>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("It"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfItalic		= static_cast<BYTE>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("HtInPts"), &dwValue) == ERROR_SUCCESS)
	{
		bHtInPts	= static_cast<BOOL>(dwValue);
	}

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("Ht"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bHtInPts)
	{
		lpLogFont->lfHeight	= HIPToHt(dwValue);
	}
	else
	{
		lpLogFont->lfHeight	= static_cast<LONG>(dwValue);
	}

	if (RegistryGetString(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("Nm"), szFaceName, LF_FACESIZE) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfPitchAndFamily	= FF_DONTCARE;

	_tcscpy_s(lpLogFont->lfFaceName, LF_FACESIZE, szFaceName);
	return TRUE;
}

BOOL SetSystemFont(LPLOGFONT lpLogFont, BOOL bHtInPts)
{
#ifdef DISABLE_MODIFY_SYSTEM_FONT
	return FALSE;
#else
	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("CS"), lpLogFont->lfCharSet) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("Wt"), lpLogFont->lfWeight) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("It"), lpLogFont->lfItalic) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bHtInPts)
	{
		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("HtInPts"), 1) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("Ht"), HtToHIP(lpLogFont->lfHeight)) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}
	else
	{
		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("HtInPts"), 0) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("Ht"), lpLogFont->lfHeight) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}

	if (RegistrySetString(HKEY_LOCAL_MACHINE, KEY_SYSFONT, _T("Nm"), lpLogFont->lfFaceName) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
#endif
}

BOOL GetPopupMenuFont(LPLOGFONT lpLogFont)
{
	DWORD	dwValue;
	BOOL	bHtInPts	= FALSE;
	TCHAR	szFaceName[LF_FACESIZE];

	ZeroMemory(lpLogFont, sizeof(LOGFONT));

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("CS"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfCharSet	= static_cast<BYTE>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("Wt"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfWeight		= static_cast<LONG>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("It"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfItalic		= static_cast<BYTE>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("HtInPts"), &dwValue) == ERROR_SUCCESS)
	{
		bHtInPts	= static_cast<BOOL>(dwValue);
	}

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("Ht"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bHtInPts)
	{
		lpLogFont->lfHeight	= HIPToHt(dwValue);
	}
	else
	{
		lpLogFont->lfHeight	= static_cast<LONG>(dwValue);
	}

	if (RegistryGetString(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("Nm"), szFaceName, LF_FACESIZE) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfPitchAndFamily	= FF_DONTCARE;

	_tcscpy_s(lpLogFont->lfFaceName, LF_FACESIZE, szFaceName);
	return TRUE;
}

BOOL SetPopupMenuFont(LPLOGFONT lpLogFont, BOOL bHtInPts)
{
	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("CS"), lpLogFont->lfCharSet) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("Wt"), lpLogFont->lfWeight) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("It"), lpLogFont->lfItalic) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bHtInPts)
	{
		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("HtInPts"), 1) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("Ht"), HtToHIP(lpLogFont->lfHeight)) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}
	else
	{
		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("HtInPts"), 0) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("Ht"), lpLogFont->lfHeight) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}

	if (RegistrySetString(HKEY_LOCAL_MACHINE, KEY_POPFONT, _T("Nm"), lpLogFont->lfFaceName) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GetMenuBarFont(LPLOGFONT lpLogFont)
{
	DWORD	dwValue;
	BOOL	bHtInPts	= FALSE;
	TCHAR	szFaceName[LF_FACESIZE];

	ZeroMemory(lpLogFont, sizeof(LOGFONT));

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("CS"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfCharSet	= static_cast<BYTE>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("Wt"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfWeight		= static_cast<LONG>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("It"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfItalic		= static_cast<BYTE>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("HtInPts"), &dwValue) == ERROR_SUCCESS)
	{
		bHtInPts	= static_cast<BOOL>(dwValue);
	}

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("Ht"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bHtInPts)
	{
		lpLogFont->lfHeight	= HIPToHt(dwValue);
	}
	else
	{
		lpLogFont->lfHeight	= static_cast<LONG>(dwValue);
	}

	if (RegistryGetString(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("Nm"), szFaceName, LF_FACESIZE) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfPitchAndFamily	= FF_DONTCARE;

	_tcscpy_s(lpLogFont->lfFaceName, LF_FACESIZE, szFaceName);
	return TRUE;
}

BOOL SetMenuBarFont(LPLOGFONT lpLogFont, BOOL bHtInPts)
{
	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("CS"), lpLogFont->lfCharSet) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("Wt"), lpLogFont->lfWeight) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("It"), lpLogFont->lfItalic) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bHtInPts)
	{
		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("HtInPts"), 1) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("Ht"), HtToHIP(lpLogFont->lfHeight)) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}
	else
	{
		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("HtInPts"), 0) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("Ht"), lpLogFont->lfHeight) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}

	if (RegistrySetString(HKEY_LOCAL_MACHINE, KEY_BARFONT, _T("Nm"), lpLogFont->lfFaceName) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GetOutOfMemoryFont(LPLOGFONT lpLogFont)
{
	DWORD	dwValue;
	BOOL	bHtInPts	= FALSE;
	TCHAR	szFaceName[LF_FACESIZE];

	ZeroMemory(lpLogFont, sizeof(LOGFONT));

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("CS"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfCharSet	= static_cast<BYTE>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("Wt"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfWeight		= static_cast<LONG>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("It"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfItalic		= static_cast<BYTE>(dwValue);

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("HtInPts"), &dwValue) == ERROR_SUCCESS)
	{
		bHtInPts	= static_cast<BOOL>(dwValue);
	}

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("Ht"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bHtInPts)
	{
		lpLogFont->lfHeight	= HIPToHt(dwValue);
	}
	else
	{
		lpLogFont->lfHeight	= static_cast<LONG>(dwValue);
	}

	if (RegistryGetString(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("Nm"), szFaceName, LF_FACESIZE) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfPitchAndFamily	= FF_DONTCARE;

	_tcscpy_s(lpLogFont->lfFaceName, LF_FACESIZE, szFaceName);
	return TRUE;
}

BOOL SetOutOfMemoryFont(LPLOGFONT lpLogFont, BOOL bHtInPts)
{
	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("CS"), lpLogFont->lfCharSet) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("Wt"), lpLogFont->lfWeight) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("It"), lpLogFont->lfItalic) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bHtInPts)
	{
		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("HtInPts"), 1) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("Ht"), HtToHIP(lpLogFont->lfHeight)) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}
	else
	{
		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("HtInPts"), 0) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("Ht"), lpLogFont->lfHeight) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}

	if (RegistrySetString(HKEY_LOCAL_MACHINE, KEY_OOMFONT, _T("Nm"), lpLogFont->lfFaceName) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GetButtonFont(LPLOGFONT lpLogFont)
{
	DWORD	dwValue;

	if (GetMenuBarFont(lpLogFont) == FALSE)
	{
		return FALSE;
	}

	if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, KEY_BUTTON, _T("FontWeight"), &dwValue) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	lpLogFont->lfWeight	= static_cast<LONG>(dwValue);

	return TRUE;
}

BOOL SetButtonFont(LPLOGFONT lpLogFont, BOOL bHtInPts)
{
	UNREFERENCED_PARAMETER(bHtInPts);

	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, KEY_BUTTON, _T("FontWeight"), lpLogFont->lfWeight) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

int GetLogFont(LPLOGFONT lpLogFont, HFONT hFont)
{
	ASSERT(hFont != NULL);
	ASSERT(lpLogFont != NULL);
	return GetObject(hFont, sizeof(LOGFONT), lpLogFont);
}

int HIPToPts(LONG lHIP)
{
	return (lHIP / 100);
}

LONG PtsToHIP(int nPts)
{
	return (nPts * 100);
}

int HtToPts(LONG lHt)
{
	return (MulDiv(-72, lHt, DRA::LogPixelsY()));
}

LONG PtsToHt(int nPts)
{
	return (-MulDiv(nPts, DRA::LogPixelsY(), 72));
}

LONG HIPToHt(LONG lHIP)
{
	return (PtsToHt(HIPToPts(lHIP)));
}

LONG HtToHIP(LONG lHt)
{
	return (PtsToHIP(HtToPts(lHt)));
}

LONG GetDefaultFontHeight(void)
{
	LCID	LocaleID	= GetSystemDefaultLCID();
	WORD	nLCID		= (WORD)(LocaleID & 0x0000FFFF);
	LONG	lHeight		= 0;

	switch (nLCID)
	{
	case 0x0411:	// Japanese
		{
			lHeight	= PtsToHt(9);
		}
		break;
	case 0x0412:	// Korean
		{
			lHeight	= PtsToHt(9);
		}
		break;
	case 0x0804:	// Chinese (PRC)
		{
			lHeight	= PtsToHt(9);
		}
		break;
	case 0x0404:	// Chinese (Taiwan)
		{
			lHeight	= PtsToHt(9);
		}
		break;
	default:		// English (U.S.) and other languages
		{
			lHeight	= PtsToHt(8);
		}
		break;
	}

	return lHeight;
}

LONG GetDefaultBoldWeight(void)
{
	LCID	LocaleID	= GetSystemDefaultLCID();
	WORD	nLCID		= (WORD)(LocaleID & 0x0000FFFF);
	LONG	lWeight		= FW_BOLD;

	switch (nLCID)
	{
	case 0x0411:	// Japanese
		{
			lWeight	= FW_SEMIBOLD;
		}
		break;
	case 0x0412:	// Korean
		{
			lWeight	= FW_SEMIBOLD;
		}
		break;
	case 0x0804:	// Chinese (PRC)
		{
			lWeight	= FW_SEMIBOLD;
		}
		break;
	case 0x0404:	// Chinese (Taiwan)
		{
			lWeight	= FW_SEMIBOLD;
		}
		break;
	default:		// English (U.S.) and other languages
		{
			lWeight	= FW_BOLD;
		}
		break;
	}

	return lWeight;
}

#define HKCU_CLEARTYPE			_T("Software\\Microsoft\\Shell")
#define HKLM_CLEARTYPE			_T("SOFTWARE\\Microsoft\\Shell")
#define HKLM_INTERNETSETTINGS	_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Internet Settings")
#define HKLM_GDIKEY				_T("SYSTEM\\GDI")
#define HKLM_CLEARTYPEKEY		_T("SYSTEM\\GDI\\CLEARTYPE")
#define HKLM_CLEARTYPESETTINGS	_T("SYSTEM\\GDI\\ClearTypeSettings")
#define HKLM_GAMMA				_T("System\\GDI\\Gamma")
#define HKLM_FONTSMOOTHINGKEY	_T("SYSTEM\\GDI\\Fontsmoothing")

#define SPI_GETFONTSMOOTHING			0x004A
#define SPI_SETFONTSMOOTHING			0x004B
#define SPI_GETFONTSMOOTHINGTYPE		0x200A
#define SPI_SETFONTSMOOTHINGTYPE		0x200B
#define SPI_GETFONTSMOOTHINGCONTRAST	0x200C
#define SPI_SETFONTSMOOTHINGCONTRAST	0x200D
#define SPI_GETFONTSMOOTHINGORIENTATION	0x2012
#define SPI_SETFONTSMOOTHINGORIENTATION	0x2013

#define FE_FONTSMOOTHINGSTANDARD		0x0001
#define FE_FONTSMOOTHINGCLEARTYPE		0x0002
#define FE_FONTSMOOTHINGDOCKING			0x8000
#define FE_FONTSMOOTHINGORIENTATIONBGR	0x0000
#define FE_FONTSMOOTHINGORIENTATIONRGB	0x0001

BOOL GetClearTypeRegistry(void)
{
	HKEY	hKey;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, HKLM_CLEARTYPEKEY, 0, 0, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	RegCloseKey(hKey);
	return TRUE;
}

BOOL GetClearTypeOffOnRotation(void)
{
	DWORD	dwValue	= 0;

	RegistryGetDWORD(HKEY_LOCAL_MACHINE, HKLM_CLEARTYPESETTINGS, _T("OffOnRotation"), &dwValue);
	return static_cast<BOOL>(dwValue);
}

BOOL GetClearType(void)
{
	BOOL	bSmooth		= FALSE;
	UINT	uiType		= 0;

	SystemParametersInfo(SPI_GETFONTSMOOTHING, 0, &bSmooth, 0);

	if (bSmooth)
	{
		SystemParametersInfo(SPI_GETFONTSMOOTHINGTYPE, 0, &uiType, 0);

		if (uiType == FE_FONTSMOOTHINGCLEARTYPE)
		{
			return TRUE;
		}
	}

	return FALSE;
}

int GetGammaSettings(void)
{
	DWORD	dwValue	= 0;

	RegistryGetDWORD(HKEY_LOCAL_MACHINE, HKLM_GAMMA, _T("Gamma Value"), &dwValue);
	return static_cast<int>(dwValue);
}

BOOL SetClearTypeRegistry(BOOL bClearType)
{
	RegistrySetDWORD(HKEY_CURRENT_USER, HKCU_CLEARTYPE, _T("ClearType"), (bClearType ? 1 : 0));
	RegistrySetDWORD(HKEY_LOCAL_MACHINE, HKLM_CLEARTYPE, _T("NoClearType"), (bClearType ? 0 : 1));
	RegistrySetDWORD(HKEY_LOCAL_MACHINE, HKLM_CLEARTYPE, _T("ClearTypeDetails"), 0);
	RegistrySetDWORD(HKEY_LOCAL_MACHINE, HKLM_CLEARTYPE, _T("ClearTypeAlgorithm"), 1);
	RegistrySetDWORD(HKEY_LOCAL_MACHINE, HKLM_INTERNETSETTINGS, _T("ClearTypeText"), (bClearType ? 1 : 0));

	if (bClearType)
	{
		HKEY	hKey;
		DWORD	dwDisposition;

		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, HKLM_CLEARTYPEKEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
		}
	}
	else
	{
		if (RegDeleteKey(HKEY_LOCAL_MACHINE, HKLM_CLEARTYPEKEY) == ERROR_SUCCESS)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL SetClearTypeOffOnRotation(BOOL bOffOnRotation)
{
	return (RegistrySetDWORD(HKEY_LOCAL_MACHINE, HKLM_CLEARTYPESETTINGS, _T("OffOnRotation"), (bOffOnRotation ? 1 : 0)) == ERROR_SUCCESS);
}

BOOL SetClearType(BOOL bClearType)
{
	LONG	lType	= FE_FONTSMOOTHINGCLEARTYPE;

	if (!SetClearTypeRegistry(bClearType))
	{
		return FALSE;
	}

	if (bClearType)
	{
		SystemParametersInfo(SPI_SETFONTSMOOTHING, 1, 0, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
		SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE, lType, &lType, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
	}
	else
	{
		SystemParametersInfo(SPI_SETFONTSMOOTHING, 0, 0, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
	}

	return TRUE;
}

BOOL SetGammaSettings(int nGamma)
{
	if ((nGamma < 1000) || (nGamma >= 2200))
	{
		nGamma	= 1000;
	}

	return (RegistrySetDWORD(HKEY_LOCAL_MACHINE, HKLM_GAMMA, _T("Gamma Value"), static_cast<DWORD>(nGamma)) == ERROR_SUCCESS);
}

BOOL GetAntialiasRegistry(BOOL& bForceGRAY16, BOOL& bExpansiveAntialias)
{
	HKEY	hKey;
	DWORD	dwValue;

	bForceGRAY16		= FALSE;
	bExpansiveAntialias	= FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, HKLM_FONTSMOOTHINGKEY, 0, 0, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	RegCloseKey(hKey);

	dwValue	= 0;

	RegistryGetDWORD(HKEY_LOCAL_MACHINE, HKLM_GDIKEY, _T("ForceGRAY16"), &dwValue);

	bForceGRAY16	= static_cast<BOOL>(dwValue != 0);

	dwValue	= 0;

	RegistryGetDWORD(HKEY_LOCAL_MACHINE, HKLM_GDIKEY, _T("ExpensiveAntialias"), &dwValue);

	bExpansiveAntialias	= static_cast<BOOL>(dwValue != 0);

	return TRUE;
}

BOOL GetAntialias(BOOL& bForceGRAY16, BOOL& bExpansiveAntialias)
{
	return (GetAntialiasRegistry(bForceGRAY16, bExpansiveAntialias));
}

BOOL SetAntialiasRegistry(BOOL bAntialias, BOOL bForceGRAY16, BOOL bExpansiveAntialias)
{
	HKEY	hKey;
	DWORD	dwDisposition;
	DWORD	dwValue;

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, HKLM_GDIKEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	if (bAntialias)
	{
		dwValue	= (bForceGRAY16 ? 1 : 0);

		RegSetValueEx(hKey, _T("ForceGRAY16"), 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwValue), sizeof(DWORD));

		dwValue	= (bExpansiveAntialias ? 1 : 0);

		RegSetValueEx(hKey, _T("ExpensiveAntialias"), 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwValue), sizeof(DWORD));
	}
	else
	{
		RegDeleteValue(hKey, _T("ForceGRAY16"));
		RegDeleteValue(hKey, _T("ExpensiveAntialias"));
	}

	RegCloseKey(hKey);

	if (bAntialias)
	{
		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, HKLM_FONTSMOOTHINGKEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		RegCloseKey(hKey);
	}
	else
	{
		if (RegDeleteKey(HKEY_LOCAL_MACHINE, HKLM_FONTSMOOTHINGKEY) != ERROR_SUCCESS)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL SetAntialias(BOOL bAntialias, BOOL bForceGRAY16, BOOL bExpansiveAntialias)
{
	return (SetAntialiasRegistry(bAntialias, bForceGRAY16, bExpansiveAntialias));
}
