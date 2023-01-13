// Utilities.h

#pragma once

INT	GetSpecialDirectoryEx(INT nFolderID, LPTSTR lpszDir, int nSize);
int	CombineFileName(LPTSTR lpszFullPath, int nSize, LPCTSTR lpszFolder, LPCTSTR lpszFileName);
int	ExtendSubPath(LPTSTR lpszFullPath, int nSize, LPCTSTR lpszBasePath, LPCTSTR lpszSubPath, BOOL bAddSeparator = FALSE);

BOOL	WriteActivateBinaryX(LPCTSTR lpszFileName, DWORD dwVersion, SYSTEMTIME stDate, BOOL bIgnore = FALSE);
BOOL	WriteActivateBinaryX(LPCTSTR lpszFileName, DWORD dwVersion, DWORD dwAdd);
DWORD	MakeVersionNumberX(WORD wProductID, WORD wVersion);
BOOL	CheckBundleX(LPCTSTR lpszPart, int nShift);
