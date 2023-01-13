// InClose.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include <tlhelp32.h>
#include "InClose.h"
#include "InClose_Settings.h"
#include "InSuite_Settings.h"
#include "InCloseWnd.h"
#include "ExPackX.h"
#include "PhoneX.h"
#include "RegistryX.h"
#include "ResourceX.h"
#include "WindowX.h"
#include "SystemX.h"
#include "DeviceInfoX.h"
#include "DisplayX.h"
#include "snapi.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#pragma comment(lib, "toolhelp.lib")

HINSTANCE	g_hInstance;
HINSTANCE	g_hResInst;

BOOL	g_bDelay		= FALSE;
DWORD	g_dwResetMethod	= 0;
DWORD	g_dwRotation	= DMDO_90;
BOOL	g_bPhoneEnabled	= FALSE;
HWND	g_hTaskBarWnd	= NULL;

DWORD	g_dwIgnoreTasksPID[MAX_ITASKS];
DWORD	g_dwIgnoreTasks	= 0;

DWORD	g_dwPMail	= NULL;
BOOL	g_fPMail	= FALSE;

TCHAR	g_szExcluded[MAX_EXCLUDED][MAX_PATH];
int		g_nExcludedType[MAX_EXCLUDED];
int		g_nExcluded	= 0;

LPCTSTR	g_lpszIgnoreProcesses[]	=
{
	_T("shell32.exe"),
	NULL
};

LPCTSTR	g_lpszIgnoreWindows[]	=
{
	_T("Pocket PC Emulation"),
	_T("CursorWindow"),
	_T("Desktop"),
	_T("ActiveSync"),
	NULL
};

LPCTSTR	g_lpszIgnoreClasses[]	=
{
	_T("MSSTARTMENU"),
	_T("Explore"),
	_T("menu_worker"),
	_T("HHTaskBar"),
	_T("MS_SIPBUTTON"),
	_T("MS_SOFTKEY_CE_1.0"),
	NULL
};

LPCTSTR	g_lpszIgnorePartialClasses[]	=
{
	_T("IME"),
	NULL
};

LPCTSTR	g_lpszInvisibleCloseProcesses[]	=
{
	_T("cprog.exe"),
	NULL
};

LPCTSTR	g_lpszInSuite[]	=
{
	NULL
};

BOOL CALLBACK	EnumCloseWindowProc(HWND hWnd, LPARAM lParam);

#define DELAY_LOADING	5000

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	g_hInstance	= hInstance;

#ifdef WIN32_PLATFORM_PSPC
	SHInitExtraControls();
#endif

#ifdef	EXPACK_ENABLED
	CExPackX	XP;
	CRegistryX	reg;
	TCHAR		szFolder[MAX_PATH + 1];
	DWORD		dwSize	= MAX_PATH;

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	if (!reg.ReadString(RV_IS_FOLDER, szFolder, &dwSize))
	{
		_tcscpy_s(szFolder, MAX_PATH + 1, _T(""));
	}

	reg.Close();
	XP.SetPackFolder(szFolder);

	g_hResInst	= XP.Load(MD_CL_MAINLX, MD_CL_MAINLXP);

	if (g_hResInst == NULL)
	{
		g_hResInst	= hInstance;
	}
#else
	g_hResInst	= hInstance;
