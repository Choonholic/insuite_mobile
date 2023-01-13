// PointerArrayX.cpp

#include "StdAfx.h"
#include "PointerArrayX.h"

CPointerArrayX::CPointerArrayX(void)
{
	m_pData		= NULL;
	m_nSize		= 0;
	m_nMaxSize	= 0;
	m_nGrowBy	= 0;
}

CPointerArrayX::~CPointerArrayX(void)
{
	delete [] reinterpret_cast<LPBYTE>(m_pData);
}

inline int CPointerArrayX::GetSize(void) const
{
	return m_nSize;
}

inline int CPointerArrayX::GetCount(void) const
{
	return m_nSize;
}

inline BOOL CPointerArrayX::IsEmpty(void) const
{
	return static_cast<BOOL>(m_nSize == 0);
}

inline int CPointerArrayX::GetUpperBound(void) const
{
	return (m_nSize - 1);
}

void CPointerArrayX::SetSize(int nNewSize, int nGrowBy)
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
		m_pData		= reinterpret_cast<void**>(new BYTE[nNewSize * sizeof(void*)]);
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewSize;

		memset(m_pData, 0, nNewSize * sizeof(void*));
	}
	else if (nNewSize <= m_nMaxSize)
	{
		if (nNewSize > m_nSize)
		{
			memset(&m_pData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(void*));
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

		void**	pNewData	= reinterpret_cast<void**>(new BYTE[nNewMax * sizeof(void*)]);

		memcpy_s(pNewData, nNewMax * sizeof(void*), m_pData, m_nSize * sizeof(void*));
		memset(&pNewData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(void*));

		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewMax;
	}
}

void CPointerArrayX::FreeExtra(void)
{
	if (m_nSize != m_nMaxSize)
	{
		void**	pNewData	= NULL;

		if (m_nSize != 0)
		{
			pNewData	= reinterpret_cast<void**>(new BYTE[m_nSize * sizeof(void*)]);

			memcpy_s(pNewData, m_nSize * sizeof(void*), m_pData, m_nSize * sizeof(void*));
		}

		delete [] reinterpret_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nMaxSize	= m_nSize;
	}
}

inline void CPointerArrayX::RemoveAll(void)
{
	SetSize(0);
}

inline void*CPointerArrayX::GetAt(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= m_nSize))
	{
		return NULL;
	}

	return m_pData[nIndex];
}

inline void CPointerArrayX::SetAt(int nIndex, void* newElement)
{
	if ((nIndex >= 0) && (nIndex < m_nSize))
	{
		m_pData[nIndex]	= newElement;
	}
}

inline void*& CPointerArrayX::ElementAt(int nIndex)
{
	return m_pData[nIndex];
}

inline const void** CPointerArrayX::GetData(void) const
{
	return (const void**)(m_pData);
}

inline void** CPointerArrayX::GetData(void)
{
	return static_cast<void**>(m_pData);
}

void CPointerArrayX::SetAtGrow(int nIndex, void* newElement)
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

inline int CPointerArrayX::Add(void* newElement)
{
	int	nIndex	= m_nSize;

	SetAtGrow(nIndex, newElement);
	return nIndex;
}

int CPointerArrayX::Append(const CPointerArrayX& src)
{
	if (this == &src)
	{
		return m_nSize;
	}

	int	nOldSize	= m_nSize;

	SetSize(m_nSize + src.m_nSize);
	memcpy_s(m_pData + nOldSize, src.m_nSize * sizeof(void*), src.m_pData, src.m_nSize * sizeof(void*));
	return nOldSize;
}

void CPointerArrayX::Copy(const CPointerArrayX& src)
{
	if (this != &src)
	{
		SetSize(src.m_nSize);
		memcpy_s(m_pData, src.m_nSize * sizeof(void*), src.m_pData, src.m_nSize * sizeof(void*));
	}
}

inline void* CPointerArrayX::operator[](int nIndex) const
{
	return GetAt(nIndex);
}

inline void*& CPointerArrayX::operator[](INT_PTR nIndex)
{
	return ElementAt(nIndex);
}

void CPointerArrayX::InsertAt(int nIndex, void* newElement, int nCount)
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
		memmove_s(&m_pData[nIndex + nCount], (m_nSize - (nCount + nIndex)) * sizeof(void*), &m_pData[nIndex], (nOldSize - nIndex) * sizeof(void*));
		memset(&m_pData[nIndex], 0, nCount * sizeof(void*));
	}

	while (nCount--)
	{
		m_pData[nIndex++]	= newElement;
	}
}

void CPointerArrayX::RemoveAt(int nIndex, int nCount)
{
	int	nUpperBound	= nIndex + nCount;

	if ((nIndex < 0) || (nCount < 0) || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
	{
		return;
	}

	int	nMoveCount	= m_nSize - nUpperBound;

	if (nMoveCount)
	{
		memmove_s(&m_pData[nIndex], (size_t)nMoveCount * sizeof(void*), &m_pData[nUpperBound], (size_t)nMoveCount * sizeof(void*));
	}

	m_nSize	-= nCount;
}

void CPointerArrayX::InsertAt(int nStartIndex, CPointerArrayX* pNewArray)
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
