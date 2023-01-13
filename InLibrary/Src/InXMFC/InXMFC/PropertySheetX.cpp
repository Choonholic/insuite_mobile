// PropertySheetX.cpp

#include "stdafx.h"
#include "PropertySheetX.h"

IMPLEMENT_DYNAMIC(CPropertySheetX, CPropertySheet)

PFNPROPSHEETCALLBACK	CPropertySheetX::m_pCallBack	= NULL;

CString	CPropertySheetX::m_strLink;
CString	CPropertySheetX::m_strTitle;

CPropertySheetX::CPropertySheetX(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_pCommandBar	= NULL;

	m_strTitle.LoadString(nIDCaption);
	HookCallback();
}

CPropertySheetX::CPropertySheetX(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_pCommandBar	= NULL;
	m_strTitle		= pszCaption;

	HookCallback();
}

CPropertySheetX::~CPropertySheetX()
{
	RemoveCommandBar();
}

BEGIN_MESSAGE_MAP(CPropertySheetX, CPropertySheet)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CPropertySheetX::SetTitle(UINT nIDTitle)
{
	m_strTitle.LoadString(nIDTitle);
}

void CPropertySheetX::SetLink(UINT nIDLink)
{
	m_strLink.LoadString(nIDLink);
}

void CPropertySheetX::SetTitle(LPCTSTR pszTitle)
{
	m_strTitle	= pszTitle;
}

void CPropertySheetX::SetLink(LPCTSTR pszLink)
{
	m_strLink	= pszLink;
}

CCommandBar* CPropertySheetX::GetCommandBarCtrl()
{
	return m_pCommandBar;
}

BOOL CPropertySheetX::OnInitDialog()
{
	CreateCommandBar();
	return CPropertySheet::OnInitDialog();
}

BOOL CPropertySheetX::ContinueModal()
{
	if (!CWnd::ContinueModal())
	{
		return FALSE;
	}

	return TRUE;
}

void CPropertySheetX::OnPaint()
{
	CPaintDC	dc(this);

	CWnd::OnPaint();

	if (m_pCommandBar != NULL)
	{
		::SetForegroundWindow(m_pCommandBar->m_hWnd);
	}
}

BOOL CPropertySheetX::CreateCommandBar()
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

void CPropertySheetX::RemoveCommandBar()
{
	if (m_pCommandBar != NULL)
	{
		delete m_pCommandBar;

		m_pCommandBar	= NULL;
	}
}

void CPropertySheetX::HookCallback()
{
	m_pCallBack			= m_psh.pfnCallback;
	m_psh.pfnCallback	= PropertySheetXCallBack;
}

int CALLBACK CPropertySheetX::PropertySheetXCallBack(HWND hWnd, UINT message, LPARAM lParam)
{
	switch (message)
	{
	case PSCB_GETLINKTEXT:
		{
			if (!m_strLink.IsEmpty())
			{
				LPTSTR	pBuf	= (LPTSTR)lParam;

				wcscpy(pBuf, m_strLink);
			}
		}
		break;
	case PSCB_GETTITLE:
		{
			if (!m_strTitle.IsEmpty())
			{
				LPTSTR	pBuf	= (LPTSTR)lParam;

				wcscpy(pBuf, m_strTitle);
			}
		}
		break;
	}

	return (m_pCallBack)(hWnd, message, lParam);
}
