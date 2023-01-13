// ColorPopupX.cpp

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
#include "ColorPopupX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CColorPopupX, CWnd)

#define CPX_DEFAULT_BOX_VALUE	(-3)
#define CPX_CANCEL_BOX_VALUE	(-2)
#define CPX_INVALID_COLOR		(-1)
#define CPX_MAX_COLORS			100

ColorTableEntryX CColorPopupX::m_crColors[] =
{
	{ RGB(0x00, 0x00, 0x00), _T("Black")			},
	{ RGB(0xA5, 0x2A, 0x00), _T("Brown")			},
	{ RGB(0x00, 0x40, 0x40), _T("Dark Olive Green")	},
	{ RGB(0x00, 0x55, 0x00), _T("Dark Green")		},
	{ RGB(0x00, 0x00, 0x5E), _T("Dark Teal")		},
	{ RGB(0x00, 0x00, 0x8B), _T("Dark blue")		},
	{ RGB(0x4B, 0x00, 0x82), _T("Indigo")			},
	{ RGB(0x28, 0x28, 0x28), _T("Dark grey")		},
	{ RGB(0x8B, 0x00, 0x00), _T("Dark red")			},
	{ RGB(0xFF, 0x68, 0x20), _T("Orange")			},
	{ RGB(0x8B, 0x8B, 0x00), _T("Dark yellow")		},
	{ RGB(0x00, 0x93, 0x00), _T("Green")			},
	{ RGB(0x38, 0x8E, 0x8E), _T("Teal")				},
	{ RGB(0x00, 0x00, 0xFF), _T("Blue")				},
	{ RGB(0x7B, 0x7B, 0xC0), _T("Blue-grey")		},
	{ RGB(0x66, 0x66, 0x66), _T("Grey - 40")		},
	{ RGB(0xFF, 0x00, 0x00), _T("Red")				},
	{ RGB(0xFF, 0xAD, 0x5B), _T("Light orange")		},
	{ RGB(0x32, 0xCD, 0x32), _T("Lime")				},
	{ RGB(0x3C, 0xB3, 0x71), _T("Sea green")		},
	{ RGB(0x7F, 0xFF, 0xD4), _T("Aqua")				},
	{ RGB(0x7D, 0x9E, 0xC0), _T("Light blue")		},
	{ RGB(0x80, 0x00, 0x80), _T("Violet")			},
	{ RGB(0x7F, 0x7F, 0x7F), _T("Grey - 50")		},
	{ RGB(0xFF, 0xC0, 0xCB), _T("Pink")				},
	{ RGB(0xFF, 0xD7, 0x00), _T("Gold")				},
	{ RGB(0xFF, 0xFF, 0x00), _T("Yellow")			},
	{ RGB(0x00, 0xFF, 0x00), _T("Bright green")		},
	{ RGB(0x40, 0xE0, 0xD0), _T("Turquoise")		},
	{ RGB(0xC0, 0xFF, 0xFF), _T("Skyblue")			},
	{ RGB(0x48, 0x00, 0x48), _T("Plum")				},
	{ RGB(0xC0, 0xC0, 0xC0), _T("Light grey")		},
	{ RGB(0xFF, 0xE4, 0xE1), _T("Rose")				},
	{ RGB(0xD2, 0xB4, 0x8C), _T("Tan")				},
	{ RGB(0xFF, 0xFF, 0xE0), _T("Light yellow")		},
	{ RGB(0x98, 0xFB, 0x98), _T("Pale green ")		},
	{ RGB(0xAF, 0xEE, 0xEE), _T("Pale turquoise")	},
	{ RGB(0x68, 0x83, 0x8B), _T("Pale blue")		},
	{ RGB(0xE6, 0xE6, 0xFA), _T("Lavender")			},
	{ RGB(0xFF, 0xFF, 0xFF), _T("White")			}
};

CColorPopupX::CColorPopupX()
{
	InitializeX();
}

CColorPopupX::CColorPopupX(CPoint pt, COLORREF cr, CWnd* pParentWnd, LPCTSTR szDefault, LPCTSTR szCancel)
{
	InitializeX();

	m_crColor		= cr;
	m_crInitial		= cr;
	m_pParent		= pParentWnd;
	m_strDefault	= ((szDefault) ? szDefault : _T(""));
	m_strCancel		= ((szCancel) ? szCancel : _T(""));

	Create(pt, cr, pParentWnd, szDefault, szCancel);
}

