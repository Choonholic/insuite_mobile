// BitmapX.h

#pragma once

#include "InAPI.h"

LPBYTE	INAPI_API	GetScreenDIBEx(int x, int y, int nWidth, int nHeight, LPDWORD lpdwSize);
LONG	INAPI_API	GetBitmapBitsEx(HBITMAP hbm, LONG cbBuffer, LPVOID lpvBits);
