// BluetoothX.cpp

#include "StdAfx.h"
#include "BluetoothX.h"
#include "snapi.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#define A2DP_ROOTKEY		HKEY_LOCAL_MACHINE
#define A2DP_SUBKEY			_T("Software\\Microsoft\\Bluetooth\\A2DP\\Settings")
#define A2DP_BITPOOL		_T("BitPool")
#define A2DP_SAMPLERATE		_T("SampleRate")
#define A2DP_MIN_BITPOOL	2
#define A2DP_MAX_BITPOOL	250
#define A2DP_MIN_SAMPLERATE	16000
#define A2DP_MAX_SAMPLERATE	48000

DWORD GetA2DPBitPoolX(void)
{
	DWORD	dwBitPool	= A2DP_DEF_BITPOOL;

	RegistryGetDWORD(A2DP_ROOTKEY, A2DP_SUBKEY, A2DP_BITPOOL, &dwBitPool);
	return dwBitPool;
}

void SetA2DPBitPoolX(DWORD dwBitPool)
{
	if ((dwBitPool < A2DP_MIN_BITPOOL) || (dwBitPool > A2DP_MAX_BITPOOL))
	{
		dwBitPool	= A2DP_DEF_BITPOOL;
	}

	RegistrySetDWORD(A2DP_ROOTKEY, A2DP_SUBKEY, A2DP_BITPOOL, dwBitPool);
}

DWORD GetA2DPSampleRateX(void)
{
	DWORD	dwSampleRate	= A2DP_DEF_SAMPLERATE;

	RegistryGetDWORD(A2DP_ROOTKEY, A2DP_SUBKEY, A2DP_SAMPLERATE, &dwSampleRate);
	return dwSampleRate;
}

void SetA2DPSampleRateX(DWORD dwSampleRate)
{
	if ((dwSampleRate < A2DP_MIN_SAMPLERATE) || (dwSampleRate > A2DP_MAX_SAMPLERATE))
	{
		dwSampleRate	= A2DP_DEF_SAMPLERATE;
	}

	RegistrySetDWORD(A2DP_ROOTKEY, A2DP_SUBKEY, A2DP_SAMPLERATE, dwSampleRate);
}

DWORD GetBluetoothStatus(void)
{
#if (_WIN32_WCE >= 0x0502)
	DWORD	dwData	= 0;
	HRESULT	hr;

	hr	= RegistryGetDWORD(SN_BLUETOOTHSTATEPOWERON_ROOT, SN_BLUETOOTHSTATEPOWERON_PATH, SN_BLUETOOTHSTATEPOWERON_VALUE, &dwData);

	if (hr == S_OK)
	{
		return dwData;
	}
#endif

	return 0;
}
