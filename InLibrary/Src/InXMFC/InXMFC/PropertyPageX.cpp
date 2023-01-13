// PropertyPageX.cpp

#include "stdafx.h"
#include "PropertyPageX.h"
#include "ColorX.h"
#include "GraphicsX.h"
#include "PropertySheetX.h"
#include "FontX.h"

IMPLEMENT_DYNCREATE(CPropertyPageX, CPropertyPage)

CPropertyPageX::CPropertyPageX() : CPropertyPage()
{
	LOGFONT	lfFont;

	GetTaskBarFont(&lfFont);
	m_fnHeader.CreateFontIndirect(&lfFont);
}

CPropertyPageX::CPropertyPageX(UINT nIDTemplate, UINT nIDCaption) : CPropertyPage(nIDTemplate, nIDCaption)
{
	LOGFONT	lfFont;

	GetTaskBarFont(&lfFont);
	m_fnHeader.CreateFontIndirect(&lfFont);
}

CPropertyPageX::CPropertyPageX(LPCTSTR lpszTemplateName, UINT nIDCaption) : CPropertyPage(lpszTemplateName, nIDCaption)
{
	LOGFONT	lfFont;

	GetTaskBarFont(&lfFont);
	m_fnHeader.CreateFontIndirect(&lfFont);
}

CPropertyPageX::~CPropertyPageX()
{
}

void CPropertyPageX::SetHeaderItem(CWnd* pWnd)
{
	if (pWnd != NULL)
	{
		pWnd->SetFont(&m_fnHeader);
	}
}

void CPropertyPageX::SetHeaderItem(UINT nID)
{
	CWnd*	pWnd	= GetDlgItem(nID);

	SetHeaderItem(pWnd);
}

void CPropertyPageX::DrawHeaderItem(CDC& dc, CWnd* pWnd)
{
	if (pWnd != NULL)
	{
		CRect	rc;
		CRect	rcLine;

		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		rcLine.SetRect(rc.left, rc.bottom + DRA::SCALEY(2), rc.right, rc.bottom + DRA::SCALEY(3));
		FillHGRectEx(dc.GetSafeHdc(), rcLine, GetUserShellColor(SHCOLOR_TASKBAR_LEFTEDGE), RGB(0xF0, 0xF0, 0xF0));
	}
}

void CPropertyPageX::DrawHeaderItem(CDC& dc, UINT nID)
{
	CWnd*	pWnd	= GetDlgItem(nID);

	DrawHeaderItem(dc, pWnd);
}

void CPropertyPageX::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPropertyPageX, CPropertyPage)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CPropertyPageX::OnPaint()
{
	CPaintDC	dc(this);
	CWnd*		pSheetWnd	= GetParent();

	::PostMessage(pSheetWnd->GetSafeHwnd(), WM_PAINT, 0, 0);
}

void CPropertyPageX::OnOK()
{
	CPropertySheetX*	pSheet	= DYNAMIC_DOWNCAST(CPropertySheetX, GetParent());

	if (pSheet != NULL)
	{
		pSheet->EndDialog(IDOK);
	}

	CPropertyPage::OnOK();
}

void CPropertyPageX::OnCancel()
{
	CPropertySheetX*	pSheet	= DYNAMIC_DOWNCAST(CPropertySheetX, GetParent());

	if (pSheet != NULL)
	{
		pSheet->EndDialog(IDCANCEL);
	}

	CPropertyPage::OnCancel();
}
