// ShellX.h

#pragma once

#include "InAPI.h"

// Shell Icon
HIMAGELIST	INAPI_API	GetSystemIconList(BOOL bType = ICON_SMALL);
int			INAPI_API	GetSystemIconIndex(LPCTSTR lpcszFileName);
HICON		INAPI_API	GetFileIcon(LPCTSTR lpcszFileName, BOOL bType = ICON_SMALL);

#ifndef SHELLICON_SET
#define SHELLICON_SET
#define SHELLICON_DEVICE			0x00
#define SHELLICON_DOCUMENT			0x01
#define SHELLICON_ITEMS				0x02
#define SHELLICON_EXCLAMATION		0x10
#define SHELLICON_WARNING			0x10
#define SHELLICON_INFORMATION		0x11
#define SHELLICON_ASTERISK			0x11
#define SHELLICON_QUESTION			0x12
#define SHELLICON_STOP				0x13
#define SHELLICON_ERROR				0x13
#define SHELLICON_HAND				0x13
#endif

HICON	INAPI_API	GetShellIcon(UINT uID, BOOL bType = ICON_SMALL);

// Special Folders
INT	INAPI_API	GetSpecialDirectoryEx(INT nFolderID, LPTSTR lpszDir, int nSize);

#ifndef SHFOLDER_SET
#define SHFOLDER_SET
#define SHFOLDER_WINDOWS			0x00
#define SHFOLDER_STARTUP			0x01
#define SHFOLDER_RECENT				0x02
#define SHFOLDER_PROGRAMS			0x03
#define SHFOLDER_PROGRAM_FILES		0x04
#define SHFOLDER_MY_DOCUMENTS		0x05
#define SHFOLDER_FONTS				0x06
#define SHFOLDER_FAVORITIES			0x07
#define SHFOLDER_DESKTOP			0x08
#define SHFOLDER_APPLICATION_DATA	0x09
#define SHFOLDER_LEN				0x0A
#endif

BOOL	INAPI_API	GetShellFolders(INT nFolderID, LPTSTR lpszDir, int nSize);

// Storage Manipulations
BOOL	INAPI_API	IsExternalStorage(LPCTSTR lpszFolder, LPTSTR lpszStorage = NULL, int nSize = 0);
BOOL	INAPI_API	IsInSameStorage(LPCTSTR lpszFile1, LPCTSTR lpszFile2);

// File Name Manipulations
int		INAPI_API	CombineFileName(LPTSTR lpszFullPath, int nSize, LPCTSTR lpszFolder, LPCTSTR lpszFileName);
BOOL	INAPI_API	CheckValidFileName(LPCTSTR lpszFilename);

int		INAPI_API	ExtendSubPath(LPTSTR lpszFullPath, int nSize, LPCTSTR lpszBasePath, LPCTSTR lpszSubPath, BOOL bAddSeparator = FALSE);
int		INAPI_API	ExtendSubPath(LPTSTR lpszBasePath, int nSize, LPCTSTR lpszSubPath, BOOL bAddSeparator = FALSE);

LPCTSTR	INAPI_API	TakeName(LPCTSTR lpszFullPath, LPTSTR lpszName, int nSize);
LPCTSTR	INAPI_API	TakePath(LPCTSTR lpszFullPath, LPTSTR lpszPath, int nSize, BOOL bAddSeparator = FALSE);
LPCTSTR	INAPI_API	TakeFileName(LPCTSTR lpszFullPath, LPTSTR lpszFileName, int nSize);
LPCTSTR	INAPI_API	TakeTitle(LPCTSTR lpszFullPath, LPTSTR lpszTitle, int nSize);
LPCTSTR	INAPI_API	TakeExtension(LPCTSTR lpszFullPath, LPTSTR lpszExt, int nSize);

// File Status
#ifndef _TFSX_
#define _TFSX_
typedef struct _tFSX
{
	SYSTEMTIME	stCreated;
	SYSTEMTIME	stModified;
	SYSTEMTIME	stAccessed;
	DWORD		dwSizeLow;
	DWORD		dwSizeHigh;
	DWORD		dwAttributes;
	TCHAR		szFullName[MAX_PATH + 1];
} FILESTATUSX, *PFILESTATUSX, FAR *LPFILESTATUSX;
#endif

BOOL	INAPI_API	GetFileStatusX(LPCTSTR lpszFileName, FILESTATUSX& rStatus);
BOOL	INAPI_API	IsValidFILETIMEX(const FILETIME& ft);
BOOL	INAPI_API	IsFileExist(LPCTSTR lpszFileName);

// Shortcut Manipulations
BOOL	INAPI_API	ParseShortcutTarget(LPCTSTR lpszShortcut, LPTSTR lpszTarget, int cbTarget, LPTSTR lpszParameters = NULL, int cbParameters = 0, LPTSTR lpszIcon = NULL, int cbIcon = 0);
BOOL	INAPI_API	ParseShortcutIcon(LPCTSTR lpszShortcut, LPTSTR lpszIcon, int cbIcon, BOOL bSelected = FALSE);
BOOL	INAPI_API	ParseIconLink(LPCTSTR lpszLink, LPTSTR lpszIcon, int cbIcon, int* pnIconID = NULL);
