// InSetup.cpp

#include "stdafx.h"
#include "resource.h"
#include "ce_setup.h"
#include "Utilities.h"
#include "InSuite_Settings.h"
#include "InSuite_Version.h"
#include "InClose.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#if defined(INCLOSE)
#include "InClose_Settings.h"
#elif defined(INFONT)
#include "InFont_Settings.h"
#elif defined(INREGISTRY)
#include "InRegistry_Settings.h"
#elif defined(INEXPLORER)
#include "InExplorer_Settings.h"
#elif defined(INGAUGE)
#include "InGauge_Settings.h"
#elif defined(INCLEAN)
#include "InClean_Settings.h"
#endif

typedef struct _tBundles
{
	TCHAR	szName[MAX_PATH + 1];
	int		nShift;
} BUNDLE_LIST;

HINSTANCE	g_hInstance	= NULL;
BOOL		g_bInClose	= FALSE;

#define BUNDLE_MAX	0

#if (BUNDLE_MAX > 0)
BUNDLE_LIST	g_tBundles[BUNDLE_MAX]	=
{
	{ _T("SLGLRQ"),		3 }	// PIDION
};
#endif

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	g_hInstance	= (HINSTANCE)hModule;

	return TRUE;
}

void WriteInformation(LPCTSTR lpszDirectory)
{
	RegistrySetString(HKEY_CURRENT_USER, RK_IS_ROOT, RV_IS_FOLDER, lpszDirectory);
	RegistrySetString(HKEY_CURRENT_USER, RK_IS_ROOT, RV_IS_VERSION, VS_IS_VERSION);
}

codeINSTALL_INIT Install_Init(HWND hwndParent, BOOL fFirstCall, BOOL fPreviouslyInstalled, LPCTSTR pszInstallDir)
{
	TCHAR	szTitle[64];
	TCHAR	szMessage[256];

	if (fFirstCall)
	{
		if (fPreviouslyInstalled)
		{
#if defined(INCLOSE)
			LoadString(g_hInstance, IDS_CL_TITLE, szTitle, 64);
			LoadString(g_hInstance, IDS_CL_PREVINSTALLED, szMessage, 256);
#elif defined(INFONT)
			LoadString(g_hInstance, IDS_FN_TITLE, szTitle, 64);
			LoadString(g_hInstance, IDS_FN_PREVINSTALLED, szMessage, 256);
#elif defined(INREGISTRY)
			LoadString(g_hInstance, IDS_RG_TITLE, szTitle, 64);
			LoadString(g_hInstance, IDS_RG_PREVINSTALLED, szMessage, 256);
#elif defined(INEXPLORER)
			LoadString(g_hInstance, IDS_EX_TITLE, szTitle, 64);
			LoadString(g_hInstance, IDS_EX_PREVINSTALLED, szMessage, 256);
#elif defined(INGAUGE)
			LoadString(g_hInstance, IDS_GG_TITLE, szTitle, 64);
			LoadString(g_hInstance, IDS_GG_PREVINSTALLED, szMessage, 256);
#elif defined(INCLEAN)
			LoadString(g_hInstance, IDS_CN_TITLE, szTitle, 64);
			LoadString(g_hInstance, IDS_CN_PREVINSTALLED, szMessage, 256);
#else
			LoadString(g_hInstance, IDS_CC_TITLE, szTitle, 64);
			LoadString(g_hInstance, IDS_CC_PREVINSTALLED, szMessage, 256);
#endif

			if (MessageBox(hwndParent, szMessage, szTitle, MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2 | MB_TOPMOST) == IDNO)
			{
				return codeINSTALL_INIT_CANCEL;
			}
		}

#if defined(INCLOSE)
#elif defined(INFONT)
#elif defined(INREGISTRY)
#elif defined(INEXPLORER)
#elif defined(INGAUGE)
#elif defined(INCLEAN)
#else
		g_bInClose	= UnloadInClose();
#endif

		TCHAR	szPrograms[MAX_PATH + 1];
		TCHAR	szPath[MAX_PATH + 1];

		GetSpecialDirectoryEx(CSIDL_PROGRAMS, szPrograms, MAX_PATH + 1);
		ExtendSubPath(szPath, MAX_PATH + 1, szPrograms, NM_IS_PROGRAMS);
		CreateDirectory(szPath, NULL);
	}

	return codeINSTALL_INIT_CONTINUE;
}

