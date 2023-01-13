// EditorSZ.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "DisplayX.h"
#include "EditorSZ.h"

IMPLEMENT_DYNAMIC(CEditorSZ, COptionsDialogX)

CEditorSZ::CEditorSZ(CWnd* pParent /*=NULL*/) : COptionsDialogX(CEditorSZ::IDD, pParent)
{
}

CEditorSZ::~CEditorSZ()
{
}

void CEditorSZ::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ESZ_DATA_H, m_ctlDataH);
	DDX_Control(pDX, IDC_ESZ_DATA, m_ctlData);
	DDX_Text(pDX, IDC_ESZ_DATA, m_strData);
}

BEGIN_MESSAGE_MAP(CEditorSZ, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CEditorSZ::OnInitDialog()
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

void CEditorSZ::OnSize(UINT nType, int cx, int cy)
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

	m_ctlData.MoveWindow(&rc);
}
