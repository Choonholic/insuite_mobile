// InCloseWnd.cpp

#include "StdAfx.h"
#include "resourceppc.h"
#include <notify.h>
#include "InClose.h"
#include "InCloseWnd.h"
#include "InLibrary_Messages.h"
#include "InClose_Settings.h"
#include "InSuite_Settings.h"
#include "FontX.h"
#include "DisplayX.h"
#include "RegistryX.h"
#include "ResourceX.h"
#include "GraphicsX.h"
#include "WindowX.h"
#include "ColorX.h"
#include "ThemeX.h"
#include "ButtonsX.h"
#include "SystemX.h"

#ifndef CLWND_WIDTH
#define CLWND_WIDTH				DRA::SCALEX(25)
#define CLWND_HEIGHT			DRA::SCALEY(26)
#endif

#ifndef ID_TIMER_CLOSE
#define ID_TIMER_CLOSE			0x0116
#define ID_TIMER_CLOSE_INT		200
#define ID_TIMER_ROTATION		0x0117
#define ID_TIMER_ROTATION_INT	500
#endif

#ifndef MENUIMAGE_TYPE
#define MENUIMAGE_TYPE
#define MENUIMAGE_NONE			0
#define MENUIMAGE_THEME			1
#endif

#ifndef PP_GENERAL
#define PP_GENERAL				0x01
#define PP_ACTIONS				0x02
#define PP_BACKGROUND			0x04
#define PP_MENU					0x08
#define PP_EXCLUDED				0x10
#endif

static WNDPROC	g_wpTaskBar	= NULL;
static WNDPROC	g_wpMenu	= NULL;

RECT	g_rcMenuPos;
POINT	g_ptMenuTap;
BOOL	g_bCloseAction	= FALSE;
BOOL	g_bPush			= FALSE;
BOOL	g_bLock			= FALSE;

LRESULT HHTaskBarWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			RECT	rc;
			POINT	pt;

			pt.x	= LOWORD(lParam);
			pt.y	= HIWORD(lParam);

			SetRect(&rc, (GetSystemMetrics(SM_CXSCREEN) - CLWND_WIDTH), 0, GetSystemMetrics(SM_CXSCREEN), CLWND_HEIGHT);

			if (PtInRect(&rc, pt))
			{
				return (SendMessage(FindChihiro(), uMsg, wParam, lParam));
			}
		}
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
		{
			if (g_bPush)
			{
				return (SendMessage(FindChihiro(), uMsg, wParam, lParam));
			}
		}
		break;
	case WM_USER_LBUTTONDOWN:
	case WM_USER_MOUSEMOVE:
	case WM_USER_LBUTTONUP:
		{
			uMsg	-= WM_USER;
		}
		break;
	}

	return ::CallWindowProc(g_wpTaskBar, hwnd, uMsg, wParam, lParam);
}

CInCloseWnd::CInCloseWnd(void)
{
	LOGFONT	lfPopupMenu;

	GetPopupMenuFont(&lfPopupMenu);

	m_hWnd				= NULL;
	m_hLock				= NULL;
	m_bPrevScreen		= TRUE;
	m_bEnable			= FALSE;
	g_bPush				= FALSE;
	m_hWndForeground	= 0;
	m_dwExStyle			= 0;
	m_bShowAnimation	= TRUE;
	m_dwRotation		= (DWORD)(-1);
	m_hBitmap			= NULL;
	m_hFont				= CreateFontIndirect(&lfPopupMenu);

	m_hMenuIcon[ICO_MCLOSE]				= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_MCLOSE), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_MCLOSED]			= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_MCLOSED), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_INTELLICLOSE]		= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_INTELLICLOSE), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_MINIMIZE]			= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_MINIMIZE), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_CLOSE]				= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_MCLOSE), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_CLOSEALL]			= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_CLOSEALL), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_CLOSEALLBUTACTIVE]	= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_CLOSEALLACTIVE), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_RESET]				= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_RESET), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_ROTATE]				= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_ROTATE), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_TODAY]				= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_TODAY), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_SCREENOFF]			= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_SCREENOFF), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_SUSPEND]			= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_SUSPEND), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_SETEXCLUDED]		= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_SETEXCLUDED), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_SETTINGS]			= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_SETTINGS), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hMenuIcon[ICO_EXIT]				= (HICON)LoadImage(GetResInst(), MAKEINTRESOURCE(IDI_EXIT), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	WM_CL_SETTINGCHANGE					= RegisterWindowMessage(MS_CL_SETTINGCHANGE);
}

CInCloseWnd::~CInCloseWnd(void)
{
	ReleaseFont(m_hFont);

	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);

		m_hBitmap	= NULL;
	}

	for (int i = 0; i < MAX_MENUICON; i++)
	{
		DestroyIcon(m_hMenuIcon[i]);
	}
}

BOOL CInCloseWnd::Create(void)
{
	if (!RegisterClass())
	{
		return FALSE;
	}

	m_hWnd	= CreateWindowEx(0, NM_CL_CLASS, NM_CL_WINDOW, WS_CHILD, 0, 0, 0, 0, g_hTaskBarWnd, NULL, GetInst(), this);

	if (!m_hWnd)
	{
		return FALSE;
	}

	if (!InstallHookHHTaskBar())
	{
		return FALSE;
	}

	if (g_bPhoneEnabled)
	{
		if (HookButtons(m_hWnd, BTNX_TEND) != BTNX_NOERROR)
		{
			return FALSE;
		}
	}

	AssignWindow();

	m_hWndForeground	= GetForegroundWindow();
	m_dwExStyle			= GetWindowLong(m_hWndForeground, GWL_EXSTYLE);

	LoadSettings();
	return TRUE;
}

