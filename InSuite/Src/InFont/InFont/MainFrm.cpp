// MainFrm.cpp

#include "stdafx.h"
#include "InFont.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

#ifdef WIN32_PLATFORM_PSPC
#define TOOLBAR_HEIGHT	24
#endif

const DWORD	dwAdornmentFlags	= 0;

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CCreateContext	ccx;

	ccx.m_pNewViewClass	= RUNTIME_CLASS(CFontView);
	m_pView				= DYNAMIC_DOWNCAST(CFontView, this->CreateView(&ccx));

	if (!m_pView)
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndCommandBar.Create(this) || !m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) || !m_wndCommandBar.AddAdornments(dwAdornmentFlags))
	{
		TRACE0("Failed to create CommandBar\n");
		return -1;
	}

	CRect	rect;

	GetWindowRect(&rect);

	// Magic Height (52) = Menu Bar Height (26) + Task Bar Height (26)
	rect.right	= rect.left + GetSystemMetrics(SM_CXSCREEN);
	rect.bottom	= rect.top + GetSystemMetrics(SM_CYSCREEN) - DRA::SCALEY(52);

	MoveWindow(&rect);
	m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);
	RecalcLayout();
	m_pView->ShowWindow(SW_SHOW);
	SetActiveView(m_pView);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
	{
		return FALSE;
	}

	cs.lpszClass	= AfxRegisterWndClass(0);

	return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}
#endif

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	m_pView->SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (m_pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if (m_wndCommandBar.GetSafeHwnd() != NULL)
	{
		ASSERT((m_pView != NULL) && (m_pView->IsKindOf(RUNTIME_CLASS(CFontView))));

		CRect	rect;
		CSize	size;

		GetClientRect(&rect);
		size.SetSize(rect.Width() - (GetSystemMetrics(SM_CXBORDER) * 2), rect.Height() - (GetSystemMetrics(SM_CYBORDER) * 2));
		m_pView->SetScrollSizes(MM_TEXT, size);
		m_pView->MoveWindow(0, 0, rect.Width(), rect.Height(), TRUE);
	}
}
