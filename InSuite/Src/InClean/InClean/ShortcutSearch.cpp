// ShortcutSearch.cpp

#include "StdAfx.h"
#include "InClean.h"
#include "ShortcutSearch.h"
#include "ShellX.h"
#include "StringX.h"

CShortcutSearch::CShortcutSearch(void)
{
	Reset();
}

CShortcutSearch::~CShortcutSearch(void)
{
	Reset();
}

void CShortcutSearch::Reset(void)
{
	ResetCount();
	m_arrFiles.RemoveAll();
}

BOOL CShortcutSearch::FoundAction(LPSEARCHINFOX lpSIX)
{
	TCHAR	szShortcut[MAX_PATH + 1];
	TCHAR	szIcon[MAX_PATH + 1];
	TCHAR	szIconLink[MAX_PATH + 1];
	BOOL	bShortcut;
	BOOL	bIcon;

	if ((lpSIX->dwAttributes & FILE_ATTRIBUTE_INROM) || (lpSIX->dwAttributes & FILE_ATTRIBUTE_ROMSTATICREF) || (lpSIX->dwAttributes & FILE_ATTRIBUTE_ROMMODULE))
	{
		return FALSE;
	}

	bShortcut	= ParseShortcutTarget(lpSIX->szFullPath, szShortcut, MAX_PATH + 1);
	bIcon		= ParseShortcutIcon(lpSIX->szFullPath, szIcon, MAX_PATH + 1, FALSE);
	
	if (bShortcut)
	{
		if (!IsFileExist(szShortcut))
		{
			m_arrFiles.Add(*lpSIX);
			return TRUE;
		}
	}
	else 
	{
		if (bIcon)
		{
			ParseIconLink(szIcon, szIconLink, MAX_PATH + 1);

			if (!IsFileExist(szIconLink))
			{
				m_arrFiles.Add(*lpSIX);
				return TRUE;
			}
		}
		else
		{
			m_arrFiles.Add(*lpSIX);
			return TRUE;
		}
	}

	return FALSE;
}
