// GraphicsX.cpp

#include "stdafx.h"
#include "GraphicsX.h"

int DrawTextEx(HDC hDC, LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT uFormat)
{
	int		nLength			= ((nCount != -1) ? nCount : _tcslen(lpszString));
	LPTSTR	lpszSubString	= new TCHAR[nLength + 1];

	_tcsncpy_s(lpszSubString, nLength + 1, lpszString, nLength);

	lpszSubString[nLength]	= NULL;

	if (uFormat & DT_DISABLE_UNDERLINE)
	{
		int	i;
		int	nCount	= 0;

		for (i = 0; i < nLength; i++)
		{
			if (lpszSubString[i] == _T('&'))
			{
				nCount++;
			}
		}

		if (nCount > 0)
		{
			LPTSTR	lpszModified	= new TCHAR[nLength + nCount + 1];
			int		nPos			= 0;

			for (i = 0; i < nLength; i++)
			{
				lpszModified[nPos++]	= lpszSubString[i];

				if (lpszSubString[i] == _T('&'))
				{
					lpszModified[nPos++]	= _T('&');
				}
			}

			lpszModified[nPos++]	= NULL;

			delete [] lpszSubString;

			lpszSubString	= lpszModified;
		}

		uFormat	&= ~(DT_DISABLE_UNDERLINE);
	}

	if (uFormat & DT_END_ELLIPSIS)
	{
		SIZE	size;
		int		nWidth	= lpRect->right - lpRect->left;

		if (nCount == -1)
		{
			nCount	= _tcslen(lpszSubString);
		}

		GetTextExtentPoint32(hDC, lpszSubString, nCount, &size);

		if (size.cx > nWidth)
		{
			int	nEstimate	= ((nCount * nWidth) / size.cx) + 1;

			if (nEstimate < nCount)
			{
				nCount	= nEstimate;
			}

			lpszSubString[nCount - 1]	= ELLIPSIS;

			GetTextExtentPoint32(hDC, lpszSubString, nCount, &size);

			while ((size.cx > nWidth) && (nCount > 1))
			{
				lpszSubString[--nCount]		= NULL;
				lpszSubString[nCount - 1]	= ELLIPSIS;

				GetTextExtentPoint32(hDC, lpszSubString, nCount, &size);
			}
		}

		uFormat	&= ~(DT_END_ELLIPSIS);
	}

	if (uFormat & DT_MODIFYSTRING)
	{
		uFormat	&= ~(DT_MODIFYSTRING);
	}

	int	nResult	= DrawText(hDC, lpszSubString, nCount, lpRect, uFormat);

	delete [] lpszSubString;
	return nResult;
}

void FillSolidRectEx(HDC hDC, LPCRECT lpRect, COLORREF cr)
{
	COLORREF	crOld	= SetBkColor(hDC, cr);

	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	SetBkColor(hDC, crOld);
}

void FillSolidRectEx(HDC hDC, int x, int y, int cx, int cy, COLORREF cr)
{
	COLORREF	crOld	= SetBkColor(hDC, cr);
	RECT		rect	= { x, y, x + cx, y + cy };

	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	SetBkColor(hDC, crOld);
}

void FillHGRectEx(HDC hDC, LPCRECT lpRect, COLORREF crLeft, COLORREF crRight)
{
	int			r1		= GetRValue(crLeft);
	int			g1		= GetGValue(crLeft);
	int			b1		= GetBValue(crLeft);
	int			r2		= GetRValue(crRight);
	int			g2		= GetGValue(crRight);
	int			b2		= GetBValue(crRight);
	int			width	= (lpRect->right - lpRect->left);
	int			height	= (lpRect->bottom - lpRect->top);
	COLORREF	crOldBkColor;

	crOldBkColor	= GetBkColor(hDC);

	for (int i = 0; i < width; i++)
	{
		int	r	= r1 + (i * (r2 - r1) / width);
		int	g	= g1 + (i * (g2 - g1) / width);
		int	b	= b1 + (i * (b2 - b1) / width);

		FillSolidRectEx(hDC, lpRect->left + i, lpRect->top, 1, height, RGB(r, g, b));
	}

	SetBkColor(hDC, crOldBkColor);
}

