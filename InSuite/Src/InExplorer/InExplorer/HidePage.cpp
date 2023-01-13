// HidePage.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "RegistryX.h"
#include "FontX.h"
#include "HidePage.h"

IMPLEMENT_DYNAMIC(CHidePage, CPropertyPageX)

CHidePage::CHidePage() : CPropertyPageX(CHidePage::IDD), m_bHideSystem(TRUE), m_bHideHidden(TRUE), m_bHideROM(TRUE), m_bHideExtension(TRUE)
{
}

CHidePage::~CHidePage()
{
}

void CHidePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_HI_HIDESYSTEM, m_bHideSystem);
	DDX_Check(pDX, IDC_HI_HIDEHIDDEN, m_bHideHidden);
	DDX_Check(pDX, IDC_HI_HIDEROM, m_bHideROM);
	DDX_Check(pDX, IDC_HI_HIDEEXTENSION, m_bHideExtension);
	DDX_Control(pDX, IDC_HI_HIDE_H, m_ctlHideH);
}

BEGIN_MESSAGE_MAP(CHidePage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CHidePage::LoadSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);

	m_bHideSystem		= (BOOL)(reg.ReadDWORD(RV_EX_SHOWSYSTEM, 0) == 0);
	m_bHideHidden		= (BOOL)(reg.ReadDWORD(RV_EX_SHOWHIDDEN, 0) == 0);
	m_bHideROM			= (BOOL)(reg.ReadDWORD(RV_EX_SHOWROM, 0) == 0);
	m_bHideExtension	= (BOOL)(reg.ReadDWORD(RV_EX_SHOWEXT, 0) == 0);

	reg.Close();
}

void CHidePage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_SHOWSYSTEM, (m_bHideSystem ? 0 : 1));
	reg.WriteDWORD(RV_EX_SHOWHIDDEN, (m_bHideHidden ? 0 : 1));
	reg.WriteDWORD(RV_EX_SHOWROM, (m_bHideROM ? 0 : 1));
	reg.WriteDWORD(RV_EX_SHOWEXT, (m_bHideExtension ? 0 : 1));
	reg.Close();
}

BOOL CHidePage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlHideH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CHidePage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CHidePage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlHideH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlHideH.MoveWindow(&rc);
}

void CHidePage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlHideH);
	CPropertyPageX::OnPaint();
}
