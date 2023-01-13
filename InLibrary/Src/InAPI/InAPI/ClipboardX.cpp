// ClipboardX.cpp

#include "StdAfx.h"
#include "ClipboardX.h"

void Cut(void)
{
	keybd_event(VK_CONTROL, 0, KEYEVENTF_SILENT, 1);
	keybd_event(_T('X'), 0, KEYEVENTF_SILENT, 1);
	keybd_event(_T('X'), 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 1);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 1);
}

void Copy(void)
{
	keybd_event(VK_CONTROL, 0, KEYEVENTF_SILENT, 1);
	keybd_event(_T('C'), 0, KEYEVENTF_SILENT, 1);
	keybd_event(_T('C'), 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 1);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 1);
}

void Paste(void)
{
	keybd_event(VK_CONTROL, 0, KEYEVENTF_SILENT, 1);
	keybd_event(_T('V'), 0, KEYEVENTF_SILENT, 1);
	keybd_event(_T('V'), 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 1);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP | KEYEVENTF_SILENT, 1);
}

BOOL CopyText(LPCTSTR lpszText)
{
	if (!OpenClipboard(NULL))
	{
		return FALSE;
	}

	EmptyClipboard();

	int		nLen	= ((_tcslen(lpszText) + 1) * sizeof(TCHAR));
	HGLOBAL	hMem	= ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, nLen);
	TCHAR*	lpMem;

	if (!hMem)
	{
		return FALSE;
	}

	lpMem	= static_cast<TCHAR*>(GlobalLock(hMem));

	_tcscpy_s(lpMem, _tcslen(lpszText) + 1, lpszText);
	GlobalUnlock(hMem);
	SetClipboardData(CF_UNICODETEXT, hMem);
	CloseClipboard();
	return TRUE;
}
