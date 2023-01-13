// ByteArrayX.h

#pragma once

#include "InAPI.h"

class INAPI_CLASS CByteArrayX
{
public:
	CByteArrayX(void);
	virtual	~CByteArrayX(void);

	int		GetSize(void) const;
	int		GetCount(void) const;
	BOOL	IsEmpty(void) const;
	int		GetUpperBound(void) const;
	void	SetSize(int nNewSize, int nGrowBy = -1);

	void	FreeExtra(void);
	void	RemoveAll(void);

	BYTE	GetAt(int nIndex) const;
	void	SetAt(int nIndex, BYTE newElement);

	BYTE&	ElementAt(int nIndex);

	const LPBYTE	GetData(void) const;
	LPBYTE			GetData(void);

	void	SetAtGrow(int nIndex, BYTE newElement);

	int		Add(BYTE newElement);

	int		Append(const CByteArrayX& src);
	void	Copy(const CByteArrayX& src);

	BYTE	operator[](int nIndex) const;
	BYTE&	operator[](int nIndex);

	void	InsertAt(int nIndex, BYTE newElement, int nCount = 1);
	void	RemoveAt(int nIndex, int nCount = 1);
	void	InsertAt(int nStartIndex, CByteArrayX* pNewArray);

protected:
	LPBYTE	m_pData;
	int		m_nSize;
	int		m_nMaxSize;
	int		m_nGrowBy;

	typedef BYTE	BASE_TYPE;
	typedef BYTE	BASE_ARG_TYPE;
};
