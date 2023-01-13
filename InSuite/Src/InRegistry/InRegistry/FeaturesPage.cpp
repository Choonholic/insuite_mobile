// FeaturesPage.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "InRegistry_Settings.h"
#include "FeaturesPage.h"
#include "RegistryX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CFeaturesPage, CPropertyPageX)

CFeaturesPage::CFeaturesPage() : CPropertyPageX(CFeaturesPage::IDD)
{
}

CFeaturesPage::~CFeaturesPage()
{
}

void CFeaturesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_FE_AUTOCOLWIDTH, m_bAutoColumnWidth);
	DDX_Check(pDX, IDC_FE_NAMEEDITOR, m_bNameEditor);
	DDX_Control(pDX, IDC_FE_FEATURES_H, m_ctlFeaturesH);
}

BEGIN_MESSAGE_MAP(CFeaturesPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CFeaturesPage::LoadSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);

	m_bAutoColumnWidth	= (BOOL)(reg.ReadDWORD(RV_RG_AUTOWIDTH, 0) == 1);
	m_bNameEditor		= (BOOL)(reg.ReadDWORD(RV_RG_NAMEEDITOR, 1) == 1);

	reg.Close();
}

void CFeaturesPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_AUTOWIDTH, (m_bAutoColumnWidth ? 1 : 0));
	reg.WriteDWORD(RV_RG_NAMEEDITOR, (m_bNameEditor ? 1 : 0));
	reg.Close();
}

BOOL CFeaturesPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlFeaturesH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CFeaturesPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CFeaturesPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlFeaturesH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlFeaturesH.MoveWindow(&rc);
}

void CFeaturesPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlFeaturesH);
	CPropertyPageX::OnPaint();
}
