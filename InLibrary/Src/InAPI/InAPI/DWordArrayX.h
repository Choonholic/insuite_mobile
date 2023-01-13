// DWordArrayX.h

#pragma once

#include "InAPI.h"

class INAPI_CLASS CDWordArrayX
{
public:
	CDWordArrayX(void);
	virtual	~CDWordArrayX(void);

	int		GetSize(void) const;
	int		GetCount(void) const;
	BOOL	IsEmpty(void) const;
	int		GetUpperBound(void) const;
	void	SetSize(int nNewSize, int nGrowBy = -1);

	void	FreeExtra(void);
	void	RemoveAll(void);

	DWORD	GetAt(int nIndex) const;
	void	SetAt(int nIndex, DWORD newElement);

	DWORD&	ElementAt(int nIndex);

	const LPDWORD	GetData(void) const;
	LPDWORD			GetData(void);

	void	SetAtGrow(int nIndex, DWORD newElement);

	int		Add(DWORD newElement);

	int		Append(const CDWordArrayX& src);
	void	Copy(const CDWordArrayX& src);

	DWORD	operator[](int nIndex) const;
	DWORD&	operator[](int nIndex);

	void	InsertAt(int nIndex, DWORD newElement, int nCount = 1);
	void	RemoveAt(int nIndex, int nCount = 1);
	void	InsertAt(int nStartIndex, CDWordArrayX* pNewArray);

protected:
	LPDWORD	m_pData;
	int		m_nSize;
	int		m_nMaxSize;
	int		m_nGrowBy;

	typedef DWORD	BASE_TYPE;
	typedef DWORD	BASE_ARG_TYPE;
};
