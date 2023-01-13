// PropertiesTimeDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "PropertiesTimeDialogX.h"
#include "DisplayX.h"

IMPLEMENT_DYNAMIC(CPropertiesTimeDialogX, COptionsDialogX)

CPropertiesTimeDialogX::CPropertiesTimeDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_PROPTIME_DIALOG, pParent)
{
	SetHeader(IDS_PROP_TIME);
}

CPropertiesTimeDialogX::~CPropertiesTimeDialogX()
{
}

void CPropertiesTimeDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_CREATED_H, m_ctlCreatedH);
	DDX_Control(pDX, IDC_PROP_MODIFIED_H, m_ctlModifiedH);
	DDX_Control(pDX, IDC_PROP_ACCESSED_H, m_ctlAccessedH);
	DDX_Control(pDX, IDC_PROP_CREATED, m_ctlCreated);
	DDX_Control(pDX, IDC_PROP_MODIFIED, m_ctlModified);
	DDX_Control(pDX, IDC_PROP_ACCESSED, m_ctlAccessed);
}

BEGIN_MESSAGE_MAP(CPropertiesTimeDialogX, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CPropertiesTimeDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_OKAY);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_MENU_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	m_ctlCreated.SetWindowText(m_strCreated);
	m_ctlModified.SetWindowText(m_strModified);
	m_ctlAccessed.SetWindowText(m_strAccessed);
	return TRUE;
}

void CPropertiesTimeDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlCreated.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - DRA::SCALEX(3);

	m_ctlCreated.MoveWindow(&rc);

	m_ctlModified.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - DRA::SCALEX(3);

	m_ctlModified.MoveWindow(&rc);

	m_ctlAccessed.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - DRA::SCALEX(3);

	m_ctlAccessed.MoveWindow(&rc);
}
