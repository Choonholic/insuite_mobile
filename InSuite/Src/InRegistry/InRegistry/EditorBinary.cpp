// EditorBinary.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "DisplayX.h"
#include "EditorBinary.h"

IMPLEMENT_DYNAMIC(CEditorBinary, COptionsDialogX)

CEditorBinary::CEditorBinary(CWnd* pParent /*=NULL*/) : COptionsDialogX(CEditorBinary::IDD, pParent)
{
	memset(m_cBuffer, 0, 4096 * sizeof(BYTE));

	m_cbSize	= 0;
}

CEditorBinary::~CEditorBinary()
{
}

void CEditorBinary::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EBI_DATA_H, m_ctlDataH);
	DDX_Control(pDX, IDC_EBI_DATA, m_ctlData);
}

BEGIN_MESSAGE_MAP(CEditorBinary, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CEditorBinary::OnInitDialog()
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

	m_ctlData.SetData(m_cBuffer, m_cbSize);
	UpdateData(FALSE);
	return TRUE;
}

void CEditorBinary::OnSize(UINT nType, int cx, int cy)
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

void CEditorBinary::OnOK()
{
	m_cbSize	= 4096;

	m_ctlData.GetData(m_cBuffer, m_cbSize);
	COptionsDialogX::OnOK();
}
