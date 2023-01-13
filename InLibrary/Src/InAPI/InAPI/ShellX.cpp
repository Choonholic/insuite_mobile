// ShellX.cpp

#include "stdafx.h"
#include "ShellX.h"
#include "StringX.h"
#include "DisplayX.h"
#include "projects.h"

#pragma comment(lib, "ceshell.lib")
#pragma comment(lib, "note_prj.lib")

#define GWES_EXE	_T("gwes.exe")
#define SHELL_96	_T("shellres.96.dll")
#define SHELL_192	_T("shellres.192.dll")
#define SHELL_128	_T("shellres.128.dll")
#define SHELL_131	_T("shellres.131.dll")

HIMAGELIST GetSystemIconList(BOOL bType)
{
	HIMAGELIST	himl;
	SHFILEINFO	shfi;

	himl	= reinterpret_cast<HIMAGELIST>(SHGetFileInfo(_T(""), NULL, &shfi, sizeof(shfi), (bType ? SHGFI_LARGEICON : SHGFI_SMALLICON) | SHGFI_SYSICONINDEX));

	return himl;
}

int GetSystemIconIndex(LPCTSTR lpszFileName)
{
	SHFILEINFO	shfi;

	SHGetFileInfo(lpszFileName, NULL, &shfi, sizeof(shfi), SHGFI_SYSICONINDEX);
	return shfi.iIcon;
}

HICON GetFileIcon(LPCTSTR lpszFileName, BOOL bType)
{
	SHFILEINFO	shfi;

	SHGetFileInfo(lpszFileName, NULL, &shfi, sizeof(shfi), (bType ? SHGFI_LARGEICON : SHGFI_SMALLICON) | SHGFI_ICON);
	return shfi.hIcon;
}

