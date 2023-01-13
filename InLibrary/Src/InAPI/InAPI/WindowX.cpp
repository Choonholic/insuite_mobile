// WindowX.cpp

#include "stdafx.h"
#include "DisplayX.h"
#include "ShellX.h"
#include "WindowX.h"

#define EXPLORE		_T("Explore")
#define CALENDAR	_T("Calendar")
#define CONTACTS	_T("MSContactsUI")
#define PEGHELP		_T("PegHelp")
#define SHFIND		_T("shfind")
#define MSCPROG		_T("MSCprog")
#define MSCLOG		_T("MSClog")
#define MSSCUT		_T("MSScut")

#define SHELL_96	_T("shellres.96.dll")
#define SHELL_192	_T("shellres.192.dll")
#define SHELL_128	_T("shellres.128.dll")
#define SHELL_131	_T("shellres.131.dll")
#define OUTRES		_T("outres.dll")
#define TAPRES		_T("tapres.dll")

HICON GetWindowIconExInternal(HWND hWnd, BOOL bType)
{
	HWND	hWndParent	= hWnd;
	HICON	hIcon		= NULL;
	TCHAR	szFilePath[MAX_PATH];

	if (!hWnd)
	{
		return NULL;
	}

	while (hWndParent)
	{
		hIcon	= reinterpret_cast<HICON>(SendMessage(hWndParent, WM_GETICON, bType, 0));

		if (hIcon)
		{
			return hIcon;
		}

		hWndParent	= GetParent(hWndParent);
	}

	if (!GetModuleFileNameEx(hWnd, szFilePath, MAX_PATH))
	{
		return NULL;
	}

	return GetFileIcon(szFilePath, bType);
}