BOOL CInCloseWnd::RegisterClass(void)
{
	WNDCLASS	wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= GetInst();
	wc.hIcon			= NULL;
	wc.hCursor			= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= NM_CL_CLASS;

	return (::RegisterClass(&wc));
}

BOOL CInCloseWnd::AssignWindow(void)
{
	DWORD	dwRotation;

	GetRotationMode(&dwRotation);

	if (m_dwRotation != dwRotation)
	{
		m_dwRotation	= dwRotation;

		MoveWindow(m_hWnd, (GetSystemMetrics(SM_CXSCREEN) - CLWND_WIDTH), 0, CLWND_WIDTH, CLWND_HEIGHT, TRUE);
		return TRUE;
	}

	return FALSE;
}

BOOL CInCloseWnd::InstallHookHHTaskBar(void)
{
	if (g_wpTaskBar != NULL)
	{
		return FALSE;
	}

	if (g_hTaskBarWnd != NULL)
	{
		g_wpTaskBar	= (WNDPROC)::GetWindowLong(g_hTaskBarWnd, GWL_WNDPROC);

		if (!::SetWindowLong(g_hTaskBarWnd, GWL_WNDPROC, (LONG)HHTaskBarWndProc))
		{
			return FALSE;
		}
	}

	return (BOOL)(g_wpTaskBar != NULL);
}

BOOL CInCloseWnd::UninstallHookHHTaskBar(void)
{
	if (g_wpTaskBar == NULL)
	{
		return FALSE;
	}

	::SetWindowLong(g_hTaskBarWnd, GWL_WNDPROC, (LONG)g_wpTaskBar);

	g_wpTaskBar	= NULL;

	return TRUE;
}

LRESULT CALLBACK CInCloseWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static	CInCloseWnd*	pInCloseWnd;
	POINT	pt;

	switch (uMsg)
	{
	case WM_CREATE:
		{
			pInCloseWnd	= (CInCloseWnd*)((CREATESTRUCT*)lParam)->lpCreateParams;

			pInCloseWnd->OnCreate(hWnd);
			return 0;
		}
	case WM_BTNX_TEND_DOWN:
		{
			if (IsLineOpened())
			{
				DoDefaultProc(WM_BTNX_TEND_DOWN);
			}
			else
			{
				if (pInCloseWnd->m_nUserAction[UDA_TEND] == IDM_NONE)
				{
					DoDefaultProc(WM_BTNX_TEND_DOWN);
				}
			}

			return 0;
		}
	case WM_BTNX_TEND_UP:
		{
			if (IsLineOpened())
			{
				DoDefaultProc(WM_BTNX_TEND_UP);
			}
			else
			{
				if (pInCloseWnd->m_nUserAction[UDA_TEND] == IDM_NONE)
				{
					DoDefaultProc(WM_BTNX_TEND_UP);
				}
				else
				{
					pInCloseWnd->OnUserAction(UDA_TEND);
				}
			}

			return 0;
		}
	case WM_LBUTTONDOWN:
		{
			pt.x	= LOWORD(lParam);
			pt.y	= HIWORD(lParam);

			pInCloseWnd->OnLButtonDown(pt);
			return 0;
		}
	case WM_LBUTTONUP:
		{
			pt.x	= LOWORD(lParam);
			pt.y	= HIWORD(lParam);

			pInCloseWnd->OnLButtonUp(pt);
			return 0;
		}
	case WM_MOUSEMOVE:
		{
			pt.x	= LOWORD(lParam);
			pt.y	= HIWORD(lParam);

			pInCloseWnd->OnMouseMove(pt);
			return 0;
		}
	case WM_TIMER:
		{
			pInCloseWnd->OnTimer(wParam);
			return 0;
		}
	case WM_COMMAND:
		{
			if (!pInCloseWnd->OnCommand(wParam, lParam))
			{
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			}

			return 0;
		}
	case WM_MEASUREITEM:
		{
			pInCloseWnd->OnMeasureItem((LPMEASUREITEMSTRUCT)lParam);
		}
		break;
	case WM_DRAWITEM:
		{
			pInCloseWnd->OnDrawItem((LPDRAWITEMSTRUCT)lParam);
		}
		break;
	case WM_ENTERMENULOOP:
		{
			pInCloseWnd->HookPopupMenu();
		}
		break;
	case WM_EXITMENULOOP:
		break;
	case WM_CLOSE:
		{
			pInCloseWnd->OnClose();
			return 0;
		}
	case WM_DESTROY:
		{
			pInCloseWnd->OnDestroy();
			return 0;
		}
	}

	if ((uMsg) && (pInCloseWnd))
	{
		if (uMsg == pInCloseWnd->WM_CL_SETTINGCHANGE)
		{
			switch (wParam)
			{
			case PP_GENERAL:
				{
					pInCloseWnd->LoadGeneralSettings();
				}
				break;
			case PP_ACTIONS:
				{
					pInCloseWnd->LoadActionsSettings();
				}
				break;
			case PP_BACKGROUND:
				{
					pInCloseWnd->LoadBackgroundSettings();
				}
				break;
			case PP_MENU:
				{
					pInCloseWnd->LoadMenuSettings();
				}
				break;
			case PP_EXCLUDED:
				{
					LoadExcludedList();
				}
				break;
			}
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CInCloseWnd::MenuWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		{
			SetWindowLong(hwnd, GWL_WNDPROC, (LONG)(WNDPROC)g_wpMenu);
		}
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			if (wParam == VK_RIGHT)
			{
				wParam			= VK_RETURN;
				g_bCloseAction	= TRUE;
			}

			if (wParam == VK_LEFT)
			{
				wParam	= VK_ESCAPE;
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			g_ptMenuTap.x	= (DWORD)(LOWORD(lParam));
			g_ptMenuTap.y	= (DWORD)(HIWORD(lParam));
		}
		break;
	}

	return CallWindowProc(g_wpMenu, hwnd, uMsg, wParam, lParam);
}

