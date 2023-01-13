// InClose.h

#pragma once

#define EXPACK_ENABLED

#ifndef RECT_WIDTH
#define RECT_WIDTH(x)	((x)->right - (x)->left)
#define RECT_HEIGHT(x)	((x)->bottom - (x)->top)
#endif

#ifndef MAX_ITASKS
#define MAX_ITASKS		32
#define MAX_TITLE_LEN	64
#define MAX_EXCLUDED	32
#endif

#ifndef EXC_NONE
#define EXC_NONE		0x00
#define EXC_MAINWIN		0x01
#define EXC_SUBWIN		0x02
#define EXC_MAINFRAME	0x01
#define EXC_DIALOG		0x02
#define EXC_ALWAYS		0x10
#endif

#ifndef WM_USER_LBUTTONDOWN
#define WM_USER_LBUTTONDOWN	(WM_USER + WM_LBUTTONDOWN)
#define WM_USER_MOUSEMOVE	(WM_USER + WM_MOUSEMOVE)
#define WM_USER_LBUTTONUP	(WM_USER + WM_LBUTTONUP)
#endif

HINSTANCE	GetInst(void);
HINSTANCE	GetResInst(void);

BOOL	ParseCommandLine(LPCTSTR pszCmdLine);
HWND	FindChihiro(void);
BOOL	BuildIgnoreTasks(void);

BOOL	IsEnableCloseWindow(HWND hWnd, BOOL bInvisible, LPTSTR lpszTitle, LPTSTR lpszClass, LPDWORD lpdwPid, LPBOOL lpbSysTask);
BOOL	IsEnableClose(void);

BOOL	IsTodayForeground(void);
BOOL	IsDesktopWindow(HWND hWnd);
BOOL	IsWirelessManagerWindow(HWND hWnd);

HWND	IsSHFindWindow(HWND hWnd);
HWND	IsPhonePrimaryWindow(HWND hWnd);
HWND	IsPhoneSecondaryWindow(HWND hWnd);
int		IsExcluded(HWND hWnd);

BOOL	IsLineOpened(void);

void	SmartMinimize(HWND hWnd);
void	PushOKButton(HWND hWnd);
void	PostCloseMessage(HWND hWnd);

void	IntelliCloseTask(void);
void	CloseTask(void);
void	CloseAllTask(void);
void	CloseAllButActiveTask(void);
void	MinimizeTask(void);

void	ResetDevice(void);
void	RotateScreen(void);
void	ScreenOff(void);
void	SuspendDevice(void);

void	Today(void);

void	SetExcludedTask(HWND hWnd);
void	LoadExcludedList(void);
void	SaveExcludedList(void);

extern DWORD	g_dwResetMethod;
extern DWORD	g_dwRotation;
extern BOOL		g_bPhoneEnabled;
extern HWND		g_hTaskBarWnd;