HICON GetShellIcon(UINT uID, BOOL bType)
{
	HICON	hIcon	= NULL;
	DWORD	dwDPI	= GetDPI();

	switch (uID)
	{
	case SHELLICON_DEVICE:
		{
			if (bType)
			{
				switch (dwDPI)
				{
				case DPI_96:
					{
						ExtractIconEx(SHELL_96, -13899, &hIcon, NULL, 1);
					}
					break;
				case DPI_192:
					{
						ExtractIconEx(SHELL_192, -13899, &hIcon, NULL, 1);
					}
					break;
				case DPI_128:
					{
						ExtractIconEx(SHELL_128, -13899, &hIcon, NULL, 1);
					}
					break;
				case DPI_131:
					{
						ExtractIconEx(SHELL_131, -13899, &hIcon, NULL, 1);
					}
					break;
				}
			}
			else
			{
				switch (dwDPI)
				{
				case DPI_96:
					{
						ExtractIconEx(SHELL_96, -13899, NULL, &hIcon, 1);
					}
					break;
				case DPI_192:
					{
						ExtractIconEx(SHELL_192, -13899, NULL, &hIcon, 1);
					}
					break;
				case DPI_128:
					{
						ExtractIconEx(SHELL_128, -13899, NULL, &hIcon, 1);
					}
					break;
				case DPI_131:
					{
						ExtractIconEx(SHELL_131, -13899, NULL, &hIcon, 1);
					}
					break;
				}
			}

			return hIcon;
		}
		break;
	case SHELLICON_DOCUMENT:
		{
			if (bType)
			{
				switch (dwDPI)
				{
				case DPI_96:
					{
						ExtractIconEx(SHELL_96, -8209, &hIcon, NULL, 1);
					}
					break;
				case DPI_192:
					{
						ExtractIconEx(SHELL_192, -8209, &hIcon, NULL, 1);
					}
					break;
				case DPI_128:
					{
						ExtractIconEx(SHELL_128, -8209, &hIcon, NULL, 1);
					}
					break;
				case DPI_131:
					{
						ExtractIconEx(SHELL_131, -8209, &hIcon, NULL, 1);
					}
					break;
				}
			}
			else
			{
				switch (dwDPI)
				{
				case DPI_96:
					{
						ExtractIconEx(SHELL_96, -8209, NULL, &hIcon, 1);
					}
					break;
				case DPI_192:
					{
						ExtractIconEx(SHELL_192, -8209, NULL, &hIcon, 1);
					}
					break;
				case DPI_128:
					{
						ExtractIconEx(SHELL_128, -8209, NULL, &hIcon, 1);
					}
					break;
				case DPI_131:
					{
						ExtractIconEx(SHELL_131, -8209, NULL, &hIcon, 1);
					}
					break;
				}
			}

			return hIcon;
		}
		break;
	case SHELLICON_ITEMS:
		{
			if (bType)
			{
				switch (dwDPI)
				{
				case DPI_96:
					{
						ExtractIconEx(SHELL_96, -8210, &hIcon, NULL, 1);
					}
					break;
				case DPI_192:
					{
						ExtractIconEx(SHELL_192, -8210, &hIcon, NULL, 1);
					}
					break;
				case DPI_128:
					{
						ExtractIconEx(SHELL_128, -8210, &hIcon, NULL, 1);
					}
					break;
				case DPI_131:
					{
						ExtractIconEx(SHELL_131, -8210, &hIcon, NULL, 1);
					}
					break;
				}
			}
			else
			{
				switch (dwDPI)
				{
				case DPI_96:
					{
						ExtractIconEx(SHELL_96, -8210, NULL, &hIcon, 1);
					}
					break;
				case DPI_192:
					{
						ExtractIconEx(SHELL_192, -8210, NULL, &hIcon, 1);
					}
					break;
				case DPI_128:
					{
						ExtractIconEx(SHELL_128, -8210, NULL, &hIcon, 1);
					}
					break;
				case DPI_131:
					{
						ExtractIconEx(SHELL_131, -8210, NULL, &hIcon, 1);
					}
					break;
				}
			}

			return hIcon;
		}
		break;
	case SHELLICON_EXCLAMATION:
		{
			if (bType)
			{
				ExtractIconEx(GWES_EXE, -48, &hIcon, NULL, 1);
			}
			else
			{
				ExtractIconEx(GWES_EXE, -48, NULL, &hIcon, 1);
			}

			return hIcon;
		}
		break;
	case SHELLICON_INFORMATION:
		{
			if (bType)
			{
				ExtractIconEx(GWES_EXE, -64, &hIcon, NULL, 1);
			}
			else
			{
				ExtractIconEx(GWES_EXE, -64, NULL, &hIcon, 1);
			}

			return hIcon;
		}
		break;
	case SHELLICON_QUESTION:
		{
			if (bType)
			{
				ExtractIconEx(GWES_EXE, -32, &hIcon, NULL, 1);
			}
			else
			{
				ExtractIconEx(GWES_EXE, -32, NULL, &hIcon, 1);
			}

			return hIcon;
		}
		break;
	case SHELLICON_STOP:
		{
			if (bType)
			{
				ExtractIconEx(GWES_EXE, -16, &hIcon, NULL, 1);
			}
			else
			{
				ExtractIconEx(GWES_EXE, -16, NULL, &hIcon, 1);
			}

			return hIcon;
		}
		break;
	}

	return NULL;
}

INT GetSpecialDirectoryEx(INT nFolderID, LPTSTR lpszDir, int nSize)
{
	int				rc;
	LPITEMIDLIST	pidl;
	BOOL			fUseIMalloc	= TRUE;
	LPMALLOC		lpMalloc	= NULL;

	if (nSize < MAX_PATH)
	{
		return E_FAIL;
	}

	rc	= SHGetMalloc(&lpMalloc);

	if (rc == E_NOTIMPL)
	{
		fUseIMalloc	= FALSE;
	}
	else if (rc != NOERROR)
	{
		return rc;
	}

	rc	= SHGetSpecialFolderLocation(NULL, nFolderID, &pidl);

	if (rc == NOERROR)
	{
		if (SHGetPathFromIDList(pidl, lpszDir))
		{
			rc	= E_FAIL;
		}

		if (fUseIMalloc)
		{
			lpMalloc->Free(pidl);
		}
	}

	if (fUseIMalloc)
	{
		lpMalloc->Release();
	}

	return rc;
}

#define REG_KEY_SHELL_FOLDERS	_T("System\\Explorer\\Shell Folders")