void FillVGRectEx(HDC hDC, LPCRECT lpRect, COLORREF crTop, COLORREF crBottom)
{
	int			r1		= GetRValue(crTop);
	int			g1		= GetGValue(crTop);
	int			b1		= GetBValue(crTop);
	int			r2		= GetRValue(crBottom);
	int			g2		= GetGValue(crBottom);
	int			b2		= GetBValue(crBottom);
	int			width	= (lpRect->right - lpRect->left);
	int			height	= (lpRect->bottom - lpRect->top);
	COLORREF	crOldBkColor;

	crOldBkColor	= GetBkColor(hDC);

	for (int i = 0; i < height; i++)
	{
		int	r	= r1 + (i * (r2 - r1) / height);
		int	g	= g1 + (i * (g2 - g1) / height);
		int	b	= b1 + (i * (b2 - b1) / height);

		FillSolidRectEx(hDC, lpRect->left, lpRect->top + i, width, 1, RGB(r, g, b));
	}

	SetBkColor(hDC, crOldBkColor);
}

void FillTransparentSolidRectEx(HDC hDC, LPCRECT lpcRect, COLORREF cr, int nLevel)
{
	int	r1	= GetRValue(cr);
	int	g1	= GetGValue(cr);
	int	b1	= GetBValue(cr);

	for (int y = lpcRect->top; y < lpcRect->bottom; y++)
	{
		for (int x = lpcRect->left; x < lpcRect->right; x++)
		{
			COLORREF	crDest	= GetPixel(hDC, x, y);
			int			r		= (((nLevel * r1) + ((0xFF - nLevel) * GetRValue(crDest))) / 0xFF);
			int			g		= (((nLevel * g1) + ((0xFF - nLevel) * GetGValue(crDest))) / 0xFF);
			int			b		= (((nLevel * b1) + ((0xFF - nLevel) * GetBValue(crDest))) / 0xFF);

			SetPixel(hDC, x, y, RGB(r, g, b));
		}
	}
}

void FillTransparentSolidRectEx(HDC hDC, int x, int y, int cx, int cy, COLORREF cr, int nLevel)
{
	RECT	rect	= { x, y, x + cx, y + cy };

	FillTransparentSolidRectEx(hDC, &rect, cr, nLevel);
}

void DrawTransparentEx(HDC hDC, LPCRECT lpRect, HDC hSource, int nLevel)
{
	int		nWidth		= (lpRect->right - lpRect->left);
	int		nHeight		= (lpRect->bottom - lpRect->top);
	HDC		hCompatible	= CreateCompatibleDC(hDC);
	HBITMAP	hBitmap		= CreateCompatibleBitmap(hDC, nWidth, nHeight);
	HBITMAP	hOldBmp		= static_cast<HBITMAP>(SelectObject(hCompatible, hBitmap));

	FillSolidRectEx(hCompatible, 0, 0, nWidth, nHeight, RGB(nLevel, nLevel, nLevel));
	BitBlt(hDC, lpRect->left, lpRect->top, nWidth, nHeight, hCompatible, 0, 0, SRCAND);
	BitBlt(hDC, lpRect->left, lpRect->top, nWidth, nHeight, hSource, 0, 0, SRCPAINT);
	SelectObject(hCompatible, hOldBmp);
	DeleteDC(hCompatible);
}

void DrawTransparentEx(HDC hDC, int x, int y, int cx, int cy, HDC hSource, int nLevel)
{
	HDC		hCompatible	= CreateCompatibleDC(hDC);
	HBITMAP	hBitmap		= CreateCompatibleBitmap(hDC, cx, cy);
	HBITMAP	hOldBmp		= static_cast<HBITMAP>(SelectObject(hCompatible, hBitmap));

	FillSolidRectEx(hCompatible, 0, 0, cx, cy, RGB(nLevel, nLevel, nLevel));
	BitBlt(hDC, x, y, cx, cy, hCompatible, 0, 0, SRCAND);
	BitBlt(hDC, x, y, cx, cy, hSource, 0, 0, SRCPAINT);
	SelectObject(hCompatible, hOldBmp);
	DeleteObject(hBitmap);
	DeleteDC(hCompatible);
}

BOOL LineEx(HDC hDC, int x1, int y1, int x2, int y2, COLORREF cr)
{
	POINT	ptLine[2]	= { { x1, y1 }, { x2, y2 } };
	HPEN	hPen		= ::CreatePen(PS_SOLID, 1, cr);
	HPEN	hPenOld		= static_cast<HPEN>(SelectObject(hDC, hPen));
	BOOL	bResult;

	bResult	= Polyline(hDC, ptLine, 2);

	SelectObject(hDC, hPenOld);
	DeleteObject(hPen);
	return bResult;
}
