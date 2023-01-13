// NetworkX.cpp

#include "stdafx.h"
#include "NetworkX.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

BOOL IsNetworkEnabled(void)
{
	DWORD	dwEnabled	= 0;

	RegistryGetDWORD(HKEY_LOCAL_MACHINE, _T("Comm/Redir"), _T("RegisterFSRoot"), &dwEnabled);
	return static_cast<BOOL>(dwEnabled);
}

BOOL EnableNetwork(BOOL bEnable)
{
	DWORD	dwEnable	= ((bEnable == TRUE) ? 1 : 0);
	HRESULT	hr			= RegistrySetDWORD(HKEY_LOCAL_MACHINE, _T("Comm/Redir"), _T("RegisterFSRoot"), dwEnable);

	return static_cast<BOOL>(hr == S_OK);
}
