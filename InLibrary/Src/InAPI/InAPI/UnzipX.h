// UnzipX.h
// Based on ZIP Utils, by Lucian Wischik, www.wischik.com/lu

#pragma once

#include "InAPI.h"
#include "ZipDefsX.h"

#ifndef UNZIP_INCLUDED
#define UNZIP_INCLUDED
#endif

typedef struct
{
	int			index;
	TCHAR		name[MAX_PATH];
	DWORD		attr;
	FILETIME	atime;
	FILETIME	ctime;
	FILETIME	mtime;
	long		comp_size;
	long		unc_size;
} ZIPENTRY;

void	INAPI_API	UnzipCodePage(UINT uiCodePage = CP_ACP);

HZIP	INAPI_API	OpenZip(const TCHAR* fn, const char* password);
HZIP	INAPI_API	OpenZip(void* z, unsigned int len, const char* password);
HZIP	INAPI_API	OpenZipHandle(HANDLE h, const char* password);

ZRESULT	INAPI_API	GetZipItem(HZIP hz, int index, ZIPENTRY* ze);

ZRESULT	INAPI_API	FindZipItem(HZIP hz, const TCHAR* name, bool ic, int* index, ZIPENTRY* ze);

ZRESULT	INAPI_API	UnzipItem(HZIP hz, int index, const TCHAR* fn);
ZRESULT	INAPI_API	UnzipItem(HZIP hz, int index, void* z, unsigned int len);
ZRESULT	INAPI_API	UnzipItemHandle(HZIP hz, int index, HANDLE h);

ZRESULT	INAPI_API	SetUnzipBaseDir(HZIP hz, const TCHAR* dir);

ZRESULT	INAPI_API	CloseZip(HZIP hz);

unsigned int	INAPI_API	FormatZipMessage(ZRESULT code, TCHAR* buf,unsigned int len);

ZRESULT			INAPI_API	CloseZipU(HZIP hz);
unsigned int	INAPI_API	FormatZipMessageU(ZRESULT code, TCHAR* buf, unsigned int len);
bool 			INAPI_API	IsZipHandleU(HZIP hz);

#ifdef ZIP_INCLUDED
#undef CloseZip
#define CloseZip(hz)		(IsZipHandleU(hz) ? CloseZipU(hz) : CloseZipZ(hz))
#else
#define CloseZip			CloseZipU
#define FormatZipMessage	FormatZipMessageU
#endif