BOOL CALLBACK CInCloseWnd::MenuEnumWndProc(HWND hWnd, LPARAM lParam)
{
	CInCloseWnd*	pInCloseWnd	= (CInCloseWnd*)lParam;
	TCHAR			szTitle[MAX_TITLE_LEN + 1];
	TCHAR			szClass[MAX_TITLE_LEN + 1];
	BOOL			bSystem	= FALSE;
	DWORD			dwIdx;
	HWND			hDirectWnd;

	if (IsExcluded(hWnd) == (EXC_MAINWIN | EXC_ALWAYS))
	{
		return TRUE;
	}

	if ((hDirectWnd = IsPhonePrimaryWindow(hWnd)) != NULL)
	{
		if (hWnd != GetForegroundWindow())
		{
			return TRUE;
		}
	}
	else if ((hDirectWnd = IsPhoneSecondaryWindow(hWnd)) != NULL)
	{
		if (hWnd != GetForegroundWindow())
		{
			return TRUE;
		}
	}

	if (IsEnableCloseWindow(hWnd, FALSE, szTitle, szClass, NULL, &bSystem))
	{
		if ((_tcscmp(szClass, _T("Dialog")) == 0) || (_tcscmp(szClass, _T("Worker")) == 0))
		{
			for (dwIdx = 0; dwIdx < pInCloseWnd->m_dwValidTasks; dwIdx++)
			{
				if (_tcscmp(szTitle, pInCloseWnd->m_TaskInfo[dwIdx].szName) == 0)
				{
					pInCloseWnd->m_TaskInfo[dwIdx].hWnd	= hWnd;

					return TRUE;
				}
			}
		}
		else
		{
			for (dwIdx = 0; dwIdx < pInCloseWnd->m_dwValidTasks; dwIdx++)
			{
				if (_tcscmp(szTitle, pInCloseWnd->m_TaskInfo[dwIdx].szName) == 0)
				{
					if ((_tcscmp(pInCloseWnd->m_TaskInfo[dwIdx].szClass, szClass) != 0) || (_tcscmp(_T("BOOKWIN"), szClass) == 0))
					{
						return TRUE;
					}
				}
			}
		}

		pInCloseWnd->m_TaskInfo[pInCloseWnd->m_dwValidTasks].hWnd		= hWnd;
		pInCloseWnd->m_TaskInfo[pInCloseWnd->m_dwValidTasks].bSysTask	= bSystem;

		_tcscpy_s(pInCloseWnd->m_TaskInfo[pInCloseWnd->m_dwValidTasks].szName, MAX_TITLE_LEN + 1, szTitle);
		_tcscpy_s(pInCloseWnd->m_TaskInfo[pInCloseWnd->m_dwValidTasks].szClass, MAX_TITLE_LEN + 1, szClass);

		pInCloseWnd->m_dwValidTasks++;
	}

	return (((pInCloseWnd->m_dwValidTasks) > (MAX_TASKS - 1)) ? FALSE : TRUE);
}

void CInCloseWnd::OnCreate(HWND hWnd)
{
	SetTimer(hWnd, ID_TIMER_CLOSE, ID_TIMER_CLOSE_INT, NULL);
	SetTimer(hWnd, ID_TIMER_ROTATION, ID_TIMER_ROTATION_INT, NULL);
}

void CInCloseWnd::OnClose()
{
	if (m_hLock != NULL)
	{
		TCHAR	szApp[MAX_PATH + 1];

		GetModuleFileName(GetInst(), szApp, MAX_PATH + 1);
		CeRunAppAtEvent(szApp, NOTIFICATION_EVENT_NONE);
		CeClearUserNotification(m_hLock);

		m_hLock	= NULL;
	}

	KillTimer(m_hWnd, ID_TIMER_CLOSE);
	KillTimer(m_hWnd, ID_TIMER_ROTATION);

	if (g_bPhoneEnabled)
	{
		UnhookButtons(m_hWnd);
	}

	UninstallHookHHTaskBar();
	DestroyWindow(m_hWnd);
}

void CInCloseWnd::OnDestroy()
{
	PostQuitMessage(0);
}

