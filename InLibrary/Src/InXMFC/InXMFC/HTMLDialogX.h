// HTMLDialogX.h

#pragma once

#include "DialogX.h"

class AFX_EXT_CLASS CHTMLDialogX : public CDialogX
{
	DECLARE_DYNAMIC(CHTMLDialogX)

public:
	CHTMLDialogX(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual	~CHTMLDialogX();

	virtual void	OnLink(const CString& strLink);
	virtual void	OnComplete(const CString& strLink);

	void	SetHTML(const CString& strHTML);
	void	RegisterImage(int nResourceId, const CString& strImage);
	void	RegisterImage(CBitmap* pBitmap, const CString& strImage);

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

protected:
	virtual void	CreateHTMLWindow();
	virtual void	DeleteImages();

	virtual LRESULT	WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	static HINSTANCE	m_hHTMLViewInstance;
	HWND				m_hWndHTML;
	CMapStringToOb		m_imagesCache;

	DECLARE_MESSAGE_MAP()
};