CColorPopupX::~CColorPopupX()
{
	m_Font.Detach();
	ReleaseFont(m_hFont);
	m_Palette.DeleteObject();
}

BEGIN_MESSAGE_MAP(CColorPopupX, CWnd)
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

BOOL CColorPopupX::Create(CPoint pt, COLORREF cr, CWnd* pParentWnd, LPCTSTR szDefault, LPCTSTR szCancel)
{
	ASSERT(pParentWnd && ::IsWindow(pParentWnd->GetSafeHwnd()));
	ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CColorPickerX)));

	m_pParent	= pParentWnd;
	m_crColor	= cr;
	m_crInitial	= cr;

	CString	szClassName	= AfxRegisterWndClass(CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_BTNFACE + 1), 0);

	if (!CWnd::CreateEx(0, szClassName, _T(""), WS_VISIBLE | WS_POPUP, pt.x, pt.y, DRA::SCALEX(100), DRA::SCALEY(100), pParentWnd->GetSafeHwnd(), 0, NULL))
	{
		return FALSE;
	}

	if (szCancel != NULL)
	{
		m_strCancel	= szCancel;
	}

	if (szDefault != NULL)
	{
		m_strDefault	= szDefault;
	}

	SetWindowSize();
	FindCellFromColor(cr);
	SetCapture();
	return TRUE;
}

void CColorPopupX::InitializeX()
{
	m_nNumColors	= (sizeof(m_crColors) / sizeof(ColorTableEntryX));

	ASSERT(m_nNumColors <= CPX_MAX_COLORS);

	if (m_nNumColors > CPX_MAX_COLORS)
	{
		m_nNumColors	= CPX_MAX_COLORS;
	}

	m_nNumColumns			= 0;
	m_nNumRows				= 0;
	m_nBoxSize				= DRA::SCALEX(18);
	m_nMargin				= DRA::SCALEX(1);
	m_nCurrentSel			= CPX_INVALID_COLOR;
	m_nChosenColorSel		= CPX_INVALID_COLOR;
	m_pParent				= NULL;
	m_crColor				= RGB(0x00, 0x00, 0x00);
	m_crInitial				= RGB(0x00, 0x00, 0x00);
	m_bChildWindowVisible	= FALSE;

	if ((m_nBoxSize - 2 * m_nMargin - DRA::SCALEX(2)) < DRA::SCALEX(5))
	{
		m_nBoxSize	= DRA::SCALEX(5) + 2 * m_nMargin + DRA::SCALEX(2);
	}

	LOGFONT	lfFont;

	GetDefaultFont(&lfFont);

	m_hFont	= CreateFontIndirect(&lfFont);

	m_Font.Attach(m_hFont);

	struct
	{
		LOGPALETTE		LogPalette;
		PALETTEENTRY	PalEntry[CPX_MAX_COLORS];
	} Palettes;

	LOGPALETTE*	pLogPalette	= (LOGPALETTE*)&Palettes;

	pLogPalette->palVersion		= 0x300;
	pLogPalette->palNumEntries	= (WORD)m_nNumColors;

	for (int i = 0; i < m_nNumColors; i++)
	{
		pLogPalette->palPalEntry[i].peRed	= GetRValue(m_crColors[i].cr);
		pLogPalette->palPalEntry[i].peGreen	= GetGValue(m_crColors[i].cr);
		pLogPalette->palPalEntry[i].peBlue	= GetBValue(m_crColors[i].cr);
		pLogPalette->palPalEntry[i].peFlags	= 0;
	}

	m_Palette.CreatePalette(pLogPalette);
}

void CColorPopupX::SetDefaultText(LPCTSTR szText)
{
	m_strDefault	= ((szText) ? szText : _T(""));
}

void CColorPopupX::SetCancelText(LPCTSTR szText)
{
	m_strCancel	= ((szText) ? szText : _T(""));
}

void CColorPopupX::SetColor(COLORREF cr)
{
	m_crColor	= cr;
	m_crInitial	= cr;
}

