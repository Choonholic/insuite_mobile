// ColorPickerX.h

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

#include "ColorPopupX.h"

void	AFX_EXT_API	DDX_ColorPickerX(CDataExchange* pDX, int nIDC, COLORREF& cr);

#define CPX_MODE_TEXT	1
#define CPX_MODE_BACK	2

class AFX_EXT_CLASS CColorPickerX : public CButton
{
	DECLARE_DYNCREATE(CColorPickerX)

public:
	CColorPickerX();
	virtual	~CColorPickerX();

	COLORREF	GetColor();
	void		SetColor(COLORREF cr);

	void	SetDefaultText(LPCTSTR szText);
	void	SetCancelText(LPCTSTR szText);
	void	SetTrackSelection(BOOL bTracking = TRUE);
	BOOL	GetTrackSelection();
	void	SetSelectionMode(UINT nMode);
	UINT	GetSelectionMode();

	void		SetBackColor(COLORREF cr);
	COLORREF	GetBackColor();
	void		SetTextColor(COLORREF cr);
	COLORREF	GetTextColor();

	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg LONG	OnSelEndOK(WPARAM wParam, LPARAM lParam);
	afx_msg LONG	OnSelEndCancel(WPARAM wParam, LPARAM lParam);
	afx_msg LONG	OnSelChange(WPARAM wParam, LPARAM lParam);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnBnClicked();

protected:
	BOOL	m_bActive;
	BOOL	m_bTrackSelection;
	UINT	m_nSelectionMode;

	COLORREF	m_crColorText;
	COLORREF	m_crColorBack;
	COLORREF	m_crBackup;

	CRect	m_rcArrow;
	CString	m_strDefault;
	CString	m_strCancel;

	CColorPopupX	m_hWndPopup;

	void	SetWindowSize();
	void	RecalcArrowRect();
	void	DrawArrow(CDC* pDC, LPRECT lprect, int nFlags);

	virtual void	PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
};
