// WordArrayX.cpp

#include "StdAfx.h"
#include "WordArrayX.h"

CWordArrayX::CWordArrayX(void)
{
	m_pData		= NULL;
	m_nSize		= 0;
	m_nMaxSize	= 0;
	m_nGrowBy	= 0;
}

CWordArrayX::~CWordArrayX(void)
{
	delete [] reinterpret_cast<LPBYTE>(m_pData);
}

inline int CWordArrayX::GetSize(void) const
{
	return m_nSize;
}

inline int CWordArrayX::GetCount(void) const
{
	return m_nSize;
}

inline BOOL CWordArrayX::IsEmpty(void) const
{
	return static_cast<BOOL>(m_nSize == 0);
}

inline int CWordArrayX::GetUpperBound(void) const
{
	return (m_nSize - 1);
}

void CWordArrayX::SetSize(int nNewSize, int nGrowBy)
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
		m_pData		= reinterpret_cast<LPWORD>(new BYTE[nNewSize * sizeof(WORD)]);
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewSize;

		memset(m_pData, 0, nNewSize * sizeof(WORD));
	}
	else if (nNewSize <= m_nMaxSize)
	{
		if (nNewSize > m_nSize)
		{
			memset(&m_pData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(WORD));
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

		LPWORD	pNewData	= reinterpret_cast<LPWORD>(new BYTE[nNewMax * sizeof(WORD)]);

		memcpy_s(pNewData, nNewMax * sizeof(WORD), m_pData, m_nSize * sizeof(WORD));
		memset(&pNewData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(WORD));

		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewMax;
	}
}

void CWordArrayX::FreeExtra(void)
{
	if (m_nSize != m_nMaxSize)
	{
		LPWORD	pNewData	= NULL;

		if (m_nSize != 0)
		{
			pNewData	= reinterpret_cast<LPWORD>(new BYTE[m_nSize * sizeof(WORD)]);

			memcpy_s(pNewData, m_nSize * sizeof(WORD), m_pData, m_nSize * sizeof(WORD));
		}

		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nMaxSize	= m_nSize;
	}
}

inline void CWordArrayX::RemoveAll(void)
{
	SetSize(0);
}

inline WORD CWordArrayX::GetAt(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= m_nSize))
	{
		return NULL;
	}

	return m_pData[nIndex];
}

inline void CWordArrayX::SetAt(int nIndex, WORD newElement)
{
	if ((nIndex >= 0) && (nIndex < m_nSize))
	{
		m_pData[nIndex]	= newElement;
	}
}

inline WORD& CWordArrayX::ElementAt(int nIndex)
{
	return m_pData[nIndex];
}

inline const LPWORD CWordArrayX::GetData(void) const
{
	return static_cast<const LPWORD>(m_pData);
}

inline LPWORD CWordArrayX::GetData(void)
{
	return static_cast<LPWORD>(m_pData);
}

void CWordArrayX::SetAtGrow(int nIndex, WORD newElement)
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

inline int CWordArrayX::Add(WORD newElement)
{
	int	nIndex	= m_nSize;

	SetAtGrow(nIndex, newElement);
	return nIndex;
}

int CWordArrayX::Append(const CWordArrayX& src)
{
	if (this == &src)
	{
		return m_nSize;
	}

	int	nOldSize	= m_nSize;

	SetSize(m_nSize + src.m_nSize);
	memcpy_s(m_pData + nOldSize, src.m_nSize * sizeof(WORD), src.m_pData, src.m_nSize * sizeof(WORD));
	return nOldSize;
}

void CWordArrayX::Copy(const CWordArrayX& src)
{
	if (this != &src)
	{
		SetSize(src.m_nSize);
		memcpy_s(m_pData, src.m_nSize * sizeof(WORD), src.m_pData, src.m_nSize * sizeof(WORD));
	}
}

inline WORD CWordArrayX::operator[](int nIndex) const
{
	return GetAt(nIndex);
}

inline WORD& CWordArrayX::operator[](INT_PTR nIndex)
{
	return ElementAt(nIndex);
}

void CWordArrayX::InsertAt(int nIndex, WORD newElement, int nCount)
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
		memmove_s(&m_pData[nIndex + nCount], (m_nSize - (nCount + nIndex)) * sizeof(WORD), &m_pData[nIndex], (nOldSize - nIndex) * sizeof(WORD));
		memset(&m_pData[nIndex], 0, nCount * sizeof(WORD));
	}

	while (nCount--)
	{
		m_pData[nIndex++]	= newElement;
	}
}

void CWordArrayX::RemoveAt(int nIndex, int nCount)
{
	int	nUpperBound	= nIndex + nCount;

	if ((nIndex < 0) || (nCount < 0) || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
	{
		return;
	}

	int	nMoveCount	= m_nSize - nUpperBound;

	if (nMoveCount)
	{
		memmove_s(&m_pData[nIndex], (size_t)nMoveCount * sizeof(WORD), &m_pData[nUpperBound], (size_t)nMoveCount * sizeof(WORD));
	}

	m_nSize	-= nCount;
}

void CWordArrayX::InsertAt(int nStartIndex, CWordArrayX* pNewArray)
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
