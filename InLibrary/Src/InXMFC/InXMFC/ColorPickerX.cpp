// ColorPickerX.cpp

// Based on ColourPicker and ColourPopup, Office 97 Style Colour Picker Control
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998 C. Maunder and A. Bischofberger.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name is included. If
// the source code in  this file is used in any commercial application
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.

#include "stdafx.h"
#include "ColorPickerX.h"
#include "DisplayX.h"

IMPLEMENT_DYNCREATE(CColorPickerX, CButton)

#ifndef CPA_NORMAL
#define CPA_NORMAL		0x00
#define CPA_PUSHED		0x01
#define CPA_DISABLED	0x02
#endif

CColorPickerX::CColorPickerX()
{
	SetTextColor(GetSysColor(COLOR_BTNTEXT));
	SetBackColor(GetSysColor(COLOR_WINDOW));

	m_bTrackSelection	= FALSE;
	m_nSelectionMode	= CPX_MODE_BACK;
	m_bActive			= FALSE;
	m_strDefault		= _T("Automatic");
	m_strCancel			= _T("Cancel");
}

CColorPickerX::~CColorPickerX()
{
}

BEGIN_MESSAGE_MAP(CColorPickerX, CButton)
	ON_MESSAGE(CPN_SELENDOK, &CColorPickerX::OnSelEndOK)
	ON_MESSAGE(CPN_SELENDCANCEL, &CColorPickerX::OnSelEndCancel)
	ON_MESSAGE(CPN_SELCHANGE, &CColorPickerX::OnSelChange)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(BN_CLICKED, &CColorPickerX::OnBnClicked)
END_MESSAGE_MAP()

void AFXAPI DDX_ColorPickerX(CDataExchange *pDX, int nIDC, COLORREF &cr)
{
	HWND	hWndCtrl	= pDX->PrepareCtrl(nIDC);

	ASSERT(hWndCtrl != NULL);

	CColorPickerX*	pColorPicker	= (CColorPickerX*)CWnd::FromHandle(hWndCtrl);

	if (pDX->m_bSaveAndValidate)
	{
		cr	= pColorPicker->GetColor();
	}
	else
	{
		pColorPicker->SetColor(cr);
	}
}

COLORREF CColorPickerX::GetColor()
{
	return ((m_nSelectionMode == CPX_MODE_TEXT) ? GetTextColor() : GetBackColor());
}

void CColorPickerX::SetColor(COLORREF cr)
{
	((m_nSelectionMode == CPX_MODE_TEXT) ? SetTextColor(cr) : SetBackColor(cr));
}

void CColorPickerX::SetDefaultText(LPCTSTR szText)
{
	m_strDefault	= ((szText) ? szText : _T(""));
}

void CColorPickerX::SetCancelText(LPCTSTR szText)
{
	m_strCancel	= ((szText) ? szText : _T(""));
}

void CColorPickerX::SetTrackSelection(BOOL bTracking)
{
	m_bTrackSelection	= bTracking;
}

BOOL CColorPickerX::GetTrackSelection()
{
	return m_bTrackSelection;
}

void CColorPickerX::SetSelectionMode(UINT nMode)
{
	m_nSelectionMode	= nMode;
}

UINT CColorPickerX::GetSelectionMode()
{
	return m_nSelectionMode;
}

void CColorPickerX::SetBackColor(COLORREF cr)
{
	m_crColorBack	= cr;

	if (IsWindow(m_hWnd))
	{
		RedrawWindow();
	}
}

COLORREF CColorPickerX::GetBackColor()
{
	return m_crColorBack;
}

void CColorPickerX::SetTextColor(COLORREF cr)
{
	m_crColorText	= cr;

	if (IsWindow(m_hWnd))
	{
		RedrawWindow();
	}
}

COLORREF CColorPickerX::GetTextColor()
{
	return m_crColorText;
}

