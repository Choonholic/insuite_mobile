// ZIPPage.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "RegistryMFCX.h"
#include "FolderDialogX.h"
#include "FontX.h"
#include "ZIPPage.h"

IMPLEMENT_DYNAMIC(CZIPPage, CPropertyPageX)

#define ZIPLEVEL_MAX	4
#define CODEPAGE_MAX	6

int	g_tZipLevels[ZIPLEVEL_MAX]	=
{
	9,			// Best
	8,			// Normal
	4,			// Fast
	1			// Store Only
};

int	g_tCodePages[CODEPAGE_MAX]	=
{
	1252,		// Western European
	CP_UTF8,	// UTF-8
	949,		// Korean
	932,		// Japanese (Shift-JIS)
	936,		// Chinese Simplified
	950			// Chinese Traditional
};

CZIPPage::CZIPPage() : CPropertyPageX(CZIPPage::IDD), m_nLevel(0), m_nCodePageSet(0), m_nCodePage(0)
{
}

CZIPPage::~CZIPPage()
{
}

void CZIPPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_ZP_SYSTEMDEFAULT, m_nCodePageSet);
	DDX_CBIndex(pDX, IDC_ZP_COMPRESSLEVEL, m_nLevel);
	DDX_CBIndex(pDX, IDC_ZP_SPECIFIC_CP, m_nCodePage);
	DDX_Control(pDX, IDC_ZP_COMPRESSLEVEL_H, m_ctlLevelH);
	DDX_Control(pDX, IDC_ZP_COMPRESSLEVEL, m_ctlLevel);
	DDX_Control(pDX, IDC_ZP_CODEPAGE_H, m_ctlCodePageH);
	DDX_Control(pDX, IDC_ZP_SPECIFIC_CP, m_ctlCodePage);
}

BEGIN_MESSAGE_MAP(CZIPPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_ZP_SYSTEMDEFAULT, &CZIPPage::OnBnClickedZpSystemdefault)
	ON_BN_CLICKED(IDC_ZP_SPECIFIC_CP_H, &CZIPPage::OnBnClickedZpSpecificCpH)
END_MESSAGE_MAP()

void CZIPPage::SetMenus()
{
	CString	strItem;
	int		i;

	for (i = 0; i < ZIPLEVEL_MAX; i++)
	{
		strItem.LoadString(IDS_ZIP_BEST + i);
		m_ctlLevel.AddString(strItem);
	}

	for (i = 0; i < CODEPAGE_MAX; i++)
	{
		strItem.LoadString(IDS_CODEPAGE_1252 + i);
		m_ctlCodePage.AddString(strItem);
	}
}

void CZIPPage::LoadSettings()
{
	CRegistryMFCX	reg;
	DWORD			dwValue;
	int				i;

	m_nLevel	= (-1);
	m_nCodePage	= (-1);

	reg.Open(HKEY_CURRENT_USER, RK_EX_ZIP);

	dwValue	= reg.ReadDWORD(RV_EX_COMPRESSLEVEL, 9);

	for (i = 0; i < ZIPLEVEL_MAX; i++)
	{
		if (g_tZipLevels[i] == dwValue)
		{
			m_nLevel	= i;
		}
	}

	if (m_nLevel < 0)
	{
		m_nLevel	= 0;
	}

	m_nCodePageSet	= reg.ReadDWORD(RV_EX_CODEPAGESET, 0);
	dwValue			= reg.ReadDWORD(RV_EX_CODEPAGE, 1252);

	for (i = 0; i < CODEPAGE_MAX; i++)
	{
		if (g_tCodePages[i] == dwValue)
		{
			m_nCodePage	= i;
		}
	}

	if (m_nCodePage < 0)
	{
		m_nCodePage	= 0;
	}

	reg.Close();
}

void CZIPPage::SaveSettings()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_ZIP);
	reg.WriteDWORD(RV_EX_COMPRESSLEVEL, g_tZipLevels[m_nLevel]);
	reg.WriteDWORD(RV_EX_CODEPAGESET, m_nCodePageSet);
	reg.WriteDWORD(RV_EX_CODEPAGE, g_tCodePages[m_nCodePage]);
	reg.Close();
}

void CZIPPage::UpdateControls()
{
	m_ctlCodePage.EnableWindow((BOOL)(m_nCodePageSet == 1));
}

BOOL CZIPPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlLevelH);
	SetHeaderItem(&m_ctlCodePageH);
	SetMenus();
	LoadSettings();
	UpdateData(FALSE);
	UpdateControls();
	return TRUE;
}

void CZIPPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CZIPPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlLevelH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlLevelH.MoveWindow(&rc);

	m_ctlLevel.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlLevel.MoveWindow(&rc);

	m_ctlCodePageH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlCodePageH.MoveWindow(&rc);

	m_ctlCodePage.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlCodePage.MoveWindow(&rc);
}

void CZIPPage::OnPaint()
{
	CPaintDC	dc(this);
	DrawHeaderItem(dc, &m_ctlLevelH);
	DrawHeaderItem(dc, &m_ctlCodePageH);
	CPropertyPageX::OnPaint();
}

void CZIPPage::OnBnClickedZpSystemdefault()
{
	UpdateData(TRUE);
	UpdateControls();
}

void CZIPPage::OnBnClickedZpSpecificCpH()
{
	UpdateData(TRUE);
	UpdateControls();
}
