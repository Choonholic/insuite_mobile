// InXMFC.cpp

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE	InXMFCDLL	= { NULL, NULL };
HINSTANCE					g_hModule	= NULL;

extern "C" BOOL APIENTRY DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("Initializing InXMFC\n");

		if (!AfxInitExtensionModule(InXMFCDLL, reinterpret_cast<HMODULE>(hInstance)))
		{
			return FALSE;
		}

		g_hModule	= InXMFCDLL.hModule;

		new CDynLinkLibrary(InXMFCDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("Terminating InXMFC\n");

		AfxTermExtensionModule(InXMFCDLL);

		g_hModule	= NULL;
	}

	return TRUE;
}
