// TreeCtrlX.h

#pragma once

#include "InLibrary_Messages.h"

#ifndef TCX_ACTIONKEYMODE
#define TCX_ACTIONKEYMODE
#define TCX_EXPANDCOLLAPSE	0
#define TCX_TAPANDHOLD		1
#endif

class AFX_EXT_CLASS CTreeCtrlX : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlX)

public:
	CTreeCtrlX();
	virtual	~CTreeCtrlX();

	void	SetCheckListMode(BOOL bMode = FALSE);
	void	SetActionKeyMode(int nMode = TCX_TAPANDHOLD);

	BOOL	IsTapAndHold();
	void	GetClickedPoint(LPPOINT lppt);

	void	FindItem(DWORD dwData, HTREEITEM& hFound, HTREEITEM hStartItem);

	afx_msg BOOL	OnContextMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnTvnKeydown(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	virtual void	PreSubclassWindow();

	BOOL	m_bTapAndHold;
	BOOL	m_bCheckListMode;
	int		m_nActionKeyMode;
	POINT	m_ptClicked;

	DECLARE_MESSAGE_MAP()
};
