// TrayX.h

#pragma once

#ifndef NIIF_NONE
#define NIIF_NONE	0
#endif

class AFX_EXT_CLASS CTrayX : public CWnd
{
	DECLARE_DYNAMIC(CTrayX)

public:
	CTrayX();
	CTrayX(CWnd* pParentWnd, UINT uCallbackMessage, LPCTSTR lpszTip, HICON hIcon, UINT uID, BOOL bHidden = FALSE);
	virtual	~CTrayX();

	BOOL	Enabled();
	BOOL	Visible();

	// Create the tray icon
	BOOL	Create(CWnd* pParentWnd, UINT uCallbackMessage, LPCTSTR lpszTip, HICON hIcon, UINT uID, BOOL bHidden = FALSE);

	// Change or retrieve the Tooltip text
	BOOL	SetTooltipText(LPCTSTR lpszTooltipText);
	BOOL	SetTooltipText(UINT nID);
	CString	GetTooltipText() const;

	// Change or retrieve the icon displayed
	BOOL	SetIcon(HICON hIcon);
	BOOL	SetIcon(LPCTSTR lpszIconName);
	BOOL	SetIcon(UINT nIDResource);
	HICON	GetIcon() const;

	BOOL	ShowIcon();
	BOOL	HideIcon();
	BOOL	AddIcon();
	BOOL	RemoveIcon();
	BOOL	MoveToRight();

	// Icon animation
	BOOL	SetIconList(UINT uFirstIconID, UINT uLastIconID); 
	BOOL	SetIconList(HICON* pHIconList, UINT nNumIcons); 
	BOOL	Animate(UINT nDelayMilliSeconds, int nNumSeconds = -1);
	BOOL	StepAnimation();
	BOOL	StopAnimation();

	// Change menu default item
	BOOL	SetMenuDefaultItem(UINT uItem, BOOL bByPos);
	void	GetMenuDefaultItem(UINT& uItem, BOOL& bByPos);

	// Change or retrieve the window to send notification messages to
	BOOL	SetNotificationWnd(CWnd* pNotifyWnd);
	CWnd*	GetNotificationWnd() const;

	// Change or retrieve the window to send menu commands to
	BOOL	SetTargetWnd(CWnd* pTargetWnd);
	CWnd*	GetTargetWnd() const;

	// Change or retrieve notification messages sent to the window
	BOOL	SetCallbackMessage(UINT uCallbackMessage);
	UINT	GetCallbackMessage() const;

	UINT_PTR	GetTimerID() const;

	// Static functions
	static void	MinimizeToTray(CWnd* pWnd);
	static void	RestoreFromTray(CWnd* pWnd);

	// Default handler for tray notification message
	virtual LRESULT	OnTrayNotification(WPARAM wParam, LPARAM lParam);
	virtual void	OnTrackMenu(CMenu* Menu);
	virtual LRESULT	OnTapIcon(void);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);

	LRESULT	OnTaskbarCreated(WPARAM wParam, LPARAM lParam);

protected:
	void	InitializeX();
	void	InstallIconPending();

	static BOOL		RemoveTaskbarIcon(CWnd* pWnd);
	virtual LRESULT	WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	// Information
	NOTIFYICONDATA	m_NID;
	BOOL			m_bEnabled;			// Does O/S support tray icon?
	BOOL			m_bHidden;			// Has the icon been hidden?
	BOOL			m_bRemoved;			// Has the icon been removed?
	BOOL			m_bShowIconPending;	// Show the icon once tha taskbar has been created
	CWnd*			m_pTargetWnd;		// Window that menu commands are sent

	CArray<HICON, HICON>	m_arrIcons;

	BOOL			m_bPushed;
	DWORD			m_dwPushedTick;

	int				m_nTrayHeight;
	UINT_PTR		m_uIDTimer;
	INT_PTR			m_nCurrentIcon;
	COleDateTime	m_StartTime;
	UINT			m_nAnimationPeriod;
	HICON			m_hSavedIcon;
	UINT			m_uDefaultMenuItemID;
	BOOL			m_bDefaultMenuItemByPos;
	UINT			m_uCreationFlags;

	static const UINT	m_nTimerID;
	static UINT			m_nMaxTooltipLength;
	static const UINT	m_nTaskbarCreatedMsg;
	static CWnd			m_wndInvisible;

	DECLARE_MESSAGE_MAP()
};
