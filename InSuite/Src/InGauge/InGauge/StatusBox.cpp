// StatusBox.cpp

#include "stdafx.h"
#include "InGauge.h"
#include "StatusBox.h"
#include "FontX.h"
#include "ColorX.h"
#include "GraphicsX.h"
#include "ItemTags.h"

#ifndef CRITICAL_NORMAL
#define CRITICAL_NORMAL		25
#define CRITICAL_REVERSED	75
#define FULL_NORMAL			98
#define FULL_REVERSED		2
#endif

#ifndef COLOR_FULLLEFT
#define COLOR_FULLLEFT		RGB(0x00, 0x00, 0xFF)
#define COLOR_FULLRIGHT		RGB(0xCC, 0xCC, 0xFF)
#define COLOR_LEFT			RGB(0x00, 0xFF, 0x30)
#define COLOR_RIGHT			RGB(0x66, 0xFF, 0xCC)
#define COLOR_CRITICALLEFT	RGB(0xFF, 0x00, 0x00)
#define COLOR_CRITICALRIGHT	RGB(0xFF, 0xCC, 0xCC)
#define COLOR_FAULTLEFT		RGB(0x80, 0x80, 0x80)
#define COLOR_FAULTRIGHT	RGB(0xCC, 0xCC, 0xCC)
#endif

#ifndef GB64_UNIT
#define GB64_UNIT			((__int64)1073741824)
#define GB64_UNDERZERO		((__int64)107374182)
#define MB64_UNIT			((__int64)1048576)
#define MB64_UNDERZERO		((__int64)104858)
#define MB_UNIT				1048576
#define MB_UNDERZERO		104858
#endif

IMPLEMENT_DYNAMIC(CStatusBox, CListBox)

CStatusBox::CStatusBox()
{
}

CStatusBox::~CStatusBox()
{
}

BEGIN_MESSAGE_MAP(CStatusBox, CListBox)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

void CStatusBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);

	LPCTSTR	lpszText	= (LPCTSTR)lpMeasureItemStruct->itemData;

	ASSERT(lpszText != NULL);

	HFONT	hFont;
	HFONT	hOldFont;
	LOGFONT	lfFont;
	CSize	sz;
	CDC*	pDC	= GetDC();

	GetDefaultFont(&lfFont);

	hFont		= CreateFontIndirect(&lfFont);
	hOldFont	= (HFONT)SelectObject(pDC->GetSafeHdc(), hFont);
	sz			= pDC->GetTextExtent(lpszText);

	SelectObject(pDC->GetSafeHdc(), hOldFont);
	DeleteObject(hFont);
	ReleaseDC(pDC);

	lpMeasureItemStruct->itemHeight	= (sz.cy * 2) + DRA::SCALEY(16);
}

void CStatusBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);

	LPCTSTR	lpszText	= (LPCTSTR)lpDrawItemStruct->itemData;

	ASSERT(lpszText != NULL);

	HDC			hMemDC		= CreateCompatibleDC(lpDrawItemStruct->hDC);
	int			nItemWidth	= lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left;
	int			nItemHeight	= lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;
	HBITMAP		hBitmap		= CreateCompatibleBitmap(lpDrawItemStruct->hDC, nItemWidth, nItemHeight);
	HBITMAP		hOldBmp		= (HBITMAP)SelectObject(hMemDC, hBitmap);
	COLORREF	crOld		= SetTextColor(hMemDC, RGB(0x00, 0x00, 0x00));
	RECT		rc;

	SetRect(&rc, DRA::SCALEX(0), DRA::SCALEY(0), nItemWidth, nItemHeight);
	FillSolidRectEx(hMemDC, &rc, GetBkColor(hMemDC));
	SetRect(&rc, DRA::SCALEX(4), 0, nItemWidth - DRA::SCALEX(4), nItemHeight);

	if (!_tcscmp(lpszText, TAG_MAIN_BATTERY))	// Main Battery
	{
		DrawMainBattery(hMemDC, &rc, lpszText);
	}
	else if (!_tcscmp(lpszText, TAG_BACKUP_BATTERY))	// Backup Battery
	{
		DrawBackupBattery(hMemDC, &rc, lpszText);
	}
	else if (!_tcscmp(lpszText, TAG_PROGRAM_MEMORY))	// Program Memory
	{
		DrawProgramMemory(hMemDC, &rc, lpszText);
	}
	else if (!_tcscmp(lpszText, TAG_STORAGE_MEMORY))	// Storage Memory
	{
		DrawStorageMemory(hMemDC, &rc, lpszText);
	}
	else	// External Memories
	{
		DrawExternalMemory(hMemDC, &rc, lpszText);
	}

	SetTextColor(hMemDC, crOld);
	BitBlt(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, nItemWidth, nItemHeight, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
}

