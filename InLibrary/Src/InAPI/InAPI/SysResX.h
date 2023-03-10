// SysResX.h

#pragma once

#include "InAPI.h"

#ifndef SYSRES_ID
#define SYSRES_ID
#define SYSRES_OPEN_TITLE			0x00000001
#define SYSRES_SAVE_TITLE			0x00000002
#define SYSRES_COLUMN_NAME			0x00000100
#define SYSRES_COLUMN_SIZE			0x00000101
#define SYSRES_COLUMN_TYPE			0x00000102
#define SYSRES_COLUMN_DATE			0x00000103
#define SYSRES_DEF_DIR				0x00000110
#define SYSRES_DEF_ROOT_NAME		0x00000111
#define SYSRES_CAPTION_CANCEL		0x00000112
#define SYSRES_CAPTION_NAME			0x00000113
#define SYSRES_CAPTION_TYPE			0x00000114
#define SYSRES_CAPTION_NEWFOLDER	0x00000115
#define SYSRES_MSG_PATH_MUST_EXIST	0x00000200
#define SYSRES_MSG_FILE_MUST_EXIST	0x00000201
#define SYSRES_MSG_CREATE_PROMPT	0x00000202
#define SYSRES_MSG_OVERWRITE_PROMPT	0x00000203
#define SYSRES_MSG_INVALIDNAME		0x00000204
#define SYSRES_MSG_ALREADYEXISTS	0x00000205
#define SYSRES_MSG_NEWFOLDERERROR	0x00000206
#define SYSRES_MSG_FOLDERTOOLONG	0x00000207
#define SYSRES_MENU_UP				0x00000300
#define SYSRES_MENU_MENU			0x00000301
#define SYSRES_MENU_SELECTALL		0x00000302
#define SYSRES_MENU_DETAIL			0x00000303
#define SYSRES_MENU_SMALLICONS		0x00000304
#endif

class INAPI_CLASS CSysResX
{
public:
	CSysResX(void);
	virtual	~CSysResX(void);

	BOOL	IsWM5(void);
	int		LoadString(UINT uID, LPTSTR lpBuffer, int cchBufferMax);

protected:
	BOOL	m_bWM2003SE;
	BOOL	m_bWM5;
	BOOL	m_bWM6;

	HINSTANCE	m_hCoreDll;
	HINSTANCE	m_hCEShell;
	HINSTANCE	m_hNoteProject;
	HINSTANCE	m_hAygShell;

	static BOOL	CALLBACK	DummyProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
