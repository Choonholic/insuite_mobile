// OptionsDialogX.cpp

#include "stdafx.h"
#include "OptionsDialogX.h"
#include "ColorX.h"
#include "FontX.h"
#include "GraphicsX.h"

#define HEADER_LINE	DRA::SCALEY(23)
#define LINE_COLOR	RGB(0x00, 0x00, 0x00)

IMPLEMENT_DYNAMIC(COptionsDialogX, CDialogX)

COptionsDialogX::COptionsDialogX() : CDialogX()
{
}

COptionsDialogX::COptionsDialogX(UINT nIDTemplate, CWnd* pParent) : CDialogX(nIDTemplate, pParent)
{
}

COptionsDialogX::COptionsDialogX(LPCTSTR lpszTemplateName, CWnd* pParent) : CDialogX(lpszTemplateName, pParent)
{
}

COptionsDialogX::~COptionsDialogX()
{
}

void COptionsDialogX::DoDataExchange(CDataExchange* pDX)
{
	CDialogX::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COptionsDialogX, CDialogX)
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void COptionsDialogX::SetHeader(UINT nID)
{
	m_strHeader.LoadString(nID);
}

void COptionsDialogX::SetHeader(LPCTSTR lpszHeader)
{
	m_strHeader	= lpszHeader;
}

LPCTSTR COptionsDialogX::GetHeader()
{
	return (LPCTSTR)m_strHeader;
}

int COptionsDialogX::GetHeaderHeight()
{
	return (HEADER_LINE + DRA::SCALEY(1));
}

void COptionsDialogX::DrawDialogTitle(HDC hDC)
{
	RECT		rect, rectText;
	COLORREF	crOld;

	::GetClientRect(m_hWnd, &rect);

	if (!m_strHeader.IsEmpty())
	{
		HFONT	hOldFont	= (HFONT)SelectObject(hDC, m_hHeaderFont);

		crOld	= SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHT));

		rectText.left		= DRA::SCALEX(8);
		rectText.top		= DRA::SCALEY(0);
		rectText.right		= rect.right - rect.left;
		rectText.bottom		= m_dwHeaderLine;

		DrawTextEx(hDC, m_strHeader, -1, &rectText, DT_VCENTER | DT_DISABLE_UNDERLINE);
		SetTextColor(hDC, crOld);
		SelectObject(hDC, hOldFont);
	}

	FillSolidRectEx(hDC, rect.left, rect.top + m_dwHeaderLine, rect.right, GetSystemMetrics(SM_CYBORDER), LINE_COLOR);
}

void COptionsDialogX::GetHeaderFont()
{
	LOGFONT	lfHeader;

	GetTaskBarFont(&lfHeader);

	m_hHeaderFont		= CreateFontIndirect(&lfHeader);
	m_dwHeaderLine		= HEADER_LINE;
}

BOOL COptionsDialogX::OnInitDialog()
{
	CDialogX::OnInitDialog();
	GetHeaderFont();
	return TRUE;
}

void COptionsDialogX::OnDestroy()
{
	CDialogX::OnDestroy();
	ReleaseFont(m_hHeaderFont);
}

void COptionsDialogX::OnPaint()
{
	CPaintDC dc(this);
	DrawDialogTitle(dc.m_hDC);
}
