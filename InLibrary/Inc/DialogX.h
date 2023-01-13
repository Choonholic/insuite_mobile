// DialogX.h

#pragma once

class AFX_EXT_CLASS CDialogX : public CDialog
{
	DECLARE_DYNAMIC(CDialogX)

public:
	CDialogX();
	CDialogX(UINT nIDTemplate, CWnd* pParent);
	CDialogX(LPCTSTR lpszTemplateName, CWnd* pParent);
	virtual	~CDialogX();

	virtual void	OnScreenRotate();

	void	SetItemFont(HFONT hFont);

	void	SetHeaderItem(CWnd* pWnd);
	void	SetHeaderItem(UINT nID);
	void	DrawHeaderItem(CDC& dc, CWnd* pWnd);
	void	DrawHeaderItem(CDC& dc, UINT nID);

	void	ShowDoneButton();
	void	HideDoneButton();

	CCommandBar*	GetCommandBarCtrl();

	afx_msg void	OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void	OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	virtual BOOL	OnInitDialog();
	afx_msg void	OnDestroy();

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	BOOL	CreateCommandBar();
	void	RemoveCommandBar();
	void	SetScreenDetector();
	void	RemoveScreenDetector();

	CFont			m_fnHeader;
	SIZE			m_sizeScreen;
	UINT_PTR		m_nTimerID;
	CCommandBar*	m_pCommandBar;

	DECLARE_MESSAGE_MAP()
};
