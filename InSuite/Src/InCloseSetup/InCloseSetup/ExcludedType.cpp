// ExcludedType.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "ExcludedType.h"
#include "DisplayX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CExcludedType, COptionsDialogX)

CExcludedType::CExcludedType(CWnd* pParent /*=NULL*/) : COptionsDialogX(CExcludedType::IDD, pParent), m_bType(FALSE), m_bAlways(FALSE)
{
}

CExcludedType::~CExcludedType()
{
}

void CExcludedType::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_EX_MAINFRAME, m_bType);
	DDX_Check(pDX, IDC_EX_ALWAYS, m_bAlways);
	DDX_Control(pDX, IDC_EX_APPTYPE_H, m_ctlAppTypeH);
	DDX_Control(pDX, IDC_EX_ADDOPTIONS_H, m_ctlAddOptionsH);
}

BEGIN_MESSAGE_MAP(CExcludedType, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CExcludedType::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();
	SetHeaderItem(&m_ctlAppTypeH);
	SetHeaderItem(&m_ctlAddOptionsH);

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
	return TRUE;
}

void CExcludedType::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlAppTypeH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;

	m_ctlAppTypeH.MoveWindow(&rc);

	m_ctlAddOptionsH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlAddOptionsH.MoveWindow(&rc);
}

void CExcludedType::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlAppTypeH);
	DrawHeaderItem(dc, &m_ctlAddOptionsH);
	COptionsDialogX::OnPaint();
}
