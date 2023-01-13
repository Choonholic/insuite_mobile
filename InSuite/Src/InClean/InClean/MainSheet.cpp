// MainSheet.cpp

#include "stdafx.h"
#include "InClean.h"
#include "MainSheet.h"

IMPLEMENT_DYNAMIC(CMainSheet, CPropertySheetX)

CMainSheet::CMainSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheetX(nIDCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_dFiles);
	AddPage(&m_dRegistry);
	AddPage(&m_dAbout);
}

CMainSheet::CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheetX(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_dFiles);
	AddPage(&m_dRegistry);
	AddPage(&m_dAbout);
}

CMainSheet::~CMainSheet()
{
}

BEGIN_MESSAGE_MAP(CMainSheet, CPropertySheetX)
	ON_COMMAND(ID_SCAN, &CMainSheet::OnScan)
END_MESSAGE_MAP()

BOOL CMainSheet::OnInitDialog()
{
	BOOL			bResult		= CPropertySheetX::OnInitDialog();
	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_SCAN);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= ID_SCAN;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_CLOSE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);
	return bResult;
}

void CMainSheet::OnScan()
{
	switch (GetActiveIndex())
	{
	case 0:
		{
			m_dFiles.Scan();
		}
		break;
	case 1:
		{
			m_dRegistry.Scan();
		}
		break;
	}
}
