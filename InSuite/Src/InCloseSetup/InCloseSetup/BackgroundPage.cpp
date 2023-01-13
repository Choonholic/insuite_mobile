// BackgroundPage.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "InClose_Settings.h"
#include "BackgroundPage.h"
#include "RegistryMFCX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CBackgroundPage, CPropertyPageX)

CBackgroundPage::CBackgroundPage() : CPropertyPageX(CBackgroundPage::IDD), m_nType(1)
{
}

CBackgroundPage::~CBackgroundPage()
{
}

void CBackgroundPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_BG_NONE, m_nType);
	DDX_Control(pDX, IDC_BG_MENUBG_H, m_ctlMenuBackgroundH);
}

BEGIN_MESSAGE_MAP(CBackgroundPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CBackgroundPage::LoadSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_CL_MENU);

	m_nType			= reg.ReadDWORD(RV_CL_TYPE, 1);

	reg.Close();
}

void CBackgroundPage::SaveSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_CL_MENU);
	reg.WriteDWORD(RV_CL_TYPE, m_nType);
	reg.Close();

	CInCloseSetupApp*	pApp	= (CInCloseSetupApp*)AfxGetApp();
	HWND				hWnd	= pApp->FindInClose();

	if (hWnd)
	{
		::SendMessage(hWnd, pApp->WM_CL_SETTINGCHAGE, PP_BACKGROUND, 0);
	}
}

BOOL CBackgroundPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlMenuBackgroundH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CBackgroundPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CBackgroundPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlMenuBackgroundH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlMenuBackgroundH.MoveWindow(&rc);
}

void CBackgroundPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlMenuBackgroundH);
	CPropertyPageX::OnPaint();
}