void CInCloseWnd::OnTimer(UINT uID)
{
	if (uID == ID_TIMER_CLOSE)
	{
		HWND	hWndForeground	= GetForegroundWindow();
		DWORD	dwExStyle		= GetWindowLong(hWndForeground, GWL_EXSTYLE);

		if ((hWndForeground != m_hWndForeground) || (dwExStyle != m_dwExStyle))
		{
			m_hWndForeground	= hWndForeground;
			m_dwExStyle			= dwExStyle;
		}
	}

	if (uID == ID_TIMER_ROTATION)
	{
		AssignWindow();

		if (g_bLock)
		{
			BOOL	bScreen	= GetDisplayMode();

			if ((bScreen == TRUE) && (m_bPrevScreen == FALSE))
			{
				DeviceLock();
			}
			
			m_bPrevScreen	= bScreen;
		}
	}
}

void CInCloseWnd::OnLButtonDown(POINT pt)
{
	SetCapture(g_hTaskBarWnd);

	g_bPush		= TRUE;
	m_bEnable	= TRUE;

	SHRGINFO	shrgi;

	shrgi.cbSize		= sizeof(SHRGINFO);
	shrgi.hwndClient	= g_hTaskBarWnd;
	shrgi.ptDown.x		= pt.x;
	shrgi.ptDown.y		= pt.y;
 	shrgi.dwFlags		= SHRG_RETURNCMD | (m_bShowAnimation ? 0 : SHRG_NOANIMATION);

	if (SHRecognizeGesture(&shrgi) == GN_CONTEXTMENU)
	{
		ReleaseCapture();

		g_bPush		= FALSE;
		m_bEnable	= FALSE;

		OnUserAction(UDA_TAPANDHOLD);
	}
}

void CInCloseWnd::OnLButtonUp(POINT pt)
{
	::ScreenToClient(m_hWnd, &pt);

	if (g_bPush)
	{
		ReleaseCapture();

		if ((pt.x >= 0) && (pt.x <= CLWND_WIDTH) && (pt.y >= 0) && (pt.y <= CLWND_HEIGHT))
		{
			OnUserAction(UDA_TAP);
		}
		else if ((pt.x >= 0) && (pt.x <= CLWND_WIDTH))
		{
			OnUserAction(UDA_VERTDRAG);
		}
		else if ((pt.y >= 0) && (pt.y <= CLWND_HEIGHT))
		{
			OnUserAction(UDA_HORIDRAG);
		}
		else
		{
			OnUserAction(UDA_DIAGDRAG);
		}
	}

	g_bPush		= FALSE;
	m_bEnable	= FALSE;
}

void CInCloseWnd::OnMouseMove(POINT pt)
{
	::ScreenToClient(m_hWnd, &pt);

	if (g_bPush)
	{
		if ((pt.x >= 0) && (pt.x <= CLWND_WIDTH) && (pt.y >= 0) && (pt.y <= CLWND_HEIGHT))
		{
			m_bEnable	= TRUE;
		}
		else
		{
			m_bEnable	= FALSE;
		}
	}
}

BOOL CInCloseWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case IDM_SHOWMENU:
		{
			TaskMenu();
		}
		break;
	case IDM_INTELLICLOSE:
		{
			IntelliCloseTask();
		}
		return TRUE;
	case IDM_MINIMIZE:
		{
			MinimizeTask();
		}
		return TRUE;
	case IDM_CLOSE:
		{
			CloseTask();
		}
		return TRUE;
	case IDM_CLOSEALL:
		{
			CloseAllTask();
		}
		return TRUE;
	case IDM_CLOSEALLBUTACTIVE:
		{
			CloseAllButActiveTask();
		}
		return TRUE;
	case IDM_RESET:
		{
			ResetDevice();
		}
		return TRUE;
	case IDM_ROTATE:
		{
			RotateScreen();
		}
		return TRUE;
	case IDM_TODAY:
		{
			Today();
		}
		return TRUE;
	case IDM_SCREENOFF:
		{
			ScreenOff();
		}
		return TRUE;
	case IDM_SUSPEND:
		{
			SuspendDevice();
		}
		return TRUE;
	case IDM_SETEXCLUDED:
		{
			SetExcludedTask(m_hWndForeground);
		}
		return TRUE;
	case IDM_SETTINGS:
		{
			Settings();
		}
		return TRUE;
	case IDM_EXIT:
		{
			PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		}
		return TRUE;
	case IDM_RESETINFO:
		{
			LoadSettings();
		}
		return TRUE;
	default:
		{
			if ((LOWORD(wParam) >= IDM_TASKBASE) && (LOWORD(wParam) < (IDM_TASKBASE + m_dwValidTasks)))
			{
				SetForegroundWindow(m_TaskInfo[LOWORD(wParam) - IDM_TASKBASE].hWnd);

				if (PtInRect(&g_rcMenuPos, g_ptMenuTap))
				{
					g_ptMenuTap.x	= -1;
					g_ptMenuTap.y	= -1;

					CloseTask();
				}
				else if (g_bCloseAction == TRUE)
				{
					g_bCloseAction	= FALSE;

					CloseTask();
				}

				return TRUE;
			}
		}
		break;
	}

	return FALSE;
}

void CInCloseWnd::OnUserAction(int nAction)
{
	if ((m_nUserAction[nAction] > IDM_UDABASE) && (m_nUserAction[nAction] < IDM_UDASENTINEL))
	{
		PostMessage(m_hWnd, WM_COMMAND, m_nUserAction[nAction], nAction);
	}
}

