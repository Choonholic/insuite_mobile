// InExplorer.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InSuite_Version.h"
#include "InSuite_Settings.h"
#include "InExplorer_Settings.h"
#include "LicenseX.h"
#include "ShellX.h"
#include "TimeX.h"
#include "RegistryMFCX.h"
#include "RegisterDialogX.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CInExplorerApp, CWinApp)
END_MESSAGE_MAP()

CInExplorerApp::CInExplorerApp() : CWinApp()
{
}

CInExplorerApp	theApp;

BOOL CInExplorerApp::InitInstance()
{
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	SHInitExtraControls();
#endif

#ifdef	EXPACK_ENABLED
	CRegistryMFCX	reg;
	CString			strBase;

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	strBase	= reg.ReadString(RV_IS_FOLDER);

	XP.SetPackFolder(strBase);
	reg.Close();

	m_hResInst	= XP.Load(MD_EX_MAINLX, MD_EX_MAINLXP);

	if (m_hResInst == NULL)
	{
		m_hResInst	= AfxGetResourceHandle();
	}

	AfxSetResourceHandle(m_hResInst);
#endif

	TCHAR		szWindows[MAX_PATH + 1];
	TCHAR		szBinary[MAX_PATH + 1];
	DWORD		dwVersion	= MakeVersionNumberX(PID_INEXPLORER_CE, PVN_INSUITE_CLASSIC);
	ULONGLONG	ullPeriod;
	int			nActivated;

	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	CombineFileName(szBinary, MAX_PATH + 1, szWindows, MD_EX_TRIAL);

	ullPeriod	= (15 * OD_100NS);
	nActivated	= ReadActivateStatusX(szBinary, dwVersion, ullPeriod);

	if ((nActivated != ILM_ACTIVATED) && (nActivated != ILM_TRIAL))
	{
		CRegisterDialogX	dRegister;

		dRegister.m_strBinary	= MD_EX_TRIAL;
		dRegister.m_wProductID	= PID_INEXPLORER_CE;
		dRegister.m_wVersion	= PVN_INSUITE_CLASSIC;
		dRegister.m_nPeriod		= 15;

		dRegister.DoModal();
		return FALSE;
	}

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
