// InXDC.cpp

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new	DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE	InXDCDLL	= { NULL, NULL };
HINSTANCE					g_hModule	= NULL;

extern "C" BOOL APIENTRY DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("Initializing InXDC\n");

		if (!AfxInitExtensionModule(InXDCDLL, reinterpret_cast<HMODULE>(hInstance)))
		{
			return 0;
		}

		g_hModule	= InXDCDLL.hModule;

		new CDynLinkLibrary(InXDCDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("Terminating InXDC\n");

		AfxTermExtensionModule(InXDCDLL);

		g_hModule	= NULL;
	}

	return TRUE;
}
