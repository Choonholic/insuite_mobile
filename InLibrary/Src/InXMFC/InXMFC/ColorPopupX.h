// ColorPopupX.h

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

#pragma once

#ifndef CPN_SELCHANGE
#define CPN_SELCHANGE		(WM_USER + 830)
#define CPN_DROPDOWN		(WM_USER + 831)
#define CPN_CLOSEUP			(WM_USER + 832)
#define CPN_SELENDOK		(WM_USER + 833)
#define CPN_SELENDCANCEL	(WM_USER + 834)
#endif

typedef struct COLOR_TABLE_ENTRYX
{
	COLORREF	cr;
	TCHAR*		szName;
} ColorTableEntryX;

class CColorPickerX;

class AFX_EXT_CLASS CColorPopupX : public CWnd
{
	DECLARE_DYNAMIC(CColorPopupX)

public:
	CColorPopupX();
	CColorPopupX(CPoint pt, COLORREF cr, CWnd* pParentWnd, LPCTSTR szDefault = NULL, LPCTSTR szCancel = NULL);
	virtual	~CColorPopupX();

	BOOL	Create(CPoint pt, COLORREF cr, CWnd* pParentWnd, LPCTSTR szDefault = NULL, LPCTSTR szCancel = NULL);

	void	InitializeX();

	void	SetDefaultText(LPCTSTR szText);
	void	SetCancelText(LPCTSTR szCancel);
	void	SetColor(COLORREF cr);
	void	SetParentWindow(CWnd* pWnd);

	virtual BOOL	PreTranslateMessage(MSG* pMsg);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL	OnQueryNewPalette();
	afx_msg void	OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void	OnKillFocus(CWnd* pNewWnd);

protected:
	static ColorTableEntryX	m_crColors[];

	int		m_nNumColors;
	int		m_nNumColumns;
	int		m_nNumRows;
	int		m_nBoxSize;
	int		m_nMargin;
	int		m_nCurrentSel;
	int		m_nChosenColorSel;
	CString	m_strDefault;
	CString	m_strCancel;
	CRect	m_rcCancel;
	CRect	m_rcDefault;
	CRect	m_rcWindow;
	BOOL	m_bChildWindowVisible;

	COLORREF	m_crInitial;
	COLORREF	m_crColor;

	HFONT		m_hFont;
	CFont		m_Font;
	CPalette	m_Palette;
	CWnd*		m_pParent;

	void	FindCellFromColor(COLORREF cr);
	BOOL	GetCellRect(int nIndex, const LPRECT& lprc);
	void	SetWindowSize();

	void	ChangeSelection(int nIndex);
	void	EndSelection(int nMessage);
	void	DrawCell(CDC* pDC, int nIndex);

	COLORREF	GetColor(int nIndex);
	LPCTSTR		GetColorName(int nIndex);

	int		GetIndex(int nRow, int nCol) const;
	int		GetRow(int nIndex) const;
	int		GetColumn(int nIndex) const;

	DECLARE_MESSAGE_MAP()
};
