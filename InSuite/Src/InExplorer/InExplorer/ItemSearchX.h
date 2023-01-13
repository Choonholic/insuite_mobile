// ItemSearchX.h

#pragma once

#include "SearchX.h"

class CItemSearchX : public CSearchX
{
public:
	CItemSearchX(void);
	virtual	~CItemSearchX(void);

	virtual BOOL	FoundAction(LPSEARCHINFOX lpSIX);

	CStringArray	m_arrFoundFiles;
};