void CColorPopupX::SetParentWindow(CWnd *pParentWnd)
{
	m_pParent	= pParentWnd;
}

BOOL CColorPopupX::PreTranslateMessage(MSG* pMsg)
{
	if (GetCapture()->GetSafeHwnd() != m_hWnd)
	{
		SetCapture();
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CColorPopupX::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int	nRow	= GetRow(m_nCurrentSel);
	int	nCol	= GetColumn(m_nCurrentSel);

	if (nChar == VK_DOWN)
	{
		if (nRow == CPX_DEFAULT_BOX_VALUE)
		{
			nRow	= 0;
			nCol	= 0;
		}
		else if (nRow == CPX_CANCEL_BOX_VALUE)
		{
			if (m_strDefault.GetLength())
			{
				nRow	= CPX_DEFAULT_BOX_VALUE;
				nCol	= CPX_DEFAULT_BOX_VALUE;
			}
			else
			{
				nRow	= 0;
				nCol	= 0;
			}
		}
		else
		{
			nRow++;

			if (GetIndex(nRow, nCol) < 0)
			{
				if (m_strCancel.GetLength())
				{
					nRow	= CPX_CANCEL_BOX_VALUE;
					nCol	= CPX_CANCEL_BOX_VALUE;
				}
				else if (m_strDefault.GetLength())
				{
					nRow	= CPX_DEFAULT_BOX_VALUE;
					nCol	= CPX_DEFAULT_BOX_VALUE;
				}
				else
				{
					nRow	= 0;
					nCol	= 0;
				}
			}
		}

		ChangeSelection(GetIndex(nRow, nCol));
	}

	if (nChar == VK_UP)
	{
		if (nRow == CPX_DEFAULT_BOX_VALUE)
		{
			if (m_strCancel.GetLength())
			{
				nRow	= CPX_CANCEL_BOX_VALUE;
				nCol	= CPX_CANCEL_BOX_VALUE;
			}
			else
			{
				nRow	= GetRow(m_nNumColors - 1);
				nCol	= GetColumn(m_nNumColors - 1);
			}
		}
		else if (nRow == CPX_CANCEL_BOX_VALUE)
		{
			nRow	= GetRow(m_nNumColors - 1);
			nCol	= GetColumn(m_nNumColors - 1);
		}
		else if (nRow > 0)
		{
			nRow--;
		}
		else
		{
			if (m_strDefault.GetLength())
			{
				nRow	= CPX_DEFAULT_BOX_VALUE;
				nCol	= CPX_DEFAULT_BOX_VALUE;
			}
			else if (m_strCancel.GetLength())
			{
				nRow	= CPX_CANCEL_BOX_VALUE;
				nCol	= CPX_CANCEL_BOX_VALUE;
			}
			else
			{
				nRow	= GetRow(m_nNumColors - 1);
				nCol	= GetColumn(m_nNumColors - 1);
			}
		}

		ChangeSelection(GetIndex(nRow, nCol));
	}

	if (nChar == VK_RIGHT)
	{
		if (nRow == CPX_DEFAULT_BOX_VALUE)
		{
			nRow	= 0;
			nCol	= 0;
		}
		else if (nRow == CPX_CANCEL_BOX_VALUE)
		{
			if (m_strDefault.GetLength())
			{
				nRow	= CPX_DEFAULT_BOX_VALUE;
				nCol	= CPX_DEFAULT_BOX_VALUE;
			}
			else
			{
				nRow	= 0;
				nCol	= 0;
			}
		}
		else if (nCol < m_nNumColumns - 1)
		{
			nCol++;
		}
		else
		{
			nCol	= 0;
			nRow++;
		}

		if (GetIndex(nRow, nCol) == CPX_INVALID_COLOR)
		{
			if (m_strCancel.GetLength())
			{
				nRow	= CPX_CANCEL_BOX_VALUE;
				nCol	= CPX_CANCEL_BOX_VALUE;
			}
			else if (m_strDefault.GetLength())
			{
				nRow	= CPX_DEFAULT_BOX_VALUE;
				nCol	= CPX_DEFAULT_BOX_VALUE;
			}
			else
			{
				nRow	= 0;
				nCol	= 0;
			}
		}

		ChangeSelection(GetIndex(nRow, nCol));
	}

	if (nChar == VK_LEFT)
	{
		if (nRow == CPX_DEFAULT_BOX_VALUE)
		{
			if (m_strCancel.GetLength())
			{
				nRow	= CPX_CANCEL_BOX_VALUE;
				nCol	= CPX_CANCEL_BOX_VALUE;
			}
			else
			{
				nRow	= GetRow(m_nNumColors - 1);
				nCol	= GetColumn(m_nNumColors - 1);
			}
		}
		else if (nRow == CPX_CANCEL_BOX_VALUE)
		{
			nRow	= GetRow(m_nNumColors - 1);
			nCol	= GetColumn(m_nNumColors - 1);
		}
		else if (nCol > 0)
		{
			nCol--;
		}
		else
		{
			if (nRow > 0)
			{
				nRow--;
				nCol	= m_nNumColumns - 1;
			}
			else
			{
				if (m_strDefault.GetLength())
				{
					nRow	= CPX_DEFAULT_BOX_VALUE;
					nCol	= CPX_DEFAULT_BOX_VALUE;
				}
				else if (m_strCancel.GetLength())
				{
					nRow	= CPX_CANCEL_BOX_VALUE;
					nCol	= CPX_CANCEL_BOX_VALUE;
				}
				else
				{
					nRow	= GetRow(m_nNumColors - 1);
					nCol	= GetColumn(m_nNumColors - 1);
				}
			}
		}

		ChangeSelection(GetIndex(nRow, nCol));
	}

	if (nChar == VK_ESCAPE)
	{
		m_crColor	= m_crInitial;

		EndSelection(CPN_SELENDCANCEL);
		return;
	}

	if (nChar == VK_RETURN || nChar == VK_SPACE)
	{
		EndSelection(CPN_SELENDOK);
		return;
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CColorPopupX::OnPaint()
{
	CPaintDC	dc(this);
	CRect		rc;

	GetClientRect(rc);
	dc.FillSolidRect(rc, GetSysColor(COLOR_WINDOWFRAME));
	rc.DeflateRect(DRA::SCALEX(1), DRA::SCALEY(1));
	dc.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));

	if (m_strDefault.GetLength())
	{
		DrawCell(&dc, CPX_DEFAULT_BOX_VALUE);
	}

	for (int i = 0; i < m_nNumColors; i++)
	{
		DrawCell(&dc, i);
	}

	if (m_strCancel.GetLength())
	{
		DrawCell(&dc, CPX_CANCEL_BOX_VALUE);
	}
}

void CColorPopupX::OnLButtonDown(UINT nFlags, CPoint point)
{
	int	nNewSelection	= CPX_INVALID_COLOR;

	point.x	-= m_nMargin;
	point.y	-= m_nMargin;

	if (m_strCancel.GetLength() && m_rcCancel.PtInRect(point))
	{
		nNewSelection	= CPX_CANCEL_BOX_VALUE;
	}
	else if (m_strDefault.GetLength() && m_rcDefault.PtInRect(point))
	{
		nNewSelection	= CPX_DEFAULT_BOX_VALUE;
	}
	else
	{
		if (m_strDefault.GetLength())
		{
			point.y	-= m_rcDefault.Height();
		}

		nNewSelection	= GetIndex(point.y / m_nBoxSize, point.x / m_nBoxSize);

		if ((nNewSelection < 0) || (nNewSelection >= m_nNumColors))
		{
			CWnd::OnLButtonDown(nFlags, point);
			return;
		}
	}

	if (nNewSelection != m_nCurrentSel)
	{
		ChangeSelection(nNewSelection);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CColorPopupX::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);

	point	= CPoint(GetMessagePos());

	if (m_rcWindow.PtInRect(point))
	{
		EndSelection(CPN_SELENDOK);
	}
	else
	{
		EndSelection(CPN_SELENDCANCEL);
	}
}

BOOL CColorPopupX::OnQueryNewPalette()
{
	Invalidate();
	return CWnd::OnQueryNewPalette();
}

void CColorPopupX::OnPaletteChanged(CWnd* pFocusWnd)
{
	CWnd::OnPaletteChanged(pFocusWnd);

	if (pFocusWnd->GetSafeHwnd() != GetSafeHwnd())
	{
		Invalidate();
	}
}

void CColorPopupX::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	ReleaseCapture();
}

