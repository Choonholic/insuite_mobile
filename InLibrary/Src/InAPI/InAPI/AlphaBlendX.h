// AlphaBlendX.h

#pragma once

#include "InAPI.h"

HBITMAP	INAPI_API	LoadAlphaBlendBuffer(LPCTSTR szHeaderFileName, LPCTSTR szBufferFileName);
HBITMAP	INAPI_API	LoadAlphaBlendBuffer(HMODULE hModule, UINT uHeaderID, UINT uBufferID, LPCTSTR lpszType);
