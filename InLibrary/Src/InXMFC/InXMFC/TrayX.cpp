// TrayX.cpp

#include "stdafx.h"
#include "TrayX.h"

IMPLEMENT_DYNAMIC(CTrayX, CWnd)

#ifndef _countof
#define _countof(x)	(sizeof(x) / sizeof(x[0]))
#endif

const UINT	CTrayX::m_nTimerID				= 4567;
const UINT	CTrayX::m_nTaskbarCreatedMsg	= ::RegisterWindowMessage(_T("TaskbarCreated"));
UINT	CTrayX::m_nMaxTooltipLength			= 64;
CWnd	CTrayX::m_wndInvisible;

CTrayX::CTrayX()
{
	InitializeX();
}

CTrayX::CTrayX(CWnd* pParentWnd, UINT uCallbackMessage, LPCTSTR lpszToolTip, HICON hIcon, UINT uID, BOOL bHidden)
{
	InitializeX();
	Create(pParentWnd, uCallbackMessage, lpszToolTip, hIcon, uID, bHidden);
}

CTrayX::~CTrayX()
{
	RemoveIcon();
	m_arrIcons.RemoveAll();
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CTrayX, CWnd)
	ON_WM_TIMER()
	ON_REGISTERED_MESSAGE(CTrayX::m_nTaskbarCreatedMsg, OnTaskbarCreated)
END_MESSAGE_MAP()

BOOL CTrayX::Enabled()
{
	return (m_bEnabled);
}

BOOL CTrayX::Visible()
{
	return (!m_bHidden);
}

BOOL CTrayX::Create(CWnd* pParentWnd, UINT uCallbackMessage, LPCTSTR lpszToolTip, HICON hIcon, UINT uID, BOOL bHidden)
{
	// Tray icon features are only for Windows 95 (or higher) and Windows CE 2.0 (or higher).
	m_bEnabled			= TRUE;
	m_nMaxTooltipLength	= _countof(m_NID.szTip);

	// Make sure we avoid conflict with other messages
	ASSERT(uCallbackMessage >= WM_APP);

	// Tray only supports tooltip text up to m_nMaxTooltipLength) characters
	ASSERT(AfxIsValidString(lpszToolTip));
	ASSERT(_tcslen(lpszToolTip) <= m_nMaxTooltipLength);

	// Create an invisible window
	CWnd::CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP, 0, 0, 0, 0, NULL, 0);

	// Load up the NOTIFYICONDATA structure
	m_NID.cbSize			= sizeof(NOTIFYICONDATA);
	m_NID.hWnd				= (pParentWnd->GetSafeHwnd() ? pParentWnd->GetSafeHwnd() : m_hWnd);
	m_NID.uID				= uID;
	m_NID.hIcon				= hIcon;
	m_NID.uFlags			= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_NID.uCallbackMessage	= uCallbackMessage;

	_tcsncpy_s(m_NID.szTip, 64, lpszToolTip, m_nMaxTooltipLength - 1);

	m_bHidden			= bHidden;
	m_uCreationFlags	= m_NID.uFlags;	// Store in case we need to recreate in OnTaskBarCreate

	BOOL	bResult	= TRUE;

	if (!m_bHidden)
	{
		bResult				= Shell_NotifyIcon(NIM_ADD, &m_NID);
		m_bShowIconPending	= !bResult;
		m_bHidden			= !bResult;
		m_bRemoved			= !bResult;
	}

	return bResult;
}

BOOL CTrayX::SetTooltipText(LPCTSTR lpszTip)
{
	ASSERT(AfxIsValidString(lpszTip));
	ASSERT(_tcslen(lpszTip) < m_nMaxTooltipLength);

	if (!m_bEnabled)
	{
		return FALSE;
	}

	m_NID.uFlags	= NIF_TIP;

	_tcsncpy_s(m_NID.szTip, 64, lpszTip, m_nMaxTooltipLength - 1);

	if (m_bHidden)
	{
		return TRUE;
	}
	else
	{
		return (Shell_NotifyIcon(NIM_MODIFY, &m_NID));
	}
}

BOOL CTrayX::SetTooltipText(UINT nID)
{
	CString	strText;

	VERIFY(strText.LoadString(nID));
	return (SetTooltipText(strText));
}

