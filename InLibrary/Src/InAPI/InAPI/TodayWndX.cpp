// TodayWndX.cpp

#include "StdAfx.h"
#include "TodayWndX.h"
#include "DisplayX.h"
#include "FontX.h"
#include "RegistryX.h"
#include "GraphicsX.h"

CTodayWndX::CTodayWndX(void)
{
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_bPaintClip	= FALSE;
	m_bLayered		= TRUE;

	WM_SH_UIMETRIC_CHANGE	= 0;
	WM_TODAYWNDX_REFRESH	= 0;

	SetRect(&m_rectWnd, 0, 0, 0, 0);
}

CTodayWndX::CTodayWndX(HINSTANCE hInstance, LPCTSTR lpszClassName, LPCTSTR lpszWindowName)
{
	m_lpszClassName		= lpszClassName;
	m_lpszWindowName	= lpszWindowName;
	m_hInstance			= hInstance;
	m_hResource			= hInstance;

	CTodayWndX();
}

CTodayWndX::CTodayWndX(HINSTANCE hInstance, HINSTANCE hResource, LPCTSTR lpszClassName, LPCTSTR lpszWindowName)
{
	m_lpszClassName		= lpszClassName;
	m_lpszWindowName	= lpszWindowName;
	m_hInstance			= hInstance;
	m_hResource			= hResource;

	CTodayWndX();
}

CTodayWndX::~CTodayWndX(void)
{
	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
	}

	ReleaseTodayFonts();
}

void CTodayWndX::RegisterTodayClass(WNDPROC wndProc)
{
	WNDCLASS wndClass;

	memset(&wndClass, 0, sizeof(wndClass));

	wndClass.hCursor		= 0;
	wndClass.hIcon			= 0;
	wndClass.hInstance		= m_hInstance;
	wndClass.lpszClassName	= m_lpszClassName;
	wndClass.lpszMenuName	= NULL;
	wndClass.style			= CS_VREDRAW | CS_HREDRAW;
	wndClass.hbrBackground	= static_cast<HBRUSH>(GetStockObject(HOLLOW_BRUSH));
	wndClass.lpfnWndProc	= wndProc;

	RegisterClass(&wndClass);
}

void CTodayWndX::UnregisterTodayClass(void)
{
	if (m_hInstance)
	{
		UnregisterClass(m_lpszClassName, m_hInstance);
	}
}

void CTodayWndX::Initialize(void)
{
}

BOOL CTodayWndX::Create(HWND hWndParent, DWORD dwStyle)
{
	m_bInitialRefresh	= TRUE;
	m_hWndParent		= hWndParent;
	m_hWnd				= CreateWindow(m_lpszClassName, m_lpszWindowName, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, GetSystemMetrics(SM_CXSCREEN), 0, m_hWndParent, NULL, m_hInstance, NULL);

	if (m_hWnd)
	{
		CreateToolTipWnd();
		GetTodayDefaults();
		OnPostCreateWindow();

		WM_SH_UIMETRIC_CHANGE	= RegisterWindowMessage(SH_UIMETRIC_CHANGE);
	}

	return (m_hWnd != NULL);
}

void CTodayWndX::Refresh(void)
{
	m_bRefresh	= TRUE;
}

void CTodayWndX::RefreshWindow(BOOL bShow)
{
	RECT	rect;

	if (m_hWnd)
	{
		if (bShow)
		{
			ShowWindow(m_hWnd, SW_SHOWNORMAL);
		}

		GetClientRect(m_hWnd, &rect);
		InvalidateRect(m_hWnd, &rect, TRUE);
		UpdateWindow(m_hWnd);
	}
}

BOOL CTodayWndX::InvokeOptions(void)
{
	if (!_tcslen(m_szOptionsPath))
	{
		return FALSE;
	}

	return (CreateProcess(m_szOptionsPath, m_szOptionsParameters, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL));
}