void CStatusBox::DrawGauge(HDC hDC, LPRECT lpRect, int nPercent, LPCTSTR lpszMessage, BOOL bReversed)
{
	CRect		rc(lpRect);
	int			nShowPercent	= nPercent;
	COLORREF	crLeft;
	COLORREF	crRight;
	LOGFONT		lfFont;
	HFONT		hFont;
	HFONT		hOldFont;

	RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, DRA::SCALEX(3), DRA::SCALEY(3));
	rc.DeflateRect(DRA::SCALEX(2), DRA::SCALEY(2));

	if ((nShowPercent < 0) || (nShowPercent > 100))
	{
		nShowPercent	= 100;
	}

	int	nLength	= (rc.Width() * nShowPercent / 100);

	FillSolidRectEx(hDC, rc, GetSystemColor(SYSCOLOR_WINDOW_BACK));

	rc.right	= rc.left + nLength;

	if (nPercent != nShowPercent)
	{
		crLeft	= COLOR_FAULTLEFT;
		crRight	= COLOR_FAULTRIGHT;
	}
	else
	{
		if (bReversed == FALSE)
		{
			if (nShowPercent <= CRITICAL_NORMAL)
			{
				crLeft	= COLOR_CRITICALLEFT;
				crRight	= COLOR_CRITICALRIGHT;
			}
			else if (nShowPercent >= FULL_NORMAL)
			{
				crLeft	= COLOR_FULLLEFT;
				crRight	= COLOR_FULLRIGHT;
			}
			else
			{
				crLeft	= COLOR_LEFT;
				crRight	= COLOR_RIGHT;
			}
		}
		else
		{
			if (nShowPercent >= CRITICAL_REVERSED)
			{
				crLeft	= COLOR_CRITICALLEFT;
				crRight	= COLOR_CRITICALRIGHT;
			}
			else if (nShowPercent <= FULL_REVERSED)
			{
				crLeft	= COLOR_FULLLEFT;
				crRight	= COLOR_FULLRIGHT;
			}
			else
			{
				crLeft	= COLOR_LEFT;
				crRight	= COLOR_RIGHT;
			}
		}
	}

	FillHGRectEx(hDC, rc, crLeft, crRight);

	COLORREF	crOldColor	= SetTextColor(hDC, RGB(0x30, 0x30, 0x30));
	int			nOldMode	= SetBkMode(hDC, TRANSPARENT);

	GetDefaultFont(&lfFont);

	hFont		= CreateFontIndirect(&lfFont);
	hOldFont	= (HFONT)SelectObject(hDC, hFont);

	rc.CopyRect(lpRect);
	rc.DeflateRect(DRA::SCALEX(4), DRA::SCALEY(1));
	DrawTextEx(hDC, lpszMessage, (-1), rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_DISABLE_UNDERLINE);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
	SetBkMode(hDC, nOldMode);
	SetTextColor(hDC, crOldColor);
}