#endif

	g_bPhoneEnabled	= DetectPhoneEdition();
	g_hTaskBarWnd	= FindWindow(_T("HHTaskBar"), NULL);

	if (g_hTaskBarWnd == NULL)
	{
		return 0;
	}

	BuildIgnoreTasks();

	if (ParseCommandLine(lpCmdLine))
	{
		return 0;
	}

	HANDLE	hMutex	= CreateMutex(NULL, NULL, NM_CL_MUTEX);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if (!GetDisplayMode())
		{
			SetDisplayMode(TRUE);
			return 0;
		}

		HWND	hWnd	= FindChihiro();

		if (hWnd != NULL)
		{
			PostMessage(hWnd, WM_COMMAND, IDM_SHOWMENU, 0);
		}

		return 0;
	}

	if (g_bDelay == TRUE)
	{
		DWORD	dwDelay	= 0;

		reg.Open(HKEY_CURRENT_USER, RK_CL_ADVANCED);

		dwDelay	= reg.ReadDWORD(RV_CL_DELAY, 5000);

		reg.Close();

		if ((dwDelay < 500) || (dwDelay > 20000))
		{
			dwDelay	= DELAY_LOADING;
		}

		Sleep(dwDelay);
	}

	CInCloseWnd	wnd;
	MSG			msg;

	if (!wnd.Create())
	{
		return 0;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

HINSTANCE GetInst(void)
{
	return g_hInstance;
}

HINSTANCE GetResInst(void)
{
	return g_hResInst;
}

BOOL ParseCommandLine(LPCTSTR pszCmdLine)
{
	if ((_tcsnicmp(pszCmdLine, _T("/A"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-A"), 2) == 0))
	{
		LoadExcludedList();
		CloseAllTask();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/B"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-B"), 2) == 0))
	{
		LoadExcludedList();
		CloseAllButActiveTask();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/C"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-C"), 2) == 0))
	{
		LoadExcludedList();
		CloseTask();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/F"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-F"), 2) == 0))
	{
		ScreenOff();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/I"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-I"), 2) == 0))
	{
		LoadExcludedList();
		IntelliCloseTask();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/K"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-K"), 2) == 0))
	{
		CRegistryX	reg;

		reg.Open(HKEY_CURRENT_USER, RK_CL_GENERAL);

		if (reg.ReadDWORD(RV_CL_WAKEUPLOCK, 0))
		{
			DeviceLock();
		}

		reg.Close();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/L"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-L"), 2) == 0))
	{
		HWND	hWnd	= FindChihiro();

		if (hWnd != NULL)
		{
			PostMessage(hWnd, WM_COMMAND, IDM_RESETINFO, 0);
			return TRUE;
		}

		return FALSE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/M"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-M"), 2) == 0))
	{
		LoadExcludedList();
		MinimizeTask();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/N"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-N"), 2) == 0))
	{
		return FALSE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/P"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-P"), 2) == 0))
	{
		SuspendDevice();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/R"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-R"), 2) == 0))
	{
		CRegistryX	reg;

		reg.Open(HKEY_CURRENT_USER, RK_CL_GENERAL);

		g_dwResetMethod	= reg.ReadDWORD(RV_CL_RESETMETHOD, 0);

		reg.Close();
		ResetDevice();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/S"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-S"), 2) == 0))
	{
		CRegistryX	reg;

		reg.Open(HKEY_CURRENT_USER, RK_CL_GENERAL);

		g_dwRotation	= reg.ReadDWORD(RV_CL_ROTATIONMODE, DMDO_90);

		reg.Close();
		RotateScreen();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/T"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-T"), 2) == 0))
	{
		Today();
		return TRUE;
	}
	else if ((_tcsnicmp(pszCmdLine, _T("/X"), 2) == 0) || (_tcsnicmp(pszCmdLine, _T("-X"), 2) == 0))
	{
		HWND	hWnd	= FindChihiro();

		if (hWnd != NULL)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}

		return TRUE;
	}

	g_bDelay	= TRUE;

	return FALSE;
}

HWND FindChihiro(void)
{
	HWND	hWnd	= FindWindow(_T("HHTaskBar"), NULL);
	UINT	nCmd	= GW_CHILD | GW_HWNDFIRST;
	BOOL	bFound	= FALSE;
	TCHAR	szClassWnd[MAX_TITLE_LEN + 1];

	if (hWnd != NULL)
	{
		while (hWnd = GetWindow(hWnd, nCmd))
		{
			GetClassName(hWnd, szClassWnd, MAX_TITLE_LEN);

			if (!_tcscmp(szClassWnd, NM_CL_CLASS))
			{
				bFound	= TRUE;
				break;
			}

			nCmd	= GW_HWNDNEXT;
		}

		if (bFound)
		{
			return hWnd;
		}
	}

	return NULL;
}

