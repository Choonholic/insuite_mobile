// FileInfoX.cpp

#include "StdAfx.h"
#include "FileInfoX.h"
#include "ShellX.h"

CFileInfoX::CFileInfoX(void)
{
	Clear();
}

CFileInfoX::CFileInfoX(LPCTSTR lpszFileName)
{
	SetFile(lpszFileName);
}

CFileInfoX::~CFileInfoX(void)
{
	Clear();
}

void CFileInfoX::SetFile(LPCTSTR lpszFileName)
{
	FILESTATUSX	fsx;

	Clear();

	if (GetFileStatusX(lpszFileName, fsx))
	{
		m_bLoaded		= TRUE;
		m_dwSize		= fsx.dwSizeLow;
		m_dwAttributes	= fsx.dwAttributes;

		memcpy_s(&m_stCreated, sizeof(SYSTEMTIME), &fsx.stCreated, sizeof(SYSTEMTIME));
		memcpy_s(&m_stModified, sizeof(SYSTEMTIME), &fsx.stModified, sizeof(SYSTEMTIME));
		memcpy_s(&m_stAccessed, sizeof(SYSTEMTIME), &fsx.stAccessed, sizeof(SYSTEMTIME));
		SHGetFileInfo(lpszFileName, 0, &m_SFI, sizeof(SHFILEINFO), SHGFI_DISPLAYNAME | SHGFI_TYPENAME);

		m_hLargeIcon	= GetFileIcon(lpszFileName, TRUE);
		m_hSmallIcon	= GetFileIcon(lpszFileName, FALSE);
	}
}

LPCTSTR CFileInfoX::GetDisplayName(void)
{
	if (m_bLoaded == FALSE)
	{
		return _T("");
	}

	return static_cast<LPCTSTR>(m_SFI.szDisplayName);
}

LPCTSTR CFileInfoX::GetTypeName(void)
{
	if (m_bLoaded == FALSE)
	{
		return _T("");
	}

	return static_cast<LPCTSTR>(m_SFI.szTypeName);
}

DWORD CFileInfoX::GetSize(void)
{
	if (m_bLoaded == FALSE)
	{
		return 0;
	}

	return m_dwSize;
}

HICON CFileInfoX::GetSmallIcon(void)
{
	if (m_bLoaded == FALSE)
	{
		return NULL;
	}

	return m_hSmallIcon;
}

HICON CFileInfoX::GetLargeIcon(void)
{
	if (m_bLoaded == FALSE)
	{
		return NULL;
	}

	return m_hLargeIcon;
}

DWORD CFileInfoX::GetAttributes(void)
{
	if (m_bLoaded == FALSE)
	{
		return 0;
	}

	return m_dwAttributes;
}

BOOL CFileInfoX::IsNormal(void)
{
	if (m_bLoaded == FALSE)
	{
		return FALSE;
	}

	return static_cast<BOOL>(m_dwAttributes == FILE_ATTRIBUTE_NORMAL);
}

BOOL CFileInfoX::IsFolder(void)
{
	if (m_bLoaded == FALSE)
	{
		return FALSE;
	}

	return static_cast<BOOL>(m_dwAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL CFileInfoX::IsReadOnly(void)
{
	if (m_bLoaded == FALSE)
	{
		return FALSE;
	}

	return static_cast<BOOL>(m_dwAttributes & FILE_ATTRIBUTE_READONLY);
}

BOOL CFileInfoX::IsHidden(void)
{
	if (m_bLoaded == FALSE)
	{
		return FALSE;
	}

	return static_cast<BOOL>(m_dwAttributes & FILE_ATTRIBUTE_HIDDEN);
}

BOOL CFileInfoX::IsArchive(void)
{
	if (m_bLoaded == FALSE)
	{
		return FALSE;
	}

	return static_cast<BOOL>(m_dwAttributes & FILE_ATTRIBUTE_ARCHIVE);
}

BOOL CFileInfoX::IsSystem(void)
{
	if (m_bLoaded == FALSE)
	{
		return FALSE;
	}

	return static_cast<BOOL>(m_dwAttributes & FILE_ATTRIBUTE_SYSTEM);
}

BOOL CFileInfoX::IsInROM(void)
{
	if (m_bLoaded == FALSE)
	{
		return FALSE;
	}

	return static_cast<BOOL>(m_dwAttributes & FILE_ATTRIBUTE_INROM);
}

BOOL CFileInfoX::IsROMModule(void)
{
	if (m_bLoaded == FALSE)
	{
		return FALSE;
	}

	return static_cast<BOOL>(m_dwAttributes & FILE_ATTRIBUTE_ROMMODULE);
}

void CFileInfoX::GetCreatedTime(SYSTEMTIME& stCreated)
{
	if (m_bLoaded == FALSE)
	{
		memset(&stCreated, 0, sizeof(SYSTEMTIME));
	}

	memcpy_s(&stCreated, sizeof(SYSTEMTIME), &m_stCreated, sizeof(SYSTEMTIME));
}

void CFileInfoX::GetModifiedTime(SYSTEMTIME& stModified)
{
	if (m_bLoaded == FALSE)
	{
		memset(&stModified, 0, sizeof(SYSTEMTIME));
	}

	memcpy_s(&stModified, sizeof(SYSTEMTIME), &m_stModified, sizeof(SYSTEMTIME));
}

void CFileInfoX::GetAccessedTime(SYSTEMTIME& stAccessed)
{
	if (m_bLoaded == FALSE)
	{
		memset(&stAccessed, 0, sizeof(SYSTEMTIME));
	}

	memcpy_s(&stAccessed, sizeof(SYSTEMTIME), &m_stAccessed, sizeof(SYSTEMTIME));
}

void CFileInfoX::Clear()
{
	memset(&m_SFI, 0, sizeof(SHFILEINFO));

	if (m_hLargeIcon)
	{
		DestroyIcon(m_hLargeIcon);
	}

	if (m_hSmallIcon)
	{
		DestroyIcon(m_hSmallIcon);
	}

	m_bLoaded		= FALSE;
	m_dwSize		= 0;
	m_dwAttributes	= 0;

	memset(&m_stCreated, 0, sizeof(SYSTEMTIME));
	memset(&m_stModified, 0, sizeof(SYSTEMTIME));
	memset(&m_stAccessed, 0, sizeof(SYSTEMTIME));
}
