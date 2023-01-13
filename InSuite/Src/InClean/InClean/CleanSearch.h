// CleanSearch.h

#pragma once

#include "SearchX.h"

class CCleanSearch : public CSearchX
{
public:
	CCleanSearch(void);
	virtual	~CCleanSearch(void);

	void	Reset(void);

	virtual BOOL	FoundAction(LPSEARCHINFOX lpSIX);
	virtual BOOL	FolderAction(LPSEARCHINFOX lpSIX, BOOL bInclude);

	CArray<SEARCHINFOX, SEARCHINFOX>	m_arrFiles;
	CArray<SEARCHINFOX, SEARCHINFOX>	m_arrFolders;
};
