// ResourceX.cpp

#include "StdAfx.h"
#include "ResourceX.h"

#define MAX_STRINGLEN	2048	// 4KB

CResourceX::CResourceX(void)
{
	m_hResourceHandle	= NULL;
	m_nCount			= 0;

	memset(m_arrLoadedString, 0, sizeof(LPTSTR) * RESOURCEX_BUFFER);
}

CResourceX::CResourceX(HINSTANCE hResourceHandle)
{
	ASSERT(hResourceHandle != NULL);

	m_hResourceHandle	= hResourceHandle;
	m_nCount			= 0;

	memset(m_arrLoadedString, 0, sizeof(LPTSTR) * RESOURCEX_BUFFER);
}

CResourceX::~CResourceX(void)
{
	ReleaseAllString();

	m_hResourceHandle	= NULL;
	m_nCount			= 0;
}

void CResourceX::SetResourceHandle(HINSTANCE hResourceHandle)
{
	ASSERT(hResourceHandle != NULL);

	m_hResourceHandle	= hResourceHandle;
}

LPTSTR CResourceX::LoadString(UINT uID)
{
	ASSERT(m_hResourceHandle != NULL);
	ASSERT(uID > 0);

	TCHAR	szBuffer[MAX_STRINGLEN];
	LPTSTR	lpszString	= NULL;

	if (::LoadString(m_hResourceHandle, uID, szBuffer, MAX_STRINGLEN) && (m_nCount < RESOURCEX_BUFFER))
	{
		lpszString	= new TCHAR[_tcslen(szBuffer) + 1];

		_tcscpy_s(lpszString, _tcslen(szBuffer) + 1, szBuffer);

		m_arrLoadedString[m_nCount]	= lpszString;
		m_nCount++;

		return lpszString;
	}

	return NULL;
}

BOOL CResourceX::ReleaseString(LPTSTR lpszString)
{
	ASSERT(lpszString != NULL);

	BOOL	bRemoved	= FALSE;

	for (int i = 0; i < m_nCount; i++)
	{
		if (m_arrLoadedString[i] == lpszString)
		{
			delete lpszString;

			for (int j = i; j < (m_nCount - 1); j++)
			{
				m_arrLoadedString[j]	= m_arrLoadedString[j + 1];
			}

			m_arrLoadedString[--m_nCount]	= NULL;
			bRemoved						= TRUE;
			break;
		}
	}

	return bRemoved;
}

void CResourceX::ReleaseAllString(void)
{
	for (int i = 0; i < m_nCount; i++)
	{
		if (m_arrLoadedString[i] != NULL)
		{
			delete reinterpret_cast<LPTSTR>(m_arrLoadedString[i]);
		}
	}

	m_nCount	= 0;

	memset(m_arrLoadedString, 0, sizeof(LPTSTR) * RESOURCEX_BUFFER);
}
