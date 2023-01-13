// UIntArrayX.cpp

#include "StdAfx.h"
#include "UIntArrayX.h"

CUIntArrayX::CUIntArrayX(void)
{
	m_pData		= NULL;
	m_nSize		= 0;
	m_nMaxSize	= 0;
	m_nGrowBy	= 0;
}

CUIntArrayX::~CUIntArrayX(void)
{
	delete [] reinterpret_cast<LPBYTE>(m_pData);
}

inline int CUIntArrayX::GetSize(void) const
{
	return m_nSize;
}

inline int CUIntArrayX::GetCount(void) const
{
	return m_nSize;
}

inline BOOL CUIntArrayX::IsEmpty(void) const
{
	return static_cast<BOOL>(m_nSize == 0);
}

inline int CUIntArrayX::GetUpperBound(void) const
{
	return (m_nSize - 1);
}

void CUIntArrayX::SetSize(int nNewSize, int nGrowBy)
{
	if (nNewSize < 0)
	{
		return;
	}

	if (nGrowBy >= 0)
	{
		m_nGrowBy	= nGrowBy;
	}

	if (nNewSize == 0)
	{
		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= NULL;
		m_nSize		= 0;
		m_nMaxSize	= 0;
	}
	else if (m_pData == NULL)
	{
		m_pData		= reinterpret_cast<UINT*>(new BYTE[nNewSize * sizeof(UINT)]);
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewSize;

		memset(m_pData, 0, nNewSize * sizeof(UINT));
	}
	else if (nNewSize <= m_nMaxSize)
	{
		if (nNewSize > m_nSize)
		{
			memset(&m_pData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(UINT));
		}

		m_nSize	= nNewSize;
	}
	else
	{
		int	nGrowArrayBy	= m_nGrowBy;
		int	nNewMax;

		if (nGrowArrayBy == 0)
		{
			nGrowArrayBy	= min(1024, max(4, m_nSize / 8));
		}

		if (nNewSize < (m_nMaxSize + nGrowArrayBy))
		{
			nNewMax	= m_nMaxSize + nGrowArrayBy;
		}
		else
		{
			nNewMax	= nNewSize;
		}

		UINT*	pNewData	= reinterpret_cast<UINT*>(new BYTE[nNewMax * sizeof(UINT)]);

		memcpy_s(pNewData, nNewMax * sizeof(UINT), m_pData, m_nSize * sizeof(UINT));
		memset(&pNewData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(UINT));

		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewMax;
	}
}

void CUIntArrayX::FreeExtra(void)
{
	if (m_nSize != m_nMaxSize)
	{
		UINT*	pNewData	= NULL;

		if (m_nSize != 0)
		{
			pNewData	= reinterpret_cast<UINT*>(new BYTE[m_nSize * sizeof(UINT)]);

			memcpy_s(pNewData, m_nSize * sizeof(UINT), m_pData, m_nSize * sizeof(UINT));
		}

		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nMaxSize	= m_nSize;
	}
}

inline void CUIntArrayX::RemoveAll(void)
{
	SetSize(0);
}

inline UINT CUIntArrayX::GetAt(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= m_nSize))
	{
		return NULL;
	}

	return m_pData[nIndex];
}

inline void CUIntArrayX::SetAt(int nIndex, UINT newElement)
{
	if ((nIndex >= 0) && (nIndex < m_nSize))
	{
		m_pData[nIndex]	= newElement;
	}
}

inline UINT& CUIntArrayX::ElementAt(int nIndex)
{
	return m_pData[nIndex];
}

inline const UINT* CUIntArrayX::GetData(void) const
{
	return static_cast<const UINT*>(m_pData);
}

inline UINT* CUIntArrayX::GetData(void)
{
	return static_cast<UINT*>(m_pData);
}

void CUIntArrayX::SetAtGrow(int nIndex, UINT newElement)
{
	if (nIndex < 0)
	{
		return;
	}

	if (nIndex >= m_nSize)
	{
		SetSize(nIndex + 1);
	}

	m_pData[nIndex]	= newElement;
}

inline int CUIntArrayX::Add(UINT newElement)
{
	int	nIndex	= m_nSize;

	SetAtGrow(nIndex, newElement);
	return nIndex;
}

int CUIntArrayX::Append(const CUIntArrayX& src)
{
	if (this == &src)
	{
		return m_nSize;
	}

	int	nOldSize	= m_nSize;

	SetSize(m_nSize + src.m_nSize);
	memcpy_s(m_pData + nOldSize, src.m_nSize * sizeof(UINT), src.m_pData, src.m_nSize * sizeof(UINT));
	return nOldSize;
}

void CUIntArrayX::Copy(const CUIntArrayX& src)
{
	if (this != &src)
	{
		SetSize(src.m_nSize);
		memcpy_s(m_pData, src.m_nSize * sizeof(UINT), src.m_pData, src.m_nSize * sizeof(UINT));
	}
}

inline UINT CUIntArrayX::operator[](int nIndex) const
{
	return GetAt(nIndex);
}

inline UINT& CUIntArrayX::operator[](INT_PTR nIndex)
{
	return ElementAt(nIndex);
}

void CUIntArrayX::InsertAt(int nIndex, UINT newElement, int nCount)
{
	if (nIndex < 0 || nCount <= 0)
	{
		return;
	}

	if (nIndex >= m_nSize)
	{
		SetSize(nIndex + nCount);
	}
	else
	{
		int	nOldSize	= m_nSize;

		SetSize(m_nSize + nCount);
		memmove_s(&m_pData[nIndex + nCount], (m_nSize - (nCount + nIndex)) * sizeof(UINT), &m_pData[nIndex], (nOldSize - nIndex) * sizeof(UINT));
		memset(&m_pData[nIndex], 0, nCount * sizeof(UINT));
	}

	while (nCount--)
	{
		m_pData[nIndex++]	= newElement;
	}
}

void CUIntArrayX::RemoveAt(int nIndex, int nCount)
{
	int	nUpperBound	= nIndex + nCount;

	if ((nIndex < 0) || (nCount < 0) || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
	{
		return;
	}

	int	nMoveCount	= m_nSize - nUpperBound;

	if (nMoveCount)
	{
		memmove_s(&m_pData[nIndex], (size_t)nMoveCount * sizeof(UINT), &m_pData[nUpperBound], (size_t)nMoveCount * sizeof(UINT));
	}

	m_nSize	-= nCount;
}

void CUIntArrayX::InsertAt(int nStartIndex, CUIntArrayX* pNewArray)
{
	if ((pNewArray == NULL) || (nStartIndex < 0))
	{
		return;
	}

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());

		for (int i = 0; i < pNewArray->GetSize(); i++)
		{
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
		}
	}
}
