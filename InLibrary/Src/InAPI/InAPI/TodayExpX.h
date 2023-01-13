// TodayExpX.h

#pragma once

#include <todaycmn.h>

HWND	APIENTRY	InitializeCustomItem(TODAYLISTITEM* ptli, HWND hWndParent);
LRESULT	CALLBACK	WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#if !defined(_NO_TODAY_OPTIONS_)
LRESULT	WINAPI		CustomItemOptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
#endif
