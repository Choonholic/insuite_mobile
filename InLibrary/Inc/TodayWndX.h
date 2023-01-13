// TodayWndX.h

#pragma once

#include "InAPI.h"
#include <todaycmn.h>

#ifndef TWXHEIGHT_DEFAULT
#define TWXHEIGHT_DEFAULT	(-1)
#endif

class INAPI_CLASS CTodayWndX
{
public:
	CTodayWndX(void);
	CTodayWndX(HINSTANCE hInstance, LPCTSTR lpszClassName, LPCTSTR lpszWindowName);
	CTodayWndX(HINSTANCE hInstance, HINSTANCE hResource, LPCTSTR lpszClassName, LPCTSTR lpszWindowName);
	virtual	~CTodayWndX(void);

	// Manipulate Window class
	void	RegisterTodayClass(WNDPROC wndProc);
	void	UnregisterTodayClass(void);

	// Initialize Class
	virtual void	Initialize(void);

	// Create Window
	BOOL	Create(HWND hWndParent, DWORD dwStyle = WS_VISIBLE | WS_CHILD);

	// Update Window
	void	Refresh(void);
	void	RefreshWindow(BOOL bShow = FALSE);

	// Invoke Options Dialog
	BOOL	InvokeOptions(void);

	// Main Message Loop
	virtual LRESULT	CALLBACK	TodayWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	CALLBACK	TodayDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// Drawing Methods
	void	PrepareTodayDC(void);

	HDC		GetTodayDCDefault(void);
	HDC		GetTodayDCClip(void);

	BOOL	PaintTodayDCDefault(HDC hDC);
	BOOL	PaintTodayDCClip(HDC hDC, LPRECT rcClip);

	BOOL	PaintTodayDCCombine(void);

	int		ReleaseTodayDCDefault(HDC hDC);
	int		ReleaseTodayDCClip(HDC hDC);

	int		ReleaseTodayDC(void);

	virtual void	DrawSurface(HDC hDC, BOOL bForcedDefault = FALSE);
	virtual void	DrawSurface(HDC hDC, LPRECT lpRect, BOOL bForcedDefault = FALSE);

	// Setting Methods
	void	SetInstance(HINSTANCE hInstance);
	void	SetResourceInstance(HINSTANCE hInstance);
	void	SetClassInfo(LPCTSTR lpszClassName, LPCTSTR lpszWindowName);
	void	SetRefreshMessage(UINT uiMessageID);

	void	DefaultSettings(void);

	void	SetItemHeight(int nHeight = TWXHEIGHT_DEFAULT);

	void	UseIcon(BOOL bFlag = TRUE);
	BOOL	SetIcon(UINT uID = IDI_ICON, int nWidth = GetSystemMetrics(SM_CXSMICON), int nHeight = GetSystemMetrics(SM_CXSMICON));

	void	SetRefreshRate(int nInterval = 5000);

	void	SetToolTipFontSize(int nDoublePoint = 16);
	void	UseToolTip(BOOL bFlag = FALSE);

	void	SetHoldFlags(DWORD dwFlags = SHRG_RETURNCMD | SHRG_NOANIMATION);

	void	SetOptionsTransparent(COLORREF crTransparent = RGB(0x01, 0x01, 0x01));
	void	SetOptionsItem(LPCTSTR lpszPath, LPCTSTR lpszParameters = NULL);

	void	RequestSelection(void);

	void	SetPaintClip(BOOL bClip = FALSE, BOOL bLayered = TRUE);

	// Retreiving Methods
	COLORREF	GetTodayColor(UINT nItem = TODAYCOLOR_TEXT);
	COLORREF	GetTodayTextColor(void);
	COLORREF	GetTodayHighlightedTextColor(void);

	HWND		GetWindow(void);
	HWND		GetParentWindow(void);
	HINSTANCE	GetInstance(void);
	HINSTANCE	GetResourceInstance(void);
	LPCTSTR		GetClassName(void);
	LPCTSTR		GetWindowName(void);

	int			GetItemWidth(void);
	int			GetItemHeight(void);
	void		GetUserArea(LPRECT lpRect);

	HICON		GetIcon(void);
	BOOL		IsUseIcon(void);
	BOOL		GetIconRect(LPRECT lpIconRect);

	BOOL		IsUseToolTip(void);

	HFONT		GetTodayNormalFont(void);
	HFONT		GetTodaySemiBoldFont(void);
	HFONT		GetTodayBoldFont(void);

