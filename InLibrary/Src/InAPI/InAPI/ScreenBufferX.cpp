// ScreenBufferX.cpp

#include "StdAfx.h"
#include "ScreenBufferX.h"

struct DIBINFO : public BITMAPINFO
{
	RGBQUAD	arColors[255];

	operator LPBITMAPINFO()			{ return reinterpret_cast<LPBITMAPINFO>(this); }
	operator LPBITMAPINFOHEADER()	{ return &bmiHeader; }
	RGBQUAD*	ColorTable()		{ return bmiColors; }
};

CScreenBufferX::CScreenBufferX(void) : m_hBitmap(NULL), m_pBuffer(NULL), m_hDC(NULL)
{
}

CScreenBufferX::~CScreenBufferX(void)
{
	Reset();
}

void CScreenBufferX::Create(int nWidth, int nHeight)
{
	ASSERT(nWidth > 0);
	ASSERT(nHeight > 0);
	CreateBitmap(nWidth, nHeight);
}

void CScreenBufferX::Create(int nWidth, int nHeight, COLORREF cr)
{
	ASSERT(nWidth > 0);
	ASSERT(nHeight > 0);
	CreateBitmap(nWidth, nHeight);

	BGRClr	bgrColor	= BGRClr(GetBValue(cr), GetGValue(cr), GetRValue(cr));
	int		nPosition	= 0;

	for (int y = 0; y < nHeight; y++)
	{
		nPosition	= m_nCorrectedWidth * y;

		for (int x = 0; x < nWidth; x++)
		{
			m_pBuffer[nPosition]	= bgrColor;
			nPosition++;
		}
	}
}

void CScreenBufferX::Create(HBITMAP hBitmap)
{
	BITMAP	bm;

	GetObject(hBitmap, sizeof(BITMAP), &bm);
	CreateBitmap(bm.bmWidth, bm.bmHeight);

	HDC	hMemDC	= CreateCompatibleDC(NULL);
	HDC	hDC		= CreateCompatibleDC(NULL);

	HBITMAP	hOldBitmap1	= static_cast<HBITMAP>(::SelectObject(hMemDC, hBitmap));
	HBITMAP	hOldBitmap2	= static_cast<HBITMAP>(::SelectObject(hDC, m_hBitmap));

	BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
	::SelectObject(hMemDC, hOldBitmap1);
	::SelectObject(hDC, hOldBitmap2);
	DeleteDC(hMemDC);
	DeleteDC(hDC);
}

void CScreenBufferX::Create(HDC hDC, LPRECT lpRect)
{
	int	nWidth	= lpRect->right - lpRect->left;
	int	nHeight	= lpRect->bottom - lpRect->top;

	CreateBitmap(nWidth, nHeight);
	BitBlt(GetDC(), 0, 0, nWidth, nHeight, hDC, lpRect->left, lpRect->top, SRCCOPY);
}

void CScreenBufferX::CreateRGB(LPVOID lpData, int nWidth, int nHeight)
{
	ASSERT(lpData);
	ASSERT(nWidth > 0);
	ASSERT(nHeight > 0);

	CreateBitmap(nWidth, nHeight);

	LPBYTE	lpByteData		= reinterpret_cast<LPBYTE>(lpData);
	int		nPosition		= 0;
	int		nDataPosition	= 0;

	for (int y = 0; y < nHeight; y++)
	{
		nPosition		= m_nCorrectedWidth * (m_nHeight - y - 1);
		nDataPosition	= nWidth * 3 * y;

		for (int x = 0; x < nWidth; x++)
		{
			m_pBuffer[nPosition].m_R	= lpByteData[nDataPosition++];
			m_pBuffer[nPosition].m_G	= lpByteData[nDataPosition++];
			m_pBuffer[nPosition].m_B	= lpByteData[nDataPosition++];
			nPosition++;
		}
	}
}

void CScreenBufferX::Reset(void)
{
	if (m_hBitmap != NULL)
	{
		ReleaseDC();
		::DeleteObject(m_hBitmap);

		m_hBitmap	= NULL;
	}
}

