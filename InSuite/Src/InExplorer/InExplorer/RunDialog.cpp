// RunDialog.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "RunDialog.h"
#include "DisplayX.h"
#include "FileDialogX.h"

IMPLEMENT_DYNAMIC(CRunDialog, COptionsDialogX)

CRunDialog::CRunDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CRunDialog::IDD, pParent)
{
}

CRunDialog::~CRunDialog()
{
}

void CRunDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RUN_FILE_H, m_ctlFileH);
	DDX_Control(pDX, IDC_RUN_FILE, m_ctlFile);
	DDX_Control(pDX, IDC_RUN_PARAMS, m_ctlParameters);
	DDX_Control(pDX, IDC_RUN_BROWSE, m_ctlBrowse);
	DDX_Text(pDX, IDC_RUN_FILE, m_strFile);
	DDX_Text(pDX, IDC_RUN_PARAMS, m_strParameters);
}

BEGIN_MESSAGE_MAP(CRunDialog, COptionsDialogX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RUN_BROWSE, &CRunDialog::OnBnClickedRunBrowse)
END_MESSAGE_MAP()

BOOL CRunDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

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

	return TRUE;
}

void CRunDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;
	int		nWidth;

	m_ctlFileH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlBrowse.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nWidth		= rc.right - rc.left;
	rc.right	= cx - nLeft;
	rc.left		= rc.right - nWidth;

	m_ctlBrowse.MoveWindow(&rc);

	m_ctlFile.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nWidth - (nLeft * 2);

	m_ctlFile.MoveWindow(&rc);

	m_ctlParameters.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlParameters.MoveWindow(&rc);
}

void CRunDialog::OnOK()
{
	CFileStatus	status;

	UpdateData(TRUE);

	if (CFile::GetStatus(m_strFile, status))
	{
		COptionsDialogX::OnOK();
	}
}

void CRunDialog::OnBnClickedRunBrowse()
{
	CFileDialogX	dFD;

	UpdateData(TRUE);
	dFD.SetDefaultPath(m_strFile);

	if (dFD.DoModal() == IDOK)
	{
		m_strFile	= dFD.GetFullPath();

		UpdateData(FALSE);
	}
}