LRESULT CTodayWndX::TodayWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!uMsg)
	{
		return 0;
	}

	if (uMsg == WM_SH_UIMETRIC_CHANGE)
	{
		ReleaseTodayFonts();
		CreateTodayFonts();
		Refresh();
		return 0;
	}

	if (uMsg == WM_TODAYWNDX_REFRESH)
	{
		OnRefreshSettings();
		Refresh();
		return 0;
	}

	switch (uMsg)
	{
	case WM_CREATE:
		{
			LPCREATESTRUCT	lpcs	= reinterpret_cast<LPCREATESTRUCT>(lParam);

			if (OnCreate(lpcs) == -1)
			{
				return -1;
			}
		}
		break;
	case WM_DESTROY:
		{
			DestroyToolTipWnd();
			OnDestroy();
			return 0;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT	ps;
			HDC			hDCDefault;
			HDC			hDCClip;

			SetInternalDC(BeginPaint(m_hWnd, &ps));
			PrepareTodayDC();

			hDCDefault	= GetTodayDCDefault();

			OnEraseBkgnd(hDCDefault);
			DrawTodayIcon(hDCDefault);
			SetTextColor(hDCDefault, GetTodayColor());
			OnPaint(hDCDefault);
			PaintTodayDCDefault(hDCDefault);

			if (m_bPaintClip == TRUE)
			{
				RECT	rcClip;

				if (m_bLayered == FALSE)
				{
					hDCClip	= GetTodayDCClip();

					OnEraseBkgnd(hDCClip);
					DrawTodayIcon(hDCClip);
					SetTextColor(hDCClip, GetTodayColor());
				}
				else
				{
					hDCClip	= hDCDefault;
				}

				OnPaintClip(hDCClip, &rcClip);
				PaintTodayDCClip(hDCClip, &rcClip);
			}

			PaintTodayDCCombine();
			ReleaseTodayDCDefault(hDCDefault);

			if ((m_bPaintClip == TRUE) && (m_bLayered == FALSE))
			{
				ReleaseTodayDCClip(hDCClip);
			}

			ReleaseTodayDC();
			EndPaint(m_hWnd, &ps);
			return 0;
		}
	case WM_ERASEBKGND:
		{
			return FALSE;
		}
	case WM_TIMER:
		{
			switch (wParam)
			{
			case 1:
				{
					KillTimer(m_hWnd, m_nTimerID);
					RefreshWindow();
				}
				break;
			}

			return FALSE;
		}
	case WM_TODAYCUSTOM_CLEARCACHE:
		{
			TODAYLISTITEM*	ptli	= (TODAYLISTITEM*)wParam;

			return static_cast<LRESULT>(OnTodayCustomClearCache(ptli));
		}
	case WM_TODAYCUSTOM_QUERYREFRESHCACHE:
		{
			BOOL			bResult	= FALSE;
			TODAYLISTITEM*	ptli	= reinterpret_cast<TODAYLISTITEM*>(wParam);
			DWORD			dwFlags	= 0;

			OnTodayCustomQueryRefreshCache(ptli, &dwFlags);

			if (dwFlags)
			{
				bResult	= TRUE;
			}

			if (m_bInitialRefresh)
			{
				ptli->cyp			= m_nItemHeight;
				ptli->grfFlags		= static_cast<DWORD>(m_bUseIcon & 0x0000FFFF);
				m_bInitialRefresh	= FALSE;
				bResult				= TRUE;
			}

			if (m_bRefresh)
			{
				m_bRefresh	= FALSE;
				bResult		= TRUE;
			}

			if (ptli->cyp != static_cast<DWORD>(m_nItemHeight))
			{
				ptli->cyp	= static_cast<DWORD>(m_nItemHeight);
				bResult		= TRUE;
			}

			if (LOWORD(ptli->grfFlags) != static_cast<WORD>(m_bUseIcon))
			{
				ptli->grfFlags	= static_cast<DWORD>(ptli->grfFlags & 0xFFFF0000);
				ptli->grfFlags	+= static_cast<WORD>(m_bUseIcon);
				bResult			= TRUE;
			}

			if (IsWindowMoved())
			{
				bResult	= TRUE;
			}

			if (bResult)
			{
				OnPostRefreshSettings(dwFlags);

				m_nTimerID	= SetTimer(m_hWnd, 1, m_nInterval, NULL);
			}

			return static_cast<LRESULT>(bResult);
		}
	case WM_LBUTTONDOWN:
		{
			if (m_bSelected)
			{
				RequestSelection();
				break;
			}

			POINT		point;
			SHRGINFO	rg;

			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);

			m_bToolTipActivate	= FALSE;
			rg.cbSize			= sizeof(SHRGINFO);
			rg.hwndClient		= m_hWnd;
			rg.ptDown.x			= point.x;
			rg.ptDown.y			= point.y;
			rg.dwFlags			= ((!m_dwHoldFlags) ? SHRG_RETURNCMD : m_dwHoldFlags);

			if (SHRecognizeGesture(&rg) == GN_CONTEXTMENU)
			{
				if (m_bUseToolTip)
				{
					RECT	rect, rectTip;

					if (!OnSetToolTip(point))
					{
						m_bToolTipActivate	= TRUE;

						SendMessage(m_hToolTipWnd, WM_SETTEXT, reinterpret_cast<WPARAM>(m_hFontT), reinterpret_cast<LPARAM>(m_szToolTipText));
						GetWindowRect(m_hWnd, &rect);
						GetWindowRect(m_hToolTipWnd, &rectTip);

						rectTip.left	= rect.left + LOWORD(lParam) - (rectTip.right - rectTip.left) - 10;

						if (rectTip.left < 0)
						{
							rectTip.left	= 0;
						}

						rectTip.top	= rect.top + HIWORD(lParam) - (rectTip.bottom - rectTip.top) - 10;

						if (rectTip.top < m_rectWndWA.top)
						{
							rectTip.top	= m_rectWndWA.top;
						}

						SetWindowPos(m_hToolTipWnd, HWND_TOPMOST, rectTip.left, rectTip.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_SHOWWINDOW);
						SetCapture(m_hWnd);
					}
					else
					{
						OnContextMenu(1, point);
					}
				}
				else
				{
					OnContextMenu(0, point);
				}
			}
			else
			{
				OnLButtonDown(wParam, point);
			}

			return 0;
		}

		break;
	case WM_LBUTTONUP:
		{
			if (m_bUseToolTip && m_bToolTipActivate)
			{
				ReleaseCapture();

				m_bToolTipActivate	= FALSE;

				ShowWindow(m_hToolTipWnd, SW_HIDE);
			}
			else
			{
				POINT	point;

				point.x	= LOWORD(lParam);
				point.y	= HIWORD(lParam);

				OnLButtonUp(wParam, point);
			}

			RefreshWindow();
			return 0;
		}
	case WM_MOUSEMOVE:
		{
			POINT	point;

			point.x	= LOWORD(lParam);
			point.y	= HIWORD(lParam);

			OnMouseMove(wParam, point);
			return 0;
		}
	case WM_SETTINGCHANGE:
		{
			if (wParam == SETTINGCHANGE_RESET)
			{
				Refresh();
			}
			else
			{
				OnSettingChange(wParam, reinterpret_cast<LPCTSTR>(lParam));
			}

			return 0;
		}
	case WM_TODAYCUSTOM_RECEIVEDSELECTION:
		{
			m_bSelected	= TRUE;

			OnReceivedSelection(static_cast<int>(wParam));
			return TRUE;
		}
	case WM_TODAYCUSTOM_LOSTSELECTION:
		{
			m_bSelected	= FALSE;

			OnLostSelection();
		}

		break;
	case WM_TODAYCUSTOM_USERNAVIGATION:
		{
			Refresh();
			return OnUserNavigation(static_cast<int>(wParam));
		}
	case WM_TODAYCUSTOM_ACTION:
		{
			OnAction();
		}
		break;
	case WM_NOTIFY:
		{
			NMHDR*	pNMHDR	= reinterpret_cast<NMHDR*>(lParam);

			return OnNotify(wParam, pNMHDR);
		}
	case WM_COMMAND:
		{
			WORD	wID			= LOWORD(wParam);
			HWND	hWndCtrl	= reinterpret_cast<HWND>(lParam);

			if (OnCommand(wID, hWndCtrl))
			{
				return 0;
			}
		}
		break;
	default:
		{
			return OnMessage(uMsg, wParam, lParam);
		}
	}

	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CTodayWndX::TodayDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			SHINITDLGINFO	shidi;

			shidi.dwMask	= SHIDIM_FLAGS;
			shidi.dwFlags	= SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg		= hWnd;

			SHInitDialog(&shidi);

			m_nOptionsTimerID	= SetTimer(hWnd, 2, 50, NULL);

			return TRUE;
		}
	case WM_TIMER:
		{
			KillTimer(hWnd, m_nOptionsTimerID);
			InvokeOptions();
			SendMessage(hWnd, WM_COMMAND, IDOK, NULL);
			return TRUE;
		}
	case WM_COMMAND:
		{
			if (LOWORD(wParam) == IDOK)
			{
				EndDialog(hWnd, LOWORD(wParam));
				return TRUE;
			}
		}
		break;
	default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	return FALSE;
}

