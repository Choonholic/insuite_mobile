// SearchX.cpp

#include "StdAfx.h"
#include "SearchX.h"
#include "ShellX.h"
#include "StringX.h"

CSearchX::CSearchX(void)
{
	SetPartName();
	SetSearchSubFolders();
	SetIncludeFolders();
	SetSearchDate();
	SetSearchSize();
	ResetCount();
}

CSearchX::~CSearchX(void)
{
}

void CSearchX::SetPartName(LPCTSTR lpszPartName)
{
	_tcscpy_s(m_szPartName, MAX_PATH + 1, ((lpszPartName != NULL) ? lpszPartName : _T("\\*.*")));
}

void CSearchX::SetSearchSubFolders(BOOL bSearch)
{
	m_bSearchSubFolders	= bSearch;
}

void CSearchX::SetIncludeFolders(BOOL bInclude)
{
	m_bIncludeFolders	= bInclude;
}

void CSearchX::SetSearchDate(LPSYSTEMTIME stFrom, LPSYSTEMTIME stTo)
{
	if (stFrom == NULL)
	{
		m_bTimeFrom	= FALSE;

		memset(&m_stFrom, 0, sizeof(SYSTEMTIME));
	}
	else
	{
		m_bTimeFrom	= TRUE;

		memcpy_s(&m_stFrom, sizeof(SYSTEMTIME), stFrom, sizeof(SYSTEMTIME));
	}

	if (stTo == NULL)
	{
		m_bTimeTo	= FALSE;

		memset(&m_stTo, 0, sizeof(SYSTEMTIME));
	}
	else
	{
		m_bTimeTo	= TRUE;

		memcpy_s(&m_stTo, sizeof(SYSTEMTIME), stTo, sizeof(SYSTEMTIME));
	}

	if (m_bTimeFrom && m_bTimeTo)
	{
		FILETIME	ftFrom;
		FILETIME	ftTo;

		SystemTimeToFileTime(&m_stFrom, &ftFrom);
		SystemTimeToFileTime(&m_stTo, &ftTo);

		if (CompareFileTime(&ftFrom, &ftTo) > 0)
		{
			SYSTEMTIME	st;

			memcpy_s(&st, sizeof(SYSTEMTIME), &m_stFrom, sizeof(SYSTEMTIME));
			memcpy_s(&m_stFrom, sizeof(SYSTEMTIME), &m_stTo, sizeof(SYSTEMTIME));
			memcpy_s(&m_stTo, sizeof(SYSTEMTIME), &st, sizeof(SYSTEMTIME));
		}
	}
}

void CSearchX::SetSearchDateFrom(SYSTEMTIME *stFrom)
{
	m_bTimeTo	= FALSE;

	memset(&m_stTo, 0, sizeof(SYSTEMTIME));

	if (stFrom == NULL)
	{
		m_bTimeFrom	= FALSE;

		memset(&m_stFrom, 0, sizeof(SYSTEMTIME));
	}
	else
	{
		m_bTimeFrom	= TRUE;

		memcpy_s(&m_stFrom, sizeof(SYSTEMTIME), stFrom, sizeof(SYSTEMTIME));
	}
}

void CSearchX::SetSearchDateTo(SYSTEMTIME *stTo)
{
	m_bTimeFrom	= FALSE;

	memset(&m_stFrom, 0, sizeof(SYSTEMTIME));

	if (stTo == NULL)
	{
		m_bTimeTo	= FALSE;

		memset(&m_stTo, 0, sizeof(SYSTEMTIME));
	}
	else
	{
		m_bTimeTo	= TRUE;

		memcpy_s(&m_stTo, sizeof(SYSTEMTIME), stTo, sizeof(SYSTEMTIME));
	}
}

void CSearchX::SetSearchSize(DWORD dwFrom, DWORD dwTo)
{
	m_dwSizeFrom	= dwFrom;
	m_dwSizeTo		= dwTo;
}

