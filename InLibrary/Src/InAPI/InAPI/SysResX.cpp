// SysResX.cpp

#include "stdafx.h"
#include "SysResX.h"
#include "ShellX.h"

CSysResX::CSysResX(void) : m_bWM2003SE(FALSE), m_bWM5(FALSE), m_bWM6(FALSE), m_hCoreDll(NULL), m_hCEShell(NULL), m_hNoteProject(NULL), m_hAygShell(NULL)
{
	SHInitExtraControls();

	m_hCoreDll	= LoadLibrary(_T("coredll.dll"));
	m_hAygShell	= LoadLibrary(_T("aygshell.dll"));
	m_hCEShell	= LoadLibrary(_T("shellres.dll"));

	if (m_hCEShell == NULL)
	{
		m_hCEShell		= LoadLibrary(_T("ceshell.dll"));
		m_hNoteProject	= LoadLibrary(_T("note_prj.dll"));
	}
	else
	{
		m_bWM2003SE		= TRUE;
		m_hNoteProject	= LoadLibrary(_T("shellresapps.dll"));

		if (!FindResource(m_hNoteProject, reinterpret_cast<LPCWSTR>(13445), RT_DIALOG))
		{
			m_bWM5	= TRUE;
		}

		if (FindResource(m_hCEShell, reinterpret_cast<LPCWSTR>(19488), RT_MENU))
		{
			m_bWM6	= TRUE;
		}
	}
}

CSysResX::~CSysResX(void)
{
	if (m_hAygShell)
	{
		FreeLibrary(m_hAygShell);
	}

	if (m_hCoreDll)
	{
		FreeLibrary(m_hCoreDll);
	}

	if (m_hCEShell)
	{
		FreeLibrary(m_hCEShell);
	}

	if (m_hNoteProject)
	{
		FreeLibrary(m_hNoteProject);
	}
}

BOOL CSysResX::IsWM5(void)
{
	return m_bWM5;
}

