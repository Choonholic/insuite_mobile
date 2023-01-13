// FontX.h

#pragma once

#include "InAPI.h"

#ifndef FONTSIZE_SMALLEST
#define FONTSIZE_SMALLEST	0
#define FONTSIZE_SMALLER	1
#define FONTSIZE_MEDIUM		2
#define FONTSIZE_LARGER		3
#define FONTSiZE_LARGEST	4
#endif

// Font Path
BOOL	INAPI_API	GetFontPath(LPTSTR lpszPath, int nSize);
BOOL	INAPI_API	SetFontPath(LPCTSTR lpszPath = NULL);

// User Font Manipulations
HFONT	INAPI_API	CreateFixedFont(LPCTSTR lpszFontFace = NULL, int nDoublePoint = 0, LONG lWeight = FW_NORMAL);
HFONT	INAPI_API	CreateUserFont(LPCTSTR lpszFontFace = NULL, LONG lWeight = FW_NORMAL);
HFONT	INAPI_API	CreateUserFont(LPLOGFONT lpLogFont);
void	INAPI_API	ReleaseFont(HFONT hFont);

void	INAPI_API	GetFixedLogFont(LPLOGFONT lpLogFont, LPCTSTR lpszFontFace = NULL, int nDoublePoint = 0, LONG lWeight = FW_NORMAL);
void	INAPI_API	GetUserLogFont(LPLOGFONT lpLogFont, LPCTSTR lpszFontFace = NULL, LONG lWeight = FW_NORMAL);
void	INAPI_API	GetUserLogFont(LPLOGFONT lpUserLogFont, LPLOGFONT lpLogFont);

// System Font Manipulations
BOOL	INAPI_API	GetDefaultFont(LPLOGFONT lpLogFont);
BOOL	INAPI_API	SetDefaultFont(LPLOGFONT lpLogFont, BOOL bHtInPts = TRUE);
BOOL	INAPI_API	GetTaskBarFont(LPLOGFONT lpLogFont);
BOOL	INAPI_API	SetTaskBarFont(LPLOGFONT lpLogFont, BOOL bHtInPts = TRUE);
BOOL	INAPI_API	GetSystemFont(LPLOGFONT lpLogFont);
BOOL	INAPI_API	SetSystemFont(LPLOGFONT lpLogFont, BOOL bHtInPts = TRUE);
BOOL	INAPI_API	GetPopupMenuFont(LPLOGFONT lpLogFont);
BOOL	INAPI_API	SetPopupMenuFont(LPLOGFONT lpLogFont, BOOL bHtInPts = TRUE);
BOOL	INAPI_API	GetMenuBarFont(LPLOGFONT lpLogFont);
BOOL	INAPI_API	SetMenuBarFont(LPLOGFONT lpLogFont, BOOL bHtInPts = TRUE);
BOOL	INAPI_API	GetOutOfMemoryFont(LPLOGFONT lpLogFont);
BOOL	INAPI_API	SetOutOfMemoryFont(LPLOGFONT lpLogFont, BOOL bHtInPts = TRUE);
BOOL	INAPI_API	GetButtonFont(LPLOGFONT lpLogFont);
BOOL	INAPI_API	SetButtonFont(LPLOGFONT lpLogFont, BOOL bHtInPts = TRUE);

// Log Font Manipulations
int		INAPI_API	GetLogFont(LPLOGFONT lpLogFont, HFONT hFont);

// Font Units Calculators
int		INAPI_API	HIPToPts(LONG lHIP);
LONG	INAPI_API	PtsToHIP(int nPts);
int		INAPI_API	HtToPts(LONG lHt);
LONG	INAPI_API	PtsToHt(int nPts);
LONG	INAPI_API	HIPToHt(LONG lHIP);
LONG	INAPI_API	HtToHIP(LONG lHt);

// Default Font Style Calculators
LONG	INAPI_API	GetDefaultFontHeight(void);
LONG	INAPI_API	GetDefaultBoldWeight(void);

// ClearType Manipulations
BOOL	INAPI_API	GetClearTypeRegistry(void);
BOOL	INAPI_API	GetClearTypeOffOnRotation(void);
BOOL	INAPI_API	GetClearType(void);
int		INAPI_API	GetGammaSettings(void);
BOOL	INAPI_API	SetClearTypeRegistry(BOOL bClearType = TRUE);
BOOL	INAPI_API	SetClearTypeOffOnRotation(BOOL bOffOnRotation = FALSE);
BOOL	INAPI_API	SetClearType(BOOL bClearType = TRUE);
BOOL	INAPI_API	SetGammaSettings(int nGamma = 1000);

// Anti-alias Manipulations
BOOL	INAPI_API	GetAntialiasRegistry(BOOL& bForceGRAY16, BOOL& bExpansiveAntialias);
BOOL	INAPI_API	GetAntialias(BOOL& bForceGRAY16, BOOL& bExpansiveAntialias);

BOOL	INAPI_API	SetAntialiasRegistry(BOOL bAntialias = TRUE, BOOL bForceGRAY16 = TRUE, BOOL bExpansiveAntialias = TRUE);
BOOL	INAPI_API	SetAntialias(BOOL bAntialias = TRUE, BOOL bForceGRAY16 = TRUE, BOOL bExpansiveAntialias = TRUE);
