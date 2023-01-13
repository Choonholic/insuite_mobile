// DisplayX.cpp

#include "stdafx.h"
#include "DisplayX.h"

typedef enum _VIDEO_POWER_STATE
{
	VideoPowerOn		= 1,
	VideoPowerStandBy,
	VideoPowerSuspend,
	VideoPowerOff
} VIDEO_POWER_STATE, *PVIDEO_POWER_STATE;

typedef struct _VIDEO_POWER_MANAGEMENT
{
	ULONG	Length;
	ULONG	DPMSVersion;
	ULONG	PowerState;
} VIDEO_POWER_MANAGEMENT, *PVIDEO_POWER_MANAGEMENT;

struct ContrastCmdInputParm
{
	int	command;
	int	parm;
};

BOOL GetVideoPowerState(VIDEO_POWER_STATE& vState)
{
	int		iESC	= GETPOWERMANAGEMENT;
	HDC		hDC		= ::GetDC(NULL);
	BOOL	bResult	= FALSE;

	vState	= VideoPowerOn;

	if (ExtEscape(hDC, QUERYESCSUPPORT, sizeof(int), reinterpret_cast<LPCSTR>(&iESC), 0, NULL) != 0)
	{
		VIDEO_POWER_MANAGEMENT	vpm;

		vpm.Length		= sizeof(VIDEO_POWER_MANAGEMENT);
		vpm.DPMSVersion	= 0x0001;

		ExtEscape(hDC, GETPOWERMANAGEMENT, 0, NULL, vpm.Length, reinterpret_cast<LPSTR>(&vpm));

		vState	= static_cast<VIDEO_POWER_STATE>(vpm.PowerState);
		bResult	= TRUE;
	}

	::ReleaseDC(NULL, hDC);
	return bResult;
}

BOOL SetVideoPowerState(VIDEO_POWER_STATE& vState)
{
	int		iESC	= SETPOWERMANAGEMENT;
	HDC		hDC		= ::GetDC(NULL);
	BOOL	bResult	= FALSE;

	if (ExtEscape(hDC, QUERYESCSUPPORT, sizeof(int), reinterpret_cast<LPCSTR>(&iESC), 0, NULL) != 0)
	{
		VIDEO_POWER_MANAGEMENT	vpm;

		vpm.Length		= sizeof(VIDEO_POWER_MANAGEMENT);
		vpm.DPMSVersion	= 0x0001;
		vpm.PowerState	= vState;

		ExtEscape(hDC, SETPOWERMANAGEMENT, vpm.Length, reinterpret_cast<LPCSTR>(&vpm), 0, NULL);

		bResult	= TRUE;
	}

	::ReleaseDC(NULL, hDC);
	return bResult;
}

BOOL HandleContrastCommand(int nCmd, int* pParm)
{
	struct ContrastCmdInputParm	ContrastCmdInputParm;

	HDC		hDC		= ::GetDC(NULL);
	int		iESC	= CONTRASTCOMMAND;
	BOOL	bResult	= FALSE;

	if (hDC != NULL)
	{
		if (ExtEscape(hDC, QUERYESCSUPPORT, sizeof(int), reinterpret_cast<LPCSTR>(&iESC), 0, NULL))
		{
			ContrastCmdInputParm.command	= nCmd;
			ContrastCmdInputParm.parm		= *pParm;

			int	iReturn	= ExtEscape(hDC, CONTRASTCOMMAND, sizeof(ContrastCmdInputParm), reinterpret_cast<LPCSTR>(&ContrastCmdInputParm), 0, NULL);

			*pParm	= ContrastCmdInputParm.parm;
			bResult	= static_cast<BOOL>(iReturn > 0);
		}

		ReleaseDC(NULL, hDC);
	}

	return bResult;
}

DWORD GetDPI(void)
{
	int	nLogPixelsX	= DRA::LogPixelsX();
	int	nLogPixelsY	= DRA::LogPixelsY();

	if (nLogPixelsX != nLogPixelsY)
	{
		// Logical Pixels of X axis and Y axis are mismatched
		return DPI_UNKNOWN;
	}

	switch (nLogPixelsX)
	{
	case 96:
	case 192:
	case 128:
	case 131:
		{
			return nLogPixelsX;
		}
	}

	return DPI_UNREVEALED;
}