void CTodayWndX::SetInstance(HINSTANCE hInstance)
{
	m_hInstance	= hInstance;
}

void CTodayWndX::SetResourceInstance(HINSTANCE hInstance)
{
	m_hResource	= hInstance;
}

void CTodayWndX::SetClassInfo(LPCTSTR lpszClassName, LPCTSTR lpszWindowName)
{
	m_lpszClassName		= lpszClassName;
	m_lpszWindowName	= lpszWindowName;
}

void CTodayWndX::SetRefreshMessage(UINT uiMessageID)
{
	WM_TODAYWNDX_REFRESH	= uiMessageID;
}

void CTodayWndX::DefaultSettings(void)
{
	SetIcon();
	SetItemHeight();
	UseIcon();
	SetRefreshRate();
	SetToolTipFontSize();
	UseToolTip();
	SetHoldFlags();
	SetOptionsTransparent();
	SetOptionsItem(_T(""));
	SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rectWndWA, 0);

	m_bSelected	= FALSE;
}

void CTodayWndX::SetItemHeight(int nHeight)
{
	if (nHeight == TWXHEIGHT_DEFAULT)
	{
		nHeight	= DRA::SCALEY(20);
	}

	m_nItemHeight	= nHeight;
}

void CTodayWndX::UseIcon(BOOL bFlag)
{
	m_bUseIcon	= bFlag;
}

