// PropertySheetX.h

#pragma once

class AFX_EXT_CLASS CPropertySheetX : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertySheetX)

public:
	CPropertySheetX(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropertySheetX(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual	~CPropertySheetX();

	void	SetTitle(UINT nIDTitle);
	void	SetLink(UINT nIDLink);
	void	SetTitle(LPCTSTR pszTitle);
	void	SetLink(LPCTSTR pszLink);

	CCommandBar*	GetCommandBarCtrl();

	virtual BOOL	OnInitDialog();
	virtual BOOL	ContinueModal();
	afx_msg void	OnPaint();

protected:
	BOOL	CreateCommandBar();
	void	RemoveCommandBar();

	void					HookCallback();
	static int	CALLBACK	PropertySheetXCallBack(HWND hWnd, UINT message, LPARAM lParam);

	static CString				m_strLink;
	static CString				m_strTitle;
	static PFNPROPSHEETCALLBACK	m_pCallBack;

	CCommandBar*	m_pCommandBar;

	DECLARE_MESSAGE_MAP()
};