void CSearchX::SetSearchSizeFrom(DWORD dwFrom)
{
	m_dwSizeFrom	= dwFrom;
	m_dwSizeTo		= INVALID_FILESIZE;
}

void CSearchX::SetSearchSizeTo(DWORD dwTo)
{
	m_dwSizeFrom	= INVALID_FILESIZE;
	m_dwSizeTo		= dwTo;
}

int CSearchX::DoSearch(void)
{
	ResetCount();
	DoSearchInternal(m_szPartName);
	return m_nCompleted;
}

ULARGE_INTEGER CSearchX::GetTotalSize(void)
{
	return m_uiTotalSize;
}

int CSearchX::GetFoundItems(void)
{
	return (m_nFoundBoth + m_nFoundFolders);
}

int CSearchX::GetCompleted(void)
{
	return m_nCompleted;
}

int CSearchX::GetFoundBoth(void)
{
	return m_nFoundBoth;
}

int CSearchX::GetFoundFiles(void)
{
	return m_nFoundFilesOnly;
}

int CSearchX::GetFoundFolders(void)
{
	return m_nFoundFolders;
}

BOOL CSearchX::FoundAction(LPSEARCHINFOX lpSIX)
{
	return FALSE;
}

BOOL CSearchX::FolderAction(LPSEARCHINFOX lpSIX, BOOL bInclude)
{
	return FALSE;
}

int CSearchX::GetFoundID(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? lpSIX->nFoundID : -1);
}

LPCTSTR CSearchX::GetFullPath(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? (LPCTSTR)lpSIX->szFullPath : NULL);
}

#ifndef USE_ULINT
DWORD CSearchX::GetFileSize(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? (DWORD)lpSIX->uiSize.LowPart : 0);
}
#else
ULARGE_INTEGER CSearchX::GetFileSize(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? lpSIX->uiSize : 0);
}
#endif

DWORD CSearchX::GetAttributes(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? lpSIX->dwAttributes : 0);
}

BOOL CSearchX::IsNormal(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? static_cast<BOOL>(lpSIX->dwAttributes == FILE_ATTRIBUTE_NORMAL) : 0);
}

BOOL CSearchX::IsFolder(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? static_cast<BOOL>(lpSIX->dwAttributes & FILE_ATTRIBUTE_DIRECTORY) : 0);
}

BOOL CSearchX::IsReadOnly(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? static_cast<BOOL>(lpSIX->dwAttributes & FILE_ATTRIBUTE_READONLY) : 0);
}

BOOL CSearchX::IsHidden(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? static_cast<BOOL>(lpSIX->dwAttributes & FILE_ATTRIBUTE_HIDDEN) : 0);
}

BOOL CSearchX::IsArchive(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? static_cast<BOOL>(lpSIX->dwAttributes & FILE_ATTRIBUTE_ARCHIVE) : 0);
}

BOOL CSearchX::IsSystem(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? static_cast<BOOL>(lpSIX->dwAttributes & FILE_ATTRIBUTE_SYSTEM) : 0);
}

BOOL CSearchX::IsInROM(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? static_cast<BOOL>(lpSIX->dwAttributes & FILE_ATTRIBUTE_INROM) : 0);
}

BOOL CSearchX::IsROMModule(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? static_cast<BOOL>(lpSIX->dwAttributes & FILE_ATTRIBUTE_ROMMODULE) : 0);
}

FILETIME CSearchX::GetCreationTime(LPSEARCHINFOX lpSIX)
{
	FILETIME	ftNULL;

	memset(&ftNULL, 0, sizeof(FILETIME));
	return (lpSIX ? lpSIX->ftCreationTime : ftNULL);
}

FILETIME CSearchX::GetAccessedTime(LPSEARCHINFOX lpSIX)
{
	FILETIME	ftNULL;

	memset(&ftNULL, 0, sizeof(FILETIME));
	return (lpSIX ? lpSIX->ftLastAccessTime : ftNULL);
}

