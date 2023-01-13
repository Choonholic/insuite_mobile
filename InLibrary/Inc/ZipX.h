// ZipX.h
// Based on ZIP Utils, by Lucian Wischik, www.wischik.com/lu

#pragma once

#include "InAPI.h"
#include "ZipDefsX.h"

#ifndef ZIP_INCLUDED
#define ZIP_INCLUDED
#endif

void	INAPI_API	ZipCodePage(UINT uiCodePage = CP_ACP);
void	INAPI_API	ZipLevel(int nLevel = 8);

HZIP	INAPI_API	CreateZip(const TCHAR* fn, const char* password);
HZIP	INAPI_API	CreateZip(void* buf, unsigned int len, const char* password);
HZIP	INAPI_API	CreateZipHandle(HANDLE h, const char* password);

ZRESULT	INAPI_API	ZipAdd(HZIP hz, const TCHAR* dstzn, const TCHAR* fn);
ZRESULT	INAPI_API	ZipAdd(HZIP hz, const TCHAR* dstzn, void* src, unsigned int len);
ZRESULT	INAPI_API	ZipAddHandle(HZIP hz, const TCHAR* dstzn, HANDLE h);
ZRESULT	INAPI_API	ZipAddHandle(HZIP hz, const TCHAR* dstzn, HANDLE h, unsigned int len);
ZRESULT	INAPI_API	ZipAddFolder(HZIP hz, const TCHAR* dstzn);

ZRESULT	INAPI_API	ZipGetMemory(HZIP hz, void** buf, unsigned long* len);

ZRESULT	INAPI_API	CloseZip(HZIP hz);

unsigned int	INAPI_API	FormatZipMessage(ZRESULT code, TCHAR* buf, unsigned int len);

ZRESULT			INAPI_API	CloseZipZ(HZIP hz);
unsigned int	INAPI_API	FormatZipMessageZ(ZRESULT code, TCHAR* buf, unsigned int len);
bool 			INAPI_API	IsZipHandleZ(HZIP hz);

#ifdef UNZIP_INCLUDED
#undef CloseZip
#define CloseZip(hz)		(IsZipHandleZ(hz) ? CloseZipZ(hz) : CloseZipU(hz))
#else
#define CloseZip			CloseZipZ
#define FormatZipMessage	FormatZipMessageZ
#endif
