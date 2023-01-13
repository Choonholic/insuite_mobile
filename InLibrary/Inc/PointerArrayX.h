// PointerArrayX.h

#pragma once

#include "InAPI.h"

class INAPI_CLASS CPointerArrayX
{
public:
	CPointerArrayX(void);
	virtual	~CPointerArrayX(void);

	int		GetSize(void) const;
	int		GetCount(void) const;
	BOOL	IsEmpty(void) const;
	int		GetUpperBound(void) const;
	void	SetSize(int nNewSize, int nGrowBy = -1);

	void	FreeExtra(void);
	void	RemoveAll(void);

	void*	GetAt(int nIndex) const;
	void	SetAt(int nIndex, void* newElement);

	void*&	ElementAt(int nIndex);

	const void**	GetData(void) const;
	void**			GetData(void);

	void	SetAtGrow(int nIndex, void* newElement);

	int		Add(void* newElement);

	int		Append(const CPointerArrayX& src);
	void	Copy(const CPointerArrayX& src);

	void*	operator[](int nIndex) const;
	void*&	operator[](int nIndex);

	void	InsertAt(int nIndex, void* newElement, int nCount = 1);
	void	RemoveAt(int nIndex, int nCount = 1);
	void	InsertAt(int nStartIndex, CPointerArrayX* pNewArray);

protected:
	void**	m_pData;
	int		m_nSize;
	int		m_nMaxSize;
	int		m_nGrowBy;

	typedef void*	BASE_TYPE;
	typedef void*	BASE_ARG_TYPE;
};
