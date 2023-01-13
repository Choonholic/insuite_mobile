// FindDialog.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "FindDialog.h"
#include "FindDateDialog.h"
#include "FindSizeDialog.h"
#include "FolderDialogX.h"
#include "RegistryMFCX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CFindDialog, COptionsDialogX)

CFindDialog::CFindDialog(CWnd* pParent) : COptionsDialogX(CFindDialog::IDD, pParent), m_strFilter(_T("*.*")), m_strFrom(_T("")), m_bSubFolders(TRUE), m_bFrom(FALSE), m_bTo(FALSE), m_bLarger(FALSE), m_dwLarger(0), m_bSmaller(FALSE), m_dwSmaller(0x40000000)
{
}

CFindDialog::~CFindDialog()
{
}

void CFindDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIND_FILTER_H, m_ctlFilterH);
	DDX_Control(pDX, IDC_FIND_FILTER, m_ctlFilter);
	DDX_Control(pDX, IDC_FIND_FROM_H, m_ctlFromH);
	DDX_Control(pDX, IDC_FIND_FROM, m_ctlFrom);
	DDX_Control(pDX, IDC_FIND_BROWSE, m_ctlBrowse);
	DDX_Control(pDX, IDC_FIND_SUBFOLDERS, m_ctlSubFolders);
	DDX_Control(pDX, IDC_FIND_DATE, m_ctlDate);
	DDX_Control(pDX, IDC_FIND_SIZE, m_ctlSize);
	DDX_Text(pDX, IDC_FIND_FILTER, m_strFilter);
	DDX_Text(pDX, IDC_FIND_FROM, m_strFrom);
	DDX_Check(pDX, IDC_FIND_SUBFOLDERS, m_bSubFolders);
}

BEGIN_MESSAGE_MAP(CFindDialog, COptionsDialogX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FIND_BROWSE, &CFindDialog::OnBnClickedFindBrowse)
	ON_BN_CLICKED(IDC_FIND_DATE, &CFindDialog::OnBnClickedFindDate)
	ON_BN_CLICKED(IDC_FIND_SIZE, &CFindDialog::OnBnClickedFindSize)
END_MESSAGE_MAP()

BOOL CFindDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CRegistryMFCX	reg;
	TCHAR			szFrom[MAX_PATH + 1];

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_FIND_FIND_ALL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	GetSpecialDirectoryEx(CSIDL_PERSONAL, szFrom, MAX_PATH + 1);
	reg.Open(HKEY_CURRENT_USER, RK_EX_FIND);

	m_strFilter	= reg.ReadString(RV_EX_FILTER, _T("*.*"));
	m_strFrom	= reg.ReadString(RV_EX_FROM, szFrom);

	reg.Close();
	UpdateData(FALSE);
	return TRUE;
}

void CFindDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	RECT	rcButton;
	int		nWidth;
	int		nLeft;

	m_ctlFilterH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlFilter.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlFilter.MoveWindow(&rc);

	m_ctlBrowse.GetWindowRect(&rcButton);
	ScreenToClient(&rcButton);

	nWidth			= rcButton.right - rcButton.left;
	rcButton.right	= cx - nLeft;
	rcButton.left	= rcButton.right - nWidth;

	m_ctlBrowse.MoveWindow(&rcButton);

	m_ctlFrom.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= rcButton.left - nLeft;

	m_ctlFrom.MoveWindow(&rc);

	m_ctlSize.GetWindowRect(&rcButton);
	ScreenToClient(&rcButton);

	nWidth			= rcButton.right - rcButton.left;
	rcButton.right	= cx - nLeft;
	rcButton.left	= rcButton.right - nWidth;

	m_ctlSize.MoveWindow(&rcButton);

	m_ctlDate.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= rcButton.left - nLeft;
	rc.left		= rc.right - nWidth;

	m_ctlDate.MoveWindow(&rc);
}

void CFindDialog::OnBnClickedFindBrowse()
{
	CFolderDialogX	dFD;

	UpdateData(TRUE);
	dFD.SetDefaultPath(m_strFrom);

	if (dFD.DoModal() == IDOK)
	{
		m_strFrom	= dFD.GetFullPath();

		UpdateData(FALSE);
	}
}

void CFindDialog::OnBnClickedFindDate()
{
	CFindDateDialog	dDate;

	dDate.m_bFrom		= m_bFrom;
	dDate.m_bTo			= m_bTo;

	memcpy(&dDate.m_stFrom, &m_stFrom, sizeof(SYSTEMTIME));
	memcpy(&dDate.m_stTo, &m_stTo, sizeof(SYSTEMTIME));
	dDate.SetHeader(IDS_FIND_TITLE);

	if (dDate.DoModal() == IDOK)
	{
		m_bFrom	= dDate.m_bFrom;
		m_bTo	= dDate.m_bTo;

		memcpy(&m_stFrom, &dDate.m_stFrom, sizeof(SYSTEMTIME));
		memcpy(&m_stTo, &dDate.m_stTo, sizeof(SYSTEMTIME));
	}
}

void CFindDialog::OnBnClickedFindSize()
{
	CFindSizeDialog	dSize;

	dSize.m_bLarger		= m_bLarger;
	dSize.m_bSmaller	= m_bSmaller;
	dSize.m_dwLarger	= m_dwLarger;
	dSize.m_dwSmaller	= m_dwSmaller;

	dSize.SetHeader(IDS_FIND_TITLE);

	if (dSize.DoModal() == IDOK)
	{
		m_bLarger	= dSize.m_bLarger;
		m_bSmaller	= dSize.m_bSmaller;
		m_dwLarger	= dSize.m_dwLarger;
		m_dwSmaller	= dSize.m_dwSmaller;
	}
}

void CFindDialog::OnOK()
{
	if (m_strFilter.IsEmpty())
	{
		COptionsDialogX::OnCancel();
	}
	else
	{
		CRegistryMFCX	reg;

		reg.Open(HKEY_CURRENT_USER, RK_EX_FIND);
		reg.WriteString(RV_EX_FILTER, m_strFilter);
		reg.WriteString(RV_EX_FROM, m_strFrom);
		reg.Close();
		COptionsDialogX::OnOK();
	}
}