BOOL CTodayWndX::SetIcon(UINT uID, int nWidth, int nHeight)
{
	int nLeft	= DRA::SCALEX(2);
	int	nTop	= DRA::SCALEY(2);

	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
		SetRect(&m_rectIcon, 0, 0, 0, 0);

		m_hIcon	= NULL;
	}

	if (m_hResource)
	{
		m_hIcon	= static_cast<HICON>(LoadImage(m_hResource, MAKEINTRESOURCE(uID), IMAGE_ICON, nWidth, nHeight, LR_DEFAULTCOLOR));

		SetRect(&m_rectIcon, nLeft, nTop, nLeft + nWidth, nTop + nHeight);
	}

	return (m_hIcon != NULL);
}

void CTodayWndX::SetRefreshRate(int nInterval)
{
	m_nInterval	= nInterval;
}

void CTodayWndX::SetToolTipFontSize(int nDoublePoint)
{
	m_nFontTDblPt	= nDoublePoint;
}

void CTodayWndX::UseToolTip(BOOL bFlag)
{
	m_bUseToolTip	= bFlag;
}

void CTodayWndX::SetHoldFlags(DWORD dwFlags)
{
	m_dwHoldFlags	= dwFlags;
}

void CTodayWndX::SetOptionsTransparent(COLORREF crTransparent)
{
	m_crOptionsTransparent	= crTransparent;
}

void CTodayWndX::SetOptionsItem(LPCTSTR lpszPath, LPCTSTR lpszParameters)
{
	_tcscpy_s(m_szOptionsPath, MAX_PATH + 1, _T(""));
	_tcscpy_s(m_szOptionsParameters, MAX_PATH + 1, _T(""));

	if (lpszPath)
	{
		_tcscpy_s(m_szOptionsPath, MAX_PATH + 1, lpszPath);

		if (lpszParameters)
		{
			_tcscpy_s(m_szOptionsParameters, MAX_PATH + 1, lpszParameters);
		}
	}
}

void CTodayWndX::RequestSelection(void)
{
	PostMessage(m_hWndParent, TODAYM_TOOKSELECTION, reinterpret_cast<WPARAM>(m_hWnd), 0);
}

void CTodayWndX::SetPaintClip(BOOL bClip, BOOL bLayered)
{
	m_bPaintClip	= bClip;
	m_bLayered		= bLayered;
}

COLORREF CTodayWndX::GetTodayColor(UINT nItem)
{
	return static_cast<COLORREF>(SendMessage(m_hWndParent, TODAYM_GETCOLOR, static_cast<WPARAM>(nItem), NULL));
}