BOOL GetShellFolders(INT nFolderID, LPTSTR lpszDir, int nSize)
{
	HKEY	hKey	= 0;
	TCHAR	szFolder[MAX_PATH + 1];
	DWORD	dwType	= REG_SZ;
	DWORD	dwSize	= MAX_PATH;
	BOOL	bResult	= FALSE;
	TCHAR	arrFolders[SHFOLDER_LEN][MAX_PATH]	=
	{
		_T("Windows"),
		_T("StartUp"),
		_T("Recent"),
		_T("Programs"),
		_T("Program Files"),
		_T("My Documents"),
		_T("Fonts"),
		_T("Favorities"),
		_T("Desktop"),
		_T("Application Data")
	};

	if ((nFolderID < SHFOLDER_WINDOWS) || (nFolderID > SHFOLDER_APPLICATION_DATA))
	{
		return bResult;
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_SHELL_FOLDERS, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, arrFolders[nFolderID], 0, &dwType, reinterpret_cast<LPBYTE>(szFolder), &dwSize) == ERROR_SUCCESS)
		{
			_tcscpy_s(lpszDir, nSize, szFolder);

			bResult	= TRUE;
		}

		RegCloseKey(hKey);
	}

	return bResult;
}

BOOL IsExternalStorage(LPCTSTR lpszFolder, LPTSTR lpszStorage, int nSize)
{
	TCHAR			szFolder[MAX_PATH + 1];
	TCHAR			szFlash[MAX_PATH + 1];
	HANDLE			hFind;
	int				nLength;
	BOOL			bFound	= FALSE;
	BOOL			bExist;
	WIN32_FIND_DATA	fd;

	if (lpszFolder == NULL)
	{
		return FALSE;
	}
	else
	{
		if (lpszFolder[0] != _T('\\'))
		{
			_tcscpy_s(szFolder, MAX_PATH + 1, _T("\\"));
			_tcscat_s(szFolder, MAX_PATH + 1, lpszFolder);
		}
		else
		{
			_tcscpy_s(szFolder, MAX_PATH + 1, lpszFolder);
		}
	}

	nLength	= _tcslen(szFolder);

	if ((szFolder[nLength - 1] == _T('\\')) && (nLength > 1))
	{
		szFolder[nLength - 1] = NULL;
	}

	if (!_tcscmp(szFolder, _T("\\")))
	{
		return FALSE;
	}

	hFind	= FindFirstFlashCard(&fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			wsprintf(szFlash, _T("\\%s"), fd.cFileName);

			if (!_tcsnicmp(szFlash, szFolder, _tcslen(szFlash)))
			{
				bFound	= TRUE;

				if ((lpszStorage != NULL) && (nSize > static_cast<int>(_tcslen(szFlash) + 1)))
				{
					_tcscpy_s(lpszStorage, nSize, szFlash);
				}

				break;
			}

			bExist	= FindNextFlashCard(hFind, &fd);
		} while (bExist);

		FindClose(hFind);
	}

	return bFound;
}

BOOL IsInSameStorage(LPCTSTR lpszFile1, LPCTSTR lpszFile2)
{
	TCHAR	szStorage1[MAX_PATH + 1];
	TCHAR	szStorage2[MAX_PATH + 1];
	BOOL	bIsExternal1	= IsExternalStorage(lpszFile1, szStorage1, MAX_PATH + 1);
	BOOL	bIsExternal2	= IsExternalStorage(lpszFile2, szStorage2, MAX_PATH + 1);

	if (bIsExternal1 && bIsExternal2)
	{
		if (!_tcscmp(szStorage1, szStorage2))
		{
			return TRUE;
		}

		return FALSE;
	}

	if (!bIsExternal1 && !bIsExternal2)
	{
		return TRUE;
	}

	return FALSE;
}

int CombineFileName(LPTSTR lpszFullPath, int nSize, LPCTSTR lpszFolder, LPCTSTR lpszFileName)
{
	if (!_tcslen(lpszFolder))
	{
		_tcscpy_s(lpszFullPath, nSize, _T("\\"));
	}
	else if (lpszFolder[0] != _T('\\'))
	{
		_tcscpy_s(lpszFullPath, nSize, _T("\\"));
		_tcscat_s(lpszFullPath, nSize, lpszFolder);
	}
	else
	{
		_tcscpy_s(lpszFullPath, nSize, lpszFolder);
	}

	if (lpszFullPath[_tcslen(lpszFullPath) - 1] != _T('\\'))
	{
		_tcscat_s(lpszFullPath, nSize, _T("\\"));
	}

	_tcscat_s(lpszFullPath, nSize, lpszFileName);
	return (_tcslen(lpszFullPath));
}

