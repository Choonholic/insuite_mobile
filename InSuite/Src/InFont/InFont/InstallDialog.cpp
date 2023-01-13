// InstallDialog.cpp

#include "stdafx.h"
#include "InFont.h"
#include "DisplayX.h"
#include "InstallDialog.h"

IMPLEMENT_DYNAMIC(CInstallDialog, COptionsDialogX)

CInstallDialog::CInstallDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CInstallDialog::IDD, pParent), m_strFileName(_T("")), m_strFontFace(_T("")), m_strVersion(_T("")), m_strCopyright(_T(""))
{
}

CInstallDialog::~CInstallDialog()
{
}

void CInstallDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IN_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_IN_FONTFACE, m_strFontFace);
	DDX_Text(pDX, IDC_IN_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_IN_COPYRIGHT, m_strCopyright);
	DDX_Control(pDX, IDC_IN_FILENAME, m_ctlFileName);
	DDX_Control(pDX, IDC_IN_FONTFACE, m_ctlFontFace);
	DDX_Control(pDX, IDC_IN_VERSION, m_ctlVersion);
	DDX_Control(pDX, IDC_IN_COPYRIGHT, m_ctlCopyright);
}

BEGIN_MESSAGE_MAP(CInstallDialog, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CInstallDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();
	m_strCopyright.Replace(_T("&"), _T("&&"));
	UpdateData(FALSE);

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_INSTALL);

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

void CInstallDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlCopyright.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlCopyright.MoveWindow(&rc);

	m_ctlFileName.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlFileName.MoveWindow(&rc);

	m_ctlFontFace.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlFontFace.MoveWindow(&rc);

	m_ctlVersion.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlVersion.MoveWindow(&rc);
}
