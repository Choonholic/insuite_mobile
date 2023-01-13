// InRegistry.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "InSuite_Version.h"
#include "InSuite_Settings.h"
#include "InRegistry_Settings.h"
#include "LicenseX.h"
#include "ShellX.h"
#include "TimeX.h"
#include "RegistryMFCX.h"
#include "RegisterDialogX.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CInRegistryApp, CWinApp)
END_MESSAGE_MAP()

CInRegistryApp::CInRegistryApp() : CWinApp()
{
}

CInRegistryApp	theApp;

BOOL CInRegistryApp::InitInstance()
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

	m_hResInst	= XP.Load(MD_RG_MAINLX, MD_RG_MAINLXP);

	if (m_hResInst == NULL)
	{
		m_hResInst	= AfxGetResourceHandle();
	}

	AfxSetResourceHandle(m_hResInst);
#endif

	TCHAR		szWindows[MAX_PATH + 1];
	TCHAR		szBinary[MAX_PATH + 1];
	DWORD		dwVersion	= MakeVersionNumberX(PID_INREGISTRY_CE, PVN_INSUITE_CLASSIC);
	ULONGLONG	ullPeriod;
	int			nActivated;

	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	CombineFileName(szBinary, MAX_PATH + 1, szWindows, MD_RG_TRIAL);

	ullPeriod	= (15 * OD_100NS);
	nActivated	= ReadActivateStatusX(szBinary, dwVersion, ullPeriod);

	if ((nActivated != ILM_ACTIVATED) && (nActivated != ILM_TRIAL))
	{
		CRegisterDialogX	dRegister;

		dRegister.m_strBinary	= MD_RG_TRIAL;
		dRegister.m_wProductID	= PID_INREGISTRY_CE;
		dRegister.m_wVersion	= PVN_INSUITE_CLASSIC;
		dRegister.m_nPeriod		= 15;

		dRegister.DoModal();
		return FALSE;
	}

	if (Import(m_lpCmdLine))
	{
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

BOOL CInRegistryApp::Import(LPCTSTR lpszCmdLine)
{
	CRegistryX	reg;
	BOOL		bGranted	= FALSE;
	BOOL		bSilent		= FALSE;
	CString		strCmdLine;
	CString		strOption;
	CString		strTitle;
	CString		strMessage;
	DWORD		dwType;
	TCHAR		szFileName[MAX_PATH + 1];

	if (_tcslen(lpszCmdLine) > 0)
	{
		strCmdLine	= lpszCmdLine;

		strCmdLine.Trim();

		if (strCmdLine.GetLength() > 3)
		{
			strOption	= strCmdLine.Left(3);

			if (!strOption.CompareNoCase(_T("-s:")) || !strOption.CompareNoCase(_T("/s:")))
			{
				bSilent		= TRUE;
				strCmdLine	= strCmdLine.Mid(3);

				strCmdLine.Trim();
			}
		}

		TakeFileName(strCmdLine, szFileName, MAX_PATH + 1);

		if (bSilent)
		{
			bGranted	= TRUE;
		}
		else
		{
			strTitle.LoadString(IDR_MAINFRAME);
			strMessage.Format(IDS_IMPORT_CONFIRM, szFileName);

			if (MessageBox(NULL, strMessage, strTitle, MB_YESNO | MB_DEFBUTTON2 | MB_ICONWARNING | MB_TOPMOST) == IDYES)
			{
				bGranted	= TRUE;
			}
		}

		if (bGranted == TRUE)
		{
			if (reg.REG5Import(strCmdLine))
			{
				strMessage.Format(IDS_IMPORT_SUCCESS, szFileName);

				dwType	= MB_OK | MB_ICONINFORMATION | MB_TOPMOST;
			}
			else
			{
				strMessage.Format(IDS_IMPORT_FAILED, szFileName);

				dwType	= MB_OK | MB_ICONERROR | MB_TOPMOST;
			}

			if (!bSilent)
			{
				MessageBox(NULL, strMessage, strTitle, dwType);
			}
		}

		return TRUE;
	}

	return FALSE;
}
