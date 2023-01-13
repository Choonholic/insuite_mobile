// RichInkWndX.cpp

#include "stdafx.h"

#pragma warning(disable:4819)
#include "RichInkWndX.h"
#pragma warning(default:4819)

IMPLEMENT_DYNAMIC(CRichInkWndX, CWnd)

CRichInkWndX::CRichInkWndX()
{
}

CRichInkWndX::~CRichInkWndX()
{
}

BEGIN_MESSAGE_MAP(CRichInkWndX, CWnd)
END_MESSAGE_MAP()

BOOL CRichInkWndX::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	HWND		hWnd;
	HINSTANCE	hInstance	= AfxGetInstanceHandle();
	BOOL		bValid		= FALSE;

	hWnd	= ::CreateWindowEx(0, WC_RICHINK, _T(""), dwStyle, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, pParentWnd->GetSafeHwnd(), (HMENU)nID, hInstance, NULL);

	if (hWnd != NULL)
	{
		bValid	= SubclassWindow(hWnd);
	}

	return bValid;
}

inline BOOL CRichInkWndX::CanUndo()
{
	ASSERT(IsWindow(m_hWnd));
	return (SendMessage(EM_CANUNDO));
}

inline BOOL CRichInkWndX::CanRedo()
{
	ASSERT(IsWindow(m_hWnd));
	return (SendMessage(EM_CANREDO));
}

inline BOOL CRichInkWndX::CanPaste(UINT uFormat)
{
	ASSERT(IsWindow(m_hWnd));
	return (SendMessage(EM_CANPASTE, uFormat));
}

inline void CRichInkWndX::UndoEvent()
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_UNDOEVENT);
}

inline void CRichInkWndX::RedoEvent()
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_REDOEVENT);
}

inline void CRichInkWndX::ClearAll(BOOL bDirty)
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_CLEARALL, (WPARAM)bDirty);
}

inline UINT CRichInkWndX::GetPageStyle()
{
	ASSERT(IsWindow(m_hWnd));
	return (SendMessage(EM_GETPAGESTYLE));
}

inline UINT CRichInkWndX::GetPenMode()
{
	ASSERT(IsWindow(m_hWnd));
	return (SendMessage(EM_GETPENMODE));
}

inline UINT CRichInkWndX::GetView()
{
	ASSERT(IsWindow(m_hWnd));
	return (SendMessage(EM_GETVIEW));
}

inline UINT CRichInkWndX::GetWrapMode()
{
	ASSERT(IsWindow(m_hWnd));
	return (SendMessage(EM_GETWRAPMODE));
}

inline UINT CRichInkWndX::GetZoomPercent()
{
	ASSERT(IsWindow(m_hWnd));
	return (SendMessage(EM_GETZOOMPERCENT));
}

inline void CRichInkWndX::SetPageStyle(UINT nPageStyle)
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_SETPAGESTYLE, nPageStyle);
}

inline void CRichInkWndX::SetPenMode(UINT nPenMode)
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_SETPENMODE, nPenMode);
}

inline void CRichInkWndX::SetView(UINT nView)
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_SETVIEW, nView);
}

inline void CRichInkWndX::SetWrapMode(UINT nWrapMode)
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_SETWRAPMODE, nWrapMode);
}

inline void CRichInkWndX::SetZoomPercent(UINT nZoomPercent)
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_SETZOOMPERCENT, nZoomPercent);
}

inline void CRichInkWndX::SetViewAttributes(const VIEWATTRIBUTES* pViewAttr)
{
	ASSERT(IsWindow(m_hWnd));
	SendMessage(EM_SETVIEWATTRIBUTES, 0, (LPARAM)pViewAttr);
}

inline UINT CRichInkWndX::SetInkLayer(UINT nInkLayer)
{
	ASSERT(IsWindow(m_hWnd));
	return SendMessage(EM_SETINKLAYER, nInkLayer);
}

inline void CRichInkWndX::InsertLinks(LPCTSTR pszLink, UINT nLength)
{
	ASSERT(IsWindow(m_hWnd));

	if (nLength == -1)
	{
		nLength	= _tcslen(pszLink);
	}

	SendMessage(EM_INSERTLINKS, nLength, (LPARAM)pszLink);
}

inline BOOL CRichInkWndX::StreamIn(UINT nFormat, EDITSTREAM* pStream)
{
	ASSERT(IsWindow(m_hWnd));
	return (!SendMessage(EM_STREAMIN, nFormat, (LPARAM)pStream));
}

inline BOOL CRichInkWndX::StreamOut(UINT nFormat, EDITSTREAM* pStream)
{
	ASSERT(IsWindow(m_hWnd));
	return (!SendMessage(EM_STREAMOUT, nFormat, (LPARAM)pStream));
}
