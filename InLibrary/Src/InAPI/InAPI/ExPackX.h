// ExPackX.h

#pragma once

#include "InAPI.h"

class INAPI_CLASS CExPackX
{
public:
	CExPackX(void);
	CExPackX(LPCTSTR lpszModule, LPCTSTR lpszPacked);
	virtual	~CExPackX(void);

	void	SetPackFolder(LPCTSTR lpszFolder);
	void	SetPackFolder(INT nSpecialFolderID);

	HINSTANCE	Load(LPCTSTR lpszModule, LPCTSTR lpszPacked);
	void		Unload(void);

	HINSTANCE	PackInstance(void);
	BOOL		IsLoaded(void);

protected:
	void		InitX(void);

	HINSTANCE	m_hInstance;
	TCHAR		m_szPackFolder[MAX_PATH + 1];
};
