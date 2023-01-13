// FindDialog.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "FindDialog.h"

IMPLEMENT_DYNAMIC(CFindDialog, COptionsDialogX)

CFindDialog::CFindDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CFindDialog::IDD, pParent), m_strFindWhat(_T("")), m_bKeys(TRUE), m_bValues(TRUE), m_bData(TRUE), m_bMatchOnly(FALSE), m_bCaseSensitive(FALSE)
{
}

CFindDialog::~CFindDialog()
{
}

void CFindDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIND_WHAT, m_ctlFindWhat);
	DDX_Control(pDX, IDC_FIND_LOOK_AT, m_ctlLookAt);
	DDX_Control(pDX, IDC_FIND_KEYS, m_ctlKeys);
	DDX_Control(pDX, IDC_FIND_VALUES, m_ctlValues);
	DDX_Control(pDX, IDC_FIND_DATA, m_ctlData);
	DDX_Text(pDX, IDC_FIND_WHAT, m_strFindWhat);
	DDX_Check(pDX, IDC_FIND_KEYS, m_bKeys);
	DDX_Check(pDX, IDC_FIND_VALUES, m_bValues);
	DDX_Check(pDX, IDC_FIND_DATA, m_bData);
	DDX_Check(pDX, IDC_FIND_MATCH, m_bMatchOnly);
	DDX_Check(pDX, IDC_FIND_CASE_SENSITIVE, m_bCaseSensitive);
}

BEGIN_MESSAGE_MAP(CFindDialog, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CFindDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

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
	return TRUE;
}

void CFindDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;
	int		nWidth;

	m_ctlLookAt.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;
	nWidth		= ((rc.right - rc.left - DRA::SCALEX(16)) / 3);

	m_ctlLookAt.MoveWindow(&rc);

	m_ctlFindWhat.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlFindWhat.MoveWindow(&rc);

	m_ctlKeys.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.left		= nLeft + (nWidth * 0) + DRA::SCALEX(8);
	rc.right	= rc.left + nWidth; 

	m_ctlKeys.MoveWindow(&rc);

	m_ctlValues.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.left		= nLeft + (nWidth * 1) + DRA::SCALEX(8);
	rc.right	= rc.left + nWidth; 

	m_ctlValues.MoveWindow(&rc);

	m_ctlData.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.left		= nLeft + (nWidth * 2) + DRA::SCALEX(8);
	rc.right	= rc.left + nWidth; 

	m_ctlData.MoveWindow(&rc);
}