BOOL CheckValidFileName(LPCTSTR lpszFilename)
{
	if (_tcschr(lpszFilename, _T('\\')))
	{
		return FALSE;
	}

	if (_tcschr(lpszFilename, _T('/')))
	{
		return FALSE;
	}

	if (_tcschr(lpszFilename, _T(':')))
	{
		return FALSE;
	}

	if (_tcschr(lpszFilename, _T('*')))
	{
		return FALSE;
	}

	if (_tcschr(lpszFilename, _T('?')))
	{
		return FALSE;
	}

	if (_tcschr(lpszFilename, _T('\"')))
	{
		return FALSE;
	}

	if (_tcschr(lpszFilename, _T('<')))
	{
		return FALSE;
	}

	if (_tcschr(lpszFilename, _T('>')))
	{
		return FALSE;
	}

	if (_tcschr(lpszFilename, _T('|')))
	{
		return FALSE;
	}

	return TRUE;
}

int ExtendSubPath(LPTSTR lpszFullPath, int nSize, LPCTSTR lpszBasePath, LPCTSTR lpszSubPath, BOOL bAddSeparator)
{
	int	nLength;

	if (!_tcslen(lpszBasePath))
	{
		_tcscpy_s(lpszFullPath, nSize, _T("\\"));
	}
	else if (lpszBasePath[0] != _T('\\'))
	{
		_tcscpy_s(lpszFullPath, nSize, _T("\\"));
		_tcscat_s(lpszFullPath, nSize, lpszBasePath);
	}
	else
	{
		_tcscpy_s(lpszFullPath, nSize, lpszBasePath);
	}

	nLength	= _tcslen(lpszFullPath);

	if (lpszFullPath[nLength - 1] == _T('\\'))
	{
		lpszFullPath[nLength - 1] = NULL;
	}

	if (lpszSubPath[0] != _T('\\'))
	{
		_tcscat_s(lpszFullPath, nSize, _T("\\"));
	}

	_tcscat_s(lpszFullPath, nSize, lpszSubPath);

	nLength	= _tcslen(lpszFullPath);

	if (lpszFullPath[nLength - 1] == _T('\\'))
	{
		lpszFullPath[nLength - 1] = NULL;
	}

	if (bAddSeparator)
	{
		_tcscat_s(lpszFullPath, nSize, _T("\\"));
	}

	return (_tcslen(lpszFullPath));
}

int ExtendSubPath(LPTSTR lpszBasePath, int nSize, LPCTSTR lpszSubPath, BOOL bAddSeparator)
{
	TCHAR	szFullPath[MAX_PATH + 1];
	int		nLength;

	nLength	= ExtendSubPath(szFullPath, MAX_PATH + 1, static_cast<LPCTSTR>(lpszBasePath), lpszSubPath, bAddSeparator);

	if (nLength <= (nSize - 1))
	{
		_tcscpy_s(lpszBasePath, nSize, szFullPath);
	}
	else
	{
		lpszBasePath[0]	= NULL;
	}

	return (_tcslen(lpszBasePath));
}

LPCTSTR TakeName(LPCTSTR lpszFullPath, LPTSTR lpszName, int nSize)
{
	LPCTSTR	lpszFound	= _tcsrchr(lpszFullPath, _T('\\'));
	int		nPos		= -1;

	if (lpszFound != NULL)
	{
		nPos	= static_cast<int>(lpszFound - lpszFullPath);
	}

	if (nPos >= 0)
	{
		_tcscpy_s(lpszName, nSize, lpszFound + 1);

		if (!_tcslen(lpszName))
		{
			_tcscpy_s(lpszName, nSize, _T("\\"));
		}
	}

	return static_cast<LPCTSTR>(lpszName);
}

