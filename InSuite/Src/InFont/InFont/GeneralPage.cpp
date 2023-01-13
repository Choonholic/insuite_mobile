// GeneralPage.cpp

#include "stdafx.h"
#include "InFont.h"
#include "InFont_Settings.h"
#include "GeneralPage.h"
#include "RegistryX.h"
#include "FontInfo.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CGeneralPage, CPropertyPageX)

CGeneralPage::CGeneralPage() : CPropertyPageX(CGeneralPage::IDD), m_bFontLinkMethod(TRUE), m_nResetMethod(0)
{
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_GE_FONTLINKMETHOD, m_bFontLinkMethod);
	DDX_Radio(pDX, IDC_GE_RESETIMMEDIATELY, m_nResetMethod);
	DDX_Control(pDX, IDC_GE_FONTLINKMETHOD_H, m_ctlFontLinkMethodH);
	DDX_Control(pDX, IDC_GE_RESETMETHOD_H, m_ctlResetMethodH);
}

BEGIN_MESSAGE_MAP(CGeneralPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CGeneralPage::LoadSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_LOCAL_MACHINE, GDI_KEY);

	m_bFontLinkMethod	= (BOOL)(reg.ReadDWORD(GDI_FONTLINKMETHODS, 1));

	reg.Close();

	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);

	m_nResetMethod	= reg.ReadDWORD(RV_FN_RESETMETHOD, 0);

	reg.Close();
}

void CGeneralPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_LOCAL_MACHINE, GDI_KEY);
	reg.WriteDWORD(GDI_FONTLINKMETHODS, (m_bFontLinkMethod ? 1 : 0));
	reg.Close();

	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);
	reg.WriteDWORD(RV_FN_RESETMETHOD, m_nResetMethod);
	reg.Close();
}

BOOL CGeneralPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlFontLinkMethodH);
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

	m_ctlFontLinkMethodH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlFontLinkMethodH.MoveWindow(&rc);

	m_ctlResetMethodH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlResetMethodH.MoveWindow(&rc);
}

void CGeneralPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlFontLinkMethodH);
	DrawHeaderItem(dc, &m_ctlResetMethodH);
	CPropertyPageX::OnPaint();
}