BOOL CScreenBufferX::Draw(HDC hDC, int x, int y, LPRECT lprc)
{
	ASSERT(m_hBitmap);
	ReleaseDC();

	RECT	rc;
	HBITMAP	hOldBmp;
	HDC		hMemDC;
	BOOL	bResult		= FALSE;

	if (lprc == NULL)
	{
		SetRect(&rc, 0, 0, m_nWidth, m_nHeight);
	}
	else
	{
		LONG	lValue;

		memcpy_s(&rc, sizeof(RECT), lprc, sizeof(RECT));

		if ((rc.left < 0) || (rc.left > m_nWidth))
		{
			return FALSE;
		}

		if ((rc.right < 0) || (rc.right > m_nWidth))
		{
			return FALSE;
		}

		if ((rc.top < 0) || (rc.top > m_nHeight))
		{
			return FALSE;
		}

		if ((rc.bottom < 0) || (rc.bottom > m_nHeight))
		{
			return FALSE;
		}

		if (rc.left > rc.right)
		{
			lValue		= rc.left;
			rc.left		= rc.right;
			rc.right	= lValue;

			return FALSE;
		}

		if (rc.top > rc.bottom)
		{
			lValue		= rc.top;
			rc.top		= rc.bottom;
			rc.bottom	= lValue;

			return FALSE;
		}

	}

	hMemDC	= CreateCompatibleDC(hDC);

	if (hMemDC == NULL)
	{
		return FALSE;
	}

	hOldBmp	= static_cast<HBITMAP>(::SelectObject(hMemDC, m_hBitmap));
	bResult	= BitBlt(hDC, x, y, rc.right - rc.left, rc.bottom - rc.top, hMemDC, rc.left, rc.top, SRCCOPY);

	::SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);
	return bResult;
}

void CScreenBufferX::SetPoint(int nX, int nY, BYTE R, BYTE G, BYTE B)
{
	m_pBuffer[m_nCorrectedWidth * (m_nHeight - nY - 1) + nX]	= BGRClr(R, G, B);
}

void CScreenBufferX::SetPoint(int nX, int nY, COLORREF cr)
{
	m_pBuffer[m_nCorrectedWidth * (m_nHeight - nY - 1) + nX]	= BGRClr(GetRValue(cr), GetGValue(cr), GetBValue(cr));
}

void CScreenBufferX::SetPointTransparent(int nX, int nY, BYTE R, BYTE G, BYTE B, int nLevel)
{
	BGRClr	src	= m_pBuffer[m_nCorrectedWidth * (m_nHeight - nY - 1) + nX];
	BGRClr	res;

	res.m_R	= (((nLevel * R) + ((0xFF - nLevel) * src.m_R)) / 0xFF);
	res.m_G	= (((nLevel * G) + ((0xFF - nLevel) * src.m_G)) / 0xFF);
	res.m_B	= (((nLevel * B) + ((0xFF - nLevel) * src.m_B)) / 0xFF);

	m_pBuffer[m_nCorrectedWidth * (m_nHeight - nY - 1) + nX]	= res;
}

void CScreenBufferX::SetPointTransparent(int nX, int nY, COLORREF cr, int nLevel)
{
	BGRClr	src	= m_pBuffer[m_nCorrectedWidth * (m_nHeight - nY - 1) + nX];
	BGRClr	res;

	res.m_R	= (((nLevel * GetRValue(cr)) + ((0xFF - nLevel) * src.m_R)) / 0xFF);
	res.m_G	= (((nLevel * GetGValue(cr)) + ((0xFF - nLevel) * src.m_G)) / 0xFF);
	res.m_B	= (((nLevel * GetBValue(cr)) + ((0xFF - nLevel) * src.m_B)) / 0xFF);

	m_pBuffer[m_nCorrectedWidth * (m_nHeight - nY - 1) + nX]	= res;
}

BGRClr CScreenBufferX::GetPoint(int nX, int nY)
{
	return m_pBuffer[m_nCorrectedWidth * (m_nHeight - nY - 1) + nX];
}

BGRClr* CScreenBufferX::GetPoints(void)
{
	return m_pBuffer;
}

