// ExPackX.cpp

#include "StdAfx.h"
#include "ExPackX.h"
#include "ShellX.h"

CExPackX::CExPackX(void)
{
	InitX();
}

CExPackX::CExPackX(LPCTSTR lpszModule, LPCTSTR lpszPacked)
{
	InitX();
	Load(lpszModule, lpszPacked);
}

CExPackX::~CExPackX(void)
{
	Unload();
}

void CExPackX::SetPackFolder(LPCTSTR lpszFolder)
{
	wcscpy_s(m_szPackFolder, MAX_PATH + 1, lpszFolder);
}

void CExPackX::SetPackFolder(INT nSpecialFolderID)
{
	GetSpecialDirectoryEx(nSpecialFolderID, m_szPackFolder, MAX_PATH + 1);
}

HINSTANCE CExPackX::Load(LPCTSTR lpszModule, LPCTSTR lpszPacked)
{
	TCHAR	szFullModule[MAX_PATH + 1];
	TCHAR	szFullPacked[MAX_PATH + 1];

	CombineFileName(szFullModule, MAX_PATH + 1, m_szPackFolder, lpszModule);
	CombineFileName(szFullPacked, MAX_PATH + 1, m_szPackFolder, lpszPacked);

	if (IsFileExist(szFullPacked))
	{
		DeleteFile(szFullModule);
		MoveFile(szFullPacked, szFullModule);
	}

	m_hInstance	= LoadLibrary(szFullModule);

	return m_hInstance;
}

void CExPackX::Unload(void)
{
	if (m_hInstance != NULL)
	{
		FreeLibrary(m_hInstance);

		m_hInstance	= NULL;
	}
}

HINSTANCE CExPackX::PackInstance(void)
{
	return m_hInstance;
}

BOOL CExPackX::IsLoaded(void)
{
	return static_cast<BOOL>(m_hInstance != NULL);
}

void CExPackX::InitX(void)
{
	m_hInstance	= NULL;

	GetSpecialDirectoryEx(CSIDL_WINDOWS, m_szPackFolder, MAX_PATH + 1);
}