	void		CreateTodayFonts(void);
	void		ReleaseTodayFonts(void);

	HFONT		CreateTodayFont(LPCTSTR lpszFontFace = NULL, int nDoublePoint = 0, LONG lWeight = FW_NORMAL);
	void		ReleaseTodayFont(HFONT hFont);

	BOOL		IsSelected(void);

protected:
	// Tools
	virtual void	DrawTodayIcon(HDC hDC);
	virtual void	GetTodayDefaults(void);

	BOOL			IsWindowMoved(void);

	void			SetInternalDC(HDC hDC = NULL);
	HDC				GetTodayDCInternal(void);

	void			CreateToolTipWnd(void);
	void			DestroyToolTipWnd(void);
	static  BOOL	CALLBACK ToolTipWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void			SetToolTipText(LPCTSTR lpszText);

	// Message handlers
	virtual int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void	OnPostCreateWindow(void);
	virtual void	OnDestroy(void);
	virtual void	OnPaint(HDC hDC);
	virtual void	OnPaintClip(HDC hDC, LPRECT rcClip);
	virtual void	OnEraseBkgnd(HDC hDC);
	virtual void	OnTodayCustomQueryRefreshCache(TODAYLISTITEM* pTodayListItem, LPDWORD lpdwFlags);
	virtual BOOL	OnTodayCustomClearCache(TODAYLISTITEM* pTodayListItem);
	virtual void	OnLButtonUp(UINT nFlags, POINT point);
	virtual void	OnLButtonDown(UINT nFlags, POINT point);
	virtual void	OnContextMenu(UINT nFlags, POINT point);
	virtual void	OnMouseMove(UINT nFlags, POINT point);
	virtual void	OnSettingChange(UINT nFlags, LPCTSTR lpszSection);
	virtual LRESULT	OnNotify(UINT nID, NMHDR* pNMHDR);
	virtual LRESULT	OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL	OnCommand(WORD wID, HWND hWndCtrl);
	virtual LRESULT	OnSetToolTip(POINT point);
	virtual void	OnReceivedSelection(int nVirtualKey);
	virtual void	OnLostSelection(void);
	virtual BOOL	OnUserNavigation(int nVirtualKey);
	virtual void	OnAction(void);
	virtual LRESULT	OnRefreshSettings();
	virtual void	OnPostRefreshSettings(DWORD dwFlags);

	// Member Variables
	HINSTANCE	m_hInstance;
	HINSTANCE	m_hResource;
	LPCTSTR		m_lpszClassName;
	LPCTSTR		m_lpszWindowName;
	HWND		m_hWnd;
	HWND		m_hWndParent;
	RECT		m_rectWnd;
	RECT		m_rectWndWA;

	HDC			m_hInternalDC;
	HDC			m_hMemDC;
	HBITMAP		m_hMemBitmap;
	HBITMAP		m_hMemOldBmp;
	HBITMAP		m_hBitmapDefault;
	HBITMAP		m_hOldBmpDefault;
	HBITMAP		m_hBitmapClip;
	HBITMAP		m_hOldBmpClip;
	int			m_nOldMode;

	BOOL		m_bPaintClip;
	BOOL		m_bLayered;

	UINT		m_nTimerID;
	UINT		m_nInterval;

	BOOL		m_bInitialRefresh;
	BOOL		m_bRefresh;

	int			m_nItemHeight;

	BOOL		m_bUseIcon;
	RECT		m_rectIcon;
	HICON		m_hIcon;

	HWND		m_hToolTipWnd;
	BOOL		m_bUseToolTip;
	BOOL		m_bToolTipActivate;
	TCHAR		m_szToolTipText[2048];
	HFONT		m_hFontT;
	int			m_nFontTDblPt;

	DWORD		m_dwHoldFlags;

	COLORREF	m_crText;
	COLORREF	m_crHighlight;
	COLORREF	m_crHighlightedText;

	HFONT		m_hFontN;
	HFONT		m_hFontS;
	HFONT		m_hFontB;

	TCHAR		m_szOptionsPath[MAX_PATH + 1];
	TCHAR		m_szOptionsParameters[MAX_PATH + 1];
	UINT		m_nOptionsTimerID;
	COLORREF	m_crOptionsTransparent;

	BOOL		m_bSelected;

	UINT		WM_SH_UIMETRIC_CHANGE;
	UINT		WM_TODAYWNDX_REFRESH;
};
