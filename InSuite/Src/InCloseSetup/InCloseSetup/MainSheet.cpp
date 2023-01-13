// MainSheet.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "MainSheet.h"

IMPLEMENT_DYNAMIC(CMainSheet, CPropertySheetX)

CMainSheet::CMainSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheetX(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_dGeneral);
	AddPage(&m_dScreen);
	AddPage(&m_dActions);
	AddPage(&m_dBackground);
	AddPage(&m_dMenu);
	AddPage(&m_dExcluded);
	AddPage(&m_dShortcuts);
	AddPage(&m_dAbout);
}

CMainSheet::CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheetX(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_dGeneral);
	AddPage(&m_dScreen);
	AddPage(&m_dActions);
	AddPage(&m_dBackground);
	AddPage(&m_dMenu);
	AddPage(&m_dExcluded);
	AddPage(&m_dShortcuts);
	AddPage(&m_dAbout);
}

CMainSheet::~CMainSheet()
{
}

BEGIN_MESSAGE_MAP(CMainSheet, CPropertySheetX)
END_MESSAGE_MAP()

BOOL CMainSheet::OnInitDialog()
{
	BOOL			bResult		= CPropertySheetX::OnInitDialog();
	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_CLOSE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);
	return bResult;
}