BOOL CInCloseWnd::OnMeasureItem(LPMEASUREITEMSTRUCT lpmis)
{
	if (lpmis->itemID)
	{
		SIZE	size;
		int		nMaxWidth	= ((GetSystemMetrics(SM_CXSCREEN) * 2) / 3);
		LPTSTR	lpszItem	= (LPTSTR)lpmis->itemData;
		HDC		hDC			= GetDC(m_hWnd);
		HFONT	hFont;

		hFont	= (HFONT)SelectObject(hDC, m_hFont);

		GetTextExtentPoint32(hDC, lpszItem, _tcslen(lpszItem), &size);
		SelectObject(hDC, hFont);
		ReleaseDC(m_hWnd, hDC);

		if (size.cx > nMaxWidth)
		{
			size.cx	= nMaxWidth;
		}

		lpmis->itemHeight	= max(size.cy, GetSystemMetrics(SM_CYSMICON)) + DRA::SCALEY(4);
		lpmis->itemWidth	= size.cx + ((GetSystemMetrics(SM_CXSMICON) + DRA::SCALEX(6)) * 2);
	}
	else
	{
		lpmis->itemHeight	= DRA::SCALEY(8);
		lpmis->itemWidth	= DRA::SCALEX(8);
	}

	return TRUE;
}

BOOL CInCloseWnd::OnDrawItem(LPDRAWITEMSTRUCT lpdis)
{
	RECT	rc		= lpdis->rcItem;
	HFONT	hFont;
	BITMAP	ObjBitmap;

	if (!lpdis->itemID)
	{
		COLORREF	cr	= GetSysColor(COLOR_WINDOWFRAME);

		FillRect(lpdis->hDC, &rc, GetSysColorBrush(COLOR_MENU));

		if (m_hBitmap)
		{
			GetObject(m_hBitmap, sizeof(BITMAP), (LPVOID)&ObjBitmap);

			switch (m_nImageType)
			{
			case MENUIMAGE_THEME:
				{
					if ((ObjBitmap.bmWidth < GetSystemMetrics(SM_CXSCREEN)) && (ObjBitmap.bmWidth >= DRA::UNSCALEX(GetSystemMetrics(SM_CXSCREEN))))
					{
						TransparentImage(lpdis->hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, m_hBitmap, DRA::UNSCALEX(GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left + GetSystemMetrics(SM_CXBORDER))), DRA::UNSCALEY(rc.top + GetSystemMetrics(SM_CYBORDER)), DRA::UNSCALEX(rc.right - rc.left), DRA::UNSCALEY(rc.bottom - rc.top), GetSysColor(COLOR_MENU));
					}
					else
					{
						TransparentImage(lpdis->hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, m_hBitmap, GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left + GetSystemMetrics(SM_CXBORDER)), rc.top + GetSystemMetrics(SM_CYBORDER), (rc.right - rc.left), (rc.bottom - rc.top), GetSysColor(COLOR_MENU));
					}
				}
				break;
			}
		}

		RECT	rcLine;

		rcLine.left		= rc.left + DRA::SCALEX(2);
		rcLine.right	= rc.right - DRA::SCALEX(2);

		FillSolidRectEx(lpdis->hDC, rc.left, rc.top + (rc.bottom - rc.top) / 2 - 1, rc.right - rc.left, GetSystemMetrics(SM_CYBORDER), cr);
	}
	else
	{
		if (lpdis->itemState & ODS_SELECTED)
		{
			COLORREF	cr		= SetTextColor(lpdis->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
			int			nMode	= SetBkMode(lpdis->hDC, TRANSPARENT);

			FillRect(lpdis->hDC, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));

			rc.left		+= GetSystemMetrics(SM_CXSMICON) + DRA::SCALEX(6);
			rc.right	-= GetSystemMetrics(SM_CXSMICON) + DRA::SCALEX(6);
			hFont		= (HFONT)SelectObject(lpdis->hDC, m_hFont);

			DrawTextEx(lpdis->hDC, (LPCTSTR)lpdis->itemData, -1, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_DISABLE_UNDERLINE | DT_END_ELLIPSIS);
			SelectObject(lpdis->hDC, hFont);

			rc		= lpdis->rcItem;
			rc.top	+= ((rc.bottom - rc.top - GetSystemMetrics(SM_CYSMICON)) / 2);
			rc.left	+= DRA::SCALEX(2);

			if ((lpdis->itemID >= IDM_TASKBASE) && (lpdis->itemID < IDM_TASKBASE + m_dwValidTasks))
			{
				HICON	hIcon	= GetWindowIconEx(m_TaskInfo[lpdis->itemID - IDM_TASKBASE].hWnd, ICON_SMALL);

				DrawIconEx(lpdis->hDC, rc.left, rc.top, hIcon, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0, NULL, DI_NORMAL);
				DestroyIcon(hIcon);

				rc.left	= rc.right - GetSystemMetrics(SM_CXSMICON) - DRA::SCALEX(2);

				DrawIconEx(lpdis->hDC, rc.left, rc.top, m_hMenuIcon[ICO_MCLOSED], GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0, NULL, DI_NORMAL);
				SetRect(&g_rcMenuPos, rc.left, rc.top, rc.left + GetSystemMetrics(SM_CXSMICON), rc.top + GetSystemMetrics(SM_CYSMICON));
			}
			else if ((lpdis->itemID >= IDM_COMMANDBASE) && (lpdis->itemID <= IDM_COMMANDBASE + MAX_MENUICON))
			{
				DrawIconEx(lpdis->hDC, rc.left, rc.top, m_hMenuIcon[lpdis->itemID - IDM_COMMANDBASE + ICO_BASE], GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0, NULL, DI_NORMAL);
			}

			SetBkMode(lpdis->hDC, nMode);
			SetTextColor(lpdis->hDC, cr);
		}
		else
		{
			COLORREF	cr;
			int			nMode	= SetBkMode(lpdis->hDC, TRANSPARENT);

			FillRect(lpdis->hDC, &rc, GetSysColorBrush(COLOR_MENU));

			if (m_hBitmap)
			{
				cr	= SetTextColor(lpdis->hDC, GetUserShellColor(SHCOLOR_STARTMENU_TEXT));

				GetObject(m_hBitmap, sizeof(BITMAP), (LPVOID)&ObjBitmap);

				switch (m_nImageType)
				{
				case MENUIMAGE_THEME:
					{
						if ((ObjBitmap.bmWidth < GetSystemMetrics(SM_CXSCREEN)) && (ObjBitmap.bmWidth >= DRA::UNSCALEX(GetSystemMetrics(SM_CXSCREEN))))
						{
							TransparentImage(lpdis->hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, m_hBitmap, DRA::UNSCALEX(GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left + GetSystemMetrics(SM_CXBORDER))), DRA::UNSCALEY(rc.top + GetSystemMetrics(SM_CYBORDER)), DRA::UNSCALEX(rc.right - rc.left), DRA::UNSCALEY(rc.bottom - rc.top), GetSysColor(COLOR_MENU));
						}
						else
						{
							TransparentImage(lpdis->hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, m_hBitmap, GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left + GetSystemMetrics(SM_CXBORDER)), rc.top + GetSystemMetrics(SM_CYBORDER), (rc.right - rc.left), (rc.bottom - rc.top), GetSysColor(COLOR_MENU));
						}
					}
					break;
				}
			}
			else
			{
				cr	= SetTextColor(lpdis->hDC, RGB(0x40, 0x40, 0x40));
			}

			rc.left		+= GetSystemMetrics(SM_CXSMICON) + DRA::SCALEX(6);
			rc.right	-= GetSystemMetrics(SM_CXSMICON) + DRA::SCALEX(6);
			hFont		= (HFONT)SelectObject(lpdis->hDC, m_hFont);

			DrawTextEx(lpdis->hDC, (LPCTSTR)lpdis->itemData, -1, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_DISABLE_UNDERLINE | DT_END_ELLIPSIS);
			SelectObject(lpdis->hDC, hFont);

			rc		= lpdis->rcItem;
			rc.top	+= ((rc.bottom - rc.top - GetSystemMetrics(SM_CYSMICON)) / 2);
			rc.left	+= DRA::SCALEX(2);

			if ((lpdis->itemID >= IDM_TASKBASE) && (lpdis->itemID < IDM_TASKBASE + m_dwValidTasks))
			{
				HICON	hIcon	= GetWindowIconEx(m_TaskInfo[lpdis->itemID - IDM_TASKBASE].hWnd, ICON_SMALL);

				DrawIconEx(lpdis->hDC, rc.left, rc.top, hIcon, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0, NULL, DI_NORMAL);
				DestroyIcon(hIcon);

				rc.left	= rc.right - GetSystemMetrics(SM_CXSMICON) - DRA::SCALEX(2);

				DrawIconEx(lpdis->hDC, rc.left, rc.top, m_hMenuIcon[ICO_MCLOSE], GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0, NULL, DI_NORMAL);
				SetRect(&g_rcMenuPos, rc.left, rc.top, rc.left + GetSystemMetrics(SM_CXSMICON), rc.top + GetSystemMetrics(SM_CYSMICON));
			}
			else if ((lpdis->itemID >= IDM_COMMANDBASE) && (lpdis->itemID < IDM_COMMANDBASE + MAX_MENUICON))
			{
				DrawIconEx(lpdis->hDC, rc.left, rc.top, m_hMenuIcon[lpdis->itemID - IDM_COMMANDBASE + ICO_BASE], GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CXSMICON), 0, NULL, DI_NORMAL);
			}

			SetBkMode(lpdis->hDC, nMode);
			SetTextColor(lpdis->hDC, cr);
		}
	}

	return TRUE;
}