void CColorPopupX::FindCellFromColor(COLORREF cr)
{
	if ((cr == CLR_DEFAULT) && m_strDefault.GetLength())
	{
		m_nChosenColorSel	= CPX_DEFAULT_BOX_VALUE;

		return;
	}

	for (int i = 0; i < m_nNumColors; i++)
	{
		if (GetColor(i) == cr)
		{
			m_nChosenColorSel	= i;

			return;
		}
	}

	if (m_strCancel.GetLength())
	{
		m_nChosenColorSel	= CPX_CANCEL_BOX_VALUE;
	}
	else
	{
		m_nChosenColorSel	= CPX_INVALID_COLOR;
	}
}

BOOL CColorPopupX::GetCellRect(int nIndex, const LPRECT &lprc)
{
	if (nIndex == CPX_CANCEL_BOX_VALUE)
	{
		::SetRect(lprc, m_rcCancel.left, m_rcCancel.top, m_rcCancel.right, m_rcCancel.bottom);
		return TRUE;
	}
	else if (nIndex == CPX_DEFAULT_BOX_VALUE)
	{
		::SetRect(lprc, m_rcDefault.left, m_rcDefault.top, m_rcDefault.right, m_rcDefault.bottom);
		return TRUE;
	}

	if ((nIndex < 0) || (nIndex >= m_nNumColors))
	{
		return FALSE;
	}

	lprc->left	= GetColumn(nIndex) * m_nBoxSize + m_nMargin;
	lprc->top	= GetRow(nIndex) * m_nBoxSize + m_nMargin;

	if (m_strDefault.GetLength())
	{
		lprc->top	+= (m_nMargin + m_rcDefault.Height());
	}

	lprc->right		= lprc->left + m_nBoxSize;
	lprc->bottom	= lprc->top + m_nBoxSize;

	return TRUE;
}

