// ProgressCtrlX.cpp

#include "stdafx.h"
#include "ProgressCtrlX.h"
#include "ColorX.h"
#include "GraphicsX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CProgressCtrlX, CProgressCtrl)

CProgressCtrlX::CProgressCtrlX()
{
	m_hDetailsFont	= NULL;

	SetDefaultParameters();
}

CProgressCtrlX::~CProgressCtrlX()
{
	if (m_hDetailsFont != NULL)
	{
		ReleaseFont(m_hDetailsFont);
	}
}

BEGIN_MESSAGE_MAP(CProgressCtrlX, CProgressCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CProgressCtrlX::SetDefaultParameters()
{
	m_crDetails		= GetShellColor(SHCOLOR_TASKBAR_TEXT);
	m_crBack		= RGB(0xCC, 0xCC, 0xCC);
	m_crFirst		= GetShellColor(SHCOLOR_TASKBAR_LEFTEDGE);
	m_crLast		= GetShellColor(SHCOLOR_TASKBAR_RIGHTEDGE);
	m_bShowDetails	= TRUE;

	if (m_hDetailsFont != NULL)
	{
		ReleaseFont(m_hDetailsFont);
	}

	LOGFONT	lfDetails;

	GetDefaultFont(&lfDetails);

	m_hDetailsFont	= CreateFontIndirect(&lfDetails);
}

void CProgressCtrlX::SetDetailsColor(COLORREF cr)
{
	m_crDetails	= cr;
}

void CProgressCtrlX::SetBarColor(COLORREF crFirst, COLORREF crLast, COLORREF crBack)
{
	m_crBack	= crBack;
	m_crFirst	= crFirst;
	m_crLast	= crLast;
}

void CProgressCtrlX::ShowDetails(BOOL bShow)
{
	m_bShowDetails	= bShow;
}

void CProgressCtrlX::SetDetailsFont(LPCTSTR lpszFontFace, int nDoublePoint, LONG lWeight)
{
	if (m_hDetailsFont != NULL)
	{
		ReleaseFont(m_hDetailsFont);
	}

	m_hDetailsFont	= CreateFixedFont(lpszFontFace, nDoublePoint, lWeight);
}

void CProgressCtrlX::OnPaint()
{
	CPaintDC	dc(this);
	int			nLower;
	int			nUpper;

	GetRange(nLower, nUpper);

	int		nLength	= nUpper - nLower;
	int		nPos	= GetPos() - nLower;
	int		nBase;
	CRect	rc;
	CRect	rcClip;
	CRect	rcBar;
	HDC		hDC		= ::GetDC(this->m_hWnd);
	HDC		hMemDC	= CreateCompatibleDC(hDC);

	GetClientRect(&rc);

	HBITMAP	hBitmap	= CreateCompatibleBitmap(hDC, rc.Width(), rc.Height());
	HBITMAP	hOldBmp	= (HBITMAP)SelectObject(hMemDC, hBitmap);

	rcClip.SetRect(0, 0, rc.Width(), rc.Height());
	FillSolidRectEx(hMemDC, &rcClip, m_crBack);
	rcClip.DeflateRect(1, 1);
	rcBar.SetRect(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);

	if (GetStyle() & PBS_VERTICAL)
	{
		nBase		= rcBar.Height();
		rcBar.top	= rcBar.bottom - (LONG)((ULONGLONG)nBase * (ULONGLONG)nPos / (ULONGLONG)nLength);

		FillVGRectEx(hMemDC, &rcBar, m_crFirst, m_crLast);
	}
	else
	{
		nBase		= rcBar.Width();
		rcBar.right	= rcBar.left + (LONG)((ULONGLONG)nBase * (ULONGLONG)nPos / (ULONGLONG)nLength);

		FillHGRectEx(hMemDC, &rcBar, m_crFirst, m_crLast);
	}

	if (m_bShowDetails)
	{
		CString	strDetails;

		SetTextColor(hMemDC, m_crDetails);
		SetBkMode(hMemDC, TRANSPARENT);
		strDetails.Format(_T("%d%%"), (LONG)((ULONGLONG)nPos * (ULONGLONG)100 / (ULONGLONG)nLength));
		DrawTextEx(hMemDC, strDetails, -1, &rcClip, DT_VCENTER | DT_CENTER | DT_DISABLE_UNDERLINE);
	}

	BitBlt(hDC, rc.left, rc.top, rc.Width(), rc.Height(), hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	::ReleaseDC(this->m_hWnd, hDC);
}

void CProgressCtrlX::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CProgressCtrlX::OnLButtonUp(UINT nFlags, CPoint point)
{
}
