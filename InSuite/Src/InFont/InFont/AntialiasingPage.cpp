// AntialiasingPage.cpp

#include "stdafx.h"
#include "InFont.h"
#include "InFont_Settings.h"
#include "AntialiasingPage.h"
#include "FontX.h"
#include "RegistryMFCX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CAntialiasingPage, CPropertyPageX)

CAntialiasingPage::CAntialiasingPage() : CPropertyPageX(CAntialiasingPage::IDD), m_nType(0), m_bApplyLS(TRUE)
{
}

CAntialiasingPage::~CAntialiasingPage()
{
}

void CAntialiasingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_AA_NONE, m_nType);
	DDX_Check(pDX, IDC_AA_APPLY_LS, m_bApplyLS);
	DDX_Control(pDX, IDC_AA_ANTIALIASING_H, m_ctlTypeH);
	DDX_Control(pDX, IDC_AA_ADDOPTIONS_H, m_ctlAddOptionsH);
}

BEGIN_MESSAGE_MAP(CAntialiasingPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CAntialiasingPage::LoadSettings()
{
	CRegistryMFCX	reg;
	BOOL			bForceGRAY16;
	BOOL			bExpansiveAntialias;

	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);

	m_nType		= reg.ReadDWORD(RV_FN_ANTIALIASING, 1);
	m_bApplyLS	= reg.ReadDWORD(RV_FN_AA_LS, 1);

	if (GetClearTypeRegistry())
	{
		m_nType	= 1;
	}
	else if (GetAntialias(bForceGRAY16, bExpansiveAntialias))
	{
		m_nType	= 2;
	}
	else
	{
		m_nType	= 0;
	}

	reg.WriteDWORD(RV_FN_ANTIALIASING, m_nType);
	reg.Close();
}

void CAntialiasingPage::SaveSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);
	reg.WriteDWORD(RV_FN_ANTIALIASING, m_nType);
	reg.WriteDWORD(RV_FN_AA_LS, m_bApplyLS);
	reg.Close();

	switch (m_nType)
	{
	case 1:		// ClearType
		{
			SetAntialias(FALSE, FALSE, FALSE);
			SetClearTypeOffOnRotation(m_bApplyLS ? FALSE : TRUE);
			SetClearType(TRUE);
		}
		break;
	case 2:		// Font Smoothing
		{
			SetClearTypeOffOnRotation(TRUE);
			SetClearType(FALSE);
			SetAntialias(TRUE, TRUE, TRUE);
		}
		break;
	default:	// None
		{
			SetClearTypeOffOnRotation(TRUE);
			SetClearType(FALSE);
			SetAntialias(FALSE, FALSE, FALSE);
		}
		break;
	}
}

BOOL CAntialiasingPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlTypeH);
	SetHeaderItem(&m_ctlAddOptionsH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CAntialiasingPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CAntialiasingPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	CRect	rc;
	int		nLeft;

	m_ctlTypeH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlTypeH.MoveWindow(&rc);

	m_ctlAddOptionsH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlAddOptionsH.MoveWindow(&rc);
}

void CAntialiasingPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlTypeH);
	DrawHeaderItem(dc, &m_ctlAddOptionsH);
	CPropertyPageX::OnPaint();
}
