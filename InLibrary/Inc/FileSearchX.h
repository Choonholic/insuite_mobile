// FileSearchX.h

#pragma once

#include "SearchX.h"

class CFileSearchX : public CSearchX
{
public:
	CFileSearchX(void);
	virtual	~CFileSearchX(void);

	virtual BOOL	FoundAction(LPSEARCHINFOX lpSIX);
	virtual BOOL	FolderAction(LPSEARCHINFOX lpSIX, BOOL bInclude);

	CArray<SEARCHINFOX, SEARCHINFOX>	m_arrFiles;
	CArray<SEARCHINFOX, SEARCHINFOX>	m_arrFolders;
};
