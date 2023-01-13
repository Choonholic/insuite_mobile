// ListCtrlX.h

#include "stdafx.h"
#include "ListCtrlX.h"

#define MAX_FLICK_TICK	750		// 0.75 seconds
#define MAX_FLICK_SLOW	6		// 6 times

IMPLEMENT_DYNAMIC(CListCtrlX, CListCtrl)

CListCtrlX::CListCtrlX()
{
	m_bTapAndHold	= FALSE;
	m_dwTick		= 0;
}

CListCtrlX::~CListCtrlX()
{
}

BEGIN_MESSAGE_MAP(CListCtrlX, CListCtrl)
	ON_NOTIFY_REFLECT(GN_CONTEXTMENU, &CListCtrlX::OnContextMenu)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_MESSAGE(WM_LCX_FLICK, &CListCtrlX::OnFlick)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CListCtrlX::IsTapAndHold()
{
	return m_bTapAndHold;
}

void CListCtrlX::SetAutoColumnWidth(int nColumn)
{
	LONG	lStyle	= ::GetWindowLong(this->GetSafeHwnd(), GWL_STYLE);

	if ((lStyle & LVS_TYPEMASK) != LVS_REPORT)
	{
		// Should be Details Mode
		return;
	}

	int		nMaxColumn	= GetHeaderCtrl()->GetItemCount();
	int		nWidthItemOnly;
	int		nWidthUseHeader;
	int		nWidth;

	if ((nColumn < LCX_ALLCOLUMNS) || (nColumn >= nMaxColumn))
	{
		return;
	}

	SetRedraw(FALSE);

	if (nColumn == LCX_ALLCOLUMNS)
	{
		for (int i = 0; i < nMaxColumn; i++)
		{
			if (lStyle & LVS_NOCOLUMNHEADER)
			{
				VERIFY(SetColumnWidth(i, LVSCW_AUTOSIZE));
			}
			else
			{
				VERIFY(SetColumnWidth(i, LVSCW_AUTOSIZE));

				nWidthItemOnly	= GetColumnWidth(i);

				VERIFY(SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER));

				nWidthUseHeader	= GetColumnWidth(i);
				nWidth			= max(nWidthItemOnly, nWidthUseHeader);

				VERIFY(SetColumnWidth(i, nWidth));
			}
		}
	}
	else
	{
		if (lStyle & LVS_NOCOLUMNHEADER)
		{
			VERIFY(SetColumnWidth(nColumn, LVSCW_AUTOSIZE));
		}
		else
		{
			VERIFY(SetColumnWidth(nColumn, LVSCW_AUTOSIZE));

			nWidthItemOnly	= GetColumnWidth(nColumn);

			VERIFY(SetColumnWidth(nColumn, LVSCW_AUTOSIZE_USEHEADER));

			nWidthUseHeader	= GetColumnWidth(nColumn);
			nWidth			= max(nWidthItemOnly, nWidthUseHeader);

			VERIFY(SetColumnWidth(nColumn, nWidth));
		}
	}

	SetRedraw(TRUE);
}

BOOL CListCtrlX::OnContextMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	UINT		uFlags;
	NMRGINFO*	pInfo	= (NMRGINFO*)pNMHDR;
	CPoint		pt;
	CRect		rc;

	pt.x	= pInfo->ptAction.x;
	pt.y	= pInfo->ptAction.y;

	ScreenToClient(&pt);

	int	nItem	= HitTest(pt, &uFlags);

	m_bTapAndHold	= TRUE;

	LPARAM	lParam	= MAKELPARAM(pInfo->ptAction.x, pInfo->ptAction.y);
	WPARAM	wParam	= (WPARAM)nItem;

	::SendMessage(GetParent()->GetSafeHwnd(), WM_LCX_TAPANDHOLD, wParam, lParam);

	*pResult	= TRUE;

	return *pResult;
}

void CListCtrlX::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bTapAndHold	= FALSE;
	m_bFlick		= FALSE;
	m_dwTick		= GetTickCount();
	m_ptDown		= point;

	if (GetFocus() != this)
	{
		SetFocus();
	}

	Default();
}

void CListCtrlX::OnMouseMove(UINT nFlags, CPoint point)
{
	DWORD	dwTick	= GetTickCount();

	if ((dwTick - m_dwTick) > MAX_FLICK_TICK)
	{
		CSize	size;

		m_bFlick	= FALSE;
		size.cx		= m_ptDown.x - point.x;
		size.cy		= m_ptDown.y - point.y;
		m_ptDown	= point;

		Scroll(size);
	}
	else
	{
		m_bFlick	= TRUE;
	}
}

void CListCtrlX::OnLButtonUp(UINT nFlags, CPoint point)
{
	DWORD	dwTick	= GetTickCount();

	m_bTapAndHold	= FALSE;

	if (m_bFlick)
	{
		DWORD	dwTime	= dwTick - m_dwTick;
		LONG	lStyle	= ::GetWindowLong(this->GetSafeHwnd(), GWL_STYLE);
		CSize	size;
		RECT	rc;

		size.cx	= m_ptDown.x - point.x;
		size.cy	= m_ptDown.y - point.y;

		GetItemRect(GetTopIndex(), &rc, LVIR_BOUNDS);

		m_nDistance	= rc.bottom - rc.top;
		m_dwSpeed	= dwTime / 10;

		if (abs(size.cy) > m_nDistance)
		{
			PostMessage(WM_LCX_FLICK, ((size.cy > 0) ? LCX_FLICK_SCROLLUP : LCX_FLICK_SCROLLDOWN), (((MAX_FLICK_TICK - dwTime) * abs(size.cy)) / 1000));
		}
	}
	else
	{
		CListCtrl::OnLButtonUp(nFlags, point);
	}
}

int CListCtrlX::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->dwExStyle	|= LVS_EX_ONECLICKACTIVATE;

	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	return 0;
}

LONG CListCtrlX::OnFlick(WPARAM wParam, LPARAM lParam)
{
	if ((lParam > 0) && (m_bFlick))
	{
		CSize	size;

		size.cx	= 0;
		size.cy	= m_nDistance;

		switch (wParam)
		{
		case LCX_FLICK_SCROLLUP:
			{
				if ((GetTopIndex() + GetCountPerPage()) < GetItemCount())
				{
					lParam--;

					Scroll(size);
					Sleep(m_dwSpeed);
					UpdateWindow();
					PostMessage(WM_LCX_FLICK, wParam, lParam);
				}
				else
				{
					m_bFlick	= FALSE;
				}
			}
			break;
		case LCX_FLICK_SCROLLDOWN:
			{
				size.cy	*= (-1);

				if (GetTopIndex() > 0)
				{
					lParam--;

					Scroll(size);
					Sleep(m_dwSpeed);
					UpdateWindow();
					PostMessage(WM_LCX_FLICK, wParam, lParam);
				}
				else
				{
					m_bFlick	= FALSE;
				}
			}
			break;
		}
	}
	else
	{
		m_bFlick	= FALSE;
	}

	return 0;
}

void CListCtrlX::PreSubclassWindow()
{
	SendMessage(CCM_SETVERSION, COMCTL32_VERSION, 0);
	CListCtrl::PreSubclassWindow();
}
