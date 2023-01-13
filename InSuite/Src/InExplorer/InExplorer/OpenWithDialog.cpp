// OpenWithDialog.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "OpenWithDialog.h"
#include "DisplayX.h"
#include "ShellX.h"
#include "FileDialogX.h"

int	CALLBACK	SortShortcuts(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

IMPLEMENT_DYNAMIC(COpenWithDialog, CDialogX)

COpenWithDialog::COpenWithDialog(CWnd* pParent /*=NULL*/) : CDialogX(COpenWithDialog::IDD, pParent)
{
}

COpenWithDialog::~COpenWithDialog()
{
}

void COpenWithDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogX::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OPEN_FILENAME, m_strFileName);
	DDX_Control(pDX, IDC_OPEN_DESC, m_ctlDescription);
	DDX_Control(pDX, IDC_OPEN_FILENAME, m_ctlFileName);
	DDX_Control(pDX, IDC_OPEN_PROGRAMS, m_ctlPrograms);
	DDX_Control(pDX, IDC_OPEN_BROWSE, m_ctlBrowse);
}

BEGIN_MESSAGE_MAP(COpenWithDialog, CDialogX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_OPEN_PROGRAMS, &COpenWithDialog::OnLvnItemchangedOpenPrograms)
	ON_BN_CLICKED(IDC_OPEN_BROWSE, &COpenWithDialog::OnBnClickedOpenBrowse)
END_MESSAGE_MAP()

void COpenWithDialog::SetHeaders()
{
	CString	strItem;

	strItem.LoadString(IDS_HEADER_NAME);
	m_ctlPrograms.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(200), 0);
}

void COpenWithDialog::SetItems()
{
	LVITEM	lvi;
	DWORD	dwStyle;
	CString	strTitle;

	BeginWaitCursor();

	dwStyle	= m_ctlPrograms.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT);
	dwStyle	|= LVS_EX_FULLROWSELECT;

	m_ctlPrograms.SetExtendedStyle(dwStyle);
	m_ctlPrograms.SetRedraw(FALSE);

	if (m_ctlPrograms.GetItemCount() > 0)
	{
		m_ctlPrograms.DeleteAllItems();
	}

	for (int i = 0; i < m_arrFiles.GetCount(); i++)
	{
		TakeTitle(m_arrFiles[i], strTitle.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
		strTitle.ReleaseBuffer();

		lvi.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.lParam		= i;
		lvi.iImage		= GetSystemIconIndex(m_arrFiles[i]);
		lvi.pszText		= strTitle.GetBuffer(strTitle.GetLength());

		m_ctlPrograms.InsertItem(&lvi);
		strTitle.ReleaseBuffer();
	}

	m_ctlPrograms.SortItems(SortShortcuts, (DWORD)&m_arrFiles);
	m_ctlPrograms.SetAutoColumnWidth();
	m_ctlPrograms.SetRedraw(TRUE);
	EndWaitCursor();
}

BOOL COpenWithDialog::OnInitDialog()
{
	CDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_OPEN);

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

	TakeFileName(m_strFile, m_strFileName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	m_strFileName.ReleaseBuffer();

	HIMAGELIST	hSysIcons	= GetSystemIconList(ICON_SMALL);

	m_iSysSmall.Attach(hSysIcons);
	m_ctlPrograms.SetImageList(&m_iSysSmall, LVSIL_SMALL);
	m_iSysSmall.Detach();
	SetHeaders();
	SetItems();
	UpdateData(FALSE);
	return TRUE;
}

void COpenWithDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	RECT	rcBrowse;
	SIZE	size;
	int		nLeft;

	m_ctlPrograms.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlDescription.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlDescription.MoveWindow(&rc);

	m_ctlFileName.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlFileName.MoveWindow(&rc);

	m_ctlBrowse.GetWindowRect(&rcBrowse);
	ScreenToClient(&rcBrowse);

	size.cx			= rcBrowse.right - rcBrowse.left;
	size.cy			= rcBrowse.bottom - rcBrowse.top;
	rcBrowse.right	= cx - nLeft;
	rcBrowse.left	= rcBrowse.right - size.cx;
	rcBrowse.bottom	= cy - nLeft;
	rcBrowse.top	= rcBrowse.bottom - size.cy;

	m_ctlBrowse.MoveWindow(&rcBrowse);

	m_ctlPrograms.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;
	rc.bottom	= rcBrowse.top - nLeft;

	m_ctlPrograms.MoveWindow(&rc);
	m_ctlPrograms.SetAutoColumnWidth();
}

void COpenWithDialog::OnPaint()
{
	CPaintDC	dc(this);
	RECT		rc;
	int			nTop;
	int			nBottom;
	HICON		hIcon	= GetFileIcon(m_strFile, ICON_BIG);

	m_ctlDescription.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nTop	= rc.top;

	m_ctlFileName.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nBottom	= rc.bottom - GetSystemMetrics(SM_CYICON);

	dc.DrawIcon(DRA::SCALEX(4), nTop + ((nBottom - nTop) / 2), hIcon);
	DestroyIcon(hIcon);
}

void COpenWithDialog::OnLvnItemchangedOpenPrograms(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW	pNMLV	= reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uNewState & LVNI_SELECTED)
	{
		ParseShortcutTarget(m_arrFiles[pNMLV->lParam], m_strTarget.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
		m_strTarget.ReleaseBuffer();
	}

	*pResult	= 0;
}

void COpenWithDialog::OnBnClickedOpenBrowse()
{
	CFileDialogX	dFD;

	dFD.SetHeader(IDS_FILE_OPENFILEWITH);
	dFD.SetFilters(_T("*.exe"));

	if (dFD.DoModal() == IDOK)
	{
		m_strTarget	= dFD.GetFullPath();

		OnOK();
	}
}

void COpenWithDialog::OnOK()
{
	UpdateData(TRUE);
	CreateProcess(m_strTarget, m_strFile, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
	CDialogX::OnOK();
}

static int SortShortcuts(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CStringArray*	pInfo	= (CStringArray*)lParamSort;
	TCHAR			szTitle1[MAX_PATH + 1];
	TCHAR			szTitle2[MAX_PATH + 1];

	TakeTitle(pInfo->GetAt(lParam1), szTitle1, MAX_PATH + 1);
	TakeTitle(pInfo->GetAt(lParam2), szTitle2, MAX_PATH + 1);
	return _tcsicmp(szTitle1, szTitle2);
}