HICON GetWindowPreIconEx(HWND hWnd, BOOL bType)
{
	TCHAR	szClass[MAX_PATH];
	TCHAR	szTitle[MAX_PATH];
	HICON	hIcon	= NULL;
	DWORD	dwDPI	= GetDPI();

	if (!hWnd)
	{
		return NULL;
	}

	GetClassName(hWnd, szClass, MAX_PATH);
	GetWindowText(hWnd, szTitle, MAX_PATH);

	if (!_tcsicmp(szClass, CALENDAR))
	{
		if (bType)
		{
			ExtractIconEx(OUTRES, -10002, &hIcon, NULL, 1);
		}
		else
		{
			ExtractIconEx(OUTRES, -10002, NULL, &hIcon, 1);
		}

		return hIcon;
	}

	if (!_tcsicmp(szClass, CONTACTS))
	{
		if (bType)
		{
			ExtractIconEx(OUTRES, -10000, &hIcon, NULL, 1);
		}
		else
		{
			ExtractIconEx(OUTRES, -10000, NULL, &hIcon, 1);
		}

		return hIcon;
	}

	if (!_tcsicmp(szClass, EXPLORE))
	{
		if (bType)
		{
			switch (dwDPI)
			{
			case DPI_96:
				{
					ExtractIconEx(SHELL_96, -8217, &hIcon, NULL, 1);
				}
				break;
			case DPI_192:
				{
					ExtractIconEx(SHELL_192, -8217, &hIcon, NULL, 1);
				}
				break;
			case DPI_128:
				{
					ExtractIconEx(SHELL_128, -8217, &hIcon, NULL, 1);
				}
				break;
			case DPI_131:
				{
					ExtractIconEx(SHELL_131, -8217, &hIcon, NULL, 1);
				}
				break;
			}
		}
		else
		{
			switch (dwDPI)
			{
			case DPI_96:
				{
					ExtractIconEx(SHELL_96, -8217, NULL, &hIcon, 1);
				}
				break;
			case DPI_192:
				{
					ExtractIconEx(SHELL_192, -8217, NULL, &hIcon, 1);
				}
				break;
			case DPI_128:
				{
					ExtractIconEx(SHELL_128, -8217, NULL, &hIcon, 1);
				}
				break;
			case DPI_131:
				{
					ExtractIconEx(SHELL_131, -8217, NULL, &hIcon, 1);
				}
				break;
			}
		}

		return hIcon;
	}

	if (!_tcsicmp(szClass, PEGHELP))
	{
		if (bType)
		{
			switch (dwDPI)
			{
			case DPI_96:
				{
					ExtractIconEx(SHELL_96, -8212, &hIcon, NULL, 1);
				}
				break;
			case DPI_192:
				{
					ExtractIconEx(SHELL_192, -8212, &hIcon, NULL, 1);
				}
				break;
			case DPI_128:
				{
					ExtractIconEx(SHELL_128, -8212, &hIcon, NULL, 1);
				}
				break;
			case DPI_131:
				{
					ExtractIconEx(SHELL_131, -8212, &hIcon, NULL, 1);
				}
				break;
			}
		}
		else
		{
			switch (dwDPI)
			{
			case DPI_96:
				{
					ExtractIconEx(SHELL_96, -8212, NULL, &hIcon, 1);
				}
				break;
			case DPI_192:
				{
					ExtractIconEx(SHELL_192, -8212, NULL, &hIcon, 1);
				}
				break;
			case DPI_128:
				{
					ExtractIconEx(SHELL_128, -8212, NULL, &hIcon, 1);
				}
				break;
			case DPI_131:
				{
					ExtractIconEx(SHELL_131, -8212, NULL, &hIcon, 1);
				}
				break;
			}
		}

		return hIcon;
	}

	if (!_tcsicmp(szClass, MSCPROG))
	{
		if (bType)
		{
			ExtractIconEx(TAPRES, -22555, &hIcon, NULL, 1);
		}
		else
		{
			ExtractIconEx(TAPRES, -22555, NULL, &hIcon, 1);
		}

		return hIcon;
	}

	if (!_tcsicmp(szClass, MSCLOG))
	{
		if (bType)
		{
			ExtractIconEx(TAPRES, -22555, &hIcon, NULL, 1);
		}
		else
		{
			ExtractIconEx(TAPRES, -22555, NULL, &hIcon, 1);
		}

		return hIcon;
	}

	if (!_tcsicmp(szClass, MSSCUT))
	{
		if (bType)
		{
			ExtractIconEx(TAPRES, -22555, &hIcon, NULL, 1);
		}
		else
		{
			ExtractIconEx(TAPRES, -22555, NULL, &hIcon, 1);
		}

		return hIcon;
	}

	if (!_tcsicmp(szClass, _T("Dialog")))
	{
		if (FindWindow(SHFIND, szTitle))
		{
			if (bType)
			{
				switch (dwDPI)
				{
				case DPI_96:
					{
						ExtractIconEx(SHELL_96, -8213, &hIcon, NULL, 1);
					}
					break;
				case DPI_192:
					{
						ExtractIconEx(SHELL_192, -8213, &hIcon, NULL, 1);
					}
					break;
				case DPI_128:
					{
						ExtractIconEx(SHELL_128, -8213, &hIcon, NULL, 1);
					}
					break;
				case DPI_131:
					{
						ExtractIconEx(SHELL_131, -8213, &hIcon, NULL, 1);
					}
					break;
				}
			}
			else
			{
				switch (dwDPI)
				{
				case DPI_96:
					{
						ExtractIconEx(SHELL_96, -8213, NULL, &hIcon, 1);
					}
					break;
				case DPI_192:
					{
						ExtractIconEx(SHELL_192, -8213, NULL, &hIcon, 1);
					}
					break;
				case DPI_128:
					{
						ExtractIconEx(SHELL_128, -8213, NULL, &hIcon, 1);
					}
					break;
				case DPI_131:
					{
						ExtractIconEx(SHELL_131, -8213, NULL, &hIcon, 1);
					}
					break;
				}
			}

			return hIcon;
		}

		if (FindWindow(CALENDAR, szTitle))
		{
			if (bType)
			{
				ExtractIconEx(OUTRES, -10002, &hIcon, NULL, 1);
			}
			else
			{
				ExtractIconEx(OUTRES, -10002, NULL, &hIcon, 1);
			}

			return hIcon;
		}

		if (FindWindow(CONTACTS, szTitle))
		{
			if (bType)
			{
				ExtractIconEx(OUTRES, -10000, &hIcon, NULL, 1);
			}
			else
			{
				ExtractIconEx(OUTRES, -10000, NULL, &hIcon, 1);
			}

			return hIcon;
		}

		HWND	hWndMSCprog;

		if ((hWndMSCprog = FindWindow(MSCPROG, szTitle)) != NULL)
		{
			if (bType)
			{
				ExtractIconEx(TAPRES, -22555, &hIcon, NULL, 1);
			}
			else
			{
				ExtractIconEx(TAPRES, -22555, NULL, &hIcon, 1);
			}

			return hIcon;
		}
	}

	return NULL;
}

HICON GetWindowIconEx(HWND hWnd, BOOL bType)
{
	HICON	hIcon;

	if ((hIcon = GetWindowPreIconEx(hWnd, bType)) != NULL)
	{
		return hIcon;
	}

	return GetWindowIconExInternal(hWnd, bType);
}

DWORD GetModuleFileNameEx(HWND hWnd, LPWSTR lpszFileName, DWORD cbFileName)
{
	DWORD	dwProcessID	= NULL;
	DWORD	dwSize		= 0;

	GetWindowThreadProcessId(hWnd, &dwProcessID);

	HINSTANCE	hInstance	= reinterpret_cast<HINSTANCE>(OpenProcess(0, 0, dwProcessID));

	if (!hInstance)
	{
		lpszFileName	= NULL;

		return dwSize;
	}

	dwSize	= GetModuleFileName(hInstance, lpszFileName, cbFileName);

	CloseHandle(hInstance);
	return dwSize;
}

BOOL IsTopMostPopupWindow(HWND hWnd, BOOL bAboveStartup)
{
	DWORD	dwStyle		= GetWindowLong(hWnd, GWL_STYLE);
	DWORD	dwExStyle	= GetWindowLong(hWnd, GWL_EXSTYLE);

	if ((dwStyle & 0x80010004) && (dwExStyle & (bAboveStartup ? 0x20000088 : 0x00000088)))
	{
		return TRUE;
	}

	return FALSE;
}
