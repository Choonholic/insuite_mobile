// InClean.cpp

#include "stdafx.h"
#include "InClean.h"
#include "InSuite_Settings.h"
#include "InClean_Settings.h"
#include "MainSheet.h"
#include "RegistryMFCX.h"

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CInCleanApp, CWinApp)
END_MESSAGE_MAP()

CInCleanApp::CInCleanApp() : CWinApp()
{
}

CInCleanApp	theApp;

BOOL CInCleanApp::InitInstance()
{
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
	SHInitExtraControls();
#endif

#ifdef	EXPACK_ENABLED
	CRegistryMFCX	reg;
	CString			strFolder;

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	strFolder	= reg.ReadString(RV_IS_FOLDER);

	XP.SetPackFolder(strFolder);
	reg.Close();

	m_hResInst	= XP.Load(MD_CN_MAINLX, MD_CN_MAINLXP);

	if (m_hResInst == NULL)
	{
		m_hResInst	= AfxGetResourceHandle();
	}

	AfxSetResourceHandle(m_hResInst);
#endif

	CMainSheet	Sheet(IDS_INCLEAN);
	INT_PTR		nResponse;

	m_pMainWnd	= &Sheet;
	nResponse	= Sheet.DoModal();

	return FALSE;
}
