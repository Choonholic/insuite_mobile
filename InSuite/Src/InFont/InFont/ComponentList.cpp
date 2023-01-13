// ComponentList.cpp

#include "stdafx.h"
#include "InFont.h"
#include "ComponentList.h"
#include "ComponentTags.h"
#include "FontX.h"
#include "GraphicsX.h"

#define MAX_LINKS	6

typedef struct _tComponentLink
{
	TCHAR	szTag[32];
	UINT	nID;
} COMPONENT_LINK;

COMPONENT_LINK	g_arrLinks[MAX_LINKS]	=
{
	{ TAG_DEFAULT_FONT,	IDS_DEFAULT_FONT	},
	{ TAG_SYSTEM_FONT,	IDS_SYSTEM_FONT		},
	{ TAG_POPUP_FONT,	IDS_POPUP_MENU_FONT	},
	{ TAG_MENUBAR_FONT,	IDS_MENU_BAR_FONT	},
	{ TAG_OOM_FONT,		IDS_OOM_FONT		},
	{ TAG_BUTTON_FONT,	IDS_BUTTON_FONT		}
};

IMPLEMENT_DYNAMIC(CComponentList, CListBox)

CComponentList::CComponentList()
{
}

CComponentList::~CComponentList()
{
}

BEGIN_MESSAGE_MAP(CComponentList, CListBox)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

void CComponentList::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);

	LPCTSTR	lpszText	= (LPCTSTR)lpMeasureItemStruct->itemData;

	ASSERT(lpszText != NULL);

	HFONT	hFont;
	HFONT	hOldFont;
	LOGFONT	lfFont;
	CString	strItem;
	CSize	szTitle;
	CSize	szPreview;
	int		nIndex	= 0;
	CDC*	pDC	= GetDC();

	for (int i = 0; i < MAX_LINKS; i++)
	{
		if (!_tcscmp(lpszText, g_arrLinks[i].szTag))
		{
			nIndex	= i;
			break;
		}
	}

	GetTaskBarFont(&lfFont);
	strItem.LoadString(g_arrLinks[nIndex].nID);

	hFont		= CreateFontIndirect(&lfFont);
	hOldFont	= (HFONT)SelectObject(pDC->GetSafeHdc(), hFont);
	szTitle		= pDC->GetTextExtent(strItem);

	SelectObject(pDC->GetSafeHdc(), hOldFont);
	DeleteObject(hFont);

	switch (nIndex)
	{
	case 0:
		{
			GetDefaultFont(&lfFont);
		}
		break;
	case 1:
		{
			GetSystemFont(&lfFont);
		}
		break;
	case 2:
		{
			GetPopupMenuFont(&lfFont);
		}
		break;
	case 3:
		{
			GetMenuBarFont(&lfFont);
		}
		break;
	case 4:
		{
			GetOutOfMemoryFont(&lfFont);
		}
		break;
	case 5:
		{
			GetButtonFont(&lfFont);
		}
		break;
	}

	strItem.Format(IDS_COMPONENTS_FORMAT, lfFont.lfFaceName, HtToPts(lfFont.lfHeight), lfFont.lfWeight);

	hFont		= CreateFontIndirect(&lfFont);
	hOldFont	= (HFONT)SelectObject(pDC->GetSafeHdc(), hFont);
	szPreview	= pDC->GetTextExtent(strItem);

	SelectObject(pDC->GetSafeHdc(), hOldFont);
	DeleteObject(hFont);
	ReleaseDC(pDC);

	lpMeasureItemStruct->itemHeight	= szTitle.cy + szPreview.cy + DRA::SCALEY(10);
}

void CComponentList::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);

	LPCTSTR	lpszText	= (LPCTSTR)lpDrawItemStruct->itemData;

	ASSERT(lpszText != NULL);

	HDC			hMemDC		= CreateCompatibleDC(lpDrawItemStruct->hDC);
	int			nItemWidth	= lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left;
	int			nItemHeight	= lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;
	HBITMAP		hBitmap		= CreateCompatibleBitmap(lpDrawItemStruct->hDC, nItemWidth, nItemHeight);
	HBITMAP		hOldBmp		= (HBITMAP)SelectObject(hMemDC, hBitmap);
	COLORREF	crOldText	= GetTextColor(hMemDC);
	COLORREF	crOldBack	= GetBkColor(hMemDC);
	HFONT		hFont;
	HFONT		hOldFont;
	LOGFONT		lfFont;
	CString		strItem;
	int			nIndex		= 0;
	RECT		rc;
	RECT		rcText;
	CSize		szText;

	SetRect(&rc, DRA::SCALEX(0), DRA::SCALEY(0), nItemWidth, nItemHeight);

	if ((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		SetTextColor(hMemDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		SetBkColor(hMemDC, GetSysColor(COLOR_HIGHLIGHT));
		FillSolidRectEx(hMemDC, &rc, GetSysColor(COLOR_HIGHLIGHT));
	}
	else
	{
		FillSolidRectEx(hMemDC, &rc, crOldBack);
	}

	SetRect(&rc, DRA::SCALEX(4), 0, nItemWidth - DRA::SCALEX(4), nItemHeight);

	rcText.left		= rc.left;
	rcText.top		= rc.top + DRA::SCALEY(4);
	rcText.right	= rc.right;
	rcText.bottom	= rc.bottom - DRA::SCALEY(4);

	for (int i = 0; i < MAX_LINKS; i++)
	{
		if (!_tcscmp(lpszText, g_arrLinks[i].szTag))
		{
			nIndex	= i;
			break;
		}
	}

	GetTaskBarFont(&lfFont);
	strItem.LoadString(g_arrLinks[nIndex].nID);

	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(hMemDC, hFont);

	DrawTextEx(hMemDC, strItem, (-1), &rcText, DT_LEFT | DT_SINGLELINE | DT_TOP);
	SelectObject(hMemDC, hOldFont);
	DeleteObject(hFont);

	switch (nIndex)
	{
	case 0:
		{
			GetDefaultFont(&lfFont);
		}
		break;
	case 1:
		{
			GetSystemFont(&lfFont);
		}
		break;
	case 2:
		{
			GetPopupMenuFont(&lfFont);
		}
		break;
	case 3:
		{
			GetMenuBarFont(&lfFont);
		}
		break;
	case 4:
		{
			GetOutOfMemoryFont(&lfFont);
		}
		break;
	case 5:
		{
			GetButtonFont(&lfFont);
		}
		break;
	}

	strItem.Format(IDS_COMPONENTS_FORMAT, lfFont.lfFaceName, HtToPts(lfFont.lfHeight), lfFont.lfWeight);

	hFont		= CreateFontIndirect(&lfFont);
	hOldFont	= (HFONT)SelectObject(hMemDC, hFont);

	DrawTextEx(hMemDC, strItem, (-1), &rcText, DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
	SelectObject(hMemDC, hOldFont);
	DeleteObject(hFont);

	SetTextColor(hMemDC, crOldText);
	SetBkColor(hMemDC, crOldBack);
	BitBlt(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, nItemWidth, nItemHeight, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
}
