// ConnectX.cpp

#include "stdafx.h"
#include "ConnectX.h"
#include "connmgr.h"
#include "snapi.h"
#include "RegistryX.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#pragma comment(lib, "cellcore.lib") 

#ifndef VK_END_ALL_DATA_CALLS
#define VK_END_ALL_DATA_CALLS		VK_F15
#endif

#define CONNECTION_WAITING_MS		1500
#define ENDALLDATACALLS_WAITING_MS	3500
#define MAX_RAS_CONNECTIONS			4

TCHAR	g_szRasConnections[MAX_RAS_CONNECTIONS][64];
int		g_nRasConnections	= 0;

HANDLE EstablishConnection(void)
{
	HRESULT					hr;
	CONNMGR_CONNECTIONINFO	ccinfo;
	HANDLE					hConnection	= NULL;
	DWORD					dwStatus	= CONNMGR_STATUS_CONNECTED;

	ZeroMemory(&ccinfo, sizeof(CONNMGR_CONNECTIONINFO));

	ccinfo.cbSize		= sizeof(CONNMGR_CONNECTIONINFO);
	ccinfo.dwParams		= CONNMGR_PARAM_GUIDDESTNET;
	ccinfo.dwPriority	= CONNMGR_PRIORITY_USERINTERACTIVE;
	ccinfo.dwFlags		= 0;
	ccinfo.bExclusive	= FALSE;
	ccinfo.bDisabled	= FALSE;
	ccinfo.guidDestNet	= IID_DestNetInternet;
	ccinfo.hWnd			= NULL;
	ccinfo.lParam		= (LPARAM)0;

	hr	= ConnMgrEstablishConnectionSync(&ccinfo, &hConnection, 15000, &dwStatus);

	if (hr == S_OK)
	{
		Sleep(CONNECTION_WAITING_MS);
	}
	else
	{
		hConnection	= INVALID_HANDLE_VALUE;
	}

	return hConnection;
}

BOOL ReleaseConnection(HANDLE hConnection)
{
	HRESULT	hr	= ConnMgrReleaseConnection(hConnection, 1);

	return static_cast<BOOL>(hr == S_OK);
}

void EndAllDataCalls(void)
{
	keybd_event((BYTE)VK_END_ALL_DATA_CALLS, (BYTE)0, KEYEVENTF_SILENT, 0);
	keybd_event((BYTE)VK_END_ALL_DATA_CALLS, (BYTE)0, KEYEVENTF_SILENT | KEYEVENTF_KEYUP, 0);
	Sleep(ENDALLDATACALLS_WAITING_MS);
}

BOOL IsCradled(void)
{
	DWORD	dwData	= 0;
	HRESULT	hr;

	hr	= RegistryGetDWORD(SN_CRADLEPRESENT_ROOT, SN_CRADLEPRESENT_PATH, SN_CRADLEPRESENT_VALUE, &dwData);

	if ((hr == S_OK) && (dwData == 1))
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IsSynchronizing(void)
{
	DWORD	dwData	= 0;
	HRESULT	hr;

	hr	= RegistryGetDWORD(SN_ACTIVESYNCSTATUS_ROOT, SN_ACTIVESYNCSTATUS_PATH, SN_ACTIVESYNCSTATUS_VALUE, &dwData);

	if ((hr == S_OK) && (dwData == 1))
	{
		return TRUE;
	}

	return FALSE;
}

#define RAS_CONNECTIONS_KEY	_T("Comm\\ConnMgr\\Providers\\{7C4B7A38-5FF7-4bc1-80F6-5DA7870BB1AA}\\Connections")

int GetRasConnections(void)
{
	CRegistryX	reg;
	DWORD		dwSize;
	int			i;

	ZeroMemory(g_szRasConnections, sizeof(TCHAR) * MAX_RAS_CONNECTIONS * 64);

	g_nRasConnections	= 0;

	if (reg.Open(HKEY_LOCAL_MACHINE, RAS_CONNECTIONS_KEY, FALSE))
	{
		g_nRasConnections	= reg.GetSubKeyCount();

		if (g_nRasConnections > MAX_RAS_CONNECTIONS)
		{
			g_nRasConnections = MAX_RAS_CONNECTIONS;
		}

		for (i = 0; i < g_nRasConnections; i++)
		{
			dwSize	= 64;

			reg.GetSubKey(i, g_szRasConnections[i], &dwSize);
		}

		reg.Close();
	}

	return g_nRasConnections;
}

BOOL EnableRasConnection(int nIndex, BOOL bEnable)
{
	TCHAR	szSubKey[MAX_PATH + 1];

	if (g_nRasConnections == 0)
	{
		return FALSE;
	}

	if ((nIndex < 0) && (nIndex >= g_nRasConnections))
	{
		return FALSE;
	}

	wsprintf(szSubKey, _T("%s\\%s"), RAS_CONNECTIONS_KEY, g_szRasConnections[nIndex]);
	RegistrySetDWORD(HKEY_LOCAL_MACHINE, szSubKey, _T("Enabled"), (bEnable ? 1 : 0));
	return TRUE;
}

LPCTSTR GetRasConnectionString(int nIndex)
{
	if (g_nRasConnections == 0)
	{
		return NULL;
	}

	if ((nIndex < 0) && (nIndex >= g_nRasConnections))
	{
		return NULL;
	}

	return g_szRasConnections[nIndex];
}

BOOL IsActiveDataCall(void)
{
	DWORD	dwData	= 0;
	HRESULT	hr;

	hr	= RegistryGetDWORD(SN_PHONEACTIVEDATACALL_ROOT, SN_PHONEACTIVEDATACALL_PATH, SN_PHONEACTIVEDATACALL_VALUE, &dwData);

	if ((hr == S_OK) && (dwData & SN_PHONEACTIVEDATACALL_BITMASK))
	{
		return TRUE;
	}

	return FALSE;
}
