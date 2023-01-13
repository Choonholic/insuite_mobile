// InAPI.cpp

#include "stdafx.h"
#include "InAPI.h"

HINSTANCE	g_hModule	= NULL;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		::OutputDebugString(_T("Initializing InAPI\n"));

		g_hModule	= reinterpret_cast<HINSTANCE>(hModule);
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		::OutputDebugString(_T("Terminating InAPI\n"));

		g_hModule	= NULL;
	}

	return TRUE;
}
