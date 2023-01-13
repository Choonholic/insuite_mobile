// InFont.cpp

#include "stdafx.h"
#include "InFont.h"
#include "InSuite_Version.h"
#include "InSuite_Settings.h"
#include "InFont_Settings.h"
#include "LicenseX.h"
#include "TimeX.h"
#include "MainFrm.h"
#include "ShellX.h"
#include "FontX.h"
#include "RegistryMFCX.h"

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CInFontApp, CWinApp)
END_MESSAGE_MAP()

CInFontApp::CInFontApp() : CWinApp()
{
}

CInFontApp	theApp;

BOOL CInFontApp::InitInstance()
{
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	SHInitExtraControls();
#endif

	if ((!_tcsnicmp(m_lpCmdLine, _T("/C"), 2)) || (!_tcsnicmp(m_lpCmdLine, _T("-C"), 2)))
	{
		TCHAR	szStartup[MAX_PATH + 1];
		TCHAR	szShortcut[MAX_PATH + 1];

		GetSpecialDirectoryEx(CSIDL_STARTUP, szStartup, MAX_PATH + 1);
		CombineFileName(szShortcut, MAX_PATH + 1, szStartup, MD_FN_SHORTCUT);
		DeleteFile(szShortcut);
		RemoveFonts();
		return FALSE;
	}

#ifdef	EXPACK_ENABLED
	CRegistryMFCX	reg;
	CString			strBase;

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	strBase	= reg.ReadString(RV_IS_FOLDER);

	XP.SetPackFolder(strBase);
	reg.Close();

	m_hResInst	= XP.Load(MD_FN_MAINLX, MD_FN_MAINLXP);

	if (m_hResInst == NULL)
	{
		m_hResInst	= AfxGetResourceHandle();
	}

	AfxSetResourceHandle(m_hResInst);
#endif

	CMainFrame*	pFrame	= new CMainFrame;

	if (!pFrame)
	{
		return FALSE;
	}

	m_pMainWnd	= pFrame;

	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}

void CInFontApp::RemoveFonts()
{
	TCHAR			szFontPath[MAX_PATH + 1];
	TCHAR			szFindPath[MAX_PATH + 1];
	TCHAR			szFound[MAX_PATH + 1];
	HANDLE			hFind;
	WIN32_FIND_DATA	fd;

	GetFontPath(szFontPath, MAX_PATH + 1);
	CombineFileName(szFindPath, MAX_PATH + 1, szFontPath, _T("*.mdf"));

	hFind	= FindFirstFile(szFindPath, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CombineFileName(szFound, MAX_PATH + 1, szFontPath, fd.cFileName);
			SetFileAttributes(szFound, FILE_ATTRIBUTE_NORMAL);
			DeleteFile(szFound);
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
}
