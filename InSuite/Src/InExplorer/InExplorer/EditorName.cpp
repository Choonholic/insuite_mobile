// EditorName.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "DisplayX.h"
#include "EditorName.h"

IMPLEMENT_DYNAMIC(CEditorName, COptionsDialogX)

CEditorName::CEditorName(CWnd* pParent /*=NULL*/) : COptionsDialogX(CEditorName::IDD, pParent)
{
}

CEditorName::~CEditorName()
{
}

void CEditorName::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ENM_NAME_H, m_ctlNameH);
	DDX_Control(pDX, IDC_ENM_NAME, m_ctlName);
	DDX_Text(pDX, IDC_ENM_NAME, m_strName);
}

BEGIN_MESSAGE_MAP(CEditorName, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CEditorName::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MODIFY);

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

	UpdateData(FALSE);
	return TRUE;
}

void CEditorName::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlNameH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlNameH.MoveWindow(&rc);

	m_ctlName.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlName.MoveWindow(&rc);
}
