// ScreenBufferX.h

#pragma once

#include "InAPI.h"

struct BGRClr
{
	BGRClr()												{}
	BGRClr(BYTE R, BYTE G, BYTE B) : m_R(R), m_G(G), m_B(B)	{}

	BYTE	m_B;
	BYTE	m_G;
	BYTE	m_R;
};

class INAPI_CLASS CScreenBufferX
{
public:
	CScreenBufferX(void);
	virtual	~CScreenBufferX(void);

	void	Create(int nWidth, int nHeight);
	void	Create(int nWidth, int nHeight, COLORREF cr);
	void	Create(HBITMAP hBitmap);
	void	Create(HDC hDC, LPRECT lpRect);
	void	CreateRGB(LPVOID lpData, int nWidth, int nHeight);
	void	Reset(void);

	BOOL	Draw(HDC hDC, int x, int y, LPRECT lprc);

	HBITMAP	GetHBitmap(void);

	void	SetPoint(int nX, int nY, BYTE R, BYTE G, BYTE B);
	void	SetPoint(int nX, int nY, COLORREF cr);
	void	SetPointTransparent(int nX, int nY, BYTE R, BYTE G, BYTE B, int nLevel);
	void	SetPointTransparent(int nX, int nY, COLORREF cr, int nLevel);
	BGRClr	GetPoint(int nX, int nY);
	BGRClr*	GetPoints(void);

	void	FillSolidRect(LPCRECT lpcRect, COLORREF cr);
	void	FillSolidRect(int x, int y, int cx, int cy, COLORREF cr);
	void	FillHGRect(LPCRECT lpRect, COLORREF crLeft, COLORREF crRight); 
	void	FillVGRect(LPCRECT lpRect, COLORREF crTop, COLORREF crBottom);

	// Level 0x00 - Fully Transparent, 0xFF - Fully Opaque
	void	FillTransparentSolidRect(LPCRECT lpcRect, COLORREF cr, int nLevel);
	void	FillTransparentSolidRect(int x, int y, int cx, int cy, COLORREF cr, int nLevel);
	void	FillTransparentHGRect(LPCRECT lpRect, COLORREF crLeft, COLORREF crRight, int nLevel); 
	void	FillTransparentVGRect(LPCRECT lpRect, COLORREF crTop, COLORREF crBottom, int nLevel);

	int		GetCorrectedWidth(void);
	int		GetWidth(void);
	int		GetHeight(void);

	HDC		GetDC(void);
	void	ReleaseDC(void);

	static	HBITMAP	CreateBitmapByRGBArray(LPVOID lpData, int nWidth, int nHeight);
	static	int		CorrectedWidth(int nWidth);

protected:
	BOOL	CreateBitmap(int nWidth, int nHeight);

	int		m_nWidth;
	int		m_nHeight;
	int		m_nCorrectedWidth;
	
	BGRClr*	m_pBuffer;

	HDC		m_hDC;
	HBITMAP	m_hBitmap;
	HBITMAP	m_hSaveBitmap;
};