void CColorPickerX::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct);

	CDC*	pDC		= CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect	rc		= lpDrawItemStruct->rcItem;
	UINT	state	= lpDrawItemStruct->itemState;
	CString	m_strText;

	if (m_bActive)
	{
		state	|= ODS_SELECTED;
	}

	pDC->FillSolidRect(rc, GetSysColor(COLOR_WINDOWFRAME));
	rc.DeflateRect(DRA::SCALEX(1), DRA::SCALEY(1));
	pDC->FillSolidRect(rc, GetSysColor(COLOR_WINDOW));
	RecalcArrowRect();

	if (m_rcArrow.Height() > rc.Height())
	{
		m_rcArrow.bottom	= m_rcArrow.top + rc.Height();
	}

	DrawArrow(pDC, m_rcArrow, ((state & ODS_SELECTED) ? CPA_PUSHED : 0) | ((state & ODS_DISABLED) ? CPA_DISABLED : 0));

	rc.right	-= m_rcArrow.Width();

	CBrush	brush(((state & ODS_DISABLED) || m_crColorBack == CLR_DEFAULT) ? GetSysColor(COLOR_WINDOW) : m_crColorBack);
	CBrush*	pOldBrush	= (CBrush*)pDC->SelectObject(&brush);

	pDC->SelectStockObject(NULL_PEN);
	rc.DeflateRect(DRA::SCALEX(1), DRA::SCALEY(1));
	pDC->Rectangle(rc);
	pDC->SelectObject(pOldBrush);
	GetWindowText(m_strText);

	if (m_strText.GetLength())
	{
		pDC->SetBkMode(TRANSPARENT);

		if (!(state & ODS_DISABLED))
		{
			pDC->SetTextColor((m_crColorText == CLR_DEFAULT) ? GetSysColor(COLOR_BTNTEXT) : m_crColorText);
			pDC->DrawText(m_strText, rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
	}

	if (state & ODS_FOCUS)
	{
		pDC->DrawFocusRect(rc);
	}
}

LONG CColorPickerX::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
	COLORREF	crNew	= (COLORREF)wParam;
	CWnd*		pParent	= GetParent();

	m_bActive	= FALSE;

	SetColor(crNew);

	if (pParent)
	{
		pParent->SendMessage(CPN_CLOSEUP, wParam, (LPARAM)GetDlgCtrlID());
		pParent->SendMessage(CPN_SELENDOK, wParam, (LPARAM)GetDlgCtrlID());
	}

	if (crNew != GetColor())
	{
		if (pParent)
		{
			pParent->SendMessage(CPN_SELCHANGE, wParam, (LPARAM)GetDlgCtrlID());
		}
	}

	return TRUE;
}

LONG CColorPickerX::OnSelEndCancel(WPARAM wParam, LPARAM /*lParam*/)
{
	CWnd*	pParent	= GetParent();

	m_bActive	= FALSE;

	SetColor((COLORREF)wParam);

	if (pParent)
	{
		pParent->SendMessage(CPN_CLOSEUP, wParam, (LPARAM)GetDlgCtrlID());
		pParent->SendMessage(CPN_SELENDCANCEL, wParam, (LPARAM)GetDlgCtrlID());
	}

	return TRUE;
}

LONG CColorPickerX::OnSelChange(WPARAM wParam, LPARAM /*lParam*/)
{
	CWnd*	pParent	= GetParent();

	if (m_bTrackSelection)
	{
		SetColor((COLORREF)wParam);
	}

	if (pParent)
	{
		pParent->SendMessage(CPN_SELCHANGE, wParam, (LPARAM)GetDlgCtrlID());
	}

	return TRUE;
}

int CColorPickerX::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	SetWindowSize();
	return 0;
}

void CColorPickerX::OnBnClicked()
{
	if (IsWindow(m_hWndPopup.GetSafeHwnd()))
	{
		m_hWndPopup.DestroyWindow();
		OnSelEndCancel((UINT)m_crBackup, 0);
		return;
	}

	CRect	rc;
	CWnd*	pParent	= GetParent();

	m_bActive	= TRUE;
	m_crBackup	= GetColor();

	GetWindowRect(rc);
	m_hWndPopup.SetDefaultText(m_strDefault);
	m_hWndPopup.SetCancelText(m_strCancel);
	m_hWndPopup.SetParentWindow(this);
	m_hWndPopup.SetColor(m_crBackup);
	m_hWndPopup.Create(CPoint(rc.left, rc.bottom), GetColor(), this, m_strDefault, m_strCancel);

	if (pParent)
	{
		pParent->SendMessage(CPN_DROPDOWN, (LPARAM)GetColor(), (WPARAM)GetDlgCtrlID());
	}
}