COLORREF CTodayWndX::GetTodayTextColor(void)
{
	return m_crText;
}

COLORREF CTodayWndX::GetTodayHighlightedTextColor(void)
{
	return m_crHighlightedText;
}

HWND CTodayWndX::GetWindow(void)
{
	return m_hWnd;
}

HWND CTodayWndX::GetParentWindow(void)
{
	return m_hWndParent;
}

HINSTANCE CTodayWndX::GetInstance(void)
{
	return m_hInstance;
}

HINSTANCE CTodayWndX::GetResourceInstance(void)
{
	return m_hResource;
}

LPCTSTR CTodayWndX::GetClassName(void)
{
	return m_lpszClassName;
}

LPCTSTR CTodayWndX::GetWindowName(void)
{
	return m_lpszWindowName;
}

int CTodayWndX::GetItemWidth(void)
{
	HDC		hDC			= GetDC(m_hWnd);
	RECT	rectWnd;

	GetClientRect(m_hWnd, &rectWnd);
	return (rectWnd.right - rectWnd.left);
}

int CTodayWndX::GetItemHeight(void)
{
	return m_nItemHeight;
}

void CTodayWndX::GetUserArea(LPRECT lpRect)
{
	RECT	rectWnd;

	GetClientRect(m_hWnd, &rectWnd);

	if (rectWnd.right > GetSystemMetrics(SM_CXSCREEN))
	{
		rectWnd.right	= GetSystemMetrics(SM_CXSCREEN);
	}

	SetRect(lpRect, (IsUseIcon() ? m_rectIcon.right + DRA::SCALEX(8) : 0) + DRA::SCALEX(2), rectWnd.top + DRA::SCALEY(2), rectWnd.right - DRA::SCALEX(2), rectWnd.bottom - DRA::SCALEY(2));
}

HICON CTodayWndX::GetIcon(void)
{
	return m_hIcon;
}

BOOL CTodayWndX::IsUseIcon(void)
{
	return m_bUseIcon;
}

BOOL CTodayWndX::GetIconRect(LPRECT lpIconRect)
{
	if (m_bUseIcon)
	{
		SetRect(lpIconRect, m_rectIcon.left, m_rectIcon.top, m_rectIcon.right, m_rectIcon.bottom);
	}

	return m_bUseIcon;
}

BOOL CTodayWndX::IsUseToolTip(void)
{
	return m_bUseToolTip;
}

HFONT CTodayWndX::GetTodayNormalFont(void)
{
	return m_hFontN;
}

HFONT CTodayWndX::GetTodaySemiBoldFont(void)
{
	return m_hFontS;
}

HFONT CTodayWndX::GetTodayBoldFont(void)
{
	return m_hFontB;
}

void CTodayWndX::CreateTodayFonts(void)
{
	m_hFontN	= CreateUserFont(NULL, FW_NORMAL);
	m_hFontS	= CreateUserFont(NULL, FW_SEMIBOLD);
	m_hFontB	= CreateUserFont(NULL, FW_BOLD);
	m_hFontT	= CreateFixedFont(NULL, m_nFontTDblPt, FW_NORMAL);
}

void CTodayWndX::ReleaseTodayFonts(void)
{
	if (m_hFontN)
	{
		DeleteObject(m_hFontN);
	}

	if (m_hFontS)
	{
		DeleteObject(m_hFontS);
	}

	if (m_hFontB)
	{
		DeleteObject(m_hFontB);
	}

	if (m_hFontT)
	{
		DeleteObject(m_hFontT);
	}
}

HFONT CTodayWndX::CreateTodayFont(LPCTSTR lpszFontFace, int nDoublePoint, LONG lWeight)
{
	return static_cast<HFONT>(CreateFixedFont(lpszFontFace, nDoublePoint, lWeight));
}

void CTodayWndX::ReleaseTodayFont(HFONT hFont)
{
	ReleaseFont(hFont);
}

BOOL CTodayWndX::IsSelected(void)
{
	return m_bSelected;
}

void CTodayWndX::DrawTodayIcon(HDC hDC)
{
	if (m_hIcon && m_bUseIcon)
	{
		SetBkMode(hDC, TRANSPARENT);
		DrawIconEx(hDC, m_rectIcon.left, m_rectIcon.top, m_hIcon, m_rectIcon.right - m_rectIcon.left, m_rectIcon.bottom - m_rectIcon.top, 0, NULL, DI_NORMAL);
	}
}