void CScreenBufferX::FillSolidRect(LPCRECT lpRect, COLORREF cr)
{
	for (int y = lpRect->top; y < lpRect->bottom; y++)
	{
		for (int x = lpRect->left; x < lpRect->right; x++)
		{
			m_pBuffer[m_nCorrectedWidth * (m_nHeight - y - 1) + x]	= BGRClr(GetRValue(cr), GetGValue(cr), GetBValue(cr));
		}
	}
}

void CScreenBufferX::FillSolidRect(int x, int y, int cx, int cy, COLORREF cr)
{
	RECT	rect	= { x, y, x + cx, y + cy };

	FillSolidRect(&rect, cr);
}

void CScreenBufferX::FillHGRect(LPCRECT lpRect, COLORREF crLeft, COLORREF crRight)
{
	int	r1		= GetRValue(crLeft);
	int	g1		= GetGValue(crLeft);
	int	b1		= GetBValue(crLeft);
	int	r2		= GetRValue(crRight);
	int	g2		= GetGValue(crRight);
	int	b2		= GetBValue(crRight);
	int	width	= (lpRect->right - lpRect->left);
	int	height	= (lpRect->bottom - lpRect->top);

	for (int i = 0; i < width; i++)
	{
		int	r	= r1 + (i * (r2 - r1) / width);
		int	g	= g1 + (i * (g2 - g1) / width);
		int	b	= b1 + (i * (b2 - b1) / width);

		FillSolidRect(lpRect->left + i, lpRect->top, 1, height, RGB(r, g, b));
	}
}

void CScreenBufferX::FillVGRect(LPCRECT lpRect, COLORREF crTop, COLORREF crBottom)
{
	int	r1		= GetRValue(crTop);
	int	g1		= GetGValue(crTop);
	int	b1		= GetBValue(crTop);
	int	r2		= GetRValue(crBottom);
	int	g2		= GetGValue(crBottom);
	int	b2		= GetBValue(crBottom);
	int	width	= (lpRect->right - lpRect->left);
	int	height	= (lpRect->bottom - lpRect->top);

	for (int i = 0; i < height; i++)
	{
		int	r	= r1 + (i * (r2 - r1) / height);
		int	g	= g1 + (i * (g2 - g1) / height);
		int	b	= b1 + (i * (b2 - b1) / height);

		FillSolidRect(lpRect->left, lpRect->top + i, width, 1, RGB(r, g, b));
	}
}

void CScreenBufferX::FillTransparentSolidRect(LPCRECT lpcRect, COLORREF cr, int nLevel)
{
	int	r1	= GetRValue(cr);
	int	g1	= GetGValue(cr);
	int	b1	= GetBValue(cr);

	for (int y = lpcRect->top; y < lpcRect->bottom; y++)
	{
		for (int x = lpcRect->left; x < lpcRect->right; x++)
		{
			SetPointTransparent(x, y, cr, nLevel);
		}
	}
}

void CScreenBufferX::FillTransparentSolidRect(int x, int y, int cx, int cy, COLORREF cr, int nLevel)
{
	RECT	rect	= { x, y, x + cx, y + cy };

	FillTransparentSolidRect(&rect, cr, nLevel);
}

void CScreenBufferX::FillTransparentHGRect(LPCRECT lpRect, COLORREF crLeft, COLORREF crRight, int nLevel)
{
	int	r1		= GetRValue(crLeft);
	int	g1		= GetGValue(crLeft);
	int	b1		= GetBValue(crLeft);
	int	r2		= GetRValue(crRight);
	int	g2		= GetGValue(crRight);
	int	b2		= GetBValue(crRight);
	int	width	= (lpRect->right - lpRect->left);
	int	height	= (lpRect->bottom - lpRect->top);

	for (int i = 0; i < width; i++)
	{
		int	r	= r1 + (i * (r2 - r1) / width);
		int	g	= g1 + (i * (g2 - g1) / width);
		int	b	= b1 + (i * (b2 - b1) / width);

		FillTransparentSolidRect(lpRect->left + i, lpRect->top, 1, height, RGB(r, g, b), nLevel);
	}
}