BOOL BuildIgnoreTasks(void)
{
	HANDLE			hProcSnap;
	PROCESSENTRY32	pe32	= { 0 };

	ZeroMemory(g_dwIgnoreTasksPID, sizeof(g_dwIgnoreTasksPID));
	ZeroMemory(&pe32, sizeof(pe32));

	g_dwIgnoreTasks	= 0;
	hProcSnap		= CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize		= sizeof(PROCESSENTRY32);

	if (hProcSnap == (HANDLE)(-1))
	{
		return FALSE;
	}

	if (Process32First(hProcSnap, &pe32))
	{
		do
		{
			LPTSTR	lpszStart;

			if (_tcsstr(pe32.szExeFile, _T("\\")))
			{
				lpszStart	= _tcsrchr(pe32.szExeFile, '\\');
			}
			else
			{
				lpszStart	= pe32.szExeFile;
			}

			for (int iIdx = 0; g_lpszIgnoreProcesses[iIdx] != NULL; iIdx++)
			{
				if (lstrcmp(g_lpszIgnoreProcesses[iIdx], lpszStart) == 0)
				{
					g_dwIgnoreTasksPID[g_dwIgnoreTasks++]	= pe32.th32ProcessID;
					break;
				}
			}
		} while (Process32Next(hProcSnap, &pe32));
	}

	CloseToolhelp32Snapshot(hProcSnap);
	return TRUE;
}

