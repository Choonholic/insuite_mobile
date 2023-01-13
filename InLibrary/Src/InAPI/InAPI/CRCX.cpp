// CRCX.cpp

#include "stdafx.h"
#include "CRCX.h"

#define CRCPOLY_LEFT	0x04C11DB7UL
#define CRCPOLY_RIGHT	0xEDB88320UL

DWORD	CRCTableLS[UCHAR_MAX + 1];
DWORD	CRCTableRS[UCHAR_MAX + 1];

void MakeCRCLSTableX(void)
{
	UINT	i, j;
	DWORD	r;

	for (i = 0; i <= UCHAR_MAX; i++)
	{
		r	= static_cast<DWORD>(static_cast<DWORD>(i) << (32 - CHAR_BIT));

		for (j = 0; j < CHAR_BIT; j++)
		{
			if (r & 0x80000000UL)
			{
				r	= ((r << 1) ^ CRCPOLY_LEFT);
			}
			else
			{
				r	<<= 1;
			}
		}

		CRCTableLS[i]	= (r & 0xFFFFFFFFUL);
	}
}

void MakeCRCRSTableX(void)
{
	UINT	i, j;
	DWORD	r;

	for (i = 0; i <= UCHAR_MAX; i++)
	{
		r	= i;

		for (j = 0; j < CHAR_BIT; j++)
		{
			if (r & 1)
			{
				r	= ((r >> 1) ^ CRCPOLY_RIGHT);
			}
			else
			{
				r	>>= 1;
			}
		}

		CRCTableRS[i]	= r;
	}
}

DWORD BuildCRCLSX(int nLength, LPBYTE lpBuffer)
{
	DWORD	r	= 0xFFFFFFFFUL;

	while (--nLength >= 0)
	{
		r	= ((r << CHAR_BIT) ^ CRCTableLS[static_cast<BYTE>(r >> (32 - CHAR_BIT)) ^ *lpBuffer++]);
	}

	return (~r & 0xFFFFFFFFUL);
}

DWORD BuildCRCRSX(int nLength, LPBYTE lpBuffer)
{
	DWORD	r	= 0xFFFFFFFFUL;

	while (--nLength >= 0)
	{
		r	= ((r >> CHAR_BIT) ^ CRCTableRS[static_cast<BYTE>(r) ^ *lpBuffer++]);
	}

	return (r & 0xFFFFFFFFUL);
}
