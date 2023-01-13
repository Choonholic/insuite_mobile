// GraphicsX.h

#pragma once

#include "InAPI.h"

// DrawTextEx
#ifndef ELLIPSIS
#define ELLIPSIS				((TCHAR)0x2026)
#endif

#ifndef DT_END_ELLIPSIS
#define DT_END_ELLIPSIS			0x00008000
#endif

#ifndef DT_MODIFYSTRING
#define DT_MODIFYSTRING			0x00010000
#endif

#ifndef DT_DISABLE_UNDERLINE
#define DT_DISABLE_UNDERLINE	0x10000000
#endif

int		INAPI_API	DrawTextEx(HDC hDC, LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT uFormat);

// FillRectEx
void	INAPI_API	FillSolidRectEx(HDC hDC, LPCRECT lpcRect, COLORREF cr);
void	INAPI_API	FillSolidRectEx(HDC hDC, int x, int y, int cx, int cy, COLORREF cr);
void	INAPI_API	FillHGRectEx(HDC hDC, LPCRECT lpRect, COLORREF crLeft, COLORREF crRight); 
void	INAPI_API	FillVGRectEx(HDC hDC, LPCRECT lpRect, COLORREF crTop, COLORREF crBottom);

// FillTransparentRectEx
// Level 0x00 - Fully Transparent, 0xFF - Fully Opaque
void	INAPI_API	FillTransparentSolidRectEx(HDC hDC, LPCRECT lpcRect, COLORREF cr, int nLevel);
void	INAPI_API	FillTransparentSolidRectEx(HDC hDC, int x, int y, int cx, int cy, COLORREF cr, int nLevel);
void	INAPI_API	FillTransparentHGRectEx(HDC hDC, LPCRECT lpRect, COLORREF crLeft, COLORREF crRight, int nLevel); 
void	INAPI_API	FillTransparentVGRectEx(HDC hDC, LPCRECT lpRect, COLORREF crTop, COLORREF crBottom, int nLevel);

// DrawTransparentEx
// Level 0x00 - Fully Transparent, 0xFF - Fully Opaque
void	INAPI_API	DrawTransparentEx(HDC hDC, LPCRECT lpRect, HDC hSource, int nLevel);
void	INAPI_API	DrawTransparentEx(HDC hDC, int x, int y, int cx, int cy, HDC hSource, int nLevel);

// LineEx
BOOL	INAPI_API	LineEx(HDC hDC, int x1, int y1, int x2, int y2, COLORREF cr);
