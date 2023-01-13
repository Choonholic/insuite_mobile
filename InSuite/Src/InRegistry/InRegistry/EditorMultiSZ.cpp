// EditorMultiSZ.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "DisplayX.h"
#include "EditorMultiSZ.h"

IMPLEMENT_DYNAMIC(CEditorMultiSZ, COptionsDialogX)

CEditorMultiSZ::CEditorMultiSZ(CWnd* pParent /*=NULL*/) : COptionsDialogX(CEditorMultiSZ::IDD, pParent)
{
}

CEditorMultiSZ::~CEditorMultiSZ()
{
}

void CEditorMultiSZ::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EMSZ_DATA_H, m_ctlDataH);
	DDX_Control(pDX, IDC_EMSZ_DATA, m_ctlData);
	DDX_Text(pDX, IDC_EMSZ_DATA, m_strData);
}

BEGIN_MESSAGE_MAP(CEditorMultiSZ, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CEditorMultiSZ::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_VALUE_MODIFY);

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

void CEditorMultiSZ::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlDataH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlDataH.MoveWindow(&rc);

	m_ctlData.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;
	rc.bottom	= cy - rc.left;

	m_ctlData.MoveWindow(&rc);
}
