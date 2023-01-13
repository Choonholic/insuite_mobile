// DeviceInfoX.cpp

#include "StdAfx.h"
#include "DeviceInfoX.h"
#include "RegistryX.h"

BOOL GetMachineNameX(LPTSTR lpszName, int nSize)
{
	return (SystemParametersInfo(SPI_GETOEMINFO, sizeof(TCHAR) * nSize, lpszName, 0));
}

BOOL GetPlatformTypeX(LPTSTR lpszType, int nSize)
{
	return (SystemParametersInfo(SPI_GETPLATFORMTYPE, sizeof(TCHAR) * nSize, lpszType, 0));
}

BOOL GetVersionX(LPDWORD lpdwMajor, LPDWORD lpdwMinor, LPDWORD lpdwBuild, LPTSTR lpszCSD, int nCSDSize)
{
	OSVERSIONINFO	ovi;

	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&ovi))
	{
		return FALSE;
	}

	if (lpdwMajor != NULL)
	{
		*lpdwMajor	= ovi.dwMajorVersion;
	}

	if (lpdwMinor != NULL)
	{
		*lpdwMinor	= ovi.dwMinorVersion;
	}

	if (lpdwBuild != NULL)
	{
		*lpdwBuild	= ovi.dwBuildNumber;
	}

	if ((lpszCSD != NULL) && (nCSDSize > static_cast<int>(_tcslen(ovi.szCSDVersion))))
	{
		_tcscpy_s(lpszCSD, nCSDSize, ovi.szCSDVersion);
	}

	return TRUE;
}

#define DX_NAME_MAXLEN	15
#define DX_NAME_DEFAULT	_T("My Device")

BOOL GetDeviceIDX(LPTSTR lpszID, int nSize)
{
	CRegistryX	reg(HKEY_LOCAL_MACHINE, _T("Ident"));
	DWORD		dwSize	= nSize;
	BOOL		bRes	= reg.ReadString(_T("Name"), lpszID, &dwSize);

	reg.Close();
	return bRes;
}

#define DX_DESC_MAXLEN	50

BOOL GetDescriptionX(LPTSTR lpszDesc, int nSize)
{
	CRegistryX	reg(HKEY_LOCAL_MACHINE, _T("Ident"));
	DWORD		dwSize	= nSize;
	BOOL		bRes	= reg.ReadString(_T("Desc"), lpszDesc, &dwSize);

	reg.Close();
	return bRes;
}

BOOL SetDeviceIDX(LPCTSTR lpszID)
{
	CRegistryX	reg(HKEY_LOCAL_MACHINE, _T("Ident"));
	BOOL		bResult	= FALSE;

	if (_tcslen(lpszID) <= DX_NAME_MAXLEN)
	{
		bResult	= reg.WriteString(_T("Name"), lpszID);
	}

	reg.Close();
	return bResult;
}

BOOL SetDescriptionX(LPCTSTR lpszDesc)
{
	CRegistryX	reg(HKEY_LOCAL_MACHINE, _T("Ident"));
	BOOL		bResult	= FALSE;

	if (_tcslen(lpszDesc) <= DX_DESC_MAXLEN)
	{
		bResult	= reg.WriteString(_T("Desc"), lpszDesc);
	}

	reg.Close();
	return bResult;
}