void CScreenBufferX::FillTransparentVGRect(LPCRECT lpRect, COLORREF crTop, COLORREF crBottom, int nLevel)
{
	int	r1		= GetRValue(crTop);
	int	g1		= GetGValue(crTop);
	int	b1		= GetBValue(crTop);
	int	r2		= GetRValue(crBottom);
	int	g2		= GetGValue(crBottom);
	int	b2		= GetBValue(crBottom);
	int	width	= (lpRect->right - lpRect->left);
	int	height	= (lpRect->bottom - lpRect->top);

	for (int i = 0; i < height; i++)
	{
		int	r	= r1 + (i * (r2 - r1) / height);
		int	g	= g1 + (i * (g2 - g1) / height);
		int	b	= b1 + (i * (b2 - b1) / height);

		FillTransparentSolidRect(lpRect->left, lpRect->top + i, width, 1, RGB(r, g, b), nLevel);
	}
}

int CScreenBufferX::GetCorrectedWidth(void)
{
	return m_nCorrectedWidth;
}

int CScreenBufferX::GetWidth(void)
{
	return m_nWidth;
}

int CScreenBufferX::GetHeight(void)
{
	return m_nHeight;
}

HBITMAP CScreenBufferX::GetHBitmap(void)
{
	return m_hBitmap;
}

HDC CScreenBufferX::GetDC(void)
{
	if (m_hDC != NULL)
	{
		return m_hDC;
	}

	m_hDC	= CreateCompatibleDC(NULL);

	if (m_hDC == NULL)
	{
		return NULL;
	}

	m_hSaveBitmap	= static_cast<HBITMAP>(::SelectObject(m_hDC, GetHBitmap()));

	return m_hDC;
}

void CScreenBufferX::ReleaseDC(void)
{
	if (m_hDC != NULL)
	{
		SelectObject(m_hDC, m_hSaveBitmap);
		DeleteDC(m_hDC);

		m_hDC	= NULL;
	}
}

HBITMAP CScreenBufferX::CreateBitmapByRGBArray(LPVOID lpData, int nWidth, int nHeight)
{
	HBITMAP			hResult	= NULL;
	CScreenBufferX	sbx;

	sbx.CreateRGB(lpData, nWidth, nHeight);

	hResult			= sbx.m_hBitmap;
	sbx.m_hBitmap	= NULL;
	sbx.m_pBuffer	= NULL;

	return hResult;
}

int CScreenBufferX::CorrectedWidth(int nWidth)
{
	return (((nWidth + 3) / 4) * 4);
}

BOOL CScreenBufferX::CreateBitmap(int nWidth, int nHeight)
{
	ASSERT(nWidth > 0);
	ASSERT(nHeight > 0);

	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
	}

	m_nCorrectedWidth	= CorrectedWidth(nWidth);
	m_nWidth			= nWidth;
	m_nHeight			= nHeight;

	DIBINFO	dibInfo;

	dibInfo.bmiHeader.biBitCount		= 24;
	dibInfo.bmiHeader.biClrImportant	= 0;
	dibInfo.bmiHeader.biClrUsed			= 0;
	dibInfo.bmiHeader.biCompression		= 0;
	dibInfo.bmiHeader.biHeight			= m_nHeight;
	dibInfo.bmiHeader.biPlanes			= 1;
	dibInfo.bmiHeader.biSize			= 40;
	dibInfo.bmiHeader.biSizeImage		= m_nCorrectedWidth * m_nHeight * 3;
	dibInfo.bmiHeader.biWidth			= m_nCorrectedWidth;
	dibInfo.bmiHeader.biXPelsPerMeter	= 3780;
	dibInfo.bmiHeader.biYPelsPerMeter	= 3780;
	dibInfo.bmiColors[0].rgbBlue		= 0;
	dibInfo.bmiColors[0].rgbGreen		= 0;
	dibInfo.bmiColors[0].rgbRed			= 0;
	dibInfo.bmiColors[0].rgbReserved	= 0;

	HDC	hDC	= ::GetDC(NULL);

	ASSERT(hDC);

	m_hBitmap	= CreateDIBSection(hDC, static_cast<const BITMAPINFO*>(dibInfo), DIB_RGB_COLORS, reinterpret_cast<void**>(&m_pBuffer), NULL, 0);

	::ReleaseDC(NULL, hDC);
	ASSERT(m_hBitmap);
	ASSERT(m_pBuffer);
	return TRUE;
}
