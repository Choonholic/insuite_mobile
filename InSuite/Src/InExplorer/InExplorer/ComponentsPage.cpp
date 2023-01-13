// ComponentsPage.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "RegistryMFCX.h"
#include "FolderDialogX.h"
#include "FontX.h"
#include "ComponentsPage.h"

IMPLEMENT_DYNAMIC(CComponentsPage, CPropertyPageX)

CComponentsPage::CComponentsPage() : CPropertyPageX(CComponentsPage::IDD), m_bVertical(TRUE), m_bShowFolderBar(TRUE), m_bShowStatusBar(TRUE), m_bFullRowSelect(TRUE)
{
}

CComponentsPage::~CComponentsPage()
{
}

void CComponentsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CO_VERTICAL, m_bVertical);
	DDX_Check(pDX, IDC_CO_FOLDERBAR, m_bShowFolderBar);
	DDX_Check(pDX, IDC_CO_STATUSBAR, m_bShowStatusBar);
	DDX_Check(pDX, IDC_CO_FULLROWSELECT, m_bFullRowSelect);
	DDX_Control(pDX, IDC_CO_COMPONENTS_H, m_ctlComponentsH);
}

BEGIN_MESSAGE_MAP(CComponentsPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CComponentsPage::LoadSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_VIEW);

	m_bVertical			= (BOOL)(reg.ReadDWORD(RV_EX_LAYOUT, 1) == 1);
	m_bShowFolderBar	= (BOOL)(reg.ReadDWORD(RV_EX_FOLDERBAR, 1) == 1);
	m_bShowStatusBar	= (BOOL)(reg.ReadDWORD(RV_EX_STATUSBAR, 1) == 1);
	m_bFullRowSelect	= (BOOL)(reg.ReadDWORD(RV_EX_FULLROWSELECT, 1) == 1);

	reg.Close();
}

void CComponentsPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_VIEW);
	reg.WriteDWORD(RV_EX_LAYOUT, ((m_bVertical == TRUE) ? 1 : 0));
	reg.WriteDWORD(RV_EX_FOLDERBAR, ((m_bShowFolderBar == TRUE) ? 1 : 0));
	reg.WriteDWORD(RV_EX_STATUSBAR, ((m_bShowStatusBar == TRUE) ? 1 : 0));
	reg.WriteDWORD(RV_EX_FULLROWSELECT, ((m_bFullRowSelect == TRUE) ? 1 : 0));
	reg.Close();
}

BOOL CComponentsPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlComponentsH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CComponentsPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CComponentsPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlComponentsH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlComponentsH.MoveWindow(&rc);
}

void CComponentsPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlComponentsH);
	CPropertyPageX::OnPaint();
}
