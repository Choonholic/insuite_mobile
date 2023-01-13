// FindPage.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "RegistryMFCX.h"
#include "FolderDialogX.h"
#include "FontX.h"
#include "FindPage.h"

IMPLEMENT_DYNAMIC(CFindPage, CPropertyPageX)

CFindPage::CFindPage() : CPropertyPageX(CFindPage::IDD), m_bAutoColumnWidth(TRUE)
{
}

CFindPage::~CFindPage()
{
}

void CFindPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_FD_AUTOCOLWIDTH, m_bAutoColumnWidth);
	DDX_Control(pDX, IDC_FD_FOUNDITEMS_H, m_ctlFoundItemsH);
}

BEGIN_MESSAGE_MAP(CFindPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CFindPage::LoadSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_FIND);

	m_bAutoColumnWidth	= (BOOL)(reg.ReadDWORD(RV_EX_AUTOWIDTH, 1) == 1);

	reg.Close();
}

void CFindPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_FIND);
	reg.WriteDWORD(RV_EX_AUTOWIDTH, ((m_bAutoColumnWidth == TRUE) ? 1 : 0));
	reg.Close();
}

BOOL CFindPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlFoundItemsH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CFindPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CFindPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlFoundItemsH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlFoundItemsH.MoveWindow(&rc);
}

void CFindPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlFoundItemsH);
	CPropertyPageX::OnPaint();
}
