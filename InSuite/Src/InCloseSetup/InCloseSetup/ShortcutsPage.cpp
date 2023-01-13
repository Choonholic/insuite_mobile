// ShortcutsPage.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "InSuite_Settings.h"
#include "InClose_Settings.h"
#include "ShortcutsPage.h"
#include "RegistryMFCX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CShortcutsPage, CPropertyPageX)

#define SC_MAXSHORTCUTS	10

SHORTCUTINFO	tShortcuts[SC_MAXSHORTCUTS]	=
{
	{ _T("-A"),	IDS_CLOSEALL,			133	},
	{ _T("-B"),	IDS_CLOSEALLBUTACTIVE,	134	},
	{ _T("-C"),	IDS_CLOSE,				129	},
	{ _T("-F"),	IDS_SCREENOFF,			139	},
	{ _T("-I"),	IDS_INTELLICLOSE,		131	},
	{ _T("-M"),	IDS_MINIMIZE,			132	},
	{ _T("-P"),	IDS_SUSPEND,			140	},
	{ _T("-R"),	IDS_RESET,				136	},
	{ _T("-S"),	IDS_ROTATE,				137	},
	{ _T("-T"),	IDS_TODAY,				138	}
};

CShortcutsPage::CShortcutsPage() : CPropertyPageX(CShortcutsPage::IDD)
{
}

CShortcutsPage::~CShortcutsPage()
{
}

void CShortcutsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SC_COMMAND_H, m_ctlCommandH);
	DDX_Control(pDX, IDC_SC_COMMAND, m_ctlCommand);
	DDX_Control(pDX, IDC_SC_CREATE, m_ctlCreate);
}

BEGIN_MESSAGE_MAP(CShortcutsPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SC_CREATE, &CShortcutsPage::OnBnClickedScCreate)
END_MESSAGE_MAP()

void CShortcutsPage::SetItems()
{
	CString	strItem;

	for (int i = 0; i < SC_MAXSHORTCUTS; i++)
	{
		strItem.LoadString(tShortcuts[i].dwStringID);
		m_ctlCommand.AddString(strItem);
	}
}

BOOL CShortcutsPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetItems();
	m_ctlCommand.SetCurSel(0);
	UpdateData(FALSE);
	return TRUE;
}

void CShortcutsPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;
	SIZE	size;

	m_ctlCommandH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlCommandH.MoveWindow(&rc);

	m_ctlCommand.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;
	size.cy		= rc.left;

	m_ctlCommand.MoveWindow(&rc);

	m_ctlCreate.GetWindowRect(&rc);
	ScreenToClient(&rc);

	size.cx		= rc.right - rc.left;
	rc.right	= cx - size.cy;
	rc.left		= rc.right - size.cx;

	m_ctlCreate.MoveWindow(&rc);
}

void CShortcutsPage::OnBnClickedScCreate()
{
	int				nItem;
	CString			strPath;
	CString			strTitle;
	CString			strMessage;
	TCHAR			szPrograms[MAX_PATH + 1];
	TCHAR			szModule[MAX_PATH + 1];
	TCHAR			szResource[MAX_PATH + 1];
	TCHAR			szShortcut[MAX_PATH + 1];
	FILESTATUSX		fsx;
	CRegistryMFCX	reg;

	UpdateData(TRUE);

	nItem	= m_ctlCommand.GetCurSel();

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	strPath	= reg.ReadString(RV_IS_FOLDER);

	reg.Close();

	GetSpecialDirectoryEx(CSIDL_PROGRAMS, szPrograms, MAX_PATH + 1);
	ExtendSubPath(szPrograms, MAX_PATH + 1, NM_IS_PROGRAMS);
	CombineFileName(szModule, MAX_PATH + 1, strPath, MD_CL_MAIN);
	CombineFileName(szResource, MAX_PATH + 1, strPath, MD_CL_MAINLX);

	if (!GetFileStatusX(szResource, fsx))
	{
		CombineFileName(szResource, MAX_PATH + 1, strPath, MD_CL_MAIN);
	}

	strPath.Format(_T("\"%s\" %s?%s,-%ld"), szModule, tShortcuts[nItem].szOption, szResource, tShortcuts[nItem].dwIconID);
	_tcscpy_s(szModule, MAX_PATH + 1, strPath);
	strPath.LoadString(tShortcuts[nItem].dwStringID);

	strPath	+= _T(".lnk");

	CombineFileName(szShortcut, MAX_PATH + 1, szPrograms, strPath);
	DeleteFile(szShortcut);
	SHCreateShortcut(szShortcut, szModule);
	strTitle.LoadString(IDS_INCLOSE);
	strPath.LoadString(tShortcuts[nItem].dwStringID);
	strMessage.Format(IDS_SC_SUCCEEDED, strPath);
	MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
}
