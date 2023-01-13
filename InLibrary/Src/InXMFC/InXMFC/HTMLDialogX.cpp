// HTMLDialogX.cpp

#include "stdafx.h"
#include "HTMLDialogX.h"

#pragma warning(disable:4819)
#include "htmlctrl.h"
#pragma warning(default:4819)

#pragma comment(lib, "htmlview.lib")

IMPLEMENT_DYNAMIC(CHTMLDialogX, CDialogX)

HINSTANCE CHTMLDialogX::m_hHTMLViewInstance	= 0;

CHTMLDialogX::CHTMLDialogX(UINT nIDTemplate, CWnd* pParent) : CDialogX(nIDTemplate, pParent)
{
}

CHTMLDialogX::~CHTMLDialogX()
{
}

BEGIN_MESSAGE_MAP(CHTMLDialogX, CDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CHTMLDialogX::OnLink(const CString& strLink)
{
}

void CHTMLDialogX::OnComplete(const CString& strLink)
{
}

void CHTMLDialogX::SetHTML(const CString& strHTML)
{
	::SendMessage(m_hWndHTML, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)_T(""));
	::SendMessage(m_hWndHTML, DTM_ADDTEXTW, FALSE, (LPARAM)(LPCTSTR)strHTML);
	::SendMessage(m_hWndHTML, DTM_ENDOFSOURCE, 0, 0);
}

void CHTMLDialogX::RegisterImage(int nResourceId, const CString& strImage)
{
	CBitmap*	pBitmap	= new CBitmap();

	pBitmap->LoadBitmap(nResourceId);
	m_imagesCache.SetAt(strImage, (CObject*)pBitmap);
}

void CHTMLDialogX::RegisterImage(CBitmap* pBitmap, const CString& strImage)
{
	m_imagesCache.SetAt(strImage, (CObject*)pBitmap);
}

BOOL CHTMLDialogX::OnInitDialog()
{
	CDialogX::OnInitDialog();
	CreateHTMLWindow();
	return TRUE;
}

void CHTMLDialogX::OnSize(UINT nType, int cx, int cy)
{
	CDialogX::OnSize(nType, cx, cy);
	::MoveWindow(m_hWndHTML, 0, 0, cx, cy, TRUE);
}

void CHTMLDialogX::CreateHTMLWindow()
{
	if (m_hHTMLViewInstance == 0)
	{
		m_hHTMLViewInstance	= ::LoadLibrary(_T("htmlview.dll"));
	}

	VERIFY(InitHTMLControl(AfxGetInstanceHandle()));

	DWORD	dwStyle	= WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
	RECT	rc;

	GetClientRect(&rc);

	m_hWndHTML	= ::CreateWindow(DISPLAYCLASS, NULL, dwStyle, rc.left, rc.top, rc.right, rc.bottom, m_hWnd, 0, m_hHTMLViewInstance, NULL);

	::SetWindowLong(m_hWndHTML, GWL_ID, 12321);
	::SetFocus(m_hWndHTML);
	::SendMessage(m_hWndHTML, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)_T(""));
}

void CHTMLDialogX::DeleteImages()
{
	POSITION	pos	= m_imagesCache.GetStartPosition();
	CString		key;
	CBitmap*	bmp;

	while (pos != NULL)
	{
		m_imagesCache.GetNextAssoc(pos, key, (CObject *&)bmp);

		delete bmp;

		m_imagesCache.RemoveKey(key);
	}
}

LRESULT CHTMLDialogX::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFY:
		{
			NM_HTMLVIEW*	pnmHTML	= (NM_HTMLVIEW*)lParam;
			LPNMHDR			pnmh	= (LPNMHDR)&(pnmHTML->hdr);

			switch (pnmh->code)
			{
			case NM_HOTSPOT:
				{
					OnLink(pnmHTML->szTarget);
					return (LRESULT)(1);
				}
			case NM_DOCUMENTCOMPLETE:
				{
					OnComplete(pnmHTML->szTarget);
					return (LRESULT)(1);
				}
			case NM_INLINE_IMAGE:
				{
					CString		strText			= pnmHTML->szTarget;
					DWORD		dwCookieValue	= pnmHTML->dwCookie;
					CBitmap*	pBitmap;

					if (!m_imagesCache.Lookup(strText, (CObject*&)pBitmap))
					{
						::SendMessage(m_hWndHTML, DTM_IMAGEFAIL, 0, (LPARAM)(INLINEIMAGEINFO FAR*)dwCookieValue);
						return (LRESULT)(1);
					}

					INLINEIMAGEINFO	imageInfo;

					imageInfo.dwCookie		= dwCookieValue;
					imageInfo.bOwnBitmap	= FALSE;
					imageInfo.hbm			= (HBITMAP)(*pBitmap);

					CSize	size	= pBitmap->GetBitmapDimension();

					imageInfo.iOrigWidth	= size.cx;
					imageInfo.iOrigHeight	= size.cy;

					::SendMessage(m_hWndHTML, DTM_SETIMAGE, 0, (LPARAM)(INLINEIMAGEINFO FAR*)&imageInfo);
					return (LRESULT)(1);
				}
			}
		}
		break;
	}

	return CDialogX::WindowProc(message, wParam, lParam);
}
