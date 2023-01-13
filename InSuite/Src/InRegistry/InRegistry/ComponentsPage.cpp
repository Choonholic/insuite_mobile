// ComponentsPage.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "InRegistry_Settings.h"
#include "ComponentsPage.h"
#include "RegistryX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CComponentsPage, CPropertyPageX)

CComponentsPage::CComponentsPage() : CPropertyPageX(CComponentsPage::IDD)
{
}

CComponentsPage::~CComponentsPage()
{
}

void CComponentsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CO_VERTICAL, m_bVertical);
	DDX_Check(pDX, IDC_CO_DATATYPE, m_bDataType);
	DDX_Check(pDX, IDC_CO_FULLROWSELECT, m_bFullRowSelect);
	DDX_Control(pDX, IDC_CO_COMPONENTS_H, m_ctlComponentsH);
}

BEGIN_MESSAGE_MAP(CComponentsPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CComponentsPage::LoadSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);

	m_bVertical			= (BOOL)(reg.ReadDWORD(RV_RG_LAYOUT, 1) == 1);
	m_bDataType			= (BOOL)(reg.ReadDWORD(RV_RG_SHOWDATATYPE, 1) == 1);
	m_bFullRowSelect	= (BOOL)(reg.ReadDWORD(RV_RG_FULLROWSELECT, 1) == 1);

	reg.Close();
}

void CComponentsPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_LAYOUT, (m_bVertical ? 1 : 0));
	reg.WriteDWORD(RV_RG_SHOWDATATYPE, (m_bDataType ? 1 : 0));
	reg.WriteDWORD(RV_RG_FULLROWSELECT, (m_bFullRowSelect ? 1 : 0));
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

	m_ctlComponentsH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlComponentsH.MoveWindow(&rc);
}

void CComponentsPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlComponentsH);
	CPropertyPageX::OnPaint();
}
