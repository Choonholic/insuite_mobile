// ListCtrlX.h

#pragma once

#include "InLibrary_Messages.h"

#ifndef LCX_ALLCOLUMNS
#define LCX_ALLCOLUMNS			(-1)
#endif

#ifndef LCX_FLICK_SCROLL
#define LCX_FLICK_SCROLL
#define LCX_FLICK_SCROLLUP		0
#define LCX_FLICK_SCROLLDOWN	1
#endif

class AFX_EXT_CLASS CListCtrlX : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlX)

public:
	CListCtrlX();
	virtual	~CListCtrlX();

	BOOL	IsTapAndHold();
	void	SetAutoColumnWidth(int nColumn = LCX_ALLCOLUMNS);

	afx_msg BOOL	OnContextMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LONG	OnFlick(WPARAM wParam, LPARAM lParam);

protected:
	virtual void	PreSubclassWindow();

	BOOL	m_bTapAndHold;
	BOOL	m_bFlick;
	DWORD	m_dwTick;
	CPoint	m_ptDown;
	DWORD	m_dwSpeed;
	int		m_nDistance;

	DECLARE_MESSAGE_MAP()
};