LPCTSTR TakePath(LPCTSTR lpszFullPath, LPTSTR lpszPath, int nSize, BOOL bAddSeparator)
{
	LPCTSTR	lpszFound	= _tcsrchr(lpszFullPath, _T('\\'));
	int		nPos		= -1;

	if (lpszFound != NULL)
	{
		nPos	= static_cast<int>(lpszFound - lpszFullPath);
	}

	if (nPos > 0)
	{
		_tcsncpy_s(lpszPath, nSize, lpszFullPath, nPos);

		lpszPath[nPos]	= NULL;

		if (bAddSeparator)
		{
			_tcscat_s(lpszPath, nSize, _T("\\"));
		}
	}
	else if (nPos == 0)
	{
		_tcscpy_s(lpszPath, nSize, _T("\\"));
	}

	return static_cast<LPCTSTR>(lpszPath);
}

LPCTSTR TakeFileName(LPCTSTR lpszFullPath, LPTSTR lpszFileName, int nSize)
{
	LPCTSTR	lpszFound	= _tcsrchr(lpszFullPath, _T('\\'));
	int		nPos		= -1;

	if (lpszFound != NULL)
	{
		nPos	= static_cast<int>(lpszFound - lpszFullPath);
	}

	if (nPos >= 0)
	{
		_tcscpy_s(lpszFileName, nSize, lpszFound + 1);
	}

	return static_cast<LPCTSTR>(lpszFileName);
}

LPCTSTR TakeTitle(LPCTSTR lpszFullPath, LPTSTR lpszTitle, int nSize)
{
	TCHAR	szFileName[MAX_PATH + 1];

	if (TakeFileName(lpszFullPath, szFileName, MAX_PATH + 1))
	{
		LPTSTR	lpszFound	= _tcsrchr(szFileName, _T('.'));
		int		nPos		= -1;

		if (lpszFound != NULL)
		{
			nPos	= static_cast<int>(lpszFound - szFileName);
		}

		if (nPos > 0)
		{
			_tcsncpy_s(lpszTitle, nSize, szFileName, nPos);

			lpszTitle[nPos]	= NULL;
		}
	}

	return static_cast<LPCTSTR>(lpszTitle);
}

LPCTSTR TakeExtension(LPCTSTR lpszFullPath, LPTSTR lpszExt, int nSize)
{
	TCHAR	szFileName[MAX_PATH + 1];

	if (TakeFileName(lpszFullPath, szFileName, MAX_PATH + 1))
	{
		LPTSTR	lpszFound	= _tcsrchr(szFileName, _T('.'));
		int		nPos		= -1;

		if (lpszFound != NULL)
		{
			nPos	= static_cast<int>(lpszFound - szFileName);
		}

		if (nPos > 0)
		{
			_tcscpy_s(lpszExt, nSize, lpszFound + 1);
		}
	}

	return static_cast<LPCTSTR>(lpszExt);
}