void CInCloseWnd::TaskMenu(void)
{
	CResourceX	res(GetResInst());
	HMENU		hMenu			= CreatePopupMenu();
	BOOL		bSeparator		= FALSE;
	BOOL		bTasks			= FALSE;
	BOOL		bEnableClose	= IsEnableClose();
	int			nItemCount		= 0;
	BOOL		bPortrait		= (BOOL)(GetSystemMetrics(SM_CXSCREEN) < GetSystemMetrics(SM_CYSCREEN));
	BITMAP		ObjBitmap;
	TCHAR		szTheme[MAX_PATH + 1];

	if (GetCapture() != m_hWnd)
	{
		SetCapture(m_hWnd);
	}

	m_dwValidTasks	= 0;

	EnumWindows(MenuEnumWndProc, (LPARAM)this);

	for (int i = 0; i < MNU_ITEMS; i++)
	{
		if (bTasks == TRUE)
		{
			bTasks	= FALSE;

			nItemCount++;

			AppendMenu(hMenu, MF_SEPARATOR | MF_OWNERDRAW, 0, _T("SeparatorX"));
		}

		if (m_MenuInfo[i].bEnable == TRUE)
		{
			if ((m_MenuInfo[i].dwItem == MNU_INTELLICLOSE) && (bEnableClose))
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_INTELLICLOSE, res.LoadString(IDS_INTELLICLOSE));
			}
			else if ((m_MenuInfo[i].dwItem == MNU_MINIMIZE) && (bEnableClose))
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_MINIMIZE, res.LoadString(IDS_MINIMIZE));
			}
			else if ((m_MenuInfo[i].dwItem == MNU_CLOSE) && (bEnableClose))
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_CLOSE, res.LoadString(IDS_CLOSE));
			}
			else if ((m_MenuInfo[i].dwItem == MNU_CLOSEALL) && (m_dwValidTasks > 0))
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_CLOSEALL, res.LoadString(IDS_CLOSEALL));
			}
			else if ((m_MenuInfo[i].dwItem == MNU_CLOSEALLBUTACTIVE) && (m_dwValidTasks > 0))
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_CLOSEALLBUTACTIVE, res.LoadString(IDS_CLOSEALLBUTACTIVE));
			}
			else if (m_MenuInfo[i].dwItem == MNU_RESET)
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_RESET, res.LoadString(IDS_RESET));
			}
			else if (m_MenuInfo[i].dwItem == MNU_ROTATE)
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_ROTATE, res.LoadString(IDS_ROTATE));
			}
			else if ((m_MenuInfo[i].dwItem == MNU_TODAY) && (!IsTodayForeground()))
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_TODAY, res.LoadString(IDS_TODAY));
			}
			else if (m_MenuInfo[i].dwItem == MNU_SCREENOFF)
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_SCREENOFF, res.LoadString(IDS_SCREENOFF));
			}
			else if (m_MenuInfo[i].dwItem == MNU_SUSPEND)
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_SUSPEND, res.LoadString(IDS_SUSPEND));
			}
			else if ((m_MenuInfo[i].dwItem == MNU_SETEXCLUDED) && (IsEnableCloseWindow(m_hWndForeground, FALSE, NULL, NULL, NULL, NULL)) && (IsExcluded(m_hWndForeground) == EXC_NONE))
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_SETEXCLUDED, res.LoadString(IDS_SETEXCLUDED));
			}
			else if (m_MenuInfo[i].dwItem == MNU_SETTINGS)
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_SETTINGS, res.LoadString(IDS_SETTINGS));
			}
			else if (m_MenuInfo[i].dwItem == MNU_EXIT)
			{
				bSeparator	= TRUE;
				nItemCount++;

				AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_EXIT, res.LoadString(IDS_EXIT));
			}
			else if ((m_MenuInfo[i].dwItem == MNU_TASKS) && (m_dwValidTasks > 0))
			{
				if (bSeparator)
				{
					bSeparator	= FALSE;
					nItemCount++;

					AppendMenu(hMenu, MF_SEPARATOR | MF_OWNERDRAW, 0, _T("SeparatorX"));
				}

				for (DWORD j = 0; j < m_dwValidTasks; j++)
				{
					nItemCount++;

					AppendMenu(hMenu, MF_STRING | MF_OWNERDRAW, IDM_TASKBASE + j, m_TaskInfo[j].szName);
				}

				bTasks	= TRUE;
			}
		}
	}

	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);

		m_hBitmap	= NULL;
	}

	if (nItemCount <= (bPortrait ? MAX_VERTICAL : MAX_HORIZONTAL))
	{
		switch (m_nImageType)
		{
		case MENUIMAGE_THEME:
			{
				if (GetWallpaper(szTheme, MAX_PATH + 1))
				{
					m_hBitmap	= SHLoadImageFile(szTheme);
				}
			}
			break;
		default:
			break;
		}
	}

	if (m_hBitmap != NULL)
	{
		GetObject(m_hBitmap, sizeof(BITMAP), (LPVOID)&ObjBitmap);

		if (ObjBitmap.bmWidth < DRA::UNSCALEX(GetSystemMetrics(SM_CXSCREEN)))
		{
			DeleteObject(m_hBitmap);

			m_hBitmap	= NULL;
		}
	}

	TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, GetSystemMetrics(SM_CXSCREEN) - DRA::SCALEX(1), CLWND_HEIGHT, m_hWnd, NULL);

	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);

		m_hBitmap	= NULL;
	}

	ReleaseCapture();

	g_bPush		= FALSE;
	m_bEnable	= FALSE;
}

