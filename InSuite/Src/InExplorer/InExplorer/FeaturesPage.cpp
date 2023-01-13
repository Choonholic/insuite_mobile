// FeaturesPage.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "RegistryX.h"
#include "FontX.h"
#include "FeaturesPage.h"

IMPLEMENT_DYNAMIC(CFeaturesPage, CPropertyPageX)

CFeaturesPage::CFeaturesPage() : CPropertyPageX(CFeaturesPage::IDD), m_bAutoColumnWidth(TRUE), m_bNameEditor(TRUE), m_bMultiSelect(FALSE)
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
	DDX_Check(pDX, IDC_FE_MULTISELECT, m_bMultiSelect);
	DDX_Control(pDX, IDC_FE_FEATURES_H, m_ctlFeaturesH);
}

BEGIN_MESSAGE_MAP(CFeaturesPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CFeaturesPage::LoadSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);

	m_bAutoColumnWidth	= (BOOL)(reg.ReadDWORD(RV_EX_AUTOWIDTH, 1) == 1);
	m_bNameEditor		= (BOOL)(reg.ReadDWORD(RV_EX_NAMEEDITOR, 1) == 1);
	m_bMultiSelect		= (BOOL)(reg.ReadDWORD(RV_EX_MULTISELECT, 0) == 1);

	reg.Close();
}

void CFeaturesPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_AUTOWIDTH, (m_bAutoColumnWidth ? 1 : 0));
	reg.WriteDWORD(RV_EX_NAMEEDITOR, (m_bNameEditor ? 1 : 0));
	reg.WriteDWORD(RV_EX_MULTISELECT, (m_bMultiSelect ? 1 : 0));
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
	int		nLeft;

	m_ctlFeaturesH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlFeaturesH.MoveWindow(&rc);
}

void CFeaturesPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlFeaturesH);
	CPropertyPageX::OnPaint();
}