codeINSTALL_EXIT Install_Exit(HWND hwndParent, LPCTSTR pszInstallDir, WORD cFailedDirs, WORD cFailedFiles, WORD cFailedRegKeys, WORD cFailedRegVals, WORD cFailedShortcuts)
{
	WriteInformation(pszInstallDir);

#if defined(INCLOSE)
	TCHAR	szTitle[64];
	TCHAR	szMessage[256];

	LoadString(g_hInstance, IDS_CL_TITLE, szTitle, 64);
	LoadString(g_hInstance, IDS_CL_ASKSTARTUP, szMessage, 256);

	if (MessageBox(hwndParent, szMessage, szTitle, MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
	{
		RegisterInClose(pszInstallDir);
		LoadInClose();
	}
	else
	{
		LoadString(g_hInstance, IDS_CL_MANUALEXEC, szMessage, 256);
		MessageBox(hwndParent, szMessage, szTitle, MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	}
#elif defined(INFONT)
#elif defined(INREGISTRY)
	DWORD		dwVersion	= MakeVersionNumberX(PID_INREGISTRY_CE, PVN_INSUITE_CLASSIC);
	SYSTEMTIME	stToday;
	TCHAR		szWindows[MAX_PATH + 1];
	TCHAR		szPath[MAX_PATH + 1];
	BOOL		bBundle		= FALSE;

	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	CombineFileName(szPath, MAX_PATH + 1, szWindows, MD_RG_TRIAL);
	GetLocalTime(&stToday);

#if (BUNDLE_MAX > 0)
	for (int iIdx = 0; iIdx < BUNDLE_MAX; iIdx++)
	{
		if (CheckBundleX(g_tBundles[iIdx].szName, g_tBundles[iIdx].nShift))
		{
			bBundle	= TRUE;
		}
	}
#endif

	if (bBundle)
	{
		WriteActivateBinaryX(szPath, dwVersion, Random());
	}
	else
	{
		WriteActivateBinaryX(szPath, dwVersion, stToday, FALSE);
	}
#elif defined(INEXPLORER)
	DWORD		dwVersion	= MakeVersionNumberX(PID_INEXPLORER_CE, PVN_INSUITE_CLASSIC);
	SYSTEMTIME	stToday;
	TCHAR		szWindows[MAX_PATH + 1];
	TCHAR		szPath[MAX_PATH + 1];
	BOOL		bBundle		= FALSE;

	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	CombineFileName(szPath, MAX_PATH + 1, szWindows, MD_EX_TRIAL);
	GetLocalTime(&stToday);

#if (BUNDLE_MAX > 0)
	for (int iIdx = 0; iIdx < BUNDLE_MAX; iIdx++)
	{
		if (CheckBundleX(g_tBundles[iIdx].szName, g_tBundles[iIdx].nShift))
		{
			bBundle	= TRUE;
		}
	}
#endif

	if (bBundle)
	{
		WriteActivateBinaryX(szPath, dwVersion, Random());
	}
	else
	{
		WriteActivateBinaryX(szPath, dwVersion, stToday, FALSE);
	}
#elif defined(INGAUGE)
#elif defined(INCLEAN)
#else
	LoadInClose();
#endif

#if FALSE
	return codeINSTALL_EXIT_UNINSTALL;
#endif

	return codeINSTALL_EXIT_DONE;
}

codeUNINSTALL_INIT Uninstall_Init(HWND hwndParent, LPCTSTR pszInstallDir)
{
	TCHAR	szTitle[64];
	TCHAR	szMessage[256];

#if defined(INCLOSE)
	UnloadInClose();
	UnregisterInClose();
	LoadString(g_hInstance, IDS_CL_TITLE, szTitle, 64);
	LoadString(g_hInstance, IDS_CL_ASKREMOVE, szMessage, 256);
#elif defined(INFONT)
	LoadString(g_hInstance, IDS_FN_TITLE, szTitle, 64);
	LoadString(g_hInstance, IDS_FN_ASKREMOVE, szMessage, 256);
#elif defined(INREGISTRY)
	LoadString(g_hInstance, IDS_RG_TITLE, szTitle, 64);
	LoadString(g_hInstance, IDS_RG_ASKREMOVE, szMessage, 256);
#elif defined(INEXPLORER)
	LoadString(g_hInstance, IDS_EX_TITLE, szTitle, 64);
	LoadString(g_hInstance, IDS_EX_ASKREMOVE, szMessage, 256);
#elif defined(INGAUGE)
	LoadString(g_hInstance, IDS_GG_TITLE, szTitle, 64);
	LoadString(g_hInstance, IDS_GG_ASKREMOVE, szMessage, 256);
#elif defined(INCLEAN)
	LoadString(g_hInstance, IDS_CN_TITLE, szTitle, 64);
	LoadString(g_hInstance, IDS_CN_ASKREMOVE, szMessage, 256);
#else
	TCHAR	szPrograms[MAX_PATH + 1];
	TCHAR	szPath[MAX_PATH + 1];

	UnloadInClose();
	GetSpecialDirectoryEx(CSIDL_PROGRAMS, szPrograms, MAX_PATH + 1);
	ExtendSubPath(szPath, MAX_PATH + 1, szPrograms, NM_IS_PROGRAMS);
	RemoveDirectory(szPath);

	LoadString(g_hInstance, IDS_CC_TITLE, szTitle, 64);
	LoadString(g_hInstance, IDS_CC_ASKREMOVE, szMessage, 256);
#endif

	if (MessageBox(hwndParent, szMessage, szTitle, MB_YESNO | MB_ICONINFORMATION | MB_DEFBUTTON2 | MB_TOPMOST) == IDYES)
	{
#if defined(INCLOSE)
		RegDeleteKey(HKEY_CURRENT_USER, RK_CL_ROOT);
#elif defined(INFONT)
		RegDeleteKey(HKEY_CURRENT_USER, RK_FN_ROOT);
#elif defined(INREGISTRY)
		RegDeleteKey(HKEY_CURRENT_USER, RK_RG_ROOT);
#elif defined(INEXPLORER)
		RegDeleteKey(HKEY_CURRENT_USER, RK_EX_ROOT);
#elif defined(INGAUGE)
		RegDeleteKey(HKEY_CURRENT_USER, RK_GG_ROOT);
#elif defined(INCLEAN)
		RegDeleteKey(HKEY_CURRENT_USER, RK_CN_ROOT);
#else
		RegDeleteKey(HKEY_CURRENT_USER, RK_IS_ROOT);
#endif
	}

#if FALSE
	return codeUNINSTALL_INIT_CANCEL;
#endif

	return codeUNINSTALL_INIT_CONTINUE;
}

codeUNINSTALL_EXIT Uninstall_Exit(HWND hwndParent)
{
	return codeUNINSTALL_EXIT_DONE;
}