BOOL CInCloseWnd::HookPopupMenu(void)
{
	HWND	hWnd	= FindWindow(_T("MNU"), NULL);

	if (hWnd == NULL)
	{
		return FALSE;
	}

	g_wpMenu	= (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)(WNDPROC)(MenuWndProc));

	return TRUE;
}

void CInCloseWnd::LoadSettings(void)
{
	LoadGeneralSettings();
	LoadActionsSettings();
	LoadBackgroundSettings();
	LoadMenuSettings();
	LoadExcludedList();
}

void CInCloseWnd::LoadGeneralSettings(void)
{
	CRegistryX	reg;
	TCHAR		szApp[MAX_PATH + 1];
	TCHAR		szParam[16];

	GetModuleFileName(GetInst(), szApp, MAX_PATH + 1);
	_tcscpy_s(szParam, 16, _T("-K"));

	if (m_hLock != NULL)
	{
		CeRunAppAtEvent(szApp, NOTIFICATION_EVENT_NONE);
		CeClearUserNotification(m_hLock);

		m_hLock	= NULL;
	}

	reg.Open(HKEY_CURRENT_USER, RK_CL_GENERAL);

	m_bShowAnimation	= reg.ReadDWORD(RV_CL_SHOWANIMATION, 1);
	g_dwResetMethod		= reg.ReadDWORD(RV_CL_RESETMETHOD, 0);
	g_dwRotation		= reg.ReadDWORD(RV_CL_ROTATIONMODE, DMDO_90);
	g_bLock				= reg.ReadDWORD(RV_CL_WAKEUPLOCK, 0);

	reg.Close();

	if (g_bLock)
	{
		CE_NOTIFICATION_TRIGGER	cent;

		memset(&cent, 0, sizeof(CE_NOTIFICATION_TRIGGER));

		cent.dwSize				= sizeof(CE_NOTIFICATION_TRIGGER);
		cent.dwType				= CNT_EVENT;
		cent.dwEvent			= NOTIFICATION_EVENT_WAKEUP;
		cent.lpszApplication	= szApp;
		cent.lpszArguments		= szParam;
		m_hLock					= CeSetUserNotificationEx(0, &cent, NULL);
	}
}

