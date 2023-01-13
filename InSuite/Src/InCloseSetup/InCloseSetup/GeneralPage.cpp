// GeneralPage.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "InSuite_Settings.h"
#include "InClose_Settings.h"
#include "GeneralPage.h"
#include "ShellX.h"
#include "RegistryMFCX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CGeneralPage, CPropertyPageX)

CGeneralPage::CGeneralPage() : CPropertyPageX(CGeneralPage::IDD), m_bStartup(FALSE), m_bShowAnimation(FALSE), m_nResetMethod(0)
{
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_GE_STARTUP, m_bStartup);
	DDX_Check(pDX, IDC_GE_SHOWANIMATION, m_bShowAnimation);
	DDX_Radio(pDX, IDC_GE_RESETIMMEDIATELY, m_nResetMethod);
	DDX_Control(pDX, IDC_GE_GENERAL_H, m_ctlGeneralH);
	DDX_Control(pDX, IDC_GE_RESETMETHOD_H, m_ctlResetMethodH);
}

BEGIN_MESSAGE_MAP(CGeneralPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CGeneralPage::LoadSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_CL_GENERAL);

	m_bStartup			= GetStartup();
	m_bShowAnimation	= reg.ReadDWORD(RV_CL_SHOWANIMATION, 1);
	m_nResetMethod		= reg.ReadDWORD(RV_CL_RESETMETHOD, 0);

	reg.Close();
}

void CGeneralPage::SaveSettings()
{
	SetStartup(m_bStartup);

	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_CL_GENERAL);
	reg.WriteDWORD(RV_CL_SHOWANIMATION, m_bShowAnimation);
	reg.WriteDWORD(RV_CL_RESETMETHOD, m_nResetMethod);
	reg.Close();

	CInCloseSetupApp*	pApp	= (CInCloseSetupApp*)AfxGetApp();
	HWND				hWnd	= pApp->FindInClose();

	if (hWnd)
	{
		::SendMessage(hWnd, pApp->WM_CL_SETTINGCHAGE, PP_GENERAL, 0);
	}
}

BOOL CGeneralPage::GetStartup()
{
	TCHAR	szStartup[MAX_PATH + 1];
	TCHAR	szShortcut[MAX_PATH + 1];

	GetSpecialDirectoryEx(CSIDL_STARTUP, szStartup, MAX_PATH + 1);
	CombineFileName(szShortcut, MAX_PATH + 1, szStartup, MD_CL_SHORTCUT);
	return (IsFileExist(szShortcut));
}

void CGeneralPage::SetStartup(BOOL bStartup)
{
	TCHAR	szStartup[MAX_PATH + 1];
	TCHAR	szShortcut[MAX_PATH + 1];

	GetSpecialDirectoryEx(CSIDL_STARTUP, szStartup, MAX_PATH + 1);

	if (bStartup)
	{
		CString			strFolder;
		TCHAR			szModule[MAX_PATH + 1];
		CRegistryMFCX	reg;

		reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

		strFolder	= reg.ReadString(RV_IS_FOLDER);

		reg.Close();

		CombineFileName(szModule, MAX_PATH + 1, strFolder, MD_CL_MAIN);
		strFolder.Format(_T("\"%s\""), szModule);
		_tcscpy_s(szModule, MAX_PATH, strFolder);
		CombineFileName(szShortcut, MAX_PATH + 1, szStartup, MD_CL_SHORTCUT);
		DeleteFile(szShortcut);
		SHCreateShortcut(szShortcut, szModule);
	}
	else
	{
		CombineFileName(szShortcut, MAX_PATH + 1, szStartup, MD_CL_SHORTCUT);
		DeleteFile(szShortcut);
	}
}

BOOL CGeneralPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlGeneralH);
	SetHeaderItem(&m_ctlResetMethodH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CGeneralPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CGeneralPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlGeneralH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlGeneralH.MoveWindow(&rc);

	m_ctlResetMethodH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlResetMethodH.MoveWindow(&rc);
}

void CGeneralPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlGeneralH);
	DrawHeaderItem(dc, &m_ctlResetMethodH);
	CPropertyPageX::OnPaint();
}