BOOL GetFileStatusX(LPCTSTR lpszFileName, FILESTATUSX &rStatus)
{
	ASSERT(lpszFileName != NULL);

	if (lpszFileName == NULL)
	{
		return FALSE;
	}

	if (_tcslen(lpszFileName) >= _MAX_PATH)
	{
		return FALSE;
	}

	_tcscpy_s(rStatus.szFullName, MAX_PATH + 1, lpszFileName);

	WIN32_FIND_DATA	fd;
	HANDLE			hFind	= FindFirstFile(const_cast<LPTSTR>(lpszFileName), &fd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	VERIFY(FindClose(hFind));

	rStatus.dwAttributes	= fd.dwFileAttributes;
	rStatus.dwSizeLow		= fd.nFileSizeLow;
	rStatus.dwSizeHigh		= fd.nFileSizeHigh;

	if (IsValidFILETIMEX(fd.ftCreationTime))
	{
		FileTimeToSystemTime(&fd.ftCreationTime, &rStatus.stCreated);
	}
	else
	{
		GetLocalTime(&rStatus.stCreated);
	}

	if (IsValidFILETIMEX(fd.ftLastAccessTime))
	{
		FileTimeToSystemTime(&fd.ftLastAccessTime, &rStatus.stAccessed);
	}
	else
	{
		GetLocalTime(&rStatus.stAccessed);
	}

	if (IsValidFILETIMEX(fd.ftLastWriteTime))
	{
		FileTimeToSystemTime(&fd.ftLastWriteTime, &rStatus.stModified);
	}
	else
	{
		GetLocalTime(&rStatus.stModified);
	}

	return TRUE;
}

BOOL IsValidFILETIMEX(const FILETIME &ft)
{
	FILETIME	ftLocal;
	SYSTEMTIME	st;

	if (!FileTimeToLocalFileTime(&ft, &ftLocal))
	{
		return FALSE;
	}

	if (!FileTimeToSystemTime(&ftLocal, &st))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL IsFileExist(LPCTSTR lpszFileName)
{
	TCHAR	szFileName[MAX_PATH + 1];

	ASSERT(lpszFileName != NULL);

	if (lpszFileName == NULL)
	{
		return FALSE;
	}

	if (_tcslen(lpszFileName) >= _MAX_PATH)
	{
		return FALSE;
	}

	if (lpszFileName[0] != _T('\\'))
	{
		TCHAR	szWindows[MAX_PATH + 1];

		GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
		CombineFileName(szFileName, MAX_PATH + 1, szWindows, lpszFileName);
	}
	else
	{
		_tcscpy_s(szFileName, MAX_PATH + 1, lpszFileName);
	}

	WIN32_FIND_DATA	fd;
	HANDLE			hFind	= FindFirstFile(szFileName, &fd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	VERIFY(FindClose(hFind));
	return TRUE;
}

#define REGKEY_SHELL_RAI	_T("SOFTWARE\\Microsoft\\Shell\\Rai\\")
#define REGVAL_ITEM			_T("1")

BOOL ParseShortcutTarget(LPCTSTR lpszShortcut, LPTSTR lpszTarget, int cbTarget, LPTSTR lpszParameters, int cbParameters, LPTSTR lpszIcon, int cbIcon)
{
	TCHAR	szSource[MAX_PATH + 1];
	TCHAR	szTarget[MAX_PATH + 1];
	TCHAR	szSubKey[MAX_PATH + 1];
	TCHAR	szSTarget[MAX_PATH + 1];
	TCHAR	szSParameters[MAX_PATH + 1];
	TCHAR	szSIcon[MAX_PATH + 1];
	TCHAR	szBuffer[MAX_PATH + 1];
	HKEY	hKey;

	_tcscpy(szSTarget, _T(""));
	_tcscpy(szSParameters, _T(""));
	_tcscpy(szSIcon, _T(""));
	_tcscpy(szSource, lpszShortcut);

	if (SHGetShortcutTarget(szSource, szTarget, MAX_PATH))
	{
		HANDLE	hFile	= CreateFile(szSource, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			char	cBuffer[MAX_PATH + 1];
			DWORD	dwRead = 0;

			memset(cBuffer, 0, MAX_PATH + 1);
			ReadFile(hFile, cBuffer, 256, &dwRead, NULL);

			::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cBuffer, strlen(cBuffer), szBuffer, MAX_PATH);

			if (FindX(szBuffer, _T('\r')) != -1)
			{
				LeftX(szBuffer, FindX(szBuffer, _T('\r')));
			}

			if (FindX(szBuffer, _T('\n')) != -1)
			{
				LeftX(szBuffer, FindX(szBuffer, _T('\n')));
			}

			if (ReverseFindX(szBuffer, _T('?')) != -1)
			{
				MidX(szSIcon, MAX_PATH + 1, szBuffer, ReverseFindX(szBuffer, _T('?')) + 1);
				TrimX(szSIcon);

				if (FindX(szSIcon, _T('/')) != -1)
				{
					_tcscpy_s(szSIcon, MAX_PATH + 1, _T(""));
				}
			}

			CloseHandle(hFile);
		}

		while (szTarget[0] == _T(':'))
		{
			_tcscpy_s(szSTarget, MAX_PATH + 1, szTarget);
			TrimX(szSTarget);

			if (FindX(szSTarget, _T(' ')) != -1)
			{
				MidX(szBuffer, MAX_PATH + 1, szSTarget, FindX(szSTarget, _T(' ')) + 1);
				TrimX(szBuffer);

				if (_tcslen(szSParameters))
				{
					_tcscat_s(szBuffer, MAX_PATH + 1, _T(" "));
					_tcscat_s(szBuffer, MAX_PATH + 1, szSParameters);
				}

				_tcscpy_s(szSParameters, MAX_PATH + 1, szBuffer);
				LeftX(szSTarget, FindX(szSTarget, _T(' ')));
			}

			wsprintf(szSubKey, _T("%s%s"), REGKEY_SHELL_RAI, szSTarget);

			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
			{
				return FALSE;
			}

			DWORD	dwType	= REG_SZ;
			DWORD	dwSize	= MAX_PATH;

			RegQueryValueEx(hKey, REGVAL_ITEM, NULL, &dwType, reinterpret_cast<LPBYTE>(szTarget), &dwSize);
			RegCloseKey(hKey);
		}

		_tcscpy_s(szSTarget, MAX_PATH + 1, szTarget);
		TrimX(szSTarget);

		if (szSTarget[0] == _T('\"'))
		{
			MidX(szSubKey, MAX_PATH + 1, szSTarget, 1, (FindX(szSTarget, _T('\"'), 1) - 1));
			TrimX(szSubKey);

			if (FindX(szSTarget, _T('\"'), 1) < (int)(_tcslen(szSTarget) - 1))
			{
				MidX(szBuffer, MAX_PATH + 1, szSTarget, (FindX(szSTarget, _T('\"'), 1) + 1));
				TrimX(szBuffer);

				if (_tcslen(szSParameters))
				{
					_tcscat_s(szBuffer, MAX_PATH + 1, _T(" "));
					_tcscat_s(szBuffer, MAX_PATH + 1, szSParameters);
				}

				_tcscpy_s(szSParameters, MAX_PATH + 1, szBuffer);
			}
		}
		else
		{
			if (FindX(szSTarget, _T(' '), 1) > 0)
			{
				LeftX(szSubKey, MAX_PATH + 1, szSTarget, FindX(szSTarget, _T(' '), 1));

				if (FindX(szSTarget, _T(' '), 1) < static_cast<int>(_tcslen(szSTarget) - 1))
				{
					MidX(szBuffer, MAX_PATH + 1, szSTarget, FindX(szSTarget, _T(' '), 1));
					TrimX(szBuffer);

					if (_tcslen(szSParameters))
					{
						_tcscat_s(szBuffer, MAX_PATH + 1, _T(" "));
						_tcscat_s(szBuffer, MAX_PATH + 1, szSParameters);
					}

					_tcscpy_s(szSParameters, MAX_PATH + 1, szBuffer);
				}
			}
			else
			{
				_tcscpy_s(szSubKey, MAX_PATH + 1, szSTarget);
			}
		}

		TrimX(szSubKey);
		_tcscpy_s(szSTarget, MAX_PATH + 1, szSubKey);

		int	nPos	= FindX(szSTarget, _T(','));

		if (nPos != -1)
		{
			if (!_tcslen(szSIcon))
			{
				_tcscpy_s(szSIcon, MAX_PATH + 1, szSTarget);
			}

			LeftX(szSubKey, MAX_PATH + 1, szSTarget, nPos);
			_tcscpy_s(szSTarget, MAX_PATH + 1, szSubKey);
		}

		if (szSTarget[0] != _T('\\'))
		{
			TCHAR	szWindows[MAX_PATH + 1];

			GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
			wsprintf(szSTarget, _T("%s\\%s"), szWindows, szSubKey);
		}

		if (lpszTarget != NULL)
		{
			_tcscpy_s(lpszTarget, cbTarget, szSTarget);
		}

		if (lpszParameters != NULL)
		{
			TrimX(szSParameters);
			_tcscpy_s(lpszParameters, cbParameters, szSParameters);
		}

		if (lpszIcon != NULL)
		{
			_tcscpy_s(lpszIcon, cbIcon, szSIcon);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL ParseShortcutIcon(LPCTSTR lpszShortcut, LPTSTR lpszIcon, int cbIcon, BOOL bSelected)
{
	TCHAR	szTarget[MAX_PATH + 1];
	TCHAR	szBuffer[MAX_PATH + 1];
	TCHAR	szSubKey[MAX_PATH + 1];
	HKEY	hKey;
	HANDLE	hFile;

	if (ParseShortcutTarget(lpszShortcut, szTarget, MAX_PATH + 1, NULL, NULL, lpszIcon, cbIcon))
	{
		return TRUE;
	}

	hFile	= CreateFile(lpszShortcut, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		char	cBuffer[MAX_PATH + 1];
		DWORD	dwRead = 0;

		memset(cBuffer, 0, MAX_PATH + 1);
		ReadFile(hFile, cBuffer, 256, &dwRead, NULL);
		CloseHandle(hFile);
		
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, cBuffer, strlen(cBuffer), szBuffer, MAX_PATH);

		if (FindX(szBuffer, _T('\r')) != -1)
		{
			LeftX(szBuffer, FindX(szBuffer, _T('\r')));
		}

		if (FindX(szBuffer, _T('\n')) != -1)
		{
			LeftX(szBuffer, FindX(szBuffer, _T('\n')));
		}

		MidX(szBuffer, FindX(szBuffer, _T('#')) + 1);

		if (szBuffer[0] == _T(':'))
		{
			_tcscpy_s(szTarget, MAX_PATH + 1, szBuffer + 1);
			TrimX(szTarget);
		}

		wsprintf(szSubKey, _T("%s\\%s"), szTarget, (bSelected ? _T("SelectIcon") : _T("DefaultIcon")));

		if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szSubKey, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		{
			return FALSE;
		}

		DWORD	dwType	= REG_SZ;
		DWORD	dwSize	= MAX_PATH;

		RegQueryValueEx(hKey, NULL, NULL, &dwType, reinterpret_cast<LPBYTE>(szBuffer), &dwSize);
		RegCloseKey(hKey);
		
		_tcscpy_s(szTarget, MAX_PATH + 1, szBuffer);
		TrimX(szTarget);

		if (szTarget[0] == _T('\"'))
		{
			MidX(szSubKey, MAX_PATH + 1, szTarget, 1, (FindX(szTarget, _T('\"'), 1) - 1));
			TrimX(szSubKey);
			_tcscpy_s(szTarget, MAX_PATH + 1, szSubKey);
		}
		else
		{
			_tcscpy_s(szSubKey, MAX_PATH + 1, szTarget);
		}

		if (szTarget[0] != _T('\\'))
		{
			TCHAR	szWindows[MAX_PATH + 1];

			GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
			wsprintf(szTarget, _T("%s\\%s"), szWindows, szSubKey);
		}

		if (lpszIcon != NULL)
		{
			_tcscpy_s(lpszIcon, cbIcon, szTarget);
		}
		
		return TRUE;
	}
	
	return FALSE;
}

BOOL ParseIconLink(LPCTSTR lpszLink, LPTSTR lpszIcon, int cbIcon, int* pnIconID)
{
	TCHAR	szTarget[MAX_PATH + 1];
	TCHAR	szBuffer[MAX_PATH + 1];
	int		nPos;
	BOOL	bValid	= FALSE;

	_tcscpy_s(szTarget, MAX_PATH + 1, lpszLink);

	if (FindX(szTarget, _T('\r')) != -1)
	{
		LeftX(szTarget, FindX(szTarget, _T('\r')));
	}

	if (FindX(szTarget, _T('\n')) != -1)
	{
		LeftX(szTarget, FindX(szTarget, _T('\n')));
	}

	TrimX(szTarget);

	if (szTarget[0] == _T('\"'))
	{
		MidX(szBuffer, MAX_PATH + 1, szTarget, 1, (FindX(szTarget, _T('\"'), 1) - 1));
		TrimX(szBuffer);
		_tcscpy_s(szTarget, MAX_PATH + 1, szBuffer);
	}
	else
	{
		_tcscpy_s(szBuffer, MAX_PATH + 1, szTarget);
	}

	if (szTarget[0] != _T('\\'))
	{
		TCHAR	szWindows[MAX_PATH + 1];

		GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
		wsprintf(szTarget, _T("%s\\%s"), szWindows, szBuffer);
	}

	nPos	= FindX(szBuffer, _T(','));

	if (nPos != -1)
	{
		LeftX(szTarget, nPos);
		MidX(szBuffer, nPos + 1);
	}

	if (lpszIcon != NULL)
	{
		_tcscpy_s(lpszIcon, cbIcon, szTarget);
	}

	if (pnIconID != NULL)
	{
		*pnIconID	= _ttoi(szBuffer);
	}
	
	return TRUE;
}
