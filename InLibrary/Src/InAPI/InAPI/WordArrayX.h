// WordArrayX.h

#pragma once

#include "InAPI.h"

class INAPI_CLASS CWordArrayX
{
public:
	CWordArrayX(void);
	virtual	~CWordArrayX(void);

	int		GetSize(void) const;
	int		GetCount(void) const;
	BOOL	IsEmpty(void) const;
	int		GetUpperBound(void) const;
	void	SetSize(int nNewSize, int nGrowBy = -1);

	void	FreeExtra(void);
	void	RemoveAll(void);

	WORD	GetAt(int nIndex) const;
	void	SetAt(int nIndex, WORD newElement);

	WORD&	ElementAt(int nIndex);

	const	LPWORD	GetData(void) const;
	LPWORD			GetData(void);

	void	SetAtGrow(int nIndex, WORD newElement);

	int		Add(WORD newElement);

	int		Append(const CWordArrayX& src);
	void	Copy(const CWordArrayX& src);

	WORD	operator[](int nIndex) const;
	WORD&	operator[](int nIndex);

	void	InsertAt(int nIndex, WORD newElement, int nCount = 1);
	void	RemoveAt(int nIndex, int nCount = 1);
	void	InsertAt(int nStartIndex, CWordArrayX* pNewArray);

protected:
	LPWORD	m_pData;
	int		m_nSize;
	int		m_nMaxSize;
	int		m_nGrowBy;

	typedef WORD	BASE_TYPE;
	typedef WORD	BASE_ARG_TYPE;
};