CString	CTrayX::GetTooltipText() const
{
	CString	strText;

	if (m_bEnabled)
	{
		strText	= m_NID.szTip;
	}

	return strText;
}

BOOL CTrayX::SetIcon(HICON hIcon)
{
	if (!m_bEnabled)
	{
		return FALSE;
	}

	m_NID.uFlags	= NIF_ICON;
	m_NID.hIcon		= hIcon;

	if (m_bHidden)
	{
		return TRUE;
	}
	else
	{
		return Shell_NotifyIcon(NIM_MODIFY, &m_NID);
	}
}

BOOL CTrayX::SetIcon(LPCTSTR lpszIconName)
{
	HICON	hIcon	= (HICON)::LoadImage(AfxGetResourceHandle(), lpszIconName, IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	if (!hIcon)
	{
		return FALSE;
	}

	BOOL	bSuccess	= SetIcon(hIcon);

	::DestroyIcon(hIcon);
	return bSuccess;
}

BOOL CTrayX::SetIcon(UINT nIDResource)
{
	return (SetIcon(MAKEINTRESOURCE(nIDResource)));
}

HICON CTrayX::GetIcon() const
{
	return ((m_bEnabled) ? m_NID.hIcon : NULL);
}

BOOL CTrayX::ShowIcon()
{
	if (m_bRemoved)
	{
		return (AddIcon());
	}

	if (!m_bHidden)
	{
		return TRUE;
	}

	AddIcon();
	return (BOOL)(m_bHidden == FALSE);
}

BOOL CTrayX::HideIcon()
{
	if ((!m_bEnabled) || (m_bRemoved) || (m_bHidden))
	{
		return TRUE;
	}

	RemoveIcon();
	return (BOOL)(m_bHidden == TRUE);
}

BOOL CTrayX::AddIcon()
{
	if (!m_bRemoved)
	{
		RemoveIcon();
	}

	if (m_bEnabled)
	{
		m_NID.uFlags	= NIF_MESSAGE | NIF_ICON | NIF_TIP;

		if (!Shell_NotifyIcon(NIM_ADD, &m_NID))
		{
			m_bShowIconPending	= TRUE;
		}
		else
		{
			m_bRemoved	= FALSE;
			m_bHidden	= FALSE;
		}
	}

	return (BOOL)(m_bRemoved == FALSE);
}

BOOL CTrayX::RemoveIcon()
{
	m_bShowIconPending	= FALSE;

	if ((!m_bEnabled) || (m_bRemoved))
	{
		return TRUE;
	}

	m_NID.uFlags	= 0;

	if (Shell_NotifyIcon(NIM_DELETE, &m_NID))
	{
		m_bRemoved	= TRUE;
		m_bHidden	= TRUE;
	}

	return (BOOL)(m_bRemoved == TRUE);
}

BOOL CTrayX::MoveToRight()
{
	RemoveIcon();
	return (AddIcon());
}

BOOL CTrayX::SetIconList(UINT uFirstIconID, UINT uLastIconID)
{
	const CWinApp*	pApp	= AfxGetApp();

	if (uFirstIconID > uLastIconID)
	{
		return FALSE;
	}

	if (!pApp)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_arrIcons.RemoveAll();

	TRY
	{
		for (UINT i = uFirstIconID; i <= uLastIconID; i++)
		{
			m_arrIcons.Add(pApp->LoadIcon(i));
		}
	}
	CATCH(CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
		m_arrIcons.RemoveAll();
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

BOOL CTrayX::SetIconList(HICON* pHIconList, UINT nNumIcons)
{
	m_arrIcons.RemoveAll();

	TRY
	{
		for (UINT i = 0; i < nNumIcons; i++)
		{
			m_arrIcons.Add(pHIconList[i]);
		}
	}
	CATCH(CMemoryException, e)
	{
		e->ReportError();
		e->Delete();
		m_arrIcons.RemoveAll();
		return FALSE;
	}
	END_CATCH

	return TRUE;
}

BOOL CTrayX::Animate(UINT nDelayMilliSeconds, int nNumSeconds)
{
	if (!m_arrIcons.GetSize())
	{
		return FALSE;
	}

	StopAnimation();

	m_nCurrentIcon		= 0;
	m_StartTime			= COleDateTime::GetCurrentTime();
	m_nAnimationPeriod	= nNumSeconds;
	m_hSavedIcon		= GetIcon();

	// Setup a timer for the animation
	m_uIDTimer	= SetTimer(m_nTimerID, nDelayMilliSeconds, NULL);

	return (BOOL)(m_uIDTimer != 0);
}

BOOL CTrayX::StepAnimation()
{
	if (!m_arrIcons.GetSize())
	{
		return FALSE;
	}

	m_nCurrentIcon++;

	if (m_nCurrentIcon >= m_arrIcons.GetSize())
	{
		m_nCurrentIcon	= 0;
	}

	return SetIcon(m_arrIcons[m_nCurrentIcon]);
}

BOOL CTrayX::StopAnimation()
{
	BOOL	bResult	= FALSE;

	if (m_uIDTimer)
	{
		bResult	= KillTimer(m_uIDTimer);
	}

	m_uIDTimer	= 0;

	if (m_hSavedIcon)
	{
		SetIcon(m_hSavedIcon);
	}

	m_hSavedIcon	= NULL;

	return bResult;
}

BOOL CTrayX::SetMenuDefaultItem(UINT uItem, BOOL bByPos)
{
	return FALSE;
}

void CTrayX::GetMenuDefaultItem(UINT& uItem, BOOL& bByPos)
{
	uItem	= m_uDefaultMenuItemID;
	bByPos	= m_bDefaultMenuItemByPos;
}

BOOL CTrayX::SetNotificationWnd(CWnd* pWnd)
{
	if (!m_bEnabled)
	{
		return FALSE;
	}

	// Make sure Notification window is valid
	if ((!pWnd) || (!::IsWindow(pWnd->GetSafeHwnd())))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_NID.hWnd		= pWnd->GetSafeHwnd();
	m_NID.uFlags	= 0;

	if (m_bHidden)
	{
		return TRUE;
	}
	else
	{
		return Shell_NotifyIcon(NIM_MODIFY, &m_NID);
	}
}

CWnd* CTrayX::GetNotificationWnd() const
{
	return (CWnd::FromHandle(m_NID.hWnd));
}

BOOL CTrayX::SetTargetWnd(CWnd* pTargetWnd)
{
	m_pTargetWnd	= pTargetWnd;

	return TRUE;
}

CWnd* CTrayX::GetTargetWnd() const
{
	if (m_pTargetWnd)
	{
		return m_pTargetWnd;
	}
	else
	{
		return AfxGetMainWnd();
	}
}

BOOL CTrayX::SetCallbackMessage(UINT uCallbackMessage)
{
	if (!m_bEnabled)
	{
		return FALSE;
	}

	// Make sure we avoid conflict with other messages
	ASSERT(uCallbackMessage >= WM_APP);

	m_NID.uCallbackMessage	= uCallbackMessage;
	m_NID.uFlags			= NIF_MESSAGE;

	if (m_bHidden)
	{
		return TRUE;
	}
	else
	{
		return (Shell_NotifyIcon(NIM_MODIFY, &m_NID));
	}
}

UINT CTrayX::GetCallbackMessage() const
{
	return (m_NID.uCallbackMessage);
}

UINT_PTR CTrayX::GetTimerID() const
{
	return (m_nTimerID);
}

void CTrayX::MinimizeToTray(CWnd* pWnd)
{
	RemoveTaskbarIcon(pWnd);
	pWnd->ModifyStyle(WS_VISIBLE, 0);
	::SetForegroundWindow(::FindWindow(_T("DesktopExplorerWindow"), NULL));
}

void CTrayX::RestoreFromTray(CWnd* pWnd)
{
	pWnd->SetParent(NULL);
	pWnd->ModifyStyle(0, WS_VISIBLE);
	pWnd->RedrawWindow(NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASE);

	// Move focus away and back again to ensure taskbar icon is recreated
	if (::IsWindow(m_wndInvisible.m_hWnd))
	{
		m_wndInvisible.SetActiveWindow();
	}

	pWnd->SetActiveWindow();
	pWnd->SetForegroundWindow();
}

#define TICK_INTERVAL	300

LRESULT CTrayX::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	if (wParam != m_NID.uID)
	{
		return 0;
	}

	CMenu	Menu;
	CMenu*	pSubMenu;
	CWnd*	pTargetWnd	= GetTargetWnd();
	DWORD	dwUpTick;

	if (!pTargetWnd)
	{
		return 0;
	}

	switch (LOWORD(lParam))
	{
	case WM_LBUTTONDOWN:
		{
			if (!m_bPushed)
			{
				m_bPushed		= TRUE;
				m_dwPushedTick	= GetTickCount();
			}
			else
			{
				m_bPushed	= FALSE;
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			if (m_bPushed)
			{
				m_bPushed	= FALSE;
				dwUpTick	= GetTickCount();

				if ((dwUpTick - m_dwPushedTick) > TICK_INTERVAL)
				{
					if (!Menu.LoadMenu(m_NID.uID))
					{
						return 0;
					}

					pSubMenu	= Menu.GetSubMenu(0);

					if (!pSubMenu)
					{
						return 0;
					}

					OnTrackMenu(pSubMenu);
					pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN, GetSystemMetrics(SM_CXSCREEN) - DRA::SCALEX(1), GetSystemMetrics(SM_CYSCREEN) - m_nTrayHeight, pTargetWnd, NULL);
					pTargetWnd->PostMessage(WM_NULL, 0, 0);
					Menu.DestroyMenu();
				}
				else
				{
					return OnTapIcon();
				}
			}
		}
		break;
	}

	return 1;
}

void CTrayX::OnTrackMenu(CMenu* Menu)
{
}

LRESULT CTrayX::OnTapIcon(void)
{
	return 1;
}

void CTrayX::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != m_uIDTimer)
	{
		ASSERT(FALSE);
		return;
	}

	COleDateTime		CurrentTime	= COleDateTime::GetCurrentTime();
	COleDateTimeSpan	period		= (CurrentTime - m_StartTime);

	if ((m_nAnimationPeriod > 0) && (m_nAnimationPeriod < period.GetTotalSeconds()))
	{
		StopAnimation();
		return;
	}

	StepAnimation();
}

