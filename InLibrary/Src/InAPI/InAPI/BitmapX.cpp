// BitmapX.cpp

#include "stdafx.h"
#include "BitmapX.h"

#define DIB_BITS	24

LPBYTE GetScreenDIBEx(int x, int y, int nWidth, int nHeight, LPDWORD lpdwSize)
{
	HDC			hDC;
	HDC			hMemDC;
	HBITMAP		hBitmap;
	HBITMAP		hOldBmp;
	PBITMAPINFO	pbmi;
	LPBYTE		lpBuffer;
	LPBYTE		lpDIB;
	int			nLength;

	nLength	= ((((nWidth * DIB_BITS) / 8) % 4) ? ((((nWidth * DIB_BITS) / 8) / 4) + 1) * 4 : (nWidth * DIB_BITS) / 8);
	hDC		= GetDC(NULL);
	pbmi	= static_cast<PBITMAPINFO>(LocalAlloc(LPTR, sizeof(BITMAPINFO)));

	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= nWidth;
	pbmi->bmiHeader.biHeight		= nHeight;
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biBitCount		= 24;
	pbmi->bmiHeader.biXPelsPerMeter	= 3780;
	pbmi->bmiHeader.biYPelsPerMeter	= 3780;
	pbmi->bmiHeader.biCompression	= BI_RGB;
	pbmi->bmiHeader.biSizeImage		= nLength * nHeight;
	pbmi->bmiHeader.biClrImportant	= 0;

	hBitmap	= CreateDIBSection(hDC, pbmi, DIB_RGB_COLORS, reinterpret_cast<VOID**>(&lpBuffer), NULL, 0);
	hMemDC	= CreateCompatibleDC(hDC);
	hOldBmp	= static_cast<HBITMAP>(SelectObject(hMemDC, static_cast<HGDIOBJ>(hBitmap)));

	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hDC, x, y, SRCCOPY);

	*lpdwSize	= sizeof(BITMAPINFO) + nLength * nHeight;
	lpDIB		= static_cast<LPBYTE>(LocalAlloc(LMEM_FIXED, *lpdwSize));

	if (lpDIB != NULL)
	{
		memcpy(lpDIB, pbmi, sizeof(BITMAPINFO));
		memcpy(lpDIB + sizeof(BITMAPINFO), lpBuffer, nLength * nHeight);
	}

	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	LocalFree(pbmi);
	ReleaseDC(NULL, hDC);
	return lpBuffer;
}

LONG GetBitmapBitsEx(HBITMAP hbm, LONG cbBuffer, LPVOID lpvBits)
{
	BITMAP	bmp;
	LONG	lRet	= 0;

	if (hbm != NULL)
	{
		if (lpvBits != NULL)
		{
			if (cbBuffer > 0)
			{
				GetObject(hbm, sizeof(BITMAP), &bmp);

				if (cbBuffer < ((bmp.bmBitsPixel / 8) * bmp.bmWidth * bmp.bmHeight))
				{
					memcpy(static_cast<LPBYTE>(lpvBits), static_cast<LPBYTE>(bmp.bmBits), cbBuffer * sizeof(BYTE));

					lRet	= cbBuffer;
				}
				else
				{
					memcpy(static_cast<LPBYTE>(lpvBits), static_cast<LPBYTE>(bmp.bmBits), ((bmp.bmBitsPixel / 8) * bmp.bmWidth * bmp.bmHeight) * sizeof(BYTE));

					lRet	= ((bmp.bmBitsPixel / 8) * bmp.bmWidth * bmp.bmHeight);
				}

				DeleteObject(&bmp);
			}
		}
	}

	return lRet;
}
