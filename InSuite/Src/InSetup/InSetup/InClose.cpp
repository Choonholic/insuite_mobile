// InClose.cpp

#include "stdafx.h"
#include "InClose.h"
#include "InClose_Settings.h"
#include "Utilities.h"

#define MAX_TITLE_LEN	64

HWND FindInClose(void)
{
	HWND	hWnd	= FindWindow(_T("HHTaskBar"), NULL);
	UINT	nCmd	= GW_CHILD | GW_HWNDFIRST;
	BOOL	bFound	= FALSE;
	TCHAR	szClassWnd[MAX_TITLE_LEN + 1];

	while (hWnd = GetWindow(hWnd, nCmd))
	{
		GetClassName(hWnd, szClassWnd, MAX_TITLE_LEN);

		if (!_tcscmp(szClassWnd, NM_CL_CLASS))
		{
			bFound	= TRUE;

			break;
		}

		nCmd	= GW_HWNDNEXT;
	}

	if (bFound)
	{
		return hWnd;
	}

	return NULL;
}

void RegisterInClose(LPCTSTR lpszDirectory)
{
	TCHAR	szStartup[MAX_PATH + 1];
	TCHAR	szShortcut[MAX_PATH + 1];
	TCHAR	szModule[MAX_PATH + 1];

	CombineFileName(szModule, MAX_PATH + 1, lpszDirectory, MD_CL_MAIN);
	wsprintf(szShortcut, _T("\"%s\""), szModule);
	_tcscpy_s(szModule, MAX_PATH + 1, szShortcut);
	GetSpecialDirectoryEx(CSIDL_STARTUP, szStartup, MAX_PATH + 1);
	CombineFileName(szShortcut, MAX_PATH + 1, szStartup, MD_CL_SHORTCUT);
	DeleteFile(szShortcut);
	SHCreateShortcut(szShortcut, szModule);
}

void UnregisterInClose(void)
{
	TCHAR	szStartup[MAX_PATH + 1];
	TCHAR	szFileName[MAX_PATH + 1];

	GetSpecialDirectoryEx(CSIDL_STARTUP, szStartup, MAX_PATH + 1);
	CombineFileName(szFileName, MAX_PATH + 1, szStartup, MD_CL_SHORTCUT);
	DeleteFile(szFileName);
}

BOOL LoadInClose(void)
{
	TCHAR				szStartup[MAX_PATH + 1];
	TCHAR				szShortcut[MAX_PATH + 1];
	SHELLEXECUTEINFO	sei;

	GetSpecialDirectoryEx(CSIDL_STARTUP, szStartup, MAX_PATH + 1);
	CombineFileName(szShortcut, MAX_PATH + 1, szStartup, MD_CL_SHORTCUT);
	memset(&sei, 0, sizeof(SHELLEXECUTEINFO));

	sei.cbSize			= sizeof(SHELLEXECUTEINFO);
	sei.fMask			= SEE_MASK_FLAG_NO_UI;
	sei.lpVerb			= _T("Open");
	sei.lpFile			= szShortcut;
	sei.lpParameters	= _T("/N");
	sei.nShow			= SW_SHOW;

	return ShellExecuteEx(&sei);
}

BOOL UnloadInClose(void)
{
	HWND	hWnd	= FindInClose();

	if (hWnd != NULL)
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		Sleep(500);
		return TRUE;
	}

	return FALSE;
}
