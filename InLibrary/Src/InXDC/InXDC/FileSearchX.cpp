// FileSearchX.cpp

#include "StdAfx.h"
#include "FileSearchX.h"

CFileSearchX::CFileSearchX(void)
{
}

CFileSearchX::~CFileSearchX(void)
{
	m_arrFiles.RemoveAll();
	m_arrFolders.RemoveAll();
}

BOOL CFileSearchX::FoundAction(LPSEARCHINFOX lpSIX)
{
	m_arrFiles.Add(*lpSIX);
	return TRUE;
}

BOOL CFileSearchX::FolderAction(LPSEARCHINFOX lpSIX, BOOL bInclude)
{
	if (bInclude == TRUE)
	{
		m_arrFolders.Add(*lpSIX);
	}

	return TRUE;
}