BOOL CTodayWndX::PaintTodayDCDefault(HDC hDC)
{
	RECT	rectWnd;

	GetClientRect(m_hWnd, &rectWnd);
	return BitBlt(m_hMemDC, 0, 0, rectWnd.right - rectWnd.left, rectWnd.bottom - rectWnd.top, hDC, 0, 0, SRCCOPY);
}

BOOL CTodayWndX::PaintTodayDCClip(HDC hDC, LPRECT rcClip)
{
	RECT	rectWnd;

	GetClientRect(m_hWnd, &rectWnd);
	return BitBlt(m_hMemDC, rcClip->left, rcClip->top, rcClip->right - rcClip->left, rcClip->bottom - rcClip->top, hDC, rcClip->left, rcClip->top, SRCCOPY);
}

BOOL CTodayWndX::PaintTodayDCCombine(void)
{
	RECT	rectWnd;

	GetClientRect(m_hWnd, &rectWnd);
	return BitBlt(m_hInternalDC, 0, 0, rectWnd.right - rectWnd.left, rectWnd.bottom - rectWnd.top, m_hMemDC, 0, 0, SRCCOPY);
}

int CTodayWndX::ReleaseTodayDCDefault(HDC hDC)
{
	if (hDC == NULL)
	{
		return 0;
	}

	SelectObject(hDC, m_hOldBmpDefault);
	DeleteObject(m_hBitmapDefault);
	DeleteDC(hDC);

	hDC	= NULL;

	return 1;
}

int CTodayWndX::ReleaseTodayDCClip(HDC hDC)
{
	if (hDC == NULL)
	{
		return 0;
	}

	SelectObject(hDC, m_hOldBmpClip);
	DeleteObject(m_hBitmapClip);
	DeleteDC(hDC);

	hDC	= NULL;

	return 1;
}

int CTodayWndX::ReleaseTodayDC(void)
{
	SelectObject(m_hMemDC, m_hMemOldBmp);
	DeleteObject(m_hMemBitmap);
	DeleteDC(m_hMemDC);
	SetBkMode(m_hInternalDC, m_nOldMode);

	m_hMemDC	= NULL;

	return 1;
}

void CTodayWndX::DrawSurface(HDC hDC, BOOL bForcedDefault)
{
	TODAYDRAWWATERMARKINFO	dwi;

	GetClientRect(m_hWnd, &dwi.rc);

	if ((bForcedDefault == TRUE) || (m_bSelected == FALSE))
	{
		dwi.hdc		= hDC;
		dwi.hwnd	= m_hWnd;

		SendMessage(m_hWndParent, TODAYM_DRAWWATERMARK, 0, reinterpret_cast<LPARAM>(&dwi));
	}
	else
	{
		COLORREF	crOldBk	= GetBkColor(hDC);

		SetBkColor(hDC, m_crHighlight);
		ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &dwi.rc, NULL, 0, NULL);
		SetBkColor(hDC, crOldBk);
	}
}

void CTodayWndX::DrawSurface(HDC hDC, LPRECT lpRect, BOOL bForcedDefault)
{
	if ((bForcedDefault == TRUE) || (m_bSelected == FALSE))
	{
		TODAYDRAWWATERMARKINFO	dwi;

		dwi.hdc	= hDC;

		SetRect(&dwi.rc, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);

		dwi.hwnd	= m_hWnd;

		SendMessage(m_hWndParent, TODAYM_DRAWWATERMARK, 0, reinterpret_cast<LPARAM>(&dwi));
	}
	else
	{
		COLORREF	crOldBk	= GetBkColor(hDC);

		SetBkColor(hDC, m_crHighlight);
		ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
		SetBkColor(hDC, crOldBk);
	}
}

void CTodayWndX::GetTodayDefaults(void)
{
	m_crText			= GetTodayColor();
	m_crHighlight		= GetTodayColor(TODAYCOLOR_HIGHLIGHT);
	m_crHighlightedText	= GetTodayColor(TODAYCOLOR_HIGHLIGHTEDTEXT);

	CreateTodayFonts();
}

