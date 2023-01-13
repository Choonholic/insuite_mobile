// DWordArrayX.cpp

#include "StdAfx.h"
#include "DWordArrayX.h"

CDWordArrayX::CDWordArrayX(void)
{
	m_pData		= NULL;
	m_nSize		= 0;
	m_nMaxSize	= 0;
	m_nGrowBy	= 0;
}

CDWordArrayX::~CDWordArrayX(void)
{
	delete [] reinterpret_cast<LPBYTE>(m_pData);
}

inline int CDWordArrayX::GetSize(void) const
{
	return m_nSize;
}

inline int CDWordArrayX::GetCount(void) const
{
	return m_nSize;
}

inline BOOL CDWordArrayX::IsEmpty(void) const
{
	return static_cast<BOOL>(m_nSize == 0);
}

inline int CDWordArrayX::GetUpperBound(void) const
{
	return (m_nSize - 1);
}

void CDWordArrayX::SetSize(int nNewSize, int nGrowBy)
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
		m_pData		= reinterpret_cast<LPDWORD>(new BYTE[nNewSize * sizeof(DWORD)]);
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewSize;

		memset(m_pData, 0, nNewSize * sizeof(DWORD));
	}
	else if (nNewSize <= m_nMaxSize)
	{
		if (nNewSize > m_nSize)
		{
			memset(&m_pData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(DWORD));
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

		LPDWORD	pNewData	= reinterpret_cast<LPDWORD>(new BYTE[nNewMax * sizeof(DWORD)]);

		memcpy_s(pNewData, nNewMax * sizeof(DWORD), m_pData, m_nSize * sizeof(DWORD));
		memset(&pNewData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(DWORD));

		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewMax;
	}
}

void CDWordArrayX::FreeExtra(void)
{
	if (m_nSize != m_nMaxSize)
	{
		LPDWORD	pNewData	= NULL;

		if (m_nSize != 0)
		{
			pNewData	= reinterpret_cast<LPDWORD>(new BYTE[m_nSize * sizeof(DWORD)]);

			memcpy_s(pNewData, m_nSize * sizeof(DWORD), m_pData, m_nSize * sizeof(DWORD));
		}

		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nMaxSize	= m_nSize;
	}
}

inline void CDWordArrayX::RemoveAll(void)
{
	SetSize(0);
}

inline DWORD CDWordArrayX::GetAt(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= m_nSize))
	{
		return NULL;
	}

	return m_pData[nIndex];
}

inline void CDWordArrayX::SetAt(int nIndex, DWORD newElement)
{
	if ((nIndex >= 0) && (nIndex < m_nSize))
	{
		m_pData[nIndex]	= newElement;
	}
}

inline DWORD& CDWordArrayX::ElementAt(int nIndex)
{
	return m_pData[nIndex];
}

inline const LPDWORD CDWordArrayX::GetData(void) const
{
	return static_cast<const LPDWORD>(m_pData);
}

inline LPDWORD CDWordArrayX::GetData(void)
{
	return static_cast<LPDWORD>(m_pData);
}

void CDWordArrayX::SetAtGrow(int nIndex, DWORD newElement)
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

inline int CDWordArrayX::Add(DWORD newElement)
{
	int	nIndex	= m_nSize;

	SetAtGrow(nIndex, newElement);
	return nIndex;
}

int CDWordArrayX::Append(const CDWordArrayX& src)
{
	if (this == &src)
	{
		return m_nSize;
	}

	int	nOldSize	= m_nSize;

	SetSize(m_nSize + src.m_nSize);
	memcpy_s(m_pData + nOldSize, src.m_nSize * sizeof(DWORD), src.m_pData, src.m_nSize * sizeof(DWORD));
	return nOldSize;
}

void CDWordArrayX::Copy(const CDWordArrayX& src)
{
	if (this != &src)
	{
		SetSize(src.m_nSize);
		memcpy_s(m_pData, src.m_nSize * sizeof(DWORD), src.m_pData, src.m_nSize * sizeof(DWORD));
	}
}

inline DWORD CDWordArrayX::operator[](int nIndex) const
{
	return GetAt(nIndex);
}

inline DWORD& CDWordArrayX::operator[](INT_PTR nIndex)
{
	return ElementAt(nIndex);
}

void CDWordArrayX::InsertAt(int nIndex, DWORD newElement, int nCount)
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
		memmove_s(&m_pData[nIndex + nCount], (m_nSize - (nCount + nIndex)) * sizeof(DWORD), &m_pData[nIndex], (nOldSize - nIndex) * sizeof(DWORD));
		memset(&m_pData[nIndex], 0, nCount * sizeof(DWORD));
	}

	while (nCount--)
	{
		m_pData[nIndex++]	= newElement;
	}
}

void CDWordArrayX::RemoveAt(int nIndex, int nCount)
{
	int	nUpperBound	= nIndex + nCount;

	if ((nIndex < 0) || (nCount < 0) || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
	{
		return;
	}

	int	nMoveCount	= m_nSize - nUpperBound;

	if (nMoveCount)
	{
		memmove_s(&m_pData[nIndex], (size_t)nMoveCount * sizeof(DWORD), &m_pData[nUpperBound], (size_t)nMoveCount * sizeof(DWORD));
	}

	m_nSize	-= nCount;
}

void CDWordArrayX::InsertAt(int nStartIndex, CDWordArrayX* pNewArray)
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