LRESULT CTrayX::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
	m_bShowIconPending	= TRUE;	// !m_bHidden

	InstallIconPending();
	return 0L;
}

void CTrayX::InitializeX()
{
	memset(&m_NID, 0, sizeof(m_NID));

	m_bEnabled				= FALSE;
	m_bHidden				= TRUE;
	m_bRemoved				= TRUE;
	m_uDefaultMenuItemID	= 0;
	m_bDefaultMenuItemByPos	= TRUE;
	m_bShowIconPending		= FALSE;
	m_uIDTimer				= 0;
	m_hSavedIcon			= NULL;
	m_pTargetWnd			= NULL;
	m_uCreationFlags		= 0;
	m_nTrayHeight			= DRA::SCALEY(26);
	m_bPushed				= FALSE;
}

void CTrayX::InstallIconPending()
{
	// Is the icon display pending, and it's not been set as "hidden"?
	if ((!m_bShowIconPending) || (m_bHidden))
	{
		return;
	}

	// Reset the flags to what was used at creation
	m_NID.uFlags	= m_uCreationFlags;

	// Try and recreate the icon
	m_bHidden	= !Shell_NotifyIcon(NIM_ADD, &m_NID);

	// If it's STILL hidden, then have another go next time
	m_bShowIconPending	= m_bHidden;

	ASSERT(m_bHidden == FALSE);
}

BOOL CTrayX::RemoveTaskbarIcon(CWnd* pWnd)
{
	LPCTSTR	pstrOwnerClass	= AfxRegisterWndClass(0);

	// Create static invisible window
	if (!::IsWindow(m_wndInvisible.m_hWnd))
	{
		if (!m_wndInvisible.CreateEx(0, pstrOwnerClass, _T(""), WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0))
		{
			return FALSE;
		}
	}

	pWnd->SetParent(&m_wndInvisible);
	return TRUE;
}

LRESULT CTrayX::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == m_NID.uCallbackMessage)
	{
		return OnTrayNotification(wParam, lParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}
