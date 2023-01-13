// FormatPage.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "RegistryMFCX.h"
#include "FolderDialogX.h"
#include "FontX.h"
#include "FormatPage.h"

IMPLEMENT_DYNAMIC(CFormatPage, CPropertyPageX)

CFormatPage::CFormatPage() : CPropertyPageX(CFormatPage::IDD), m_bSimpleDate(TRUE), m_bSimpleSize(TRUE)
{
}

CFormatPage::~CFormatPage()
{
}

void CFormatPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_FO_SIMPLEDATE, m_bSimpleDate);
	DDX_Check(pDX, IDC_FO_SIMPLESIZE, m_bSimpleSize);
	DDX_Control(pDX, IDC_FO_FORMAT_H, m_ctlFormatH);
}

BEGIN_MESSAGE_MAP(CFormatPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CFormatPage::LoadSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_VIEW);

	m_bSimpleDate	= (BOOL)(reg.ReadDWORD(RV_EX_DATEFORMAT, 1) == 0);
	m_bSimpleSize	= (BOOL)(reg.ReadDWORD(RV_EX_SIZEFORMAT, 1) == 0);

	reg.Close();
}

void CFormatPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_VIEW);
	reg.WriteDWORD(RV_EX_DATEFORMAT, ((m_bSimpleDate == TRUE) ? 0 : 1));
	reg.WriteDWORD(RV_EX_SIZEFORMAT, ((m_bSimpleSize == TRUE) ? 0 : 1));
	reg.Close();
}

BOOL CFormatPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlFormatH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CFormatPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CFormatPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlFormatH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlFormatH.MoveWindow(&rc);
}

void CFormatPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlFormatH);
	CPropertyPageX::OnPaint();
}