BOOL CTodayWndX::IsWindowMoved(void)
{
	RECT	rectWnd;

	GetWindowRect(m_hWnd, &rectWnd);

	if ((rectWnd.left != m_rectWnd.left) || (rectWnd.top != m_rectWnd.top) || (rectWnd.right != m_rectWnd.right) || (rectWnd.bottom != m_rectWnd.bottom))
	{
		SetRect(&m_rectWnd, rectWnd.left, rectWnd.top, rectWnd.right, rectWnd.bottom);
		return TRUE;
	}

	return FALSE;
}

void CTodayWndX::SetInternalDC(HDC hDC)
{
	m_hInternalDC = hDC;
}

void CTodayWndX::PrepareTodayDC(void)
{
	RECT	rectWnd;

	m_nOldMode	= GetBkMode(m_hInternalDC);

	GetClientRect(m_hWnd, &rectWnd);
	SetBkMode(m_hInternalDC, TRANSPARENT);

	m_hMemDC		= CreateCompatibleDC(m_hInternalDC);
	m_hMemBitmap	= CreateCompatibleBitmap(m_hInternalDC, rectWnd.right - rectWnd.left, rectWnd.bottom - rectWnd.top);
	m_hMemOldBmp	= static_cast<HBITMAP>(SelectObject(m_hMemDC, m_hMemBitmap));
}

HDC CTodayWndX::GetTodayDCDefault(void)
{
	HDC		hMemDC;
	RECT	rectWnd;

	GetClientRect(m_hWnd, &rectWnd);

	hMemDC				= CreateCompatibleDC(m_hMemDC);
	m_hBitmapDefault	= CreateCompatibleBitmap(m_hMemDC, rectWnd.right - rectWnd.left, rectWnd.bottom - rectWnd.top);
	m_hOldBmpDefault	= static_cast<HBITMAP>(SelectObject(hMemDC, m_hBitmapDefault));

	SetBkMode(hMemDC, TRANSPARENT);
	return hMemDC;
}

HDC CTodayWndX::GetTodayDCClip(void)
{
	HDC		hMemDC;
	RECT	rectWnd;

	GetClientRect(m_hWnd, &rectWnd);

	hMemDC			= CreateCompatibleDC(m_hMemDC);
	m_hBitmapClip	= CreateCompatibleBitmap(m_hMemDC, rectWnd.right - rectWnd.left, rectWnd.bottom - rectWnd.top);
	m_hOldBmpClip	= static_cast<HBITMAP>(SelectObject(hMemDC, m_hBitmapClip));

	SetBkMode(hMemDC, TRANSPARENT);
	return hMemDC;
}

void CTodayWndX::CreateToolTipWnd(void)
{
	if ((m_bUseToolTip) && (m_hToolTipWnd == NULL))
	{
		HGLOBAL			hGlobal;
		LPDLGTEMPLATE	lpdt;
		LPWORD			lpword;

		hGlobal	= GlobalAlloc(GMEM_ZEROINIT, 1024);

		if (!hGlobal)
		{
			m_bUseToolTip	= FALSE;

			return;
		}

		lpdt					= reinterpret_cast<LPDLGTEMPLATE>(GlobalLock(hGlobal));
		lpdt->style				= WS_POPUP | WS_BORDER;
		lpdt->dwExtendedStyle	= 0;
		lpdt->cdit				= 0;
		lpdt->x					= 0;
		lpdt->y					= 0;
		lpdt->cx				= 40;
		lpdt->cy				= 10;

		lpword		= reinterpret_cast<LPWORD>(lpdt + 1);
		*lpword++	= 0;
		*lpword++	= 0;
		*lpword++	= 0;
		*lpword++	= 0;

		GlobalUnlock(hGlobal);

		m_hToolTipWnd	= CreateDialogIndirect(m_hInstance, reinterpret_cast<LPCDLGTEMPLATE>(hGlobal), m_hWnd, ToolTipWndProc);

		GlobalFree(hGlobal);
	}
}

void CTodayWndX::DestroyToolTipWnd(void)
{
	if (m_bUseToolTip)
	{
		DestroyWindow(m_hToolTipWnd);

		m_hToolTipWnd	= NULL;
	}
}