void CColorPopupX::SetWindowSize()
{
	CSize	TextSize;

	if (m_strCancel.GetLength() || m_strDefault.GetLength())
	{
		CClientDC	dc(this);
		CFont*		pOldFont	= (CFont*)dc.SelectObject(&m_Font);

		TextSize	= CSize(0, 0);

		if (m_strCancel.GetLength())
		{
			TextSize	= dc.GetTextExtent(m_strCancel);
		}

		if (m_strDefault.GetLength())
		{
			CSize	DefaultSize	= dc.GetTextExtent(m_strDefault);

			if (DefaultSize.cx > TextSize.cx)
			{
				TextSize.cx	= DefaultSize.cx;
			}

			if (DefaultSize.cy > TextSize.cy)
			{
				TextSize.cy	= DefaultSize.cy;
			}
		}

		dc.SelectObject(pOldFont);

		TextSize	+= CSize(2 * m_nMargin, 2 * m_nMargin);
		TextSize.cy	+= 2 * m_nMargin + DRA::SCALEY(2);
	}

	m_nNumColumns	= 8;
	m_nNumRows		= m_nNumColors / m_nNumColumns;

	if (m_nNumColors % m_nNumColumns)
	{
		m_nNumRows++;
	}

	CRect	rc;

	GetWindowRect(rc);
	m_rcWindow.SetRect(rc.left, rc.top, rc.left + m_nNumColumns * m_nBoxSize + 2 * m_nMargin, rc.top  + m_nNumRows * m_nBoxSize + 2 * m_nMargin);

	if (m_strDefault.GetLength())
	{
		if (TextSize.cx > m_rcWindow.Width())
		{
			m_rcWindow.right	= m_rcWindow.left + TextSize.cx;
		}

		TextSize.cx	= m_rcWindow.Width() - 2 * m_nMargin;

		m_rcDefault.SetRect(m_nMargin, m_nMargin, m_nMargin + TextSize.cx, 2 * m_nMargin + TextSize.cy);

		m_rcWindow.bottom	+= m_rcDefault.Height() + 2 * m_nMargin;
	}

	if (m_strCancel.GetLength())
	{
		if (TextSize.cx > m_rcWindow.Width())
		{
			m_rcWindow.right	= m_rcWindow.left + TextSize.cx;
		}

		TextSize.cx	= m_rcWindow.Width() - 2 * m_nMargin;

		m_rcCancel.SetRect(m_nMargin, m_rcWindow.Height(), m_nMargin + TextSize.cx, m_rcWindow.Height() + m_nMargin + TextSize.cy);

		m_rcWindow.bottom	+= m_rcCancel.Height() + 2 * m_nMargin;
	}

	CSize	ScreenSize(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));

	if (m_rcWindow.right > ScreenSize.cx)
	{
		m_rcWindow.OffsetRect(-(m_rcWindow.right - ScreenSize.cx), 0);
	}

	if (m_rcWindow.left < 0)
	{
		m_rcWindow.OffsetRect(-m_rcWindow.left, 0);
	}

	if (m_rcWindow.bottom > ScreenSize.cy)
	{
		CRect	rcParent;

		m_pParent->GetWindowRect(rcParent);
		m_rcWindow.OffsetRect(0, -(rcParent.Height() + m_rcWindow.Height()));
	}

	MoveWindow(m_rcWindow, TRUE);
}

