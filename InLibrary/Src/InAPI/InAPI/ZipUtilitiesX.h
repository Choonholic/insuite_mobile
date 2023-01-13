// ZipUtilitiesX.h

#pragma once

#include "InAPI.h"

ZRESULT	INAPI_API	AddFileToZip(const TCHAR* zipfn, const TCHAR* zename, const TCHAR* zefn);
ZRESULT	INAPI_API	RemoveFileFromZip(const TCHAR* zipfn, const TCHAR* zename);
