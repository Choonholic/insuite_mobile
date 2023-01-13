// AlphaBlendX.cpp

#include "stdafx.h"
#include "AlphaBlendX.h"

HBITMAP LoadAlphaBlendBuffer(LPCTSTR szHeaderFileName, LPCTSTR szBufferFileName)
{
	HANDLE		hFile;
	DWORD		dwRead;
	HDC			hDC;
	BITMAPINFO	bmi;
	LPVOID		pBits;
	HBITMAP		hBitmap;

	hDC	= GetDC(NULL);

	if (hDC == NULL)
	{
		return NULL;
	}

	hFile	= CreateFile(szHeaderFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	ReadFile(hFile, &bmi, sizeof(BITMAPINFOHEADER), &dwRead, NULL);
	CloseHandle(hFile);

	hBitmap	= CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, NULL);

	if (hBitmap == NULL)
	{
		return NULL;
	}

	hFile	= CreateFile(szBufferFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	ReadFile(hFile, pBits, bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * sizeof(RGBQUAD), &dwRead, NULL);
	CloseHandle(hFile);
	ReleaseDC(NULL, hDC);
	return hBitmap;
}

HBITMAP LoadAlphaBlendBuffer(HMODULE hModule, UINT uHeaderID, UINT uBufferID, LPCTSTR lpszType)
{
	HDC			hDC;
	BITMAPINFO	bmi;
	LPVOID		pBits;
	HBITMAP		hBitmap;
	HRSRC		hResInfo;
	DWORD		dwSize;
	HGLOBAL		hResData;
	LPVOID		lpPtr;

	hDC	= GetDC(NULL);

	if (hDC == NULL)
	{
		return NULL;
	}

	hResInfo	= FindResource(hModule, MAKEINTRESOURCE(uHeaderID), lpszType);

	if (hResInfo == NULL)
	{
		return NULL;
	}

	dwSize		= SizeofResource(hModule, hResInfo);
	hResData	= LoadResource(hModule, hResInfo);

	if (hResData == NULL)
	{
		return NULL;
	}

	lpPtr	= LockResource(hResData);

	memcpy_s(&bmi, sizeof(BITMAPINFOHEADER), lpPtr, sizeof(BITMAPINFOHEADER));

	hBitmap	= CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, NULL);

	if (hBitmap == NULL)
	{
		return NULL;
	}

	hResInfo	= FindResource(hModule, MAKEINTRESOURCE(uBufferID), lpszType);

	if (hResInfo == NULL)
	{
		return NULL;
	}

	dwSize		= SizeofResource(hModule, hResInfo);
	hResData	= LoadResource(hModule, hResInfo);

	if (hResData == NULL)
	{
		return NULL;
	}

	lpPtr	= LockResource(hResData);

	memcpy_s(pBits, bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * sizeof(RGBQUAD), lpPtr, bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * sizeof(RGBQUAD));
	ReleaseDC(NULL, hDC);
	return hBitmap;
}
