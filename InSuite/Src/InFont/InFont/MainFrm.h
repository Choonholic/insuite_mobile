// MainFrm.h

#pragma once

#include "FontView.h"

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
	virtual	~CMainFrame();

	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL	OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef _DEBUG
	virtual void	AssertValid() const;
#endif

	CCommandBar	m_wndCommandBar;
	CFontView*	m_pView;

protected:
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	DECLARE_DYNAMIC(CMainFrame)
	DECLARE_MESSAGE_MAP()
};
