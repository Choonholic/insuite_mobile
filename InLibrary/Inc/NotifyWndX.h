// NotifyWndX.h

#pragma once

#ifndef NWX_STATUS
#define NWX_STATUS
#define NWX_NONE	0x00
#define NWX_OPENING	0x01
#define NWX_CLOSING	0x02
#define NWX_SHOWING	0x03
#endif

class AFX_EXT_CLASS CNotifyWndX : public CFrameWnd
{
	DECLARE_DYNCREATE(CNotifyWndX)

public:
	CNotifyWndX();
	CNotifyWndX(DWORD dwTime);
	CNotifyWndX(int nWidth, int nHeight);
	CNotifyWndX(int nWidth, int nHeight, DWORD dwTime);
	virtual	~CNotifyWndX();

	virtual CRect	CalcWndRect(BOOL bFull);
	virtual BOOL	Show();
	virtual void	Hide();
	virtual BOOL	ShowImmediately();
	virtual void	HideImmediately();

	void	Close();

	int		GetStatus();
	void	SetShowTime(DWORD dwTime);
	void	SetWndSize(int nWidth, int nHeight);
	void	SetPopupSpeed(int nSpeed);

	afx_msg void	OnDestroy();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	afx_msg void	OnClose();
	afx_msg void	OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void	OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

protected:
	void	InitializeX();

	DWORD	m_dwShowTime;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nSpeed;

	UINT	m_nTimerID;
	UINT	m_nDetectID;
	SIZE	m_sizeScreen;
	int		m_nStatus;
	BOOL	m_bSafeToClose;

	DECLARE_MESSAGE_MAP()
};
