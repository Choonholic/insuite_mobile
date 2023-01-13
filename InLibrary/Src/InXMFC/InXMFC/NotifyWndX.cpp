// NotifyWndX.cpp

#include "stdafx.h"
#include "NotifyWndX.h"

IMPLEMENT_DYNCREATE(CNotifyWndX, CFrameWnd)

CNotifyWndX::CNotifyWndX()
{
	InitializeX();
}

CNotifyWndX::CNotifyWndX(DWORD dwTime)
{
	InitializeX();

	m_dwShowTime	= dwTime;
}

CNotifyWndX::CNotifyWndX(int nWidth, int nHeight)
{
	InitializeX();

	m_nWidth	= nWidth;
	m_nHeight	= nHeight;
}

CNotifyWndX::CNotifyWndX(int nWidth, int nHeight, DWORD dwTime)
{
	InitializeX();

	m_nWidth		= nWidth;
	m_nHeight		= nHeight;
	m_dwShowTime	= dwTime;
}

CNotifyWndX::~CNotifyWndX()
{
}

BEGIN_MESSAGE_MAP(CNotifyWndX, CFrameWnd)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

CRect CNotifyWndX::CalcWndRect(BOOL bFull)
{
	CRect	rcInitial;

	m_sizeScreen.cx		= GetSystemMetrics(SM_CXSCREEN);
	m_sizeScreen.cy		= GetSystemMetrics(SM_CYSCREEN);
	rcInitial.right		= GetSystemMetrics(SM_CXSCREEN);
	rcInitial.bottom	= GetSystemMetrics(SM_CYSCREEN) - DRA::SCALEY(26);
	rcInitial.left		= rcInitial.right - m_nWidth;
	rcInitial.top		= rcInitial.bottom - ((bFull == TRUE) ? m_nHeight : 0);

	return rcInitial;
}

BOOL CNotifyWndX::Show()
{
	CRect	rcInitial	= CalcWndRect(FALSE);

	if (!Create(NULL, _T("CNotifyWndX"), WS_POPUP, rcInitial, NULL, NULL, WS_EX_TOPMOST | WS_EX_TOOLWINDOW))
	{
		return FALSE;
	}

	ASSERT(m_nTimerID == 0);

	m_nTimerID	= SetTimer(0x0721, 1, NULL);

	ASSERT(m_nDetectID == 0);

	m_nDetectID	= SetTimer(0x0722, 1000, NULL);

	ShowWindow(SW_SHOWNOACTIVATE);

	m_nStatus	= NWX_OPENING;

	return TRUE;
}

void CNotifyWndX::Hide()
{
	if ((m_nStatus == NWX_OPENING) || (m_nStatus == NWX_SHOWING))
	{
		m_nStatus	= NWX_CLOSING;

		SetTimer(m_nTimerID, 1, NULL);
	}
}

BOOL CNotifyWndX::ShowImmediately()
{
	CRect	rcInitial	= CalcWndRect(TRUE);

	if (!Create(NULL, _T("CNotifyWndX"), WS_POPUP, rcInitial, NULL, NULL, WS_EX_TOPMOST | WS_EX_TOOLWINDOW))
	{
		return FALSE;
	}

	ASSERT(m_nTimerID == 0);

	m_nTimerID	= SetTimer(0x0721, m_dwShowTime, NULL);

	ASSERT(m_nDetectID == 0);

	m_nDetectID	= SetTimer(0x0722, 1000, NULL);

	ShowWindow(SW_SHOWNOACTIVATE);

	m_nStatus	= NWX_SHOWING;

	return TRUE;
}

void CNotifyWndX::HideImmediately()
{
	if ((m_nStatus == NWX_OPENING) || (m_nStatus == NWX_SHOWING))
	{
		CRect	rcWnd;

		m_nStatus	= NWX_CLOSING;

		ShowWindow(SW_HIDE);
		GetWindowRect(&rcWnd);

		rcWnd.top	= rcWnd.bottom;

		MoveWindow(&rcWnd);
	}
}

void CNotifyWndX::Close()
{
	m_bSafeToClose	= TRUE;

	SendMessage(WM_CLOSE);
}

int CNotifyWndX::GetStatus()
{
	return m_nStatus;
}

void CNotifyWndX::SetShowTime(DWORD dwTime)
{
	m_dwShowTime	= dwTime;
}

void CNotifyWndX::SetWndSize(int nWidth, int nHeight)
{
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;
}

void CNotifyWndX::SetPopupSpeed(int nSpeed)
{
	ASSERT(nSpeed > 0);

	m_nSpeed	= nSpeed;
}

void CNotifyWndX::OnDestroy()
{
	if (m_nTimerID != 0)
	{
		KillTimer(m_nTimerID);

		m_nTimerID	= 0;
	}

	if (m_nDetectID != 0)
	{
		KillTimer(m_nDetectID);

		m_nDetectID	= 0;
	}

	CFrameWnd::OnDestroy();
}

void CNotifyWndX::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID)
	{
		switch (m_nStatus)
		{
		case NWX_OPENING:
			{
				CRect	rcWnd;

				GetWindowRect(&rcWnd);

				if (rcWnd.Height() >= m_nHeight)
				{
					m_nStatus	= NWX_SHOWING;

					SetTimer(m_nTimerID, m_dwShowTime, NULL);
				}
				else
				{
					rcWnd.top	-= m_nSpeed;

					if (rcWnd.Height() >= m_nHeight)
					{
						rcWnd.top	= rcWnd.bottom - m_nHeight;
					}

					MoveWindow(&rcWnd);
				}
			}
			break;
		case NWX_SHOWING:
			{
				Hide();
			}
			break;
		case NWX_CLOSING:
			{
				CRect	rcWnd;

				GetWindowRect(&rcWnd);

				rcWnd.top	+= m_nSpeed;

				if (rcWnd.Height() <= DRA::SCALEY(0))
				{
					ShowWindow(SW_HIDE);
				}
				else
				{
					MoveWindow(&rcWnd);
				}
			}
			break;
		default:
			{
				ASSERT(FALSE);
			}
			break;
		}
	}
	else if (nIDEvent == m_nDetectID)
	{
		int	nWidth	= GetSystemMetrics(SM_CXSCREEN);
		int	nHeight	= GetSystemMetrics(SM_CYSCREEN);

		if ((m_sizeScreen.cx != nWidth) || (m_sizeScreen.cy != nHeight))
		{
			m_sizeScreen.cx	= nWidth;
			m_sizeScreen.cy	= nHeight;

			CRect	rcWnd;
			CRect	rcInitial	= CalcWndRect(FALSE);

			GetWindowRect(&rcWnd);

			rcInitial.top	= rcInitial.bottom - rcWnd.Height();

			MoveWindow(&rcInitial);
		}

	}
	else
	{
		CFrameWnd::OnTimer(nIDEvent);
	}
}

void CNotifyWndX::OnClose()
{
	if (m_bSafeToClose == TRUE)
	{
		CFrameWnd::OnClose();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

void CNotifyWndX::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CNotifyWndX::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CWnd::OnSettingChange(uFlags, lpszSection);
}

void CNotifyWndX::InitializeX()
{
	m_dwShowTime	= 5000;
	m_nWidth		= DRA::SCALEX(160);
	m_nHeight		= DRA::SCALEY(100);
	m_nSpeed		= DRA::SCALEY(4);
	m_nTimerID		= 0;
	m_nDetectID		= 0;
	m_sizeScreen.cx	= 0;
	m_sizeScreen.cy	= 0;
	m_nStatus		= NWX_NONE;
	m_bSafeToClose	= FALSE;
}