BOOL IsEnableCloseWindow(HWND hWnd, BOOL bInvisible, LPTSTR lpszTitle, LPTSTR lpszClass, LPDWORD lpdwPid, LPBOOL lpbSysTask)
{
	CResourceX	res(GetResInst());
	TCHAR		szBuffer[MAX_TITLE_LEN + 1];
	DWORD		dwIdx;
	DWORD		dwPid	= 0;
	BOOL		bComp	= FALSE;

	if (lpbSysTask != NULL)
	{
		*lpbSysTask	= FALSE;
	}

	if (lpszTitle)
	{
		*lpszTitle	= NULL;
	}

	if (lpszClass)
	{
		*lpszClass	= NULL;
	}

	GetWindowThreadProcessId(hWnd, &dwPid);

	if (lpdwPid)
	{
		*lpdwPid	= dwPid;
	}

	TCHAR		szPath[MAX_PATH + 1]	= _T("");
	HINSTANCE	hInstTarget				= (HINSTANCE)OpenProcess(0, 0, dwPid);

	GetModuleFileName(hInstTarget, szPath, MAX_PATH);

	if (g_dwPMail)
	{
		if ((dwPid == g_dwPMail) || (_tcsstr(szPath, _T("device.exe")) != NULL) || (_tcsstr(szPath, _T("rnaapp.exe")) != NULL))
		{
			if (lpdwPid)
			{
				*lpdwPid	= g_dwPMail;
			}

			if (lpszClass)
			{
				if (dwPid != g_dwPMail)
				{
					_tcscpy_s(lpszClass, MAX_TITLE_LEN + 1, _T("XXX"));
				}
				else
				{
					GetClassName(hWnd, lpszClass, MAX_TITLE_LEN);
				}
			}

			if (_tcsstr(szPath, _T("rnaapp.exe")) != NULL)
			{
				return FALSE;
			}

			return TRUE;
		}
	}

	if (!IsWindowVisible(hWnd))
	{
		if (bInvisible)
		{
			BOOL	bFound	= FALSE;

			for (dwIdx = 0; g_lpszInvisibleCloseProcesses[dwIdx]; dwIdx++)
			{
				if (_tcsstr(szPath, g_lpszInvisibleCloseProcesses[dwIdx]) != NULL)
				{
					bFound	= TRUE;
				}
			}

			if (!bFound)
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	GetWindowText(hWnd, szBuffer, MAX_TITLE_LEN);

	if (_tcslen(szBuffer) == 0)
	{
		if (dwPid == g_dwIgnoreTasksPID[0])
		{
			_tcscpy_s(szBuffer, MAX_TITLE_LEN + 1, res.LoadString(IDS_TITLE_SETTINGS));

			bComp	= TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if ((dwPid == g_dwIgnoreTasksPID[0]) && (_tcscmp(_T("Desktop"), szBuffer) == 0))
		{
			_tcscpy_s(szBuffer, MAX_TITLE_LEN + 1, res.LoadString(IDS_TITLE_DESKTOP));
		}
	}

	if (lpszTitle != NULL)
	{
		_tcscpy_s(lpszTitle, MAX_TITLE_LEN + 1, szBuffer);
	}

	for (dwIdx = 0; g_lpszIgnoreWindows[dwIdx]; dwIdx++)
	{
		if (_tcscmp(szBuffer, g_lpszIgnoreWindows[dwIdx]) == 0)
		{
			return FALSE;
		}
	}

	if (GetClassName(hWnd, szBuffer, MAX_TITLE_LEN) == 0)
	{
		return FALSE;
	}

	for (dwIdx = 0; g_lpszIgnoreClasses[dwIdx]; dwIdx++)
	{
		if (_tcscmp(szBuffer, g_lpszIgnoreClasses[dwIdx]) == 0)
		{
			return FALSE;
		}
	}

	for (dwIdx = 0; g_lpszIgnorePartialClasses[dwIdx]; dwIdx++)
	{
		if (_tcsstr(szBuffer, g_lpszIgnorePartialClasses[dwIdx]) != NULL)
		{
			return FALSE;
		}
	}

	if (lpszClass)
	{
		_tcscpy_s(lpszClass, MAX_TITLE_LEN + 1, szBuffer);
	}

	if (bComp)
	{
		return FALSE;
	}

	for (dwIdx = 0; dwIdx < g_dwIgnoreTasks; dwIdx++)
	{
		if (dwPid == g_dwIgnoreTasksPID[dwIdx])
		{
			if (lpbSysTask == NULL)
			{
				return FALSE;
			}

			*lpbSysTask = TRUE;

			if (!(GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_CAPTIONOKBTN))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL IsEnableClose(void)
{
	return (!IsDesktopWindow(GetForegroundWindow()));
}

BOOL IsTodayForeground(void)
{
	HWND	hWnd	= GetForegroundWindow();
	TCHAR	szClass[MAX_TITLE_LEN + 1];

	GetClassName(hWnd, szClass, MAX_TITLE_LEN);

	if (_tcscmp(szClass, _T("DesktopExplorerWindow")) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IsDesktopWindow(HWND hWnd)
{
	if ((!hWnd) || (!IsWindowVisible(hWnd)) || (!IsWindowEnabled(hWnd)))
	{
		return FALSE;
	}

	TCHAR	szClass[MAX_TITLE_LEN + 1];

	GetClassName(hWnd, szClass, MAX_TITLE_LEN);

	if (_tcscmp(szClass, _T("DesktopExplorerWindow")) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

HWND IsWirelessManagerWindow(HWND hWnd)
{
	TCHAR	szClass[MAX_TITLE_LEN + 1];
	TCHAR	szTitle[MAX_TITLE_LEN + 1];
	HWND	hDirectWnd;
	DWORD	dwStyle		= GetWindowLong(hWnd, GWL_STYLE);

	GetClassName(hWnd, szClass, MAX_TITLE_LEN);
	GetWindowText(hWnd, szTitle, MAX_TITLE_LEN);

	if (_tcscmp(szClass, _T("MSWrlsMgr")) == 0)
	{
		// Main Wireless Manager Frame
		return hWnd;
	}

	if (_tcscmp(szClass, _T("Dialog")) == 0)
	{
		if ((hDirectWnd = FindWindow(_T("MSWrlsMgr"), szTitle)) != NULL)
		{
			if (dwStyle == 0x90008004)
			{
				// List Box Frame
				return hDirectWnd;
			}
		}
	}

	return NULL;
}

HWND IsSHFindWindow(HWND hWnd)
{
	TCHAR	szClass[MAX_TITLE_LEN + 1];
	TCHAR	szTitle[MAX_TITLE_LEN + 1];
	HWND	hDirectWnd;

	GetClassName(hWnd, szClass, MAX_TITLE_LEN);
	GetWindowText(hWnd, szTitle, MAX_TITLE_LEN);

	if (_tcscmp(szClass, _T("Dialog")) == 0)
	{
		if ((hDirectWnd = FindWindow(_T("shfind"), szTitle)) != NULL)
		{
			return hDirectWnd;
		}
	}

	return NULL;
}

HWND IsPhonePrimaryWindow(HWND hWnd)
{
	if (g_bPhoneEnabled == FALSE)
	{
		return NULL;
	}

	TCHAR	szClass[MAX_TITLE_LEN + 1];
	TCHAR	szTitle[MAX_TITLE_LEN + 1];
	DWORD	dwStyle		= GetWindowLong(hWnd, GWL_STYLE);
	DWORD	dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);
	HWND	hDirectWnd;

	GetClassName(hWnd, szClass, MAX_TITLE_LEN);
	GetWindowText(hWnd, szTitle, MAX_TITLE_LEN);

	if (_tcscmp(szClass, _T("MSCprog")) == 0)
	{
		// Main Phone Window Frame
		return hWnd;
	}

	if (_tcscmp(szClass, _T("MSClog")) == 0)
	{
		// Log Window Frame
		return hWnd;
	}

	if (_tcscmp(szClass, _T("Dialog")) == 0)
	{
		if ((hDirectWnd = FindWindow(_T("MSCprog"), szTitle)) != NULL)
		{
			if (((dwStyle == 0x00008004) || (dwStyle == 0x10008004)) && (dwExStyle & 0x00000004))
			{
				// Button and Status Frame
				return hDirectWnd;
			}
		}

		HWND	hWndMSCprog	= FindWindow(_T("MSCprog"), NULL);

		if (hWndMSCprog != NULL)
		{
			TCHAR	szMSCprog[MAX_TITLE_LEN + 1];

			GetWindowText(hWndMSCprog, szMSCprog, MAX_TITLE_LEN);
			_tcscat_s(szMSCprog, MAX_TITLE_LEN + 1, _T(" - "));

			DWORD	dwMSCprog	= _tcslen(szMSCprog);

			if (!_tcsncmp(szMSCprog, szTitle, dwMSCprog))
			{
				// Incoming Window Frame
				return hWndMSCprog;
			}
		}
	}

	return NULL;
}

HWND IsPhoneSecondaryWindow(HWND hWnd)
{
	if (g_bPhoneEnabled == FALSE)
	{
		return NULL;
	}

	TCHAR	szClass[MAX_TITLE_LEN + 1];
	TCHAR	szTitle[MAX_TITLE_LEN + 1];
	DWORD	dwStyle		= GetWindowLong(hWnd, GWL_STYLE);
	DWORD	dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);

	GetClassName(hWnd, szClass, MAX_TITLE_LEN);
	GetWindowText(hWnd, szTitle, MAX_TITLE_LEN);

	if (_tcscmp(szClass, _T("MSScut")) == 0)
	{
		// Shortcut Window Frame
		return hWnd;
	}

	if (_tcscmp(szClass, _T("Dialog")) == 0)
	{
		if (FindWindow(_T("MSCprog"), szTitle))
		{
			if (((dwStyle != 0x00008004) && (dwStyle != 0x10008004)) || (dwExStyle != 0x00000004))
			{
				// Misc Phone Dialogs
				return hWnd;
			}
		}
	}

	return NULL;
}

int IsExcluded(HWND hWnd)
{
	TCHAR	szFileName[MAX_PATH + 1];
	DWORD	dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);
	int		i;

	GetModuleFileNameEx(hWnd, szFileName, MAX_PATH);

	for (i = 0; i < g_nExcluded; i++)
	{
		if (!_tcscmp(g_szExcluded[i], szFileName))
		{
			if ((g_nExcludedType[i] & EXC_MAINFRAME) && (dwExStyle & 0x80000000))
			{
				return EXC_SUBWIN;
			}

			return (EXC_MAINWIN + (g_nExcludedType[i] & EXC_ALWAYS));
		}
	}

	return EXC_NONE;
}

BOOL IsLineOpened(void)
{
	DWORD	dwStatus;
	HRESULT	hr;

	hr	= RegistryGetDWORD(SN_PHONEINCOMINGCALL_ROOT, SN_PHONEINCOMINGCALL_PATH, SN_PHONEINCOMINGCALL_VALUE, &dwStatus);

	if ((hr == S_OK) && (dwStatus & (SN_PHONEINCOMINGCALL_BITMASK | SN_PHONECALLCALLING_BITMASK | SN_PHONECALLTALKING_BITMASK)))
	{
		return TRUE;
	}

	return FALSE;
}

void SmartMinimize(HWND hWnd)
{
	if (hWnd == GetForegroundWindow())
	{
		RECT	rc;
		HWND	hwndTaskBar = FindWindow(_T("HHTaskBar"), NULL);

		if (hwndTaskBar != NULL)
		{
			GetClientRect(hwndTaskBar, &rc);
			PostMessage(hwndTaskBar, WM_USER_LBUTTONDOWN, 0, MAKELONG(RECT_WIDTH(&rc) - 1, RECT_HEIGHT(&rc) - 1));
			PostMessage(hwndTaskBar, WM_USER_LBUTTONUP, 0, MAKELONG(RECT_WIDTH(&rc) - 1, RECT_HEIGHT(&rc) - 1));
		}

		return;
	}

	DWORD	dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);

	if (dwExStyle & 0x80000000)
	{
		PushOKButton(hWnd);
		return;
	}

	Today();
}

void PushOKButton(HWND hWnd)
{
	PostMessage(hWnd, WM_COMMAND, 0x10000001, 0);
}

void PostCloseMessage(HWND hWnd)
{
	TCHAR	szBuffer[MAX_TITLE_LEN + 1];

	if (GetClassName(hWnd, szBuffer, MAX_TITLE_LEN) > 0)
	{
		if ((szBuffer[0] == _T('W')) && (szBuffer[1] == _T('M')) && (szBuffer[2] == _T('P')) && (szBuffer[3] == _T(' ')))
		{
			Sleep(100);
		}

		keybd_event(VK_CONTROL, 0, KEYEVENTF_SILENT, 1);
		keybd_event(_T('Q'), 0, KEYEVENTF_SILENT, 1);
		keybd_event(_T('Q'), 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 1);
		keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 1);

		if (_tcscmp(szBuffer, _T("Dialog")) == 0)
		{
			PushOKButton(hWnd);
		}

		if (IsWindow(hWnd))
		{
			PostMessage(hWnd, WM_ACTIVATE, MAKEWPARAM(WA_INACTIVE, 0), (LPARAM)hWnd);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
}

void IntelliCloseTask(void)
{
	HWND	hDirectWnd;
	HWND	hWnd		= GetForegroundWindow();
	DWORD	dwType		= 0;
	DWORD	dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);

	if ((hDirectWnd = IsSHFindWindow(hWnd)) != NULL)
	{
		hWnd		= hDirectWnd;
		dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);
	}
	else if ((hDirectWnd = IsPhonePrimaryWindow(hWnd)) != NULL)
	{
		SmartMinimize(hDirectWnd);
		return;
	}
	else if ((hDirectWnd = IsPhoneSecondaryWindow(hWnd)) != NULL)
	{
		hWnd	= hDirectWnd;
	}

	if ((hDirectWnd = IsWirelessManagerWindow(hWnd)) != NULL)
	{
		hWnd	= hDirectWnd;
	}

	if (IsDesktopWindow(hWnd))
	{
		return;
	}

	if (IsExcluded(hWnd) & EXC_MAINWIN)
	{
		if (dwExStyle & 0x80000000)
		{
			Today();
		}
		else if (IsWindow(hWnd))
		{
			SmartMinimize(hWnd);
		}

		return;
	}

	if (!IsEnableCloseWindow(hWnd, TRUE, NULL, NULL, NULL, NULL))
	{
		if (IsWindow(hWnd))
		{
			SmartMinimize(hWnd);
		}

		return;
	}

	if (dwExStyle & 0x80000000)
	{
		PushOKButton(hWnd);
	}
	else
	{
		PostCloseMessage(hWnd);
	}
}

void CloseTask(void)
{
	HWND	hDirectWnd;
	HWND	hWnd		= GetForegroundWindow();
	DWORD	dwType		= 0;
	DWORD	dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);

	if ((hDirectWnd = IsSHFindWindow(hWnd)) != NULL)
	{
		hWnd		= hDirectWnd;
		dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);
	}
	else if ((hDirectWnd = IsPhonePrimaryWindow(hWnd)) != NULL)
	{
		SmartMinimize(hDirectWnd);
		return;
	}
	else if ((hDirectWnd = IsPhoneSecondaryWindow(hWnd)) != NULL)
	{
		hWnd	= hDirectWnd;
	}

	if ((hDirectWnd = IsWirelessManagerWindow(hWnd)) != NULL)
	{
		hWnd	= hDirectWnd;
	}

	if (IsDesktopWindow(hWnd))
	{
		return;
	}

	if (IsExcluded(hWnd) == (EXC_MAINWIN | EXC_ALWAYS))
	{
		if (dwExStyle & 0x80000000)
		{
			Today();
		}
		else if (IsWindow(hWnd))
		{
			SmartMinimize(hWnd);
		}

		return;
	}

	if (!IsEnableCloseWindow(hWnd, TRUE, NULL, NULL, NULL, NULL))
	{
		if (IsWindow(hWnd))
		{
			SmartMinimize(hWnd);
		}

		return;
	}

	PostCloseMessage(hWnd);
}

void CloseAllTask(void)
{
	EnumWindows(EnumCloseWindowProc, NULL);
	Today();
}

void CloseAllButActiveTask(void)
{
	HWND	hWnd	= GetForegroundWindow();

	EnumWindows(EnumCloseWindowProc, (LPARAM)hWnd);
	SetForegroundWindow(hWnd);
}

void MinimizeTask(void)
{
	Today();
}

void ResetDevice(void)
{
	CResourceX	res(GetResInst());
	DWORD		dwFlags;

	if (MessageBox(NULL, res.LoadString(IDS_RESET_NOTICE), res.LoadString(IDS_RESET), MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_SETFOREGROUND | MB_TOPMOST) == IDYES)
	{
		if (g_dwResetMethod)
		{
			dwFlags	= EWX_DEFER | EWX_PROMPT;
		}
		else
		{
			dwFlags	= 0;
		}

		Reset(dwFlags);
	}
}

void RotateScreen(void)
{
	CResourceX		res(GetResInst());
	DWORD			dwRotation;
	DWORD			dwMajor;
	DWORD			dwMinor;

	GetVersionX(&dwMajor, &dwMinor);

	if ((dwMajor < 4) || ((dwMajor == 4) && (dwMinor < 21)))
	{
		MessageBox(NULL, res.LoadString(IDS_ROTATE_NOSUPPORT), res.LoadString(IDS_INCLOSE), MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
		return;
	}

	if (GetRotationMode(&dwRotation) != DISP_CHANGE_SUCCESSFUL)
	{
		MessageBox(NULL, res.LoadString(IDS_ROTATE_NOSUPPORT), res.LoadString(IDS_INCLOSE), MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
		return;
	}

	if (dwRotation != DMDO_0)
	{
		SetRotationMode(DMDO_0);
	}
	else
	{
		SetRotationMode(g_dwRotation);
	}
}

void ScreenOff(void)
{
	SetDisplayMode(FALSE);
}

void SuspendDevice(void)
{
	Suspend();
}

void Today(void)
{
	SetForegroundWindow(FindWindow(_T("DesktopExplorerWindow"), NULL));
}

void SetExcludedTask(HWND hWnd)
{
	if (g_nExcluded < MAX_EXCLUDED)
	{
		TCHAR		szFileName[MAX_PATH + 1];
		TCHAR		szWindow[MAX_TITLE_LEN + 1];
		DWORD		dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);
		CResourceX	res(GetResInst());
		TCHAR		szMessage[512];
		int			nRes;

		GetModuleFileNameEx(hWnd, szFileName, MAX_PATH);
		GetWindowText(hWnd, szWindow, MAX_TITLE_LEN);

		if (dwExStyle & 0x80000000)
		{
			wsprintf(szMessage, res.LoadString(IDS_EXCLUDED_DIALOG), szWindow);

			nRes	= MessageBox(NULL, szMessage, res.LoadString(IDS_SETEXCLUDED), MB_YESNOCANCEL | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST);

			switch (nRes)
			{
			case IDYES:
				{
					_tcscpy_s(g_szExcluded[g_nExcluded], MAX_PATH, szFileName);

					g_nExcludedType[g_nExcluded]	= EXC_DIALOG;
					g_nExcluded++;

					SaveExcludedList();
				}
				break;
			case IDNO:
				{
					_tcscpy_s(g_szExcluded[g_nExcluded], MAX_PATH, szFileName);

					g_nExcludedType[g_nExcluded]	= EXC_MAINFRAME;
					g_nExcluded++;

					SaveExcludedList();
				}
				break;
			}
		}
		else
		{
			wsprintf(szMessage, res.LoadString(IDS_EXCLUDED_MAINFRAME), szWindow);

			nRes	= MessageBox(NULL, szMessage, res.LoadString(IDS_SETEXCLUDED), MB_YESNO | MB_ICONINFORMATION | MB_SETFOREGROUND | MB_TOPMOST);

			if (nRes == IDYES)
			{
				_tcscpy_s(g_szExcluded[g_nExcluded], MAX_PATH, szFileName);

				g_nExcludedType[g_nExcluded]	= EXC_MAINFRAME;
				g_nExcluded++;

				SaveExcludedList();
			}
		}
	}
}

void LoadExcludedList(void)
{
	CRegistryX	reg;
	TCHAR		szKey[MAX_PATH + 1];
	DWORD		dwSize;

	g_nExcluded	= 0;

	memset(g_szExcluded, 0, sizeof(TCHAR) * MAX_PATH * MAX_EXCLUDED);
	reg.Open(HKEY_CURRENT_USER, RK_CL_EXCLUDED);

	g_nExcluded	= reg.ReadDWORD(RV_CL_COUNT, 0);

	reg.Close();

	if (g_nExcluded > MAX_EXCLUDED)
	{
		g_nExcluded = MAX_EXCLUDED;
	}

	for (int i = 0; i < g_nExcluded; i++)
	{
		dwSize	= MAX_PATH;

		wsprintf(szKey, _T("%s\\%03d"), RK_CL_EXCLUDED, i);
		reg.Open(HKEY_CURRENT_USER, szKey);
		reg.ReadString(RV_CL_MODULE, g_szExcluded[i], &dwSize);

		g_nExcludedType[i]	= reg.ReadDWORD(RV_CL_TYPE);

		reg.Close();
	}
}

void SaveExcludedList(void)
{
	CRegistryX	reg;
	TCHAR		szKey[MAX_PATH + 1];
	TCHAR		szDefaultItem[MAX_PATH + 1];
	DWORD		dwSize	= MAX_PATH;

	reg.Open(HKEY_CURRENT_USER, RK_CL_EXCLUDED);
	reg.ReadString(RV_CL_DEFAULTITEM, szDefaultItem, &dwSize);
	reg.Close();
	reg.DeleteKey(HKEY_CURRENT_USER, RK_CL_EXCLUDED);
	reg.Open(HKEY_CURRENT_USER, RK_CL_EXCLUDED);
	reg.WriteString(RV_CL_DEFAULTITEM, szDefaultItem);
	reg.WriteDWORD(RV_CL_COUNT, g_nExcluded);
	reg.Close();

	for (int i = 0; i < g_nExcluded; i++)
	{
		wsprintf(szKey, _T("%s\\%03d"), RK_CL_EXCLUDED, i);
		reg.Open(HKEY_CURRENT_USER, szKey);
		reg.WriteString(RV_CL_MODULE, g_szExcluded[i]);
		reg.WriteDWORD(RV_CL_TYPE, g_nExcludedType[i]);
		reg.Close();
	}
}

BOOL CALLBACK EnumCloseWindowProc(HWND hWnd, LPARAM lParam)
{
	HWND	hDirectWnd;
	DWORD	dwType		= 0;
	DWORD	dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);

	if (hWnd == (HWND)lParam)
	{
		return TRUE;
	}

	if ((hDirectWnd = IsSHFindWindow(hWnd)) != NULL)
	{
		hWnd		= hDirectWnd;
		dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);
	}
	else if ((hDirectWnd = IsPhonePrimaryWindow(hWnd)) != NULL)
	{
		SmartMinimize(hDirectWnd);
		return TRUE;
	}
	else if ((hDirectWnd = IsPhoneSecondaryWindow(hWnd)) != NULL)
	{
		hWnd	= hDirectWnd;
	}

	if ((hDirectWnd = IsWirelessManagerWindow(hWnd)) != NULL)
	{
		hWnd	= hDirectWnd;
	}

	if (IsExcluded(hWnd) == (EXC_MAINWIN | EXC_ALWAYS))
	{
		if (dwExStyle & 0x80000000)
		{
			Today();
		}
		else if (IsWindow(hWnd))
		{
			SmartMinimize(hWnd);
		}

		return TRUE;
	}

	if (!IsEnableCloseWindow(hWnd, TRUE, NULL, NULL, NULL, NULL))
	{
		if (IsWindow(hWnd))
		{
			SmartMinimize(hWnd);
		}

		return TRUE;
	}

	PostCloseMessage(hWnd);
	return TRUE;
}