int CSysResX::LoadString(UINT uID, LPTSTR lpBuffer, int cchBufferMax)
{
	HINSTANCE		hInstance	= m_hCEShell;
	UINT			uIDInt		= 0;
	HMENU			hMenu;
	MENUITEMINFO	mii;

	switch (uID)
	{
	case SYSRES_OPEN_TITLE:
	case SYSRES_SAVE_TITLE:
		{
			hInstance	= m_hNoteProject;
			uIDInt		= ((uID == SYSRES_OPEN_TITLE) ? 52 : 51);

			if (m_bWM2003SE)
			{
				uIDInt	+= 13454;
			}
		}
		break;
	case SYSRES_COLUMN_NAME:
		{
			uIDInt	= ((!m_bWM2003SE) ? 3 : 8451);
		}
		break;
	case SYSRES_COLUMN_SIZE:
	case SYSRES_COLUMN_TYPE:
	case SYSRES_COLUMN_DATE:
		{
			switch (uID)
			{
			case SYSRES_COLUMN_TYPE:
				{
					uIDInt	+= 1;
				}
			case SYSRES_COLUMN_SIZE:
				{
					uIDInt	+= 1;
				}
			case SYSRES_COLUMN_DATE:
				{
					uIDInt	+= 4;
				}
			}

			if (m_bWM2003SE)
			{
				uIDInt	+= 8448;
			}
		}
		break;
	case SYSRES_DEF_DIR:
		{
			uIDInt	= (m_bWM2003SE ? 8809 : 36933);
		}
		break;
	case SYSRES_DEF_ROOT_NAME:
		{
			if (m_bWM6)
			{
				uIDInt	= 19466;
			}
			else
			{
				LPCTSTR	szBrowse;

				if (m_bWM2003SE)
				{
					szBrowse	= _T("browsres.dll");
					uIDInt		= 4131;
				}
				else
				{
					szBrowse	= _T("browser.dll");
					uIDInt		= 533;
				}

				hInstance	= LoadLibrary(szBrowse);
			}
		}
		break;
	case SYSRES_MSG_PATH_MUST_EXIST:
		{
			uIDInt	= (m_bWM2003SE ? 8771 : 36934);
		}
		break;
	case SYSRES_MSG_FILE_MUST_EXIST:
		{
			uIDInt	+= 1;
		}
	case SYSRES_MSG_CREATE_PROMPT:
		{
			uIDInt	+= 1;
		}
	case SYSRES_MSG_OVERWRITE_PROMPT:
		{
			uIDInt	+= 36924;

			if (m_bWM2003SE)
			{
				uIDInt	-= 28160;
			}
		}
		break;
	case SYSRES_CAPTION_CANCEL:
	case SYSRES_CAPTION_NAME:
	case SYSRES_CAPTION_TYPE:
		{
			int	iDlg;

			if (!m_bWM5)
			{
				hInstance	= m_hNoteProject;
				iDlg		= (m_bWM2003SE ? 13445 : 112);
				uIDInt		= ((uID == SYSRES_CAPTION_NAME) ? 2060 : 2067);
			}
			else
			{
				hInstance	= LoadLibrary(_T("tshres.dll"));
				iDlg		= 16210;
				uIDInt		= ((uID == SYSRES_CAPTION_NAME) ? 16211 : 16215);
			}

			if (uID == SYSRES_CAPTION_CANCEL)
			{
				uIDInt	= IDCANCEL;
			}

			HWND	hOrgDlg	= CreateDialog(hInstance, reinterpret_cast<LPCTSTR>(iDlg), NULL, DummyProc);

			if (!hOrgDlg)
			{
				hOrgDlg	= CreateDialog(m_hCoreDll, reinterpret_cast<LPCTSTR>(1602), GetForegroundWindow(), DummyProc);
				uIDInt	= ((uID == SYSRES_CAPTION_NAME) ? 1703 : 1702);
			}

			int	nRes	= GetDlgItemText(hOrgDlg, uIDInt, lpBuffer, cchBufferMax);

			DestroyWindow(hOrgDlg);

			if ((hInstance != m_hNoteProject) && (hInstance != m_hCEShell))
			{
				FreeLibrary(hInstance);
			}

			return nRes;
		}
		break;
	case SYSRES_CAPTION_NEWFOLDER:
		{
			uIDInt	= 36870;

			if (m_bWM2003SE)
			{
				uIDInt	= 8610;
			}
		}
		break;
	case SYSRES_MSG_INVALIDNAME:
		{
			uIDInt	= 36914;

			if (m_bWM2003SE)
			{
				uIDInt	= 8754;
			}
		}
		break;
	case SYSRES_MSG_ALREADYEXISTS:
		{
			uIDInt	= 36915;

			if (m_bWM2003SE)
			{
				uIDInt	= 8755;
			}
		}
		break;
	case SYSRES_MSG_NEWFOLDERERROR:
		{
			uIDInt	= 36890;

			if (m_bWM2003SE)
			{
				uIDInt	= 8730;
			}
		}
		break;
	case SYSRES_MSG_FOLDERTOOLONG:
		{
			uIDInt	= 36893;

			if (m_bWM2003SE)
			{
				uIDInt	= 8733;
			}
		}
		break;
	case SYSRES_MENU_UP:
	case SYSRES_MENU_MENU:
	case SYSRES_MENU_SELECTALL:
		{
			if (m_bWM6)
			{
				hMenu	= LoadMenu(hInstance, MAKEINTRESOURCE(19488));
			}
			else
			{
				hInstance	= LoadLibrary(_T("browsres.dll"));
				hMenu		= LoadMenu(hInstance, MAKEINTRESOURCE(4184));
			}

			*lpBuffer	= 0;

			mii.cbSize		= sizeof(MENUITEMINFO);
			mii.fMask		= MIIM_TYPE;
			mii.fType		= MFT_STRING;
			mii.dwTypeData	= lpBuffer;
			mii.cch			= cchBufferMax;

			if (uID == SYSRES_MENU_SELECTALL)
			{
				GetMenuItemInfo(hMenu, 22005, FALSE, &mii);
			}
			else
			{
				GetMenuItemInfo(hMenu, ((uID == SYSRES_MENU_UP) ? 0 : 1), TRUE, &mii);
			}

			DestroyMenu(hMenu);

			if ((hInstance != m_hNoteProject) && (hInstance != m_hCEShell))
			{
				FreeLibrary(hInstance);
			}

			return wcslen(lpBuffer);
		}
	case SYSRES_MENU_DETAIL:
		{
			uIDInt	+= 1;
		}
	case SYSRES_MENU_SMALLICONS:
		{
			uIDInt	+= 8661;
		}
		break;
	default:
		{
			return 0;
		}
	}

	int	nRes	= ::LoadString(hInstance, uIDInt, lpBuffer, cchBufferMax);

	if ((hInstance != m_hNoteProject) && (hInstance != m_hCEShell))
	{
		FreeLibrary(hInstance);
	}

	return nRes;
}

BOOL CSysResX::DummyProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (IsWindowVisible(hwndDlg))
	{
		ShowWindow(hwndDlg, SW_HIDE);
	}

	return FALSE;
}
