// InGauge.cpp

#include "stdafx.h"
#include "InGauge.h"
#include "InSuite_Settings.h"
#include "InGauge_Settings.h"
#include "MainSheet.h"
#include "DeviceInfoX.h"
#include "RegistryMFCX.h"

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CInGaugeApp, CWinApp)
END_MESSAGE_MAP()

CInGaugeApp::CInGaugeApp() : CWinApp()
{
}

CInGaugeApp	theApp;

BOOL CInGaugeApp::InitInstance()
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

	m_hResInst	= XP.Load(MD_GG_MAINLX, MD_GG_MAINLXP);

	if (m_hResInst == NULL)
	{
		m_hResInst	= AfxGetResourceHandle();
	}

	AfxSetResourceHandle(m_hResInst);
#endif

	CMainSheet	Sheet(IDS_INGAUGE);
	INT_PTR		nResponse;
	TCHAR		szModel[64];

	GetMachineNameX(szModel, 64);
	Sheet.SetTitle(szModel);

	m_pMainWnd	= &Sheet;
	nResponse	= Sheet.DoModal();

	return FALSE;
}