FILETIME CSearchX::GetModifiedTime(LPSEARCHINFOX lpSIX)
{
	FILETIME	ftNULL;

	memset(&ftNULL, 0, sizeof(FILETIME));
	return (lpSIX ? lpSIX->ftLastWriteTime : ftNULL);
}

DWORD CSearchX::GetObjectID(LPSEARCHINFOX lpSIX)
{
	return (lpSIX ? lpSIX->dwOID : 0);
}

void CSearchX::ResetCount()
{
	m_nCompleted			= 0;
	m_nFoundBoth			= 0;
	m_nFoundFilesOnly		= 0;
	m_nFoundFolders			= 0;
	m_uiTotalSize.QuadPart	= 0;
}

BOOL CSearchX::DoSearchInternal(LPCTSTR lpszPartName)
{
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	TCHAR			szDir[MAX_PATH + 1];
	TCHAR			szNew[MAX_PATH + 1];
	TCHAR			szSpec[MAX_PATH + 1];
	TCHAR			szFolder[MAX_PATH + 1];
	int				nSep;
	INT				rc;
	SEARCHINFOX		SIX;
	ULARGE_INTEGER	uiSize;

	// Separate subfolder from search specification.
	_tcscpy_s(szDir, MAX_PATH + 1, lpszPartName);

	nSep	= ReverseFindX(szDir, _T('\\'));

	if (nSep < 0)
	{
		_tcscpy_s(szNew, MAX_PATH + 1, _T("\\"));
		_tcscpy_s(szSpec, MAX_PATH + 1, szDir);
	}
	else
	{
		LeftX(szNew, MAX_PATH + 1, szDir, nSep + 1);
		MidX(szSpec, MAX_PATH + 1, szDir, nSep);
	}

	// Find Matching Files
	hFind	= FindFirstFile(szDir, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				++m_nFoundBoth;

				if (m_bIncludeFolders == TRUE)
				{
					uiSize.LowPart			= fd.nFileSizeLow;
					uiSize.HighPart			= fd.nFileSizeHigh;
					m_uiTotalSize.QuadPart	+= uiSize.QuadPart;

					SIX.nFoundID		= m_nFoundBoth;
					SIX.uiSize.LowPart	= fd.nFileSizeLow;
					SIX.uiSize.HighPart	= fd.nFileSizeHigh;
					SIX.dwAttributes	= fd.dwFileAttributes;
					SIX.dwOID			= fd.dwOID;

					_tcscpy_s(SIX.szFullPath, MAX_PATH + 1, szNew);
					_tcscat_s(SIX.szFullPath, MAX_PATH + 1, fd.cFileName);
					memcpy_s(&SIX.ftCreationTime, sizeof(FILETIME), &fd.ftCreationTime, sizeof(FILETIME));
					memcpy_s(&SIX.ftLastAccessTime, sizeof(FILETIME), &fd.ftLastAccessTime, sizeof(FILETIME));
					memcpy_s(&SIX.ftLastWriteTime, sizeof(FILETIME), &fd.ftLastWriteTime, sizeof(FILETIME));

					if (FoundAction(&SIX))
					{
						++m_nCompleted;
					}
				}
			}
			else
			{
				BOOL	bValid	= TRUE;

				if (m_bTimeFrom)
				{
					FILETIME	ftFrom;

					SystemTimeToFileTime(&m_stFrom, &ftFrom);

					if (CompareFileTime(&ftFrom, &fd.ftCreationTime) > 0)
					{
						bValid	= FALSE;
					}
				}

				if (m_bTimeTo)
				{
					FILETIME	ftTo;

					SystemTimeToFileTime(&m_stTo, &ftTo);

					if (CompareFileTime(&ftTo, &fd.ftCreationTime) < 0)
					{
						bValid	= FALSE;
					}
				}

				if (m_dwSizeFrom != INVALID_FILESIZE)
				{
					if (fd.nFileSizeLow < m_dwSizeFrom)
					{
						bValid	= FALSE;
					}
				}

				if (m_dwSizeTo != INVALID_FILESIZE)
				{
					if (fd.nFileSizeLow > m_dwSizeTo)
					{
						bValid	= FALSE;
					}
				}

				if (bValid)
				{
					++m_nFoundBoth;
					++m_nFoundFilesOnly;

					uiSize.LowPart			= fd.nFileSizeLow;
					uiSize.HighPart			= fd.nFileSizeHigh;
					m_uiTotalSize.QuadPart	+= uiSize.QuadPart;

					SIX.nFoundID		= m_nFoundBoth;
					SIX.uiSize.LowPart	= fd.nFileSizeLow;
					SIX.uiSize.HighPart	= fd.nFileSizeHigh;
					SIX.dwAttributes	= fd.dwFileAttributes;
					SIX.dwOID			= fd.dwOID;

					_tcscpy_s(SIX.szFullPath, MAX_PATH + 1, szNew);
					_tcscat_s(SIX.szFullPath, MAX_PATH + 1, fd.cFileName);
					memcpy_s(&SIX.ftCreationTime, sizeof(FILETIME), &fd.ftCreationTime, sizeof(FILETIME));
					memcpy_s(&SIX.ftLastAccessTime, sizeof(FILETIME), &fd.ftLastAccessTime, sizeof(FILETIME));
					memcpy_s(&SIX.ftLastWriteTime, sizeof(FILETIME), &fd.ftLastWriteTime, sizeof(FILETIME));

					if (FoundAction(&SIX))
					{
						++m_nCompleted;
					}
				}
			}

			rc	= FindNextFile(hFind, &fd);
		} while (rc);

		FindClose(hFind);
	}
	else
	{
		rc	= GetLastError();

		if ((rc != ERROR_FILE_NOT_FOUND) && (rc != ERROR_NO_MORE_FILES))
		{
			return FALSE;
		}
	}

	// Exit from here if do not search sub folders.
	if (!m_bSearchSubFolders)
	{
		return TRUE;
	}

	// Create generic search string for all folders.
	wsprintf(szFolder, _T("%s*.*"), szNew);

	// Search all sub folders.
	hFind	= FindFirstFile(szFolder, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				++m_nFoundFolders;

				uiSize.LowPart			= fd.nFileSizeLow;
				uiSize.HighPart			= fd.nFileSizeHigh;
				m_uiTotalSize.QuadPart	+= uiSize.QuadPart;

				SIX.nFoundID		= 0;
				SIX.uiSize.LowPart	= fd.nFileSizeLow;
				SIX.uiSize.HighPart	= fd.nFileSizeHigh;
				SIX.dwAttributes	= fd.dwFileAttributes;
				SIX.dwOID			= fd.dwOID;

				_tcscpy_s(SIX.szFullPath, MAX_PATH + 1, szNew);
				_tcscat_s(SIX.szFullPath, MAX_PATH + 1, fd.cFileName);
				memcpy_s(&SIX.ftCreationTime, sizeof(FILETIME), &fd.ftCreationTime, sizeof(FILETIME));
				memcpy_s(&SIX.ftLastAccessTime, sizeof(FILETIME), &fd.ftLastAccessTime, sizeof(FILETIME));
				memcpy_s(&SIX.ftLastWriteTime, sizeof(FILETIME), &fd.ftLastWriteTime, sizeof(FILETIME));

				FolderAction(&SIX, m_bIncludeFolders);

				wsprintf(szFolder, _T("%s%s%s"), szNew, fd.cFileName, szSpec);

				DoSearchInternal(szFolder);
			}

			rc	= FindNextFile(hFind, &fd);
		} while (rc);

		FindClose(hFind);
	}
	else
	{
		rc	= GetLastError();

		if ((rc != ERROR_FILE_NOT_FOUND) && (rc != ERROR_NO_MORE_FILES))
		{
			return FALSE;
		}
	}

	return TRUE;
}