void CInCloseWnd::LoadActionsSettings(void)
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_CL_ACTIONS);

	m_nUserAction[UDA_TAP]			= IDM_UDABASE + reg.ReadDWORD(RV_CL_TAP, 2);
	m_nUserAction[UDA_TAPANDHOLD]	= IDM_UDABASE + reg.ReadDWORD(RV_CL_TAPANDHOLD, 1);
	m_nUserAction[UDA_HORIDRAG]		= IDM_UDABASE + reg.ReadDWORD(RV_CL_HORIDRAG, 8);
	m_nUserAction[UDA_VERTDRAG]		= IDM_UDABASE + reg.ReadDWORD(RV_CL_VERTDRAG, 1);
	m_nUserAction[UDA_DIAGDRAG]		= IDM_UDABASE + reg.ReadDWORD(RV_CL_DIAGDRAG, 7);
	m_nUserAction[UDA_TEND]			= IDM_UDABASE + reg.ReadDWORD(RV_CL_TEND, 0);

	reg.Close();
}

void CInCloseWnd::LoadBackgroundSettings(void)
{
	CRegistryX	reg;
	DWORD		dwLength	= MAX_PATH;

	reg.Open(HKEY_CURRENT_USER, RK_CL_MENU);

	m_nImageType	= reg.ReadDWORD(RV_CL_TYPE, 1);

	reg.Close();
}

void CInCloseWnd::LoadMenuSettings(void)
{
	MENUINFO	DefaultMenu[MNU_ITEMS]	=
	{
		{ TRUE,		MNU_INTELLICLOSE		},
		{ TRUE,		MNU_MINIMIZE			},
		{ TRUE,		MNU_CLOSE				},
		{ TRUE,		MNU_CLOSEALL			},
		{ FALSE,	MNU_CLOSEALLBUTACTIVE	},
		{ TRUE,		MNU_TASKS				},
		{ TRUE,		MNU_RESET				},
		{ TRUE,		MNU_ROTATE				},
		{ FALSE,	MNU_TODAY				},
		{ FALSE,	MNU_SCREENOFF			},
		{ FALSE,	MNU_SUSPEND				},
		{ TRUE,		MNU_SETEXCLUDED			},
		{ TRUE,		MNU_SETTINGS			},
		{ FALSE,	MNU_EXIT				}
	};

	CRegistryX	reg;
	TCHAR		szMenu[MAX_PATH + 1];

	for (int i = 0; i < MNU_ITEMS; i++)
	{
		wsprintf(szMenu, _T("%s\\%04X"), RK_CL_MENUITEMS, i);
		reg.Open(HKEY_CURRENT_USER, szMenu);

		m_MenuInfo[i].bEnable	= reg.ReadDWORD(RV_CL_ENABLE, DefaultMenu[i].bEnable);
		m_MenuInfo[i].dwItem	= reg.ReadDWORD(RV_CL_ITEM, DefaultMenu[i].dwItem);

		reg.Close();
	}
}

void CInCloseWnd::Settings(void)
{
	CRegistryX	reg;
	BOOL		bResult;
	TCHAR		szFolder[MAX_PATH + 1];
	TCHAR		szChihiro[MAX_PATH + 1];
	DWORD		dwSize	= MAX_PATH;

	reg.Open(HKEY_CURRENT_USER, RK_CL_ROOT);

	bResult	= reg.ReadString(RV_CL_SETUPMODULE, szChihiro, &dwSize);

	reg.Close();

	if ((bResult) && (dwSize != 0))
	{
		if (CreateProcess(szChihiro, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL))
		{
			return;
		}
	}

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	if (!reg.ReadString(RV_IS_FOLDER, szFolder, &dwSize))
	{
		_tcscpy_s(szFolder, MAX_PATH + 1, _T(""));
	}

	reg.Close();

	wsprintf(szChihiro, _T("%s\\%s"), szFolder, MD_CL_SETUP);

	if (CreateProcess(szChihiro, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL))
	{
		return;
	}

	CResourceX	res(GetResInst());

	MessageBox(NULL, res.LoadString(IDS_SETUP_FAILED), res.LoadString(IDS_INCLOSE), MB_OK | MB_ICONERROR | MB_TOPMOST);
}