BOOL CTodayWndX::ToolTipWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT	ps;
	RECT		rect;
	HDC			hDC;
	static		TCHAR szBuffer[2048];
	static		HFONT hFont;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_TIMER:
		{
			ShowWindow(hWnd, SW_HIDE);
		}
		break;
	case WM_SETTEXT:
		{
			if (lParam == 0)
			{
				break;
			}

			_tcscpy_s(szBuffer, 2048, reinterpret_cast<TCHAR *>(lParam));

			hDC		= GetDC(hWnd);
			hFont	= reinterpret_cast<HFONT>(wParam);

			HFONT	hFontOld = static_cast<HFONT>(SelectObject(hDC, hFont));
			RECT	rectToolTip;

			DrawText(hDC, szBuffer, -1, &rectToolTip, DT_CALCRECT);
			SelectObject(hDC, hFontOld);
			ReleaseDC(hWnd, hDC);
			SetWindowPos(hWnd, 0, 0, 0, rectToolTip.right - rectToolTip.left + 6, rectToolTip.bottom - rectToolTip.top + 6, SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW | SWP_NOACTIVATE);
		}

		break;
	case WM_PAINT:
		{
			hDC	= BeginPaint(hWnd, &ps);

			GetClientRect(hWnd, static_cast<LPRECT>(&rect));
			FillRect(hDC, &rect, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

			HFONT	hFontOld = static_cast<HFONT>(SelectObject(hDC, hFont));
			RECT	rectToolTip;

			SetRect(&rectToolTip, 2, 2, rect.right - 2, rect.bottom - 2);
			DrawText(hDC, szBuffer, -1, &rectToolTip, DT_LEFT);
			SelectObject(hDC, hFontOld);
			EndPaint(hWnd, &ps);
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void CTodayWndX::SetToolTipText(LPCTSTR lpszText)
{
	_tcscpy_s(m_szToolTipText, 2048, lpszText);
}

int CTodayWndX::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

void CTodayWndX::OnPostCreateWindow(void)
{
}

void CTodayWndX::OnDestroy(void)
{
	DefWindowProc(m_hWnd, WM_DESTROY, 0, 0);
}

void CTodayWndX::OnPaint(HDC hDC)
{
}

void CTodayWndX::OnPaintClip(HDC hDC, LPRECT rcClip)
{
}

void CTodayWndX::OnEraseBkgnd(HDC hDC)
{
	DrawSurface(hDC, FALSE);
}

void CTodayWndX::OnTodayCustomQueryRefreshCache(TODAYLISTITEM *pTodayListItem, LPDWORD lpdwFlags)
{
}

BOOL CTodayWndX::OnTodayCustomClearCache(TODAYLISTITEM *pTodayListItem)
{
	return FALSE;
}

void CTodayWndX::OnLButtonUp(UINT nFlags, POINT point)
{
	DefWindowProc(m_hWnd, WM_LBUTTONUP, nFlags, MAKELONG(point.x, point.y));
}

void CTodayWndX::OnLButtonDown(UINT nFlags, POINT point)
{
	DefWindowProc(m_hWnd, WM_LBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
}

void CTodayWndX::OnContextMenu(UINT nFlags, POINT point)
{
}

void CTodayWndX::OnMouseMove(UINT nFlags, POINT point)
{
	DefWindowProc(m_hWnd, WM_MOUSEMOVE, nFlags, MAKELONG(point.x, point.y));
}

void CTodayWndX::OnSettingChange(UINT nFlags, LPCTSTR lpszSection)
{
}

LRESULT CTodayWndX::OnNotify(UINT nID, NMHDR* pNMHDR)
{
	return 0;
}

LRESULT CTodayWndX::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

BOOL CTodayWndX::OnCommand(WORD wID, HWND hWndCtrl)
{
	return FALSE;
}

LRESULT CTodayWndX::OnSetToolTip(POINT point)
{
	return 0;
}

void CTodayWndX::OnReceivedSelection(int nVirtualKey)
{
}

void CTodayWndX::OnLostSelection(void)
{
}

BOOL CTodayWndX::OnUserNavigation(int nVirtualKey)
{
	return FALSE;
}

void CTodayWndX::OnAction(void)
{
}

LRESULT	CTodayWndX::OnRefreshSettings()
{
	return 0;
}

void CTodayWndX::OnPostRefreshSettings(DWORD dwFlags)
{
}