DWORD GetNarrowResolution(void)
{
	int	nWidth	= GetSystemMetrics(SM_CXSCREEN);
	int	nHeight	= GetSystemMetrics(SM_CYSCREEN);

	if (nWidth < nHeight)
	{
		return static_cast<DWORD>(nWidth);
	}

	return static_cast<DWORD>(nHeight);
}

DWORD GetWideResolution(void)
{
	int	nWidth	= GetSystemMetrics(SM_CXSCREEN);
	int	nHeight	= GetSystemMetrics(SM_CYSCREEN);

	if (nWidth > nHeight)
	{
		return static_cast<DWORD>(nWidth);
	}

	return static_cast<DWORD>(nHeight);
}

DWORD GetScreenResolutionMode(void)
{
	int	nWidth	= GetSystemMetrics(SM_CXSCREEN);
	int	nHeight	= GetSystemMetrics(SM_CYSCREEN);

	if (nWidth < nHeight)
	{
		return SRMODE_PORTRAIT;
	}

	if (nWidth > nHeight)
	{
		return SRMODE_LANDSCAPE;
	}

	return SRMODE_SQUARE;
}

void ToggleDisplayMode(void)
{
	SetDisplayMode(!GetDisplayMode());
}

BOOL GetDisplayMode(void)
{
	VIDEO_POWER_STATE	vps	= VideoPowerOn;

	GetVideoPowerState(vps);
	return static_cast<BOOL>(vps == VideoPowerOn);
}

void SetDisplayMode(BOOL bState)
{
	VIDEO_POWER_STATE	vps;

	vps	= ((bState) ? VideoPowerOn : VideoPowerOff);

	SetVideoPowerState(vps);
}

LONG GetRotationMode(LPDWORD lpdwRotation)
{
	LONG	lResult	= 0;
	DEVMODE	devmode	= { 0 };

	*lpdwRotation		= DMDO_0;
	devmode.dmSize		= sizeof(DEVMODE);
	devmode.dmFields	= DM_DISPLAYORIENTATION;
	lResult				= ChangeDisplaySettingsEx(NULL, &devmode, 0, CDS_TEST, NULL);

	if (lResult == DISP_CHANGE_SUCCESSFUL)
	{
		*lpdwRotation	= devmode.dmDisplayOrientation;
	}

	return lResult;
}

LONG SetRotationMode(DWORD dwRotation)
{
	LONG	lResult	= 0;
	DEVMODE	devmode	= { 0 };
	HKEY	hKey;
	DWORD	dwDisposition;
	DWORD	dwLandscape	= 0;

	devmode.dmSize					= sizeof(DEVMODE);
	devmode.dmDisplayOrientation	= dwRotation;
	devmode.dmFields				= DM_DISPLAYORIENTATION;
	lResult							= ChangeDisplaySettingsEx(NULL, &devmode, NULL, 0, NULL);

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, _T("System\\GDI\\Rotation"), 0, NULL, 0, 0, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
	{
		switch (dwRotation)
		{
		case DMDO_0:
			{
				dwRotation	= 0;
				dwLandscape	= 0;
			}
			break;
		case DMDO_90:
			{
				dwRotation	= 90;
				dwLandscape	= 1;
			}
			break;
		case DMDO_180:
			{
				dwRotation	= 180;
				dwLandscape	= 0;
			}
			break;
		case DMDO_270:
			{
				dwRotation	= 270;
				dwLandscape	= 1;
			}
			break;
		}

		RegSetValueEx(hKey, _T("Angle"), 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwRotation), sizeof(DWORD));
		RegSetValueEx(hKey, _T("LandscapeMode"), 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwLandscape), sizeof(DWORD));
		RegCloseKey(hKey);
	}

	return lResult;
}

LONG ResetRotationMode(void)
{
	return (SetRotationMode(DMDO_0));
}