void CColorPickerX::SetWindowSize()
{
	CSize	sizeArrow;
	CRect	rc;
	CWnd*	pParent	= GetParent();
	DWORD	dwDPI	= GetDPI();

	switch (dwDPI)
	{
	case DPI_96:
		{
			sizeArrow.cx	= 13;
			sizeArrow.cy	= 14;
		}
		break;
	case DPI_192:
		{
			sizeArrow.cx	= 26;
			sizeArrow.cy	= 28;
		}
		break;
	case DPI_128:
	case DPI_131:
		{
			sizeArrow.cx	= 17;
			sizeArrow.cy	= 18;
		}
		break;
	}

	GetWindowRect(rc);

	if (pParent)
	{
		pParent->ScreenToClient(rc);
	}

	int	nWidth	= max(rc.Width(), 2 * sizeArrow.cx + 2 * DRA::SCALEX(1));
	int	nHeight	= max(rc.Height(), sizeArrow.cy + 2 * DRA::SCALEY(1));

	MoveWindow(rc.left, rc.top, nWidth, nHeight, TRUE);
}

void CColorPickerX::RecalcArrowRect()
{
	CSize	sizeArrow;
	CRect	rc;
	DWORD	dwDPI	= GetDPI();

	switch (dwDPI)
	{
	case DPI_96:
		{
			sizeArrow.cx	= 13;
			sizeArrow.cy	= 14;
		}
		break;
	case DPI_192:
		{
			sizeArrow.cx	= 26;
			sizeArrow.cy	= 28;
		}
		break;
	case DPI_128:
	case DPI_131:
		{
			sizeArrow.cx	= 17;
			sizeArrow.cy	= 18;
		}
		break;
	}

	GetWindowRect(rc);
	ScreenToClient(rc);
	m_rcArrow.SetRect(rc.right - sizeArrow.cx - DRA::SCALEX(1), rc.top + DRA::SCALEY(1), rc.right - DRA::SCALEX(1), rc.bottom - DRA::SCALEY(1));
}

void CColorPickerX::DrawArrow(CDC* pDC, LPRECT lprect, int nFlags)
{
	int			nWidth;
	int			nHeight;
	int			nLeft;
	int			nTop;
	COLORREF	crArrow;
	COLORREF	crBack;
	DWORD		dwDPI	= GetDPI();

	switch (dwDPI)
	{
	case DPI_96:
		{
			nWidth	= 7;
			nHeight	= 4;
			nLeft	= 3;
		}
		break;
	case DPI_192:
		{
			nWidth	= 17;
			nHeight	= 9;
			nLeft	= 5;
		}
		break;
	case DPI_128:
	case DPI_131:
		{
			nWidth	= 9;
			nHeight	= 5;
			nLeft	= 3;
		}
		break;
	}

	switch (nFlags)
	{
	case CPA_PUSHED:
		{
			crArrow	= GetSysColor(COLOR_WINDOW);
			crBack	= GetSysColor(COLOR_BTNTEXT);
		}
		break;
	case CPA_DISABLED:
		{
			crArrow	= GetSysColor(COLOR_GRAYTEXT);
			crBack	= GetSysColor(COLOR_WINDOW);
		}
		break;
	default:
		{
			crArrow	= GetSysColor(COLOR_BTNTEXT);
			crBack	= GetSysColor(COLOR_WINDOW);
		}
		break;
	}

	pDC->FillSolidRect(lprect, crBack);

	nLeft	+= lprect->left;
	nTop	= lprect->top + ((lprect->bottom - lprect->top - nHeight) / 2);

	for (int i = 0; i < nHeight; i++)
	{
		pDC->FillSolidRect(nLeft + i, nTop + i, nWidth - (i * 2), 1, crArrow);
	}
}

void CColorPickerX::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
	SetWindowSize();
}
