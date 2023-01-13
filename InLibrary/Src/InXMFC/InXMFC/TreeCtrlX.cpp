// TreeCtrlX.cpp

#include "stdafx.h"
#include "TreeCtrlX.h"

IMPLEMENT_DYNAMIC(CTreeCtrlX, CTreeCtrl)

CTreeCtrlX::CTreeCtrlX()
{
	m_bTapAndHold		= FALSE;
	m_bCheckListMode	= FALSE;
	m_nActionKeyMode	= TCX_TAPANDHOLD;
}

CTreeCtrlX::~CTreeCtrlX()
{
}

BEGIN_MESSAGE_MAP(CTreeCtrlX, CTreeCtrl)
	ON_NOTIFY_REFLECT(GN_CONTEXTMENU, &CTreeCtrlX::OnContextMenu)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, &CTreeCtrlX::OnTvnKeydown)
END_MESSAGE_MAP()

void CTreeCtrlX::SetCheckListMode(BOOL bMode)
{
	m_bCheckListMode	= bMode;
}

void CTreeCtrlX::SetActionKeyMode(int nMode)
{
	m_nActionKeyMode	= nMode;
}

BOOL CTreeCtrlX::IsTapAndHold()
{
	return m_bTapAndHold;
}

void CTreeCtrlX::GetClickedPoint(LPPOINT lppt)
{
	lppt->x	= m_ptClicked.x;
	lppt->y	= m_ptClicked.y;
}

void CTreeCtrlX::FindItem(DWORD dwData, HTREEITEM& hFound, HTREEITEM hStartItem)
{
	if (hStartItem == NULL)
	{
		FindItem(dwData, hFound, GetRootItem());
		return;
	}

	if (dwData == GetItemData(hStartItem))
	{
		hFound	= hStartItem;
	}

	HTREEITEM	hChild	= GetNextItem(hStartItem, TVGN_CHILD);

	for ( ; hChild != NULL && hFound == NULL ; )
	{
		FindItem(dwData, hFound, hChild);

		hChild	= GetNextSiblingItem(hChild);
	}
}

BOOL CTreeCtrlX::OnContextMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	UINT		uFlags;
	NMRGINFO*	pInfo	= (NMRGINFO*)pNMHDR;
	CPoint		pt;
	CRect		rc;

	pt.x	= pInfo->ptAction.x;
	pt.y	= pInfo->ptAction.y;

	ScreenToClient(&pt);

	HTREEITEM	hItem	= HitTest(pt, &uFlags);

	m_bTapAndHold	= TRUE;

	LPARAM	lParam	= MAKELPARAM(pInfo->ptAction.x, pInfo->ptAction.y);
	WPARAM	wParam	= (WPARAM)hItem;

	::SendMessage(GetParent()->GetSafeHwnd(), WM_TCX_TAPANDHOLD, wParam, lParam);

	*pResult	= TRUE;

	return *pResult;
}

void CTreeCtrlX::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bTapAndHold	= FALSE;
	m_ptClicked.x	= point.x;
	m_ptClicked.y	= point.y;

	if (GetFocus() != this)
	{
		SetFocus();
	}

	Default();
}

void CTreeCtrlX::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bTapAndHold	= FALSE;

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CTreeCtrlX::OnTvnKeydown(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVKEYDOWN	pTVKeyDown	= reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);

	*pResult	= 0;

	if (pTVKeyDown->wVKey == VK_RETURN)
	{
		HTREEITEM	hItem	= GetSelectedItem();

		if (hItem != NULL)
		{
			if (m_bCheckListMode)
			{
				BOOL	bChecked	= GetCheck(hItem);

				SetCheck(hItem, !bChecked);

				LPARAM	lParam	= (LPARAM)this->GetSafeHwnd();
				WPARAM	wParam	= (WPARAM)hItem;

				::SendMessage(GetParent()->GetSafeHwnd(), WM_TCX_CHECKTOGGLE, wParam, lParam);

				*pResult	= 1;
			}
			else
			{
				switch (m_nActionKeyMode)
				{
				case TCX_EXPANDCOLLAPSE:
					{
						if ((GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) != TVIS_EXPANDED)
						{
							Expand(hItem, TVE_EXPAND);
						}
						else
						{
							Expand(hItem, TVE_COLLAPSE);
						}

						EnsureVisible(hItem);
					}
					break;
				case TCX_TAPANDHOLD:
					{
						RECT	rc;

						if (GetItemRect(hItem, &rc, TRUE))
						{
							ClientToScreen(&rc);

							LPARAM	lParam	= MAKELPARAM((rc.left + ((rc.right - rc.left) / 2)), (rc.top + ((rc.bottom - rc.top) / 2)));
							WPARAM	wParam	= (WPARAM)hItem;

							::SendMessage(GetParent()->GetSafeHwnd(), WM_TCX_TAPANDHOLD, wParam, lParam);
						}
					}
					break;
				}

				*pResult	= 1;
			}
		}
	}
	else if (pTVKeyDown->wVKey == VK_RIGHT)
	{
		HTREEITEM	hItem	= GetSelectedItem();

		if (hItem != NULL)
		{
			if ((GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) != TVIS_EXPANDED)
			{
				Expand(hItem, TVE_EXPAND);
				EnsureVisible(hItem);

				*pResult	= 1;
			}
		}
	}
	else if (pTVKeyDown->wVKey == VK_LEFT)
	{
		HTREEITEM	hItem	= GetSelectedItem();

		if (hItem != NULL)
		{
			if ((GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) == TVIS_EXPANDED)
			{
				Expand(hItem, TVE_COLLAPSE);
				EnsureVisible(hItem);

				*pResult	= 1;
			}
		}
	}
}

void CTreeCtrlX::PreSubclassWindow()
{
	SendMessage(CCM_SETVERSION, COMCTL32_VERSION, 0);
	CTreeCtrl::PreSubclassWindow();
}
