// ShortcutSearch.h

#pragma once

#include "SearchX.h"

class CShortcutSearch : public CSearchX
{
public:
	CShortcutSearch(void);
	virtual	~CShortcutSearch(void);

	void	Reset(void);

	virtual BOOL	FoundAction(LPSEARCHINFOX lpSIX);

	CArray<SEARCHINFOX, SEARCHINFOX>	m_arrFiles;
};