void CColorPopupX::ChangeSelection(int nIndex)
{
	CClientDC	dc(this);

	if (nIndex > m_nNumColors)
	{
		nIndex	= CPX_CANCEL_BOX_VALUE;
	}

	if (((m_nCurrentSel >= 0) && (m_nCurrentSel < m_nNumColors)) || (m_nCurrentSel == CPX_CANCEL_BOX_VALUE) || (m_nCurrentSel == CPX_DEFAULT_BOX_VALUE))
	{
		int	nOldSel	= m_nCurrentSel;

		m_nCurrentSel	= CPX_INVALID_COLOR;

		DrawCell(&dc, nOldSel);
	}

	m_nCurrentSel	= nIndex;

	DrawCell(&dc, m_nCurrentSel);

	if (m_nCurrentSel == CPX_CANCEL_BOX_VALUE)
	{
		m_pParent->SendMessage(CPN_SELCHANGE, (LPARAM)m_crInitial, 0);
	}
	else if (m_nCurrentSel == CPX_DEFAULT_BOX_VALUE)
	{
		m_crColor	= CLR_DEFAULT;

		m_pParent->SendMessage(CPN_SELCHANGE, (LPARAM)CLR_DEFAULT, 0);
	}
	else
	{
		m_crColor	= GetColor(m_nCurrentSel);

		m_pParent->SendMessage(CPN_SELCHANGE, (LPARAM)m_crColor, 0);
	}
}

void CColorPopupX::EndSelection(int nMessage)
{
	ReleaseCapture();

	if (nMessage == CPN_SELENDCANCEL)
	{
		m_crColor	= m_crInitial;
	}

	m_pParent->SendMessage(nMessage, (LPARAM)m_crColor, 0);

	if (!m_bChildWindowVisible)
	{
		DestroyWindow();
	}
}

