// InCloseWnd.h

#pragma once

#ifndef MAX_ALLITEMS
#define MAX_ALLITEMS
#define MAX_VERTICAL			15
#define MAX_HORIZONTAL			10
#endif

#ifndef MAX_MENUICON
#define MAX_MENUICON			15
#define ICO_MCLOSE				0
#define ICO_MCLOSED				1
#define ICO_INTELLICLOSE		2
#define ICO_MINIMIZE			3
#define ICO_CLOSE				4
#define ICO_CLOSEALL			5
#define ICO_CLOSEALLBUTACTIVE	6
#define ICO_RESET				7
#define ICO_ROTATE				8
#define ICO_TODAY				9
#define ICO_SCREENOFF			10
#define ICO_SUSPEND				11
#define ICO_SETEXCLUDED			12
#define ICO_SETTINGS			13
#define ICO_EXIT				14
#define ICO_BASE				(ICO_INTELLICLOSE)
#endif

#ifndef MAX_UDA
#define MAX_UDA					6
#define UDA_TAP					0
#define UDA_TAPANDHOLD			1
#define UDA_HORIDRAG			2
#define UDA_VERTDRAG			3
#define UDA_DIAGDRAG			4
#define UDA_TEND				5
#endif

#ifndef MNU_ITEMS
#define MNU_ITEMS				14
#define MNU_INTELLICLOSE		0x0001
#define MNU_MINIMIZE			0x0002
#define MNU_CLOSE				0x0004
#define MNU_CLOSEALL			0x0008
#define MNU_CLOSEALLBUTACTIVE	0x0010
#define MNU_RESET				0x0020
#define MNU_ROTATE				0x0040
#define MNU_TODAY				0x0080
#define MNU_SCREENOFF			0x0100
#define MNU_SUSPEND				0x0200
#define MNU_SETEXCLUDED			0x0400
#define MNU_SETTINGS			0x0800
#define MNU_EXIT				0x1000
#define MNU_TASKS				0x2000
#endif

#ifndef MAX_TASKS
#define MAX_TASKS				32
#endif

typedef struct _tagTASKINFO
{
	BOOL	bSysTask;
	HWND	hWnd;
	TCHAR	szName[MAX_TITLE_LEN + 1];
	TCHAR	szClass[MAX_TITLE_LEN + 1];
	RECT	rc;
} TASKINFO, FAR *LPTASKINFO;

typedef struct _tagMENUINFO
{
	BOOL	bEnable;
	DWORD	dwItem;
} MENUINFO, FAR* LPMENUINFO;

class CInCloseWnd
{
public:
	CInCloseWnd(void);
	virtual	~CInCloseWnd(void);

	BOOL	Create(void);

protected:
	BOOL	RegisterClass(void);
	BOOL	AssignWindow(void);

	BOOL	InstallHookHHTaskBar(void);
	BOOL	UninstallHookHHTaskBar(void);

	static LRESULT	CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT	CALLBACK MenuWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL		CALLBACK MenuEnumWndProc(HWND hWnd, LPARAM lParam);

	void	OnCreate(HWND hWnd);
	void	OnClose(void);
	void	OnDestroy(void);
	void	OnTimer(UINT uID);
	void	OnLButtonDown(POINT pt);
	void	OnLButtonUp(POINT pt);
	void	OnMouseMove(POINT pt);
	BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
	void	OnUserAction(int nAction);
	BOOL	OnMeasureItem(LPMEASUREITEMSTRUCT lpmis);
	BOOL	OnDrawItem(LPDRAWITEMSTRUCT lpdis);

	void	TaskMenu(void);
	BOOL	HookPopupMenu(void);

	void	LoadSettings(void);
	void	LoadGeneralSettings(void);
	void	LoadActionsSettings(void);
	void	LoadButtonSettings(void);
	void	LoadBackgroundSettings(void);
	void	LoadMenuSettings(void);

	void	Settings(void);

	UINT		WM_CL_SETTINGCHANGE;
	DWORD		m_dwRotation;

	HWND		m_hWnd;
	HANDLE		m_hLock;
	BOOL		m_bPrevScreen;

	HBITMAP		m_hBitmap;

	BOOL		m_bEnable;
	HWND		m_hWndForeground;
	DWORD		m_dwExStyle;

	BOOL		m_bShowAnimation;
	int			m_nImageType;

	HFONT		m_hFont;
	HICON		m_hMenuIcon[MAX_MENUICON];
	UINT		m_nUserAction[MAX_UDA];
	MENUINFO	m_MenuInfo[MNU_ITEMS];

	DWORD		m_dwValidTasks;
	TASKINFO	m_TaskInfo[MAX_TASKS];
};
