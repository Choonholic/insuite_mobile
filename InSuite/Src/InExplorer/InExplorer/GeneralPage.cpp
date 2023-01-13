// GeneralPage.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "RegistryMFCX.h"
#include "FolderDialogX.h"
#include "FontX.h"
#include "GeneralPage.h"

IMPLEMENT_DYNAMIC(CGeneralPage, CPropertyPageX)

CGeneralPage::CGeneralPage() : CPropertyPageX(CGeneralPage::IDD), m_nStartup(0)
{
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_GE_LASTOPENED, m_nStartup);
	DDX_Check(pDX, IDC_GE_DOUBLETAP, m_bDoubleTap);
	DDX_Control(pDX, IDC_GE_STARTUP_H, m_ctlStartupH);
	DDX_Control(pDX, IDC_GE_MISC_H, m_ctlMiscH);
	DDX_Control(pDX, IDC_GE_SPECIFIC, m_ctlSpecific);
}

BEGIN_MESSAGE_MAP(CGeneralPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_GE_LASTOPENED, &CGeneralPage::OnBnClickedGeLastopened)
	ON_BN_CLICKED(IDC_GE_SPECIFIC_H, &CGeneralPage::OnBnClickedGeSpecificH)
	ON_BN_CLICKED(IDC_GE_SPECIFIC, &CGeneralPage::OnBnClickedGeSpecific)
END_MESSAGE_MAP()

void CGeneralPage::LoadSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_GENERAL);

	m_nStartup		= reg.ReadDWORD(RV_EX_STARTUP, 0);
	m_strSpecific	= reg.ReadString(RV_EX_OPENFOLDER, _T("\\"));
	m_bDoubleTap	= (BOOL)(reg.ReadDWORD(RV_EX_DOUBLETAP, 1) == 1);

	reg.Close();
	m_ctlSpecific.SetWindowText(m_strSpecific);
}

void CGeneralPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_GENERAL);
	reg.WriteDWORD(RV_EX_STARTUP, m_nStartup);
	reg.WriteString(RV_EX_OPENFOLDER, m_strSpecific);
	reg.WriteDWORD(RV_EX_DOUBLETAP, ((m_bDoubleTap == TRUE) ? 1 : 0));
	reg.Close();
}

void CGeneralPage::UpdateControls()
{
	m_ctlSpecific.EnableWindow((BOOL)(m_nStartup == 1));
}

BOOL CGeneralPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlStartupH);
	SetHeaderItem(&m_ctlMiscH);
	LoadSettings();
	UpdateData(FALSE);
	UpdateControls();
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

	m_ctlStartupH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlStartupH.MoveWindow(&rc);

	m_ctlMiscH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlMiscH.MoveWindow(&rc);

	m_ctlSpecific.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlSpecific.MoveWindow(&rc);
}

void CGeneralPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlStartupH);
	DrawHeaderItem(dc, &m_ctlMiscH);
	CPropertyPageX::OnPaint();
}

void CGeneralPage::OnBnClickedGeLastopened()
{
	UpdateData(TRUE);
	UpdateControls();
}

void CGeneralPage::OnBnClickedGeSpecificH()
{
	UpdateData(TRUE);
	UpdateControls();
}

void CGeneralPage::OnBnClickedGeSpecific()
{
	CFolderDialogX	dFD;

	dFD.SetDefaultPath(m_strSpecific);

	if (dFD.DoModal() == IDOK)
	{
		m_strSpecific	= dFD.GetFullPath();

		m_ctlSpecific.SetWindowText(m_strSpecific);
	}
}
