// FileInfoX.h

#pragma once

#include "InAPI.h"

class INAPI_CLASS CFileInfoX
{
public:
	CFileInfoX(void);
	CFileInfoX(LPCTSTR lpszFileName);
	virtual	~CFileInfoX(void);

	void	SetFile(LPCTSTR lpszFileName);

	LPCTSTR	GetDisplayName(void);
	LPCTSTR	GetTypeName(void);

	DWORD	GetSize(void);

	HICON	GetSmallIcon(void);
	HICON	GetLargeIcon(void);

	DWORD	GetAttributes(void);
	BOOL	IsNormal(void);
	BOOL	IsFolder(void);
	BOOL	IsReadOnly(void);
	BOOL	IsHidden(void);
	BOOL	IsArchive(void);
	BOOL	IsSystem(void);
	BOOL	IsInROM(void);
	BOOL	IsROMModule(void);

	void	GetCreatedTime(SYSTEMTIME& stCreated);
	void	GetModifiedTime(SYSTEMTIME& stModified);
	void	GetAccessedTime(SYSTEMTIME& stAccessed);

protected:
	void	Clear(void);

	BOOL	m_bLoaded;
	DWORD	m_dwSize;
	HICON	m_hLargeIcon;
	HICON	m_hSmallIcon;
	DWORD	m_dwAttributes;

	SHFILEINFO	m_SFI;
	SYSTEMTIME	m_stCreated;
	SYSTEMTIME	m_stModified;
	SYSTEMTIME	m_stAccessed;
};
