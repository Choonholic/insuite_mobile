// ThemeX.h

#pragma once

#include "InAPI.h"

// Wallpaper Manipulation
BOOL	INAPI_API	SetWallpaper(LPCTSTR szFilename);
BOOL	INAPI_API	GetWallpaper(LPTSTR szFilename, int nLen);
BOOL	INAPI_API	ResetWallpaper(void);
