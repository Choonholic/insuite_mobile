// BluetoothX.h

#pragma once

#include "InAPI.h"

// Quality Information
//
// Middle Quality
// - Mono:         44.1kHz, 19 = 127kb/s
//                 48.0kHz, 18 = 132kb/s
// - Joint Stereo: 44.1kHz, 35 = 229kb/s
//                 48.0kHz, 33 = 237kb/s
//
// High Quality
// - Mono:         44.1kHz, 31 = 193kb/s
//                 48.0kHz, 29 = 198kb/s
// - Joint Stereo: 44.1kHz, 53 = 328kb/s
//                 48.0kHz, 51 = 345kb/s

#ifndef A2DP_VALUES
#define A2DP_VALUES
#define A2DP_DEF_BITPOOL	58
#define A2DP_DEF_SAMPLERATE	48000
#endif

DWORD	INAPI_API	GetA2DPBitPoolX(void);
void	INAPI_API	SetA2DPBitPoolX(DWORD dwBitPool = A2DP_DEF_BITPOOL);

DWORD	INAPI_API	GetA2DPSampleRateX(void);
void	INAPI_API	SetA2DPSampleRateX(DWORD dwSampleRate = A2DP_DEF_SAMPLERATE);

#ifndef BT_ERROR
#define BT_ERROR				0x00
#define BT_POWERON				0x01
#define BT_DISCOVERABLE			0x02
#define BT_A2DP_CONNECTED		0x04
#define BT_PRESENT				0x08
#define BT_HANDSFREE_CONTROL	0x10
#define BT_HANDSFREE_AUDIO		0x20
#endif

DWORD	INAPI_API	GetBluetoothStatus(void);