void CStatusBox::DrawMainBattery(HDC hDC, LPRECT lprc, LPCTSTR lpszText)
{
	int		nTextHeight	= (((lprc->bottom - lprc->top) - DRA::SCALEY(16)) / 2);
	RECT	rc;
	LOGFONT	lfSystem;
	LOGFONT	lfFont;
	HFONT	hFont;
	HFONT	hOldFont;
	CString	strItem;

	SYSTEM_POWER_STATUS_EX2	spsex2;

	GetDefaultFont(&lfSystem);
	memcpy_s(&lfFont, sizeof(LOGFONT), &lfSystem, sizeof(LOGFONT));

	rc.left		= lprc->left + DRA::SCALEX(2);
	rc.right	= lprc->right - DRA::SCALEX(72);
	rc.top		= lprc->top + DRA::SCALEY(4);
	rc.bottom	= rc.top + nTextHeight;

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	strItem.LoadString(IDS_PW_MAIN);
	DrawTextEx(hDC, strItem, (-1), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	rc.left		= lprc->left;
	rc.right	= lprc->right;
	rc.top		= rc.bottom + DRA::SCALEY(2);
	rc.bottom	= rc.top + nTextHeight + DRA::SCALEY(6);

	GetSystemPowerStatusEx2(&spsex2, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE);

	if (spsex2.BatteryLifePercent == 0xFF)
	{
		strItem.LoadString(IDS_GAUGE_UNKNOWN);
	}
	else
	{
		strItem.Format(IDS_GAUGE_FORMAT, spsex2.BatteryLifePercent);
	}

	DrawGauge(hDC, &rc, spsex2.BatteryLifePercent, strItem);

	if (spsex2.ACLineStatus == AC_LINE_ONLINE)
	{
		rc.left		= lprc->right - DRA::SCALEX(68);
		rc.right	= lprc->right - DRA::SCALEX(2);
		rc.top		= lprc->top + DRA::SCALEY(4);
		rc.bottom	= rc.top + nTextHeight;

		lfFont.lfWeight	= FW_NORMAL;
		hFont			= CreateFontIndirect(&lfFont);
		hOldFont		= (HFONT)SelectObject(hDC, hFont);

		strItem.LoadString(IDS_PW_CHARGING);
		DrawTextEx(hDC, strItem, (-1), &rc, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
		SelectObject(hDC, hOldFont);
		DeleteObject(hFont);
	}
}

void CStatusBox::DrawBackupBattery(HDC hDC, LPRECT lprc, LPCTSTR lpszText)
{
	int		nTextHeight	= (((lprc->bottom - lprc->top) - DRA::SCALEY(16)) / 2);
	RECT	rc;
	LOGFONT	lfSystem;
	LOGFONT	lfFont;
	HFONT	hFont;
	HFONT	hOldFont;
	CString	strItem;

	SYSTEM_POWER_STATUS_EX2	spsex2;

	GetDefaultFont(&lfSystem);
	memcpy_s(&lfFont, sizeof(LOGFONT), &lfSystem, sizeof(LOGFONT));

	rc.left		= lprc->left + DRA::SCALEX(2);
	rc.right	= lprc->right - DRA::SCALEX(2);
	rc.top		= lprc->top + DRA::SCALEY(4);
	rc.bottom	= rc.top + nTextHeight;

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	strItem.LoadString(IDS_PW_BACKUP);
	DrawTextEx(hDC, strItem, (-1), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	rc.left		= lprc->left;
	rc.right	= lprc->right;
	rc.top		= rc.bottom + DRA::SCALEY(2);
	rc.bottom	= rc.top + nTextHeight + DRA::SCALEY(6);

	GetSystemPowerStatusEx2(&spsex2, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE);

	if (spsex2.BackupBatteryLifePercent == 0xFF)
	{
		strItem.LoadString(IDS_GAUGE_UNKNOWN);
	}
	else
	{
		strItem.Format(IDS_GAUGE_FORMAT, spsex2.BackupBatteryLifePercent);
	}

	DrawGauge(hDC, &rc, spsex2.BackupBatteryLifePercent, strItem);
}

void CStatusBox::DrawProgramMemory(HDC hDC, LPRECT lprc, LPCTSTR lpszText)
{
	int		nTextHeight	= (((lprc->bottom - lprc->top) - DRA::SCALEY(16)) / 2);
	RECT	rc;
	LOGFONT	lfSystem;
	LOGFONT	lfFont;
	HFONT	hFont;
	HFONT	hOldFont;
	CString	strItem;
	CString	strUnit;
	int		nPercent;

	MEMORYSTATUS	mst;

	GetDefaultFont(&lfSystem);
	memcpy_s(&lfFont, sizeof(LOGFONT), &lfSystem, sizeof(LOGFONT));

	rc.left		= lprc->left + DRA::SCALEX(2);
	rc.right	= lprc->right - DRA::SCALEX(72);
	rc.top		= lprc->top + DRA::SCALEY(4);
	rc.bottom	= rc.top + nTextHeight;

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	strItem.LoadString(IDS_ST_PROGRAM);
	DrawTextEx(hDC, strItem, (-1), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	rc.left		= lprc->left;
	rc.right	= lprc->right - DRA::SCALEX(70);
	rc.top		= rc.bottom + DRA::SCALEY(2);
	rc.bottom	= rc.top + nTextHeight + DRA::SCALEY(6);

	mst.dwLength	= sizeof(MEMORYSTATUS);

	GlobalMemoryStatus(&mst);

	nPercent	= 100 - (((mst.dwAvailPhys / MB_UNIT) * 100) / (mst.dwTotalPhys / MB_UNIT));

	strItem.Format(IDS_GAUGE_FORMAT, nPercent);
	DrawGauge(hDC, &rc, nPercent, strItem, TRUE);

	strUnit.Format(IDS_UNIT_MB);
	strItem.Format(IDS_STORAGE_FORMAT, (mst.dwTotalPhys / MB_UNIT), ((mst.dwTotalPhys % MB_UNIT) / MB_UNDERZERO), strUnit);

	rc.left		= lprc->right - DRA::SCALEX(68);
	rc.right	= lprc->right - DRA::SCALEX(2);
	rc.top		= lprc->top;
	rc.bottom	= rc.top + ((lprc->bottom - lprc->top) / 2) - DRA::SCALEY(1);

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	DrawTextEx(hDC, strItem, (-1), &rc, DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	strUnit.Format(IDS_UNIT_MB);
	strItem.Format(IDS_STORAGE_FORMAT, (mst.dwAvailPhys / MB_UNIT), ((mst.dwAvailPhys % MB_UNIT) / MB_UNDERZERO), strUnit);

	rc.left		= lprc->right - DRA::SCALEX(68);
	rc.right	= lprc->right - DRA::SCALEX(2);
	rc.top		= lprc->top + ((lprc->bottom - lprc->top) / 2) + DRA::SCALEY(1);
	rc.bottom	= lprc->bottom;

	lfFont.lfWeight	= FW_NORMAL;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	DrawTextEx(hDC, strItem, (-1), &rc, DT_RIGHT | DT_SINGLELINE | DT_TOP);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
}

void CStatusBox::DrawStorageMemory(HDC hDC, LPRECT lprc, LPCTSTR lpszText)
{
	int		nTextHeight	= (((lprc->bottom - lprc->top) - DRA::SCALEY(16)) / 2);
	RECT	rc;
	LOGFONT	lfSystem;
	LOGFONT	lfFont;
	HFONT	hFont;
	HFONT	hOldFont;
	CString	strItem;
	CString	strUnit;
	int		nPercent;

	STORE_INFORMATION	si;

	GetDefaultFont(&lfSystem);
	memcpy_s(&lfFont, sizeof(LOGFONT), &lfSystem, sizeof(LOGFONT));

	rc.left		= lprc->left + DRA::SCALEX(2);
	rc.right	= lprc->right - DRA::SCALEX(72);
	rc.top		= lprc->top + DRA::SCALEY(4);
	rc.bottom	= rc.top + nTextHeight;

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	strItem.LoadString(IDS_ST_STORAGE);
	DrawText(hDC, strItem, (-1), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	rc.left		= lprc->left;
	rc.right	= lprc->right - DRA::SCALEX(70);
	rc.top		= rc.bottom + DRA::SCALEY(2);
	rc.bottom	= rc.top + nTextHeight + DRA::SCALEY(6);

	GetStoreInformation(&si);

	nPercent	= 100 - (((si.dwFreeSize / MB_UNIT) * 100) / (si.dwStoreSize / MB_UNIT));

	strItem.Format(IDS_GAUGE_FORMAT, nPercent);
	DrawGauge(hDC, &rc, nPercent, strItem, TRUE);

	strUnit.Format(IDS_UNIT_MB);
	strItem.Format(IDS_STORAGE_FORMAT, (si.dwStoreSize / MB_UNIT), ((si.dwStoreSize % MB_UNIT) / MB_UNDERZERO), strUnit);

	rc.left		= lprc->right - DRA::SCALEX(68);
	rc.right	= lprc->right - DRA::SCALEX(2);
	rc.top		= lprc->top;
	rc.bottom	= rc.top + ((lprc->bottom - lprc->top) / 2);

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	DrawTextEx(hDC, strItem, (-1), &rc, DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	strUnit.Format(IDS_UNIT_MB);
	strItem.Format(IDS_STORAGE_FORMAT, (si.dwFreeSize / MB_UNIT), ((si.dwFreeSize % MB_UNIT) / MB_UNDERZERO), strUnit);

	rc.left		= lprc->right - DRA::SCALEX(68);
	rc.right	= lprc->right - DRA::SCALEX(2);
	rc.top		= lprc->top + ((lprc->bottom - lprc->top) / 2) + DRA::SCALEY(1);
	rc.bottom	= lprc->bottom;

	lfFont.lfWeight	= FW_NORMAL;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	DrawTextEx(hDC, strItem, (-1), &rc, DT_RIGHT | DT_SINGLELINE | DT_TOP);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
}

void CStatusBox::DrawExternalMemory(HDC hDC, LPRECT lprc, LPCTSTR lpszText)
{
	int		nTextHeight	= (((lprc->bottom - lprc->top) - DRA::SCALEY(16)) / 2);
	RECT	rc;
	LOGFONT	lfSystem;
	LOGFONT	lfFont;
	HFONT	hFont;
	HFONT	hOldFont;
	CString	strItem;
	CString	strUnit;
	int		nPercent;

	ULARGE_INTEGER	uiFree;
	ULARGE_INTEGER	uiTotal;

	GetDefaultFont(&lfSystem);
	memcpy_s(&lfFont, sizeof(LOGFONT), &lfSystem, sizeof(LOGFONT));

	rc.left		= lprc->left + DRA::SCALEX(2);
	rc.right	= lprc->right - DRA::SCALEX(62);
	rc.top		= lprc->top + DRA::SCALEY(4);
	rc.bottom	= rc.top + nTextHeight;

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	strItem	= lpszText;

	DrawText(hDC, strItem, (-1), &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	rc.left		= lprc->left;
	rc.right	= lprc->right - DRA::SCALEX(70);
	rc.top		= rc.bottom + DRA::SCALEY(2);
	rc.bottom	= rc.top + nTextHeight + DRA::SCALEY(6);

	if (GetDiskFreeSpaceEx(lpszText, &uiFree, &uiTotal, NULL) == NULL)
	{
		nPercent			= 255;
		uiFree.QuadPart		= 0;
		uiTotal.QuadPart	= 0;
	}
	else
	{
		nPercent	= 100 - (int)(__int64)(((__int64)uiFree.QuadPart / MB64_UNIT * 100) / ((__int64)uiTotal.QuadPart / MB64_UNIT));
	}

	strItem.Format(IDS_GAUGE_FORMAT, nPercent);
	DrawGauge(hDC, &rc, nPercent, strItem, TRUE);

	if (((__int64)uiTotal.QuadPart) >= GB64_UNIT)
	{
		strUnit.Format(IDS_UNIT_GB);
		strItem.Format(IDS_STORAGE_FORMAT, (int)(__int64)(((__int64)uiTotal.QuadPart) / GB64_UNIT), (int)(__int64)((((__int64)uiTotal.QuadPart) % GB64_UNIT) / GB64_UNDERZERO), strUnit);
	}
	else
	{
		strUnit.Format(IDS_UNIT_MB);
		strItem.Format(IDS_STORAGE_FORMAT, (int)(__int64)(((__int64)uiTotal.QuadPart) / MB64_UNIT), (int)(__int64)((((__int64)uiTotal.QuadPart) % MB64_UNIT) / MB64_UNDERZERO), strUnit);
	}

	rc.left		= lprc->right - DRA::SCALEX(68);
	rc.right	= lprc->right - DRA::SCALEX(2);
	rc.top		= lprc->top;
	rc.bottom	= rc.top + ((lprc->bottom - lprc->top) / 2);

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	DrawText(hDC, strItem, (-1), &rc, DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	if (((__int64)uiFree.QuadPart) >= GB64_UNIT)
	{
		strUnit.Format(IDS_UNIT_GB);
		strItem.Format(IDS_STORAGE_FORMAT, (int)(__int64)(((__int64)uiFree.QuadPart) / GB64_UNIT), (int)(__int64)((((__int64)uiFree.QuadPart) % GB64_UNIT) / GB64_UNDERZERO), strUnit);
	}
	else
	{
		strUnit.Format(IDS_UNIT_MB);
		strItem.Format(IDS_STORAGE_FORMAT, (int)(__int64)(((__int64)uiFree.QuadPart) / MB64_UNIT), (int)(__int64)((((__int64)uiFree.QuadPart) % MB64_UNIT) / MB64_UNDERZERO), strUnit);
	}

	rc.left		= lprc->right - DRA::SCALEX(68);
	rc.right	= lprc->right - DRA::SCALEX(2);
	rc.top		= lprc->top + ((lprc->bottom - lprc->top) / 2) + DRA::SCALEY(1);
	rc.bottom	= lprc->bottom;

	lfFont.lfWeight	= FW_NORMAL;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hDC, hFont);

	DrawTextEx(hDC, strItem, (-1), &rc, DT_RIGHT | DT_SINGLELINE | DT_TOP);
	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);
}
