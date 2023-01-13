// SearchX.h

#pragma once

#include "InAPI.h"

#ifndef INVALID_FILESIZE
#define INVALID_FILESIZE	0xFFFFFFFF
#endif

typedef struct
{
	int				nFoundID;
	TCHAR			szFullPath[MAX_PATH + 1];
	ULARGE_INTEGER	uiSize;
	DWORD			dwAttributes;
	FILETIME		ftCreationTime;
	FILETIME		ftLastAccessTime;
	FILETIME		ftLastWriteTime;
	DWORD			dwOID;
} SEARCHINFOX, FAR* LPSEARCHINFOX;

class INAPI_CLASS CSearchX
{
public:
	CSearchX(void);
	virtual	~CSearchX(void);

	void	SetPartName(LPCTSTR lpszPartName = NULL);

	void	SetSearchSubFolders(BOOL bSearch = TRUE);
	void	SetIncludeFolders(BOOL bInclude = TRUE);

	void	SetSearchDate(LPSYSTEMTIME stFrom = NULL, LPSYSTEMTIME stTo = NULL);
	void	SetSearchDateFrom(LPSYSTEMTIME stFrom);
	void	SetSearchDateTo(LPSYSTEMTIME stTo);

	void	SetSearchSize(DWORD	dwFrom = INVALID_FILESIZE, DWORD dwTo = INVALID_FILESIZE);
	void	SetSearchSizeFrom(DWORD dwFrom = INVALID_FILESIZE);
	void	SetSearchSizeTo(DWORD dwTo = INVALID_FILESIZE);

	int	DoSearch(void);

	int	GetFoundItems(void);
	int	GetCompleted(void);
	int	GetFoundBoth(void);
	int	GetFoundFiles(void);
	int	GetFoundFolders(void);

	ULARGE_INTEGER	GetTotalSize(void);

	virtual BOOL	FoundAction(LPSEARCHINFOX lpSIX);
	virtual BOOL	FolderAction(LPSEARCHINFOX lpSIX, BOOL bInclude);

	int	GetFoundID(LPSEARCHINFOX lpSIX);

	LPCTSTR	GetFullPath(LPSEARCHINFOX lpSIX);

#ifndef USE_ULINT
	DWORD			GetFileSize(LPSEARCHINFOX lpSIX);
#else
	ULARGE_INTEGER	GetFileSize(LPSEARCHINFOX lpSIX);
#endif

	DWORD	GetAttributes(LPSEARCHINFOX lpSIX);
	BOOL	IsNormal(LPSEARCHINFOX lpSIX);
	BOOL	IsFolder(LPSEARCHINFOX lpSIX);
	BOOL	IsReadOnly(LPSEARCHINFOX lpSIX);
	BOOL	IsHidden(LPSEARCHINFOX lpSIX);
	BOOL	IsArchive(LPSEARCHINFOX lpSIX);
	BOOL	IsSystem(LPSEARCHINFOX lpSIX);
	BOOL	IsInROM(LPSEARCHINFOX lpSIX);
	BOOL	IsROMModule(LPSEARCHINFOX lpSIX);

	FILETIME	GetCreationTime(LPSEARCHINFOX lpSIX);
	FILETIME	GetAccessedTime(LPSEARCHINFOX lpSIX);
	FILETIME	GetModifiedTime(LPSEARCHINFOX lpSIX);

	DWORD	GetObjectID(LPSEARCHINFOX lpSIX);

protected:
	void	ResetCount(void);
	BOOL	DoSearchInternal(LPCTSTR lpszPartName);

	int		m_nCompleted;
	int		m_nFoundBoth;
	int		m_nFoundFilesOnly;
	int		m_nFoundFolders;

	ULARGE_INTEGER	m_uiTotalSize;

	TCHAR	m_szPartName[MAX_PATH + 1];
	BOOL	m_bSearchSubFolders;
	BOOL	m_bIncludeFolders;

	SYSTEMTIME	m_stFrom;
	SYSTEMTIME	m_stTo;
	BOOL		m_bTimeFrom;
	BOOL		m_bTimeTo;
	DWORD		m_dwSizeFrom;
	DWORD		m_dwSizeTo;
};
