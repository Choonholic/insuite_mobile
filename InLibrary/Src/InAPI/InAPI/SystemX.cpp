// SystemX.cpp

#include "stdafx.h"
#include <winioctl.h>
#include "SystemX.h"

extern "C"	__declspec(dllimport)	void	SetCleanRebootFlag(void);
extern "C"	__declspec(dllimport)	BOOL	KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);

#ifndef IOCTL_HAL_REBOOT
#define IOCTL_HAL_REBOOT		CTL_CODE(FILE_DEVICE_HAL, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif	// IOCTL_HAL_REBOOT

#ifndef IOCTL_HAL_GET_DEVICEID
#define IOCTL_HAL_GET_DEVICEID	CTL_CODE(FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif	// IOCTL_HAL_GET_DEVICEID

BOOL Reset(UINT uExFlags)
{
	return (ExitWindowsEx(EWX_REBOOT | uExFlags, 0));
}

BOOL FullReset(void)
{
	SetCleanRebootFlag();
	return (KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL));
}

BOOL Suspend(void)
{
	::keybd_event(VK_OFF, 0, 0, 0);
	::keybd_event(VK_OFF, 0, KEYEVENTF_KEYUP, 0);
	return TRUE;
}

BOOL Shutdown(void)
{
#if (_WIN32_WCE < 0x0502)
	return FALSE;
#else
	return (ExitWindowsEx(EWX_POWEROFF, 0));
#endif
}

BOOL DeviceLock(void)
{
	::keybd_event(VK_F22, 0, 0, 0);
	::keybd_event(VK_F22, 0, KEYEVENTF_KEYUP, 0);
	return TRUE;
}

#define DEVID_LENGTH	26
#define BUFID_LENGTH	(DEVID_LENGTH * 2)

BOOL GetDeviceUniqueIDW(LPWSTR lpBuffer, int nSize)
{
	const int	nBuffSize	= 4096;
	BYTE		arrOutBuff[nBuffSize];
	DWORD		dwOutBytes;

	if (nSize < (BUFID_LENGTH + 1))
	{
		return FALSE;
	}

	memset(lpBuffer, 0, (sizeof(WCHAR) * nSize));

	if (KernelIoControl(IOCTL_HAL_GET_DEVICEID, 0, 0, arrOutBuff, nBuffSize, &dwOutBytes))
	{
		WCHAR	cData[3];
		int		nStart	= 0;

		if (dwOutBytes < DEVID_LENGTH)
		{
			return FALSE;
		}

		if (dwOutBytes > DEVID_LENGTH)
		{
			nStart	= (dwOutBytes - DEVID_LENGTH) / 2;
		}

		for (int i = nStart; i < (nStart + DEVID_LENGTH); i++)
		{
			wsprintf(cData, L"%02X", arrOutBuff[i]);

			lpBuffer[(i - nStart) * 2 + 0]	= cData[0];
			lpBuffer[(i - nStart) * 2 + 1]	= cData[1];
		}

		return TRUE;
	}

	return FALSE;
}

BOOL GetDeviceUniqueIDA(LPSTR lpBuffer, int nSize)
{
	const int	nBuffSize	= 4096;
	BYTE		arrOutBuff[nBuffSize];
	DWORD		dwOutBytes;

	if (nSize < (BUFID_LENGTH + 1))
	{
		return FALSE;
	}

	memset(lpBuffer, 0, (sizeof(BYTE) * nSize));

	if (KernelIoControl(IOCTL_HAL_GET_DEVICEID, 0, 0, arrOutBuff, nBuffSize, &dwOutBytes))
	{
		char	cData[3];
		int		nStart	= 0;

		if (dwOutBytes < DEVID_LENGTH)
		{
			return FALSE;
		}

		if (dwOutBytes > DEVID_LENGTH)
		{
			nStart	= (dwOutBytes - DEVID_LENGTH) / 2;
		}

		for (int i = nStart; i < (nStart + DEVID_LENGTH); i++)
		{
			sprintf(cData, "%02X", arrOutBuff[i]);

			lpBuffer[(i - nStart) * 2 + 0]	= cData[0];
			lpBuffer[(i - nStart) * 2 + 1]	= cData[1];
		}

		return TRUE;
	}

	return FALSE;
}
