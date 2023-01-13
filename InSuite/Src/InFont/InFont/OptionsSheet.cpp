// OptionsSheet.cpp

#include "stdafx.h"
#include "InFont.h"
#include "OptionsSheet.h"

IMPLEMENT_DYNAMIC(COptionsSheet, CPropertySheetX)

COptionsSheet::COptionsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheetX(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_dGeneralPage);
	AddPage(&m_dAntialiasingPage);
	AddPage(&m_dFolderPage);
}

COptionsSheet::COptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheetX(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_dGeneralPage);
	AddPage(&m_dAntialiasingPage);
	AddPage(&m_dFolderPage);
}

COptionsSheet::~COptionsSheet()
{
}

BEGIN_MESSAGE_MAP(COptionsSheet, CPropertySheetX)
END_MESSAGE_MAP()

BOOL COptionsSheet::OnInitDialog()
{
	BOOL			bResult		= CPropertySheetX::OnInitDialog();
	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_CLOSE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);
	return bResult;
}
