// WindowX.h

#pragma once

#include "InAPI.h"

// Retrieve Window Icon
HICON	INAPI_API	GetWindowIconEx(HWND hWnd, BOOL bType = ICON_BIG);

// Retrieve Module Name from Window Handle
DWORD	INAPI_API	GetModuleFileNameEx(HWND hWnd, LPWSTR lpszFileName, DWORD cbFileName);

// Detect Top-most Popup Windows
BOOL	INAPI_API	IsTopMostPopupWindow(HWND hWnd, BOOL bAboveStartup = TRUE);
