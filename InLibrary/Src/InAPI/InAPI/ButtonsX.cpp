// ButtonsX.cpp

#include "StdAfx.h"
#include "ButtonsX.h"
#include "ShellX.h"
#include "PhoneX.h"
#include "RegistryX.h"
#include "InLibrary_Messages.h"

#ifndef MAX_HOOKWND
#define MAX_HOOKWND			32
#endif

// Hook Information
INT		g_nHookCnt			= 0;
HWND	g_hHookWnd[MAX_HOOKWND];
UINT	g_uHookBtn[MAX_HOOKWND];

// Windows and Procedures
HWND	g_hWndTB			= NULL;
HWND	g_hWndMC			= NULL;
LONG	g_lOldTBProc		= 0;
LONG	g_lOldMCProc		= 0;

// Original Message Backups
HWND	g_hWndTTALKDown		= NULL;
HWND	g_hWndTTALKUp		= NULL;
HWND	g_hWndTENDDown		= NULL;
HWND	g_hWndTENDUp		= NULL;
HWND	g_hWndKEYLOCKDown	= NULL;
HWND	g_hWndKEYLOCKUp		= NULL;
HWND	g_hWndTVOLUPDown	= NULL;
HWND	g_hWndTVOLUPUp		= NULL;
HWND	g_hWndTVOLDOWNDown	= NULL;
HWND	g_hWndTVOLDOWNUp	= NULL;

#define HKID_TEND			0x00000000
#define HKID_TTALK			0x00000001
#define HKID_TVOLUMEUP		0x00000002
#define HKID_TVOLUMEDOWN	0x00000003
#define HKID_KEYLOCK		0x00000006

