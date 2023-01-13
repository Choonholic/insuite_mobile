// FileX.h

#pragma once

#include "InAPI.h"

#ifndef BOM_NOUNICODE
#define BOM_NOUNICODE	0x00000000
#define BOM_NORMAL		0x0000FEFF
#define BOM_REVERSED	0x0000FFFE
#define BOM_FILEERROR	0xFFFFFFFF
#endif

#ifdef _UNICODE
#define ReadStringX		ReadStringXW
#else
#define ReadStringX		ReadStringXA
#endif

DWORD	INAPI_API	CheckUnicodeBOM(HANDLE hFile);

BOOL	INAPI_API	ReadStringXW(HANDLE hFile, LPTSTR lpszString, int nSize, BOOL bIncludeCRLF = FALSE);
BOOL	INAPI_API	ReadStringXA(HANDLE hFile, LPTSTR lpszString, int nSize, BOOL bIncludeCRLF = FALSE);
