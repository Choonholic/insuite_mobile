// UIntArrayX.h

#pragma once

#include "InAPI.h"

class INAPI_CLASS CUIntArrayX
{
public:
	CUIntArrayX(void);
	virtual	~CUIntArrayX(void);

	int		GetSize(void) const;
	int		GetCount(void) const;
	BOOL	IsEmpty(void) const;
	int		GetUpperBound(void) const;
	void	SetSize(int nNewSize, int nGrowBy = -1);

	void	FreeExtra(void);
	void	RemoveAll(void);

	UINT	GetAt(int nIndex) const;
	void	SetAt(int nIndex, UINT newElement);

	UINT&	ElementAt(int nIndex);

	const	UINT*	GetData(void) const;
	UINT*			GetData(void);

	void	SetAtGrow(int nIndex, UINT newElement);

	int		Add(UINT newElement);

	int		Append(const CUIntArrayX& src);
	void	Copy(const CUIntArrayX& src);

	UINT	operator[](int nIndex) const;
	UINT&	operator[](int nIndex);

	void	InsertAt(int nIndex, UINT newElement, int nCount = 1);
	void	RemoveAt(int nIndex, int nCount = 1);
	void	InsertAt(int nStartIndex, CUIntArrayX* pNewArray);

protected:
	UINT*	m_pData;
	int		m_nSize;
	int		m_nMaxSize;
	int		m_nGrowBy;

	typedef UINT	BASE_TYPE;
	typedef UINT	BASE_ARG_TYPE;
};