LRESULT CALLBACK WndTBProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int		i;
	BOOL	bInvoked	= FALSE;

	for (i = 0; i < g_nHookCnt; i++)
	{
		if (g_uHookBtn[i] & BTNX_TTALK)
		{
			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(0x0000, VK_TTALK)))
			{
				g_hWndTTALKDown	= hwnd;
				bInvoked		= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_TTALK_DOWN, 0, 0);
			}

			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(MOD_KEYUP, VK_TTALK)))
			{
				g_hWndTTALKUp	= hwnd;
				bInvoked		= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_TTALK_UP, 0, 0);
			}
		}

		if (g_uHookBtn[i] & BTNX_KEYLOCK)
		{
			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(0x0000, VK_F22)))
			{
				g_hWndKEYLOCKDown	= hwnd;
				bInvoked			= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_KEYLOCK_DOWN, 0, 0);
			}

			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(MOD_KEYUP, VK_F22)))
			{
				g_hWndKEYLOCKUp		= hwnd;
				bInvoked			= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_KEYLOCK_UP, 0, 0);
			}
		}

		if (g_uHookBtn[i] & BTNX_TVOLUME)
		{
			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(0x0000, VK_TVOLUMEUP)))
			{
				g_hWndTVOLUPDown	= hwnd;
				bInvoked			= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_TVOLUMEUP_DOWN, 0, 0);
			}

			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(MOD_KEYUP, VK_TVOLUMEUP)))
			{
				g_hWndTVOLUPUp		= hwnd;
				bInvoked			= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_TVOLUMEUP_UP, 0, 0);
			}

			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(0x0000, VK_TVOLUMEDOWN)))
			{
				g_hWndTVOLDOWNDown	= hwnd;
				bInvoked			= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_TVOLUMEDOWN_DOWN, 0, 0);
			}

			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(MOD_KEYUP, VK_TVOLUMEDOWN)))
			{
				g_hWndTVOLDOWNUp	= hwnd;
				bInvoked			= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_TVOLUMEDOWN_UP, 0, 0);
			}
		}
	}

	if (!bInvoked)
	{
		return CallWindowProc((WNDPROC)g_lOldTBProc, hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK WndMCProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int		i;
	BOOL	bInvoked	= FALSE;

	for (i = 0; i < g_nHookCnt; i++)
	{
		if (g_uHookBtn[i] & BTNX_TEND)
		{
			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(0x0000, VK_TEND)))
			{
				g_hWndTENDDown	= hwnd;
				bInvoked		= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_TEND_DOWN, 0, 0);
			}

			if ((uMsg == WM_HOTKEY) && (lParam == MAKELPARAM(MOD_KEYUP, VK_TEND)))
			{
				g_hWndTENDUp	= hwnd;
				bInvoked		= TRUE;

				PostMessage(g_hHookWnd[i], WM_BTNX_TEND_UP, 0, 0);
			}
		}
	}

	if (!bInvoked)
	{
		return CallWindowProc((WNDPROC)g_lOldMCProc, hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void CleanUpButtons(void)
{
	if ((g_hWndTB != NULL) && (g_lOldTBProc != 0))
	{
		SetWindowLong(g_hWndTB, GWL_WNDPROC, g_lOldTBProc);
	}

	if ((g_hWndMC != NULL) && (g_lOldMCProc != 0))
	{
		SetWindowLong(g_hWndMC, GWL_WNDPROC, g_lOldMCProc);
	}

	memset(&g_hHookWnd, 0, sizeof(HWND) * MAX_HOOKWND);
	memset(&g_uHookBtn, 0, sizeof(UINT) * MAX_HOOKWND);

	g_nHookCnt			= 0;
	g_hWndTB			= NULL;
	g_hWndMC			= NULL;
	g_lOldTBProc		= 0;
	g_lOldMCProc		= 0;
	g_hWndTTALKDown		= NULL;
	g_hWndTTALKUp		= NULL;
	g_hWndTENDDown		= NULL;
	g_hWndTENDUp		= NULL;
	g_hWndKEYLOCKDown	= NULL;
	g_hWndKEYLOCKUp		= NULL;
	g_hWndTVOLUPDown	= NULL;
	g_hWndTVOLUPUp		= NULL;
	g_hWndTVOLDOWNDown	= NULL;
	g_hWndTVOLDOWNUp	= NULL;
}

#define HOOK_WAIT	300					// 5 Minutes
#define PHONE_EXEC	_T("cprog.exe")		// Phone

INT HookButtons(HWND hWnd, UINT uButtons)
{
	BOOL	bTBReady	= FALSE;
	BOOL	bMCReady	= FALSE;
	HWND	hWndTB		= NULL;
	HWND	hWndMC		= NULL;
	int		i;

	if (g_nHookCnt == MAX_HOOKWND)
	{
		return BTNX_LIMITEXCEEDED;
	}

	if (uButtons & (BTNX_TTALK | BTNX_TEND))
	{
		if (!DetectPhoneEdition())
		{
			return BTNX_NOPHONE;
		}
	}

	for (i = 0; i < g_nHookCnt; i++)
	{
		if (g_hHookWnd[i] == hWnd)
		{
			g_uHookBtn[i]	&= ~uButtons;
			g_uHookBtn[i]	|= uButtons;

			return BTNX_NOERROR;
		}
	}

	if (uButtons & (BTNX_TTALK | BTNX_KEYLOCK))
	{
		bTBReady	= TRUE;

		if ((g_hWndTB != NULL) && (g_lOldTBProc != 0))
		{
		}
		else
		{
			for (i = 0; i < HOOK_WAIT; i++)
			{
				if ((hWndTB = FindWindow(_T("HHTaskBar"), NULL)) != NULL)
				{
					break;
				}

				Sleep(1000);
			}

			if (hWndTB == NULL)
			{
				return BTNX_NOTASKBAR;
			}
		}
	}

	if (uButtons & BTNX_TEND)
	{
		bMCReady	= TRUE;

		if ((g_hWndMC != NULL) && (g_lOldMCProc != 0))
		{
		}
		else
		{
			for (i = 0; i < HOOK_WAIT; i++)
			{
				if ((hWndMC = FindWindow(_T("MSCprog"), NULL)) != NULL)
				{
					break;
				}

				Sleep(1000);
			}

			if (hWndMC == NULL)
			{
				return BTNX_NOMSCPROG;
			}
		}
	}

	if (hWndTB != NULL)
	{
		g_hWndTB		= hWndTB;
		g_lOldTBProc	= GetWindowLong(g_hWndTB, GWL_WNDPROC);

		SetWindowLong(g_hWndTB, GWL_WNDPROC, (LONG)WndTBProc);
	}

	if (hWndMC != NULL)
	{
		g_hWndMC		= hWndMC;
		g_lOldMCProc	= GetWindowLong(g_hWndMC, GWL_WNDPROC);

		SetWindowLong(g_hWndMC, GWL_WNDPROC, (LONG)WndMCProc);
	}

	if ((bTBReady == TRUE) || (bMCReady == TRUE))
	{
		g_hHookWnd[g_nHookCnt]	= hWnd;
		g_uHookBtn[g_nHookCnt]	= uButtons;
		g_nHookCnt++;

		return BTNX_NOERROR;
	}

	return BTNX_NOHOOK;
}

INT UnhookButtons(HWND hWnd)
{
	int		i;
	int		j;
	BOOL	bFound	= FALSE;

	if (g_nHookCnt == 0)
	{
		return BTNX_NOHOOK;
	}

	for (i = 0; i < g_nHookCnt; i++)
	{
		if (g_hHookWnd[i] == hWnd)
		{
			for (j = i; j < (g_nHookCnt - 1); j++)
			{
				g_hHookWnd[j]	= g_hHookWnd[j + 1];
				g_uHookBtn[j]	= g_uHookBtn[j + 1];
			}

			g_hHookWnd[g_nHookCnt - 1]	= NULL;
			g_uHookBtn[g_nHookCnt - 1]	= 0;
			bFound						= TRUE;
			g_nHookCnt--;
			break;
		}
	}

	if (bFound == FALSE)
	{
		return BTNX_NOWND;
	}

	if (g_nHookCnt == 0)
	{
		CleanUpButtons();
	}

	return BTNX_NOERROR;
}

LRESULT DoDefaultProc(UINT uMessage)
{
	LRESULT	lResult	= 0;

	switch (uMessage)
	{
	case WM_BTNX_TTALK_DOWN:
		{
			if (g_hWndTTALKDown == NULL)
			{
				return 0;
			}

			lResult			= CallWindowProc((WNDPROC)g_lOldTBProc, g_hWndTTALKDown, WM_HOTKEY, HKID_TTALK, MAKELPARAM(0x0000, VK_TTALK));
			g_hWndTTALKDown	= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_TTALK_UP:
		{
			if (g_hWndTTALKUp == NULL)
			{
				return 0;
			}

			lResult			= CallWindowProc((WNDPROC)g_lOldTBProc, g_hWndTTALKUp, WM_HOTKEY, HKID_TTALK, MAKELPARAM(MOD_KEYUP, VK_TTALK));
			g_hWndTTALKUp	= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_KEYLOCK_DOWN:
		{
			if (g_hWndKEYLOCKDown == NULL)
			{
				return 0;
			}

			lResult				= CallWindowProc((WNDPROC)g_lOldTBProc, g_hWndKEYLOCKDown, WM_HOTKEY, HKID_KEYLOCK, MAKELPARAM(0x0000, VK_F22));
			g_hWndKEYLOCKDown	= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_KEYLOCK_UP:
		{
			if (g_hWndKEYLOCKUp == NULL)
			{
				return 0;
			}

			lResult			= CallWindowProc((WNDPROC)g_lOldTBProc, g_hWndKEYLOCKUp, WM_HOTKEY, HKID_KEYLOCK, MAKELPARAM(MOD_KEYUP, VK_F22));
			g_hWndKEYLOCKUp	= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_TVOLUMEUP_DOWN:
		{
			if (g_hWndTVOLUPDown == NULL)
			{
				return 0;
			}

			lResult				= CallWindowProc((WNDPROC)g_lOldTBProc, g_hWndTVOLUPDown, WM_HOTKEY, HKID_TVOLUMEUP, MAKELPARAM(0x0000, VK_TVOLUMEUP));
			g_hWndTVOLUPDown	= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_TVOLUMEUP_UP:
		{
			if (g_hWndTVOLUPUp == NULL)
			{
				return 0;
			}

			lResult				= CallWindowProc((WNDPROC)g_lOldTBProc, g_hWndTVOLUPUp, WM_HOTKEY, HKID_TVOLUMEUP, MAKELPARAM(MOD_KEYUP, VK_TVOLUMEUP));
			g_hWndTVOLUPUp		= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_TVOLUMEDOWN_DOWN:
		{
			if (g_hWndTVOLDOWNDown == NULL)
			{
				return 0;
			}

			lResult				= CallWindowProc((WNDPROC)g_lOldTBProc, g_hWndTVOLDOWNDown, WM_HOTKEY, HKID_TVOLUMEDOWN, MAKELPARAM(0x0000, VK_TVOLUMEDOWN));
			g_hWndTVOLDOWNDown	= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_TVOLUMEDOWN_UP:
		{
			if (g_hWndTVOLDOWNUp == NULL)
			{
				return 0;
			}

			lResult				= CallWindowProc((WNDPROC)g_lOldTBProc, g_hWndTVOLDOWNUp, WM_HOTKEY, HKID_TVOLUMEDOWN, MAKELPARAM(MOD_KEYUP, VK_TVOLUMEDOWN));
			g_hWndTVOLDOWNUp	= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_TEND_DOWN:
		{
			if (g_hWndTENDDown == NULL)
			{
				return 0;
			}

			lResult			= CallWindowProc((WNDPROC)g_lOldMCProc, g_hWndTENDDown, WM_HOTKEY, HKID_TEND, MAKELPARAM(0x0000, VK_TEND));
			g_hWndTENDDown	= NULL;

			return lResult;
		}
		break;
	case WM_BTNX_TEND_UP:
		{
			if (g_hWndTENDUp == NULL)
			{
				return 0;
			}

			lResult			= CallWindowProc((WNDPROC)g_lOldMCProc, g_hWndTENDUp, WM_HOTKEY, HKID_TEND, MAKELPARAM(MOD_KEYUP, VK_TEND));
			g_hWndTENDUp	= NULL;

			return lResult;
		}
		break;
	}

	return lResult;
}


#define MIN_APPBUTTON	0xC1
#define MAX_APPBUTTON	0xCF
#define DEF_APPKEY		_T("SOFTWARE\\Microsoft\\Shell\\Keys")

BOOL IsAvailableAppButton(int nIndex)
{
	CRegistryX	reg;
	TCHAR		szKey[MAX_PATH + 1];
	BOOL		bExist	= FALSE;

	if ((nIndex < MIN_APPBUTTON) || (nIndex > MAX_APPBUTTON))
	{
		return bExist;
	}

	wsprintf(szKey, _T("40%02X"), nIndex);
	reg.Open(HKEY_LOCAL_MACHINE, DEF_APPKEY);

	if (reg.IsSubKeyExist(szKey))
	{
		bExist	= TRUE;
	}

	reg.Close();
	return bExist;
}

typedef BOOL	(__stdcall *UnregisterFunc1Proc)(UINT, UINT);

BOOL UnregisterFunction(UINT uiMod, UINT uiKey)
{
	HINSTANCE			hCoreDll;
	UnregisterFunc1Proc	procUnregisterFunc1;
	BOOL				bResult;

	hCoreDll	= LoadLibrary(_T("coredll.dll"));

	ASSERT(hCoreDll != NULL);

	procUnregisterFunc1	= (UnregisterFunc1Proc)GetProcAddress(hCoreDll, _T("UnregisterFunc1"));

	ASSERT(procUnregisterFunc1 != NULL);

	bResult	= procUnregisterFunc1(uiMod, uiKey);

	FreeLibrary(hCoreDll);
	return bResult;
}
