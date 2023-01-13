// CRCX.h

#pragma once

#include "InAPI.h"

#define CRC_RIGHTSHIFT	// Default: Right Shift Method

#ifdef CRC_LEFTSHIFT
#define MakeCRCTableX	MakeCRCLSTableX
#define BuildCRCX		BuildCRCLSX
#else
#define MakeCRCTableX	MakeCRCRSTableX
#define BuildCRCX		BuildCRCRSX
#endif

void	INAPI_API	MakeCRCLSTableX(void);
void	INAPI_API	MakeCRCRSTableX(void);

DWORD	INAPI_API	BuildCRCLSX(int nLength, LPBYTE lpBuffer);
DWORD	INAPI_API	BuildCRCRSX(int nLength, LPBYTE lpBuffer);
