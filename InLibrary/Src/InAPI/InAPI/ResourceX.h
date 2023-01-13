// ResourceX.h

#pragma once

#include "InAPI.h"

#ifndef RESOURCEX_BUFFER
#define RESOURCEX_BUFFER	128
#endif

class INAPI_CLASS CResourceX
{
public:
	CResourceX(void);
	CResourceX(HINSTANCE hResourceHandle);
	virtual	~CResourceX(void);

	void	SetResourceHandle(HINSTANCE hResourceHandle);

	LPTSTR	LoadString(UINT uID);
	BOOL	ReleaseString(LPTSTR lpszString);
	void	ReleaseAllString(void);

protected:
	HINSTANCE	m_hResourceHandle;
	LPTSTR		m_arrLoadedString[RESOURCEX_BUFFER];
	int			m_nCount;
};
