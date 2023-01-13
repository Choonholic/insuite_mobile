// ScreenPage.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "InSuite_Settings.h"
#include "InClose_Settings.h"
#include "ScreenPage.h"
#include "ShellX.h"
#include "RegistryMFCX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CScreenPage, CPropertyPageX)

CScreenPage::CScreenPage() : CPropertyPageX(CScreenPage::IDD), m_nRotation(0), m_bWakeupLock(FALSE)
{
}

CScreenPage::~CScreenPage()
{
}

void CScreenPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_SR_RIGHTHANDED, m_nRotation);
	DDX_Check(pDX, IDC_SR_WAKEUPLOCK, m_bWakeupLock);
	DDX_Control(pDX, IDC_SR_ROTATION_H, m_ctlRotationH);
	DDX_Control(pDX, IDC_SR_DEVICELOCK_H, m_ctlDeviceLockH);
}

BEGIN_MESSAGE_MAP(CScreenPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CScreenPage::LoadSettings()
{
	CRegistryMFCX	reg;
	DWORD			dwRotation;

	reg.Open(HKEY_CURRENT_USER, RK_CL_GENERAL);

	dwRotation		= reg.ReadDWORD(RV_CL_ROTATIONMODE, DMDO_90);
	m_bWakeupLock	= reg.ReadDWORD(RV_CL_WAKEUPLOCK, 0);

	reg.Close();

	switch (dwRotation)
	{
	case DMDO_270:
		{
			m_nRotation	= 1;
		}
		break;
	case DMDO_90:
	default:
		{
			m_nRotation	= 0;
		}
		break;
	}
}

void CScreenPage::SaveSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_CL_GENERAL);
	reg.WriteDWORD(RV_CL_ROTATIONMODE, ((m_nRotation == 1) ? DMDO_270 : DMDO_90));
	reg.WriteDWORD(RV_CL_WAKEUPLOCK, ((m_bWakeupLock == TRUE) ? 1 : 0));
	reg.Close();

	CInCloseSetupApp*	pApp	= (CInCloseSetupApp*)AfxGetApp();
	HWND				hWnd	= pApp->FindInClose();

	if (hWnd)
	{
		::SendMessage(hWnd, pApp->WM_CL_SETTINGCHAGE, PP_GENERAL, 0);
	}
}

BOOL CScreenPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlRotationH);
	SetHeaderItem(&m_ctlDeviceLockH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CScreenPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CScreenPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlRotationH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlRotationH.MoveWindow(&rc);

	m_ctlDeviceLockH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlDeviceLockH.MoveWindow(&rc);
}

void CScreenPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlRotationH);
	DrawHeaderItem(dc, &m_ctlDeviceLockH);
	CPropertyPageX::OnPaint();
}
