// EditorDWORD.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "DisplayX.h"
#include "EditorDWORD.h"

IMPLEMENT_DYNAMIC(CEditorDWORD, COptionsDialogX)

CEditorDWORD::CEditorDWORD(CWnd* pParent /*=NULL*/) : COptionsDialogX(CEditorDWORD::IDD, pParent), m_nBase(0)
{
}

CEditorDWORD::~CEditorDWORD()
{
}

void CEditorDWORD::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDW_DATA_H, m_ctlDataH);
	DDX_Control(pDX, IDC_EDW_DATA, m_ctlData);
	DDX_Control(pDX, IDC_EDW_BASE_H, m_ctlBaseH);
	DDX_Control(pDX, IDC_EDW_BASE, m_ctlBase);
	DDX_CBIndex(pDX, IDC_EDW_BASE, m_nBase);
}

BEGIN_MESSAGE_MAP(CEditorDWORD, COptionsDialogX)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_EDW_BASE, &CEditorDWORD::OnCbnSelchangeEdwBase)
END_MESSAGE_MAP()

BOOL CEditorDWORD::OnInitDialog()
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

	CString	strItem;

	strItem.LoadString(IDS_EDITOR_DWORD_HEX);
	m_ctlBase.AddString(strItem);
	strItem.LoadString(IDS_EDITOR_DWORD_DEC);
	m_ctlBase.AddString(strItem);
	m_ctlData.SetBase((m_nBase == 0) ? BASE_HEX : BASE_DEC);
	m_ctlData.SetDWordValue(m_dwData);
	UpdateData(FALSE);
	return TRUE;
}

void CEditorDWORD::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	RECT	rcBase;
	SIZE	size;
	int		nLeft;

	m_ctlDataH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlBaseH.GetWindowRect(&rcBase);
	ScreenToClient(&rcBase);

	size.cx			= rcBase.right - rcBase.left;
	rcBase.right	= cx - nLeft;
	rcBase.left		= rcBase.right - size.cx;

	m_ctlBaseH.MoveWindow(&rcBase);

	m_ctlBase.GetWindowRect(&rc);
	ScreenToClient(&rc);

	size.cy		= rc.bottom - rc.top;
	rc.left		= rcBase.left;
	rc.right	= rcBase.right;

	m_ctlBase.MoveWindow(&rc);

	m_ctlDataH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= rcBase.left - nLeft;

	m_ctlDataH.MoveWindow(&rc);

	m_ctlData.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= rcBase.left - nLeft;
	rc.bottom	= rc.top + size.cy;

	m_ctlData.MoveWindow(&rc);
}

void CEditorDWORD::OnOK()
{
	m_dwData	= m_ctlData.GetDWordValue();

	COptionsDialogX::OnOK();
}

void CEditorDWORD::OnCbnSelchangeEdwBase()
{
	UpdateData(TRUE);
	m_ctlData.SetBase((m_nBase == 0) ? BASE_HEX : BASE_DEC);
}
