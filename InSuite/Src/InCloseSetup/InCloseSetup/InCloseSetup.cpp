// InCloseSetup.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "InSuite_Settings.h"
#include "InClose_Settings.h"
#include "MainSheet.h"
#include "RegistryMFCX.h"

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CInCloseSetupApp, CWinApp)
END_MESSAGE_MAP()

CInCloseSetupApp::CInCloseSetupApp() : CWinApp()
{
}

CInCloseSetupApp	theApp;

BOOL CInCloseSetupApp::InitInstance()
{
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	SHInitExtraControls();
#endif

	WM_CL_SETTINGCHAGE	= RegisterWindowMessage(MS_CL_SETTINGCHANGE);

#ifdef	EXPACK_ENABLED
	CRegistryMFCX	reg;
	CString			strFolder;

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	strFolder	= reg.ReadString(RV_IS_FOLDER);

	XP.SetPackFolder(strFolder);
	reg.Close();

	m_hResInst	= XP.Load(MD_CL_SETUPLX, MD_CL_SETUPLXP);

	if (m_hResInst == NULL)
	{
		m_hResInst	= AfxGetResourceHandle();
	}

	AfxSetResourceHandle(m_hResInst);
#endif

	CMainSheet	Sheet(IDS_INCLOSE);
	INT_PTR		nResponse;

	Sheet.SetTitle(IDS_TITLE);

	m_pMainWnd	= &Sheet;
	nResponse	= Sheet.DoModal();

	return FALSE;
}

HWND CInCloseSetupApp::FindInClose()
{
	HWND	hWnd	= FindWindow(_T("HHTaskBar"), NULL);
	UINT	nCmd	= GW_CHILD | GW_HWNDFIRST;
	BOOL	bFound	= FALSE;
	TCHAR	szClassWnd[MAX_PATH + 1];

	while (hWnd = GetWindow(hWnd, nCmd))
	{
		GetClassName(hWnd, szClassWnd, MAX_PATH);

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