void CColorPopupX::DrawCell(CDC* pDC, int nIndex)
{
	if (m_strCancel.GetLength() && (nIndex == CPX_CANCEL_BOX_VALUE))
	{
		CRect	rcTextButton	= m_rcCancel;

		rcTextButton.top	+= 2 * m_nMargin;

		pDC->FillSolidRect(rcTextButton, ::GetSysColor(COLOR_WINDOW));
		pDC->FillSolidRect(m_rcCancel.left + 2 * m_nMargin, m_rcCancel.top, m_rcCancel.Width() - 4 * m_nMargin, DRA::SCALEY(1), ::GetSysColor(COLOR_BTNTEXT));

		CFont*	pOldFont	= (CFont*)pDC->SelectObject(&m_Font);

		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(m_strCancel, rcTextButton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SelectObject(pOldFont);
		return;
	}

	if (m_strDefault.GetLength() && (nIndex == CPX_DEFAULT_BOX_VALUE))
	{
		pDC->FillSolidRect(m_rcDefault, ::GetSysColor(COLOR_WINDOW));

		CRect	rcTextButton	= m_rcDefault;

		rcTextButton.DeflateRect(DRA::SCALEX(1), DRA::SCALEY(1));

		CRect	rcLine	= rcTextButton;

		rcLine.DeflateRect(2 * m_nMargin, 2 * m_nMargin);

		CPen	pen(PS_SOLID, DRA::SCALEX(1), ::GetSysColor(COLOR_3DSHADOW));
		CPen*	pOldPen	= pDC->SelectObject(&pen);

		pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(rcLine);
		pDC->SelectObject(pOldPen);

		CFont*	pOldFont	= (CFont*)pDC->SelectObject(&m_Font);

		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(m_strDefault, rcTextButton, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SelectObject(pOldFont);
		return;
	}

	CRect	rc;

	if (!GetCellRect(nIndex, rc))
	{
		return;
	}

	CPalette*	pOldPalette	= NULL;

	if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		pOldPalette	= pDC->SelectPalette(&m_Palette, FALSE);

		pDC->RealizePalette();
	}

	CBrush	brush(PALETTERGB(GetRValue(GetColor(nIndex)), GetGValue(GetColor(nIndex)), GetBValue(GetColor(nIndex))));
	CPen	pen;

	pen.CreatePen(PS_SOLID, DRA::SCALEX(1), ((m_nChosenColorSel == nIndex) ? GetSysColor(COLOR_WINDOWFRAME) : GetSysColor(COLOR_3DSHADOW)));

	CBrush*	pOldBrush	= (CBrush*)pDC->SelectObject(&brush);
	CPen*	pOldPen		= (CPen*)pDC->SelectObject(&pen);

	rc.DeflateRect(m_nMargin + 1, m_nMargin + 1);
	pDC->Rectangle(rc);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	brush.DeleteObject();
	pen.DeleteObject();

	if (pOldPalette && (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE))
	{
		pDC->SelectPalette(pOldPalette, FALSE);
	}
}

COLORREF CColorPopupX::GetColor(int nIndex)
{
	return m_crColors[nIndex].cr;
}

LPCTSTR CColorPopupX::GetColorName(int nIndex)
{
	return m_crColors[nIndex].szName;
}

int CColorPopupX::GetIndex(int nRow, int nCol) const
{
	if (((nRow == CPX_CANCEL_BOX_VALUE) || (nCol == CPX_CANCEL_BOX_VALUE)) && m_strCancel.GetLength())
	{
		return CPX_CANCEL_BOX_VALUE;
	}
	else if (((nRow == CPX_DEFAULT_BOX_VALUE) || (nCol == CPX_DEFAULT_BOX_VALUE)) && m_strDefault.GetLength())
	{
		return CPX_DEFAULT_BOX_VALUE;
	}
	else if ((nRow < 0) || (nCol < 0) || (nRow >= m_nNumRows) || (nCol >= m_nNumColumns))
	{
		return CPX_INVALID_COLOR;
	}
	else
	{
		if ((nRow * m_nNumColumns + nCol) >= m_nNumColors)
		{
			return CPX_INVALID_COLOR;
		}
	}

	return (nRow * m_nNumColumns + nCol);
}

int CColorPopupX::GetRow(int nIndex) const
{
	if ((nIndex == CPX_CANCEL_BOX_VALUE) && m_strCancel.GetLength())
	{
		return CPX_CANCEL_BOX_VALUE;
	}
	else if ((nIndex == CPX_DEFAULT_BOX_VALUE) && m_strDefault.GetLength())
	{
		return CPX_DEFAULT_BOX_VALUE;
	}
	else if ((nIndex < 0) || (nIndex >= m_nNumColors))
	{
		return CPX_INVALID_COLOR;
	}

	return (nIndex / m_nNumColumns);
}

int CColorPopupX::GetColumn(int nIndex) const
{
	if ((nIndex == CPX_CANCEL_BOX_VALUE) && m_strCancel.GetLength())
	{
		return CPX_CANCEL_BOX_VALUE;
	}
	else if ((nIndex == CPX_DEFAULT_BOX_VALUE) && m_strDefault.GetLength())
	{
		return CPX_DEFAULT_BOX_VALUE;
	}
	else if ((nIndex < 0) || (nIndex >= m_nNumColors))
	{
		return CPX_INVALID_COLOR;
	}

	return (nIndex % m_nNumColumns);
}
