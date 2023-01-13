// ByteArrayX.cpp

#include "StdAfx.h"
#include "ByteArrayX.h"

CByteArrayX::CByteArrayX(void)
{
	m_pData		= NULL;
	m_nSize		= 0;
	m_nMaxSize	= 0;
	m_nGrowBy	= 0;
}

CByteArrayX::~CByteArrayX(void)
{
	delete [] static_cast<LPBYTE>(m_pData);
}

inline int CByteArrayX::GetSize(void) const
{
	return m_nSize;
}

inline int CByteArrayX::GetCount(void) const
{
	return m_nSize;
}

inline BOOL CByteArrayX::IsEmpty(void) const
{
	return static_cast<BOOL>(m_nSize == 0);
}

inline int CByteArrayX::GetUpperBound(void) const
{
	return (m_nSize - 1);
}

void CByteArrayX::SetSize(int nNewSize, int nGrowBy)
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
		delete [] static_cast<LPBYTE>(m_pData);

		m_pData		= NULL;
		m_nSize		= 0;
		m_nMaxSize	= 0;
	}
	else if (m_pData == NULL)
	{
		m_pData		= (BYTE *)new BYTE[nNewSize * sizeof(BYTE)];
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewSize;

		memset(m_pData, 0, nNewSize * sizeof(BYTE));
	}
	else if (nNewSize <= m_nMaxSize)
	{
		if (nNewSize > m_nSize)
		{
			memset(&m_pData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(BYTE));
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

		LPBYTE	pNewData	= static_cast<LPBYTE>(new BYTE[nNewMax * sizeof(BYTE)]);

		memcpy_s(pNewData, nNewMax * sizeof(BYTE), m_pData, m_nSize * sizeof(BYTE));
		memset(&pNewData[m_nSize], 0, (nNewSize - m_nSize) * sizeof(BYTE));

		delete [] static_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nSize		= nNewSize;
		m_nMaxSize	= nNewMax;
	}
}

void CByteArrayX::FreeExtra(void)
{
	if (m_nSize != m_nMaxSize)
	{
		LPBYTE	pNewData	= NULL;

		if (m_nSize != 0)
		{
			pNewData	= static_cast<LPBYTE>(new BYTE[m_nSize * sizeof(BYTE)]);

			memcpy_s(pNewData, m_nSize * sizeof(BYTE), m_pData, m_nSize * sizeof(BYTE));
		}

		delete [] static_cast<LPBYTE>(m_pData);

		m_pData		= pNewData;
		m_nMaxSize	= m_nSize;
	}
}

inline void CByteArrayX::RemoveAll(void)
{
	SetSize(0);
}

inline BYTE CByteArrayX::GetAt(int nIndex) const
{
	if ((nIndex < 0) || (nIndex >= m_nSize))
	{
		return NULL;
	}

	return m_pData[nIndex];
}

inline void CByteArrayX::SetAt(int nIndex, BYTE newElement)
{
	if ((nIndex >= 0) && (nIndex < m_nSize))
	{
		m_pData[nIndex]	= newElement;
	}
}

inline BYTE& CByteArrayX::ElementAt(int nIndex)
{
	return m_pData[nIndex];
}

inline const LPBYTE CByteArrayX::GetData(void) const
{
	return static_cast<const LPBYTE>(m_pData);
}

inline LPBYTE CByteArrayX::GetData(void)
{
	return static_cast<LPBYTE>(m_pData);
}

void CByteArrayX::SetAtGrow(int nIndex, BYTE newElement)
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

inline int CByteArrayX::Add(BYTE newElement)
{
	int	nIndex	= m_nSize;

	SetAtGrow(nIndex, newElement);
	return nIndex;
}

int CByteArrayX::Append(const CByteArrayX& src)
{
	if (this == &src)
	{
		return m_nSize;
	}

	int	nOldSize	= m_nSize;

	SetSize(m_nSize + src.m_nSize);
	memcpy_s(m_pData + nOldSize, src.m_nSize * sizeof(BYTE), src.m_pData, src.m_nSize * sizeof(BYTE));
	return nOldSize;
}

void CByteArrayX::Copy(const CByteArrayX& src)
{
	if (this != &src)
	{
		SetSize(src.m_nSize);
		memcpy_s(m_pData, src.m_nSize * sizeof(BYTE), src.m_pData, src.m_nSize * sizeof(BYTE));
	}
}

inline BYTE CByteArrayX::operator[](int nIndex) const
{
	return GetAt(nIndex);
}

inline BYTE& CByteArrayX::operator[](INT_PTR nIndex)
{
	return ElementAt(nIndex);
}

void CByteArrayX::InsertAt(int nIndex, BYTE newElement, int nCount)
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
		memmove_s(&m_pData[nIndex + nCount], (m_nSize - (nCount + nIndex)) * sizeof(BYTE), &m_pData[nIndex], (nOldSize - nIndex) * sizeof(BYTE));
		memset(&m_pData[nIndex], 0, nCount * sizeof(BYTE));
	}

	while (nCount--)
	{
		m_pData[nIndex++]	= newElement;
	}
}

void CByteArrayX::RemoveAt(int nIndex, int nCount)
{
	int	nUpperBound	= nIndex + nCount;

	if ((nIndex < 0) || (nCount < 0) || (nUpperBound > m_nSize) || (nUpperBound < nIndex) || (nUpperBound < nCount))
	{
		return;
	}

	int	nMoveCount	= m_nSize - nUpperBound;

	if (nMoveCount)
	{
		memmove_s(&m_pData[nIndex], (size_t)nMoveCount * sizeof(BYTE), &m_pData[nUpperBound], (size_t)nMoveCount * sizeof(BYTE));
	}

	m_nSize	-= nCount;
}

void CByteArrayX::InsertAt(int nStartIndex, CByteArrayX* pNewArray)
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
