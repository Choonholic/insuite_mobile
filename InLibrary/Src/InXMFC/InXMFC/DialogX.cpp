// DialogX.cpp

#include "stdafx.h"
#include "DialogX.h"
#include "DisplayX.h"
#include "ColorX.h"
#include "GraphicsX.h"
#include "FontX.h"

#ifndef DX_NONE
#define DX_NONE		0x00
#define DX_TBNOTUSE	0x01
#define DX_TBUSE	0x02
#define DX_TBSET	0xF0
#endif

#ifndef DX_TIMER
#define DX_TIMER	0x0607
#define DX_INTERVAL	1000	// Check Screen-Orientation Interval
#endif

IMPLEMENT_DYNAMIC(CDialogX, CDialog)

CDialogX::CDialogX() : CDialog()
{
	LOGFONT	lfFont;

	GetTaskBarFont(&lfFont);
	m_fnHeader.CreateFontIndirect(&lfFont);

	m_pCommandBar	= NULL;
}

CDialogX::CDialogX(UINT nIDTemplate, CWnd* pParent) : CDialog(nIDTemplate, pParent)
{
	LOGFONT	lfFont;

	GetTaskBarFont(&lfFont);
	m_fnHeader.CreateFontIndirect(&lfFont);

	m_pCommandBar	= NULL;
}

CDialogX::CDialogX(LPCTSTR lpszTemplateName, CWnd* pParent) : CDialog(lpszTemplateName, pParent)
{
	LOGFONT	lfFont;

	GetTaskBarFont(&lfFont);
	m_fnHeader.CreateFontIndirect(&lfFont);

	m_pCommandBar	= NULL;
}

CDialogX::~CDialogX()
{
	RemoveCommandBar();
}

void CDialogX::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDialogX, CDialog)
	ON_WM_ACTIVATE()
	ON_WM_SETTINGCHANGE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CDialogX::OnScreenRotate()
{
	CenterWindow();
}

void CDialogX::SetItemFont(HFONT hFont)
{
	CFont*	pFont	= CFont::FromHandle(hFont);

	SetFont(pFont);
}

void CDialogX::SetHeaderItem(CWnd* pWnd)
{
	if (pWnd != NULL)
	{
		pWnd->SetFont(&m_fnHeader);
	}
}

void CDialogX::SetHeaderItem(UINT nID)
{
	CWnd*	pWnd	= GetDlgItem(nID);

	SetHeaderItem(pWnd);
}

void CDialogX::DrawHeaderItem(CDC& dc, CWnd* pWnd)
{
	if (pWnd != NULL)
	{
		CRect	rc;
		CRect	rcLine;

		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		rcLine.SetRect(rc.left, rc.bottom + DRA::SCALEY(2), rc.right, rc.bottom + DRA::SCALEY(3));
		FillHGRectEx(dc.GetSafeHdc(), rcLine, GetUserShellColor(SHCOLOR_TASKBAR_LEFTEDGE), RGB(0xF0, 0xF0, 0xF0));
	}
}

void CDialogX::DrawHeaderItem(CDC& dc, UINT nID)
{
	CWnd*	pWnd	= GetDlgItem(nID);

	DrawHeaderItem(dc, pWnd);
}

void CDialogX::ShowDoneButton()
{
	ModifyStyle(WS_NONAVDONEBUTTON, 0, SWP_NOSIZE); 
	::SHDoneButton(m_hWnd, SHDB_SHOW);
}

void CDialogX::HideDoneButton()
{
	ModifyStyle(0, WS_NONAVDONEBUTTON, SWP_NOSIZE); 
	::SHDoneButton(m_hWnd, SHDB_HIDE);
}

CCommandBar* CDialogX::GetCommandBarCtrl()
{
	return m_pCommandBar;
}

void CDialogX::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (m_bFullScreen)
	{
		CDialog::OnActivate(nState, pWndOther, bMinimized);
	}
	else
	{
		CWnd::OnActivate(nState, pWndOther, bMinimized);
	}
}

void CDialogX::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	if (uFlags == SETTINGCHANGE_RESET)
	{
		m_sizeScreen.cx	= GetSystemMetrics(SM_CXSCREEN);
		m_sizeScreen.cy	= GetSystemMetrics(SM_CYSCREEN);

		OnScreenRotate();
		return;
	}

	if (m_bFullScreen)
	{
		CDialog::OnSettingChange(uFlags, lpszSection);
	}
	else
	{
		CWnd::OnSettingChange(uFlags, lpszSection);
	}
}

void CDialogX::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID)
	{
		SIZE	sizeCurrent;

		sizeCurrent.cx	= GetSystemMetrics(SM_CXSCREEN);
		sizeCurrent.cy	= GetSystemMetrics(SM_CYSCREEN);

		if ((sizeCurrent.cx != m_sizeScreen.cx) || (sizeCurrent.cy != m_sizeScreen.cy))
		{
			m_sizeScreen.cx	= sizeCurrent.cx;
			m_sizeScreen.cy	= sizeCurrent.cy;

			OnScreenRotate();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CDialogX::OnInitDialog()
{
	CDialog::OnInitDialog();
	CreateCommandBar();
	SetScreenDetector();
	CenterWindow();
	return TRUE;
}

void CDialogX::OnDestroy()
{
	RemoveScreenDetector();
	CDialog::OnDestroy();
}

BOOL CDialogX::CreateCommandBar()
{
	m_pCommandBar	= new CCommandBar;

	if (m_pCommandBar != NULL)
	{
		if (m_pCommandBar->Create(this, WS_CHILD | CBRS_TOP) == TRUE)
		{
			return TRUE;
		}

		RemoveCommandBar();
	}

	return FALSE;
}

void CDialogX::RemoveCommandBar()
{
	if (m_pCommandBar != NULL)
	{
		delete m_pCommandBar;

		m_pCommandBar	= NULL;
	}
}

void CDialogX::SetScreenDetector()
{
	if (m_bFullScreen == FALSE)
	{
		m_nTimerID	= SetTimer(DX_TIMER, DX_INTERVAL, NULL);
	}
}

void CDialogX::RemoveScreenDetector()
{
	if (m_bFullScreen == FALSE)
	{
		KillTimer(m_nTimerID);
	}
}
