// CleanSearch.cpp

#include "StdAfx.h"
#include "InClean.h"
#include "CleanSearch.h"

CCleanSearch::CCleanSearch(void)
{
	Reset();
}

CCleanSearch::~CCleanSearch(void)
{
	Reset();
}

void CCleanSearch::Reset(void)
{
	ResetCount();
	m_arrFiles.RemoveAll();
	m_arrFolders.RemoveAll();
}

BOOL CCleanSearch::FoundAction(LPSEARCHINFOX lpSIX)
{
	if ((lpSIX->dwAttributes & FILE_ATTRIBUTE_INROM) || (lpSIX->dwAttributes & FILE_ATTRIBUTE_ROMSTATICREF) || (lpSIX->dwAttributes & FILE_ATTRIBUTE_ROMMODULE))
	{
		return FALSE;
	}

	m_arrFiles.Add(*lpSIX);
	return TRUE;
}

BOOL CCleanSearch::FolderAction(LPSEARCHINFOX lpSIX, BOOL bInclude)
{
	if (bInclude)
	{
		m_arrFolders.Add(*lpSIX);
	}

	return TRUE;
}
