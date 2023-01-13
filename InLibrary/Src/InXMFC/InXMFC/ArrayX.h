// ArrayX.h

// Based on CSortedArray / CSortedArrayEx, Version 1.40 (11 August 2009)
// Interface for an MFC template class which provides sorting and ordered insertion derived from the MFC class CArray or ATL class CSimpleArray
//
// Copyright (c) 1999 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)
// All rights reserved.
//
// Copyright / Usage Details:
//
// You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise)
// when your product is released in binary form. You are allowed to modify the source code in any way you want
// except you cannot modify the copyright details at the top of each module. If you want to distribute source
// code with your application, then you are only allowed to distribute versions released by the author. This is
// to maintain a single distribution point for the source code.

#pragma once

#ifdef SORTEDARRAY_ATL_ONLY
#ifndef __ATLCOLL_H__
#include <atlcoll.h>
#pragma message("To avoid this message, please put atlcoll.h in your pre compiled header (usually stdafx.h)")
#endif
#else
#ifndef __AFXTEMPL_H__
#include <afxtempl.h> 
#pragma message("To avoid this message, please put afxtempl.h in your pre compiled header (usually stdafx.h)")
#endif
#endif

template<class TYPE, class ARG_TYPE = const TYPE&>
class CSortedArrayCompareFunction
{
public:
	typedef int					COMPARE_FUNCTION(ARG_TYPE element1, ARG_TYPE element2);
	typedef COMPARE_FUNCTION*	LPCOMPARE_FUNCTION;

	CSortedArrayCompareFunction() : m_pCompareFunction(NULL)
	{
	}

	void SetCompareFunction(LPCOMPARE_FUNCTION pCompareFunction)
	{
		ATLASSERT(pCompareFunction);

		m_pCompareFunction	= pCompareFunction;
	}

	LPCOMPARE_FUNCTION GetCompareFunction() const
	{
		return m_pCompareFunction;
	}

#ifdef _DEBUG
	BOOL IsCompareFunctionValid() const
	{
		return (m_pCompareFunction != NULL);
	}
#endif

protected:
	LPCOMPARE_FUNCTION	m_pCompareFunction;
};

template<class COMPARE_TYPE>
class CSortedArrayCompareFunctor
{
public:
	void SetCompareFunction(const COMPARE_TYPE& pCompareFunctor)
	{
		m_pCompareFunction	= pCompareFunctor;
	}

	COMPARE_TYPE GetCompareFunction() const
	{
		return m_pCompareFunction;
	}

#ifdef _DEBUG
	BOOL IsCompareFunctionValid() const
	{
		return TRUE;
	}
#endif

protected:
	COMPARE_TYPE	m_pCompareFunction;
};

#ifdef SORTEDARRAY_ATL_ONLY
template<typename TYPE, class COMPARE_TYPE, class ARG_TYPE = const TYPE&, class ETraits = CElementTraits<TYPE>>
class CSortedArrayBase : public CAtlArray<TYPE, ETraits>, public COMPARE_TYPE
#else
template<class TYPE, class COMPARE_TYPE, class ARG_TYPE = const TYPE&>
class CSortedArrayBase : public CArray<TYPE, ARG_TYPE>, public COMPARE_TYPE
#endif
{
public:
#ifdef SORTEDARRAY_ATL_ONLY
	INT_PTR GetSize() const
	{
		return GetCount();
	}

	bool SetSize(size_t nNewSize, int nGrowBy = -1)
	{
		return SetCount(nNewSize, nGrowBy);
	}
#endif

	INT_PTR OrderedInsert(ARG_TYPE newElement, INT_PTR nCount = 1)
	{
		ATLASSERT(IsCompareFunctionValid());

		INT_PTR		nLowIndex	= 0;
		INT_PTR		nHighIndex	= GetSize() - 1;
		const TYPE*	pData		= GetData();
		INT_PTR		nMidIndex	= nHighIndex / 2;

		while (nHighIndex >= nLowIndex)
		{
			ATLASSUME(pData);

			int	nResult	= m_pCompareFunction(newElement, pData[nMidIndex]);

			if (!nResult)
			{
				break;
			}

			if (nResult < 0)
			{
				nHighIndex	= nMidIndex - 1;
			}
			else
			{
				nLowIndex	= nMidIndex + 1;
			}

			nMidIndex	= ((nHighIndex - nLowIndex) / 2) + nLowIndex;
		}

		InsertAt(nMidIndex, newElement, nCount);
		return nMidIndex;
	}

	void StableSort(INT_PTR nLowIndex = 0, INT_PTR nHighIndex = -1)
	{
		if (nHighIndex == -1)
		{
			nHighIndex	= GetSize() - 1;
		}

		INT_PTR	nLength	= nHighIndex - nLowIndex + 1;

		if (nLength <= 1)
		{
			return;
		}

		ATLASSERT((nLowIndex >= 0) && (nLowIndex < GetSize()));
		ATLASSERT(nHighIndex < GetSize());
		ATLASSERT(IsCompareFunctionValid());

		TYPE*	pData	= GetData();

		ATLASSUME(pData);

		for (INT_PTR i = nLowIndex + 1; i <= nHighIndex; ++i)
		{
			TYPE	tmp	= pData[i];
			INT_PTR	j	= i - 1;

			while ((j >= 0) && (m_pCompareFunction(pData[j], tmp) > 0))
			{
				pData[j + 1]	= pData[j];
				--j;
			}

			pData[j + 1]	= tmp;
		}
	}

	void Sort(INT_PTR nLowIndex = 0, INT_PTR nHighIndex = -1)
	{
		if (nHighIndex == -1)
		{
			nHighIndex	= GetSize() - 1;
		}

		INT_PTR	nLength	= nHighIndex - nLowIndex + 1;

		if (nLength <= 1)
		{
			return;
		}

		ATLASSERT((nLowIndex >= 0) && (nLowIndex < GetSize()));
		ATLASSERT(nHighIndex < GetSize());
		ATLASSERT(IsCompareFunctionValid());

		TYPE*	pData	= GetData();

		ATLASSUME(pData);

		if (nLowIndex < nHighIndex)
		{
			INT_PTR	nPivotPosition	= (nLowIndex + nHighIndex) / 2;
			TYPE	tmp				= pData[nLowIndex];

			pData[nLowIndex]		= pData[nPivotPosition];
			pData[nPivotPosition]	= tmp;

			TYPE	key	= pData[nLowIndex];
			INT_PTR	i	= nLowIndex + 1;
			INT_PTR	j	= nHighIndex;

			while (i <= j)
			{
				while ((i <= nHighIndex) && (m_pCompareFunction(pData[i], key) <= 0))
				{
					i++;
				}

				while((j >= nLowIndex) && (m_pCompareFunction(pData[j], key) > 0))
				{
					j--;
				}

				if (i < j)
				{
					tmp			= pData[i];
					pData[i]	= pData[j];
					pData[j]	= tmp;
				}
			}

			tmp					= pData[nLowIndex];
			pData[nLowIndex]	= pData[j];
			pData[j]			= tmp;

			INT_PTR	nTempIndex		= j - 1;
			INT_PTR	nRecurseCount	= nTempIndex - nLowIndex + 1;

			if (nRecurseCount == 2)
			{
				if (m_pCompareFunction(pData[nLowIndex], pData[nTempIndex]) > 0)
				{
					tmp					= pData[nLowIndex];
					pData[nLowIndex]	= pData[nTempIndex];
					pData[nTempIndex]	= tmp;
				}
			}
			else if (nRecurseCount > 2)
			{
				Sort(nLowIndex, nTempIndex);
			}

			nTempIndex		= j + 1;
			nRecurseCount	= nHighIndex - nTempIndex + 1;

			if (nRecurseCount == 2)
			{
				if (m_pCompareFunction(pData[nTempIndex], pData[nHighIndex]) > 0)
				{
					tmp					= pData[nTempIndex];
					pData[nTempIndex]	= pData[nHighIndex];
					pData[nHighIndex]	= tmp;
				}
			}
			else if (nRecurseCount > 2)
			{
				Sort(nTempIndex, nHighIndex);
			}
		}
	}

	void UniqueSort(INT_PTR nLowIndex = 0, INT_PTR nHighIndex = -1)
	{
		if (nHighIndex == -1)
		{
			nHighIndex	= GetSize() - 1;
		}

		INT_PTR	nLength	= nHighIndex - nLowIndex + 1;

		if (nLength <= 1)
		{
			return;
		}

		Sort(nLowIndex, nHighIndex);

		for (INT_PTR i = nHighIndex; i > nLowIndex; i--)
		{
			if (m_pCompareFunction(operator[](i), operator[](i - 1)) == 0)
			{
				RemoveAt(i);
			}
		}
	}

	INT_PTR Find(ARG_TYPE element, INT_PTR nLowIndex = 0, INT_PTR nHighIndex = -1) const
	{
		ATLASSERT(IsCompareFunctionValid());

		INT_PTR	nSize	= GetSize();

		if (nSize == 0)
		{
			return -1;
		}

		INT_PTR		left	= nLowIndex;
		INT_PTR		right	= nHighIndex;
		const TYPE*	pData	= GetData();

		if (right == -1)
		{
			right	= nSize - 1;
		}

		ATLASSERT(left <= right);
		ATLASSUME(pData);

		if (left == right)
		{
			return ((m_pCompareFunction(pData[left], element) == 0) ? left : -1);
		}

		INT_PTR	nFoundIndex	= -1;

		while ((nFoundIndex == -1) && (left != right))
		{
			INT_PTR	nCompareIndex;

			if (right == (left + 2))
			{
				nCompareIndex	= left + 1;
			}
			else
			{
				nCompareIndex	= ((right - left) >> 1) + left;
			}

			int	nCompare	= m_pCompareFunction(pData[nCompareIndex], element);

			if (nCompare < 0)
			{
				if ((right - left) == 1)
				{
					if (m_pCompareFunction(pData[right], element) == 0)
					{
						nFoundIndex	= right;
					}
					else if (m_pCompareFunction(pData[left], element) == 0)
					{
						nFoundIndex	= left;
					}
					else
					{
						left	= right;
					}
				}
				else
				{
					left	= nCompareIndex;
				}
			}
			else if (nCompare > 0)
			{
				if ((right - left) == 1)
				{
					if (m_pCompareFunction(pData[right], element) == 0)
					{
						nFoundIndex	= right;
					}
					else if (m_pCompareFunction(pData[left], element) == 0)
					{
						nFoundIndex	= left;
					}
					else
					{
						right	= left;
					}
				}
				else
				{
					right = nCompareIndex;
				}
			}
			else
			{
				nFoundIndex	= nCompareIndex;
			}
		}

		return nFoundIndex;
	}

	BOOL IsSorted() const
	{
		ATLASSERT(IsCompareFunctionValid());

		BOOL	bSorted	= TRUE;
		INT_PTR	nSize	= GetSize();

		if (nSize >= 2)
		{
			const TYPE*	pData	= GetData();

			ATLASSUME(pData);

			for (INT_PTR i=1; i<nSize && bSorted; i++)
			{
				bSorted	= (m_pCompareFunction(pData[i - 1], pData[i]) <= 0);
			}
		}

		return bSorted;
	}
};

#ifdef SORTEDARRAY_ATL_ONLY
template<typename TYPE, class ARG_TYPE = const TYPE&, class ETraits = CElementTraits<TYPE>>
class CSortedArray : public CSortedArrayBase<TYPE, CSortedArrayCompareFunction<TYPE, ARG_TYPE>, ARG_TYPE, ETraits>
#else
template<class TYPE, class ARG_TYPE = const TYPE&>
class CSortedArray : public CSortedArrayBase<TYPE, CSortedArrayCompareFunction<TYPE, ARG_TYPE>, ARG_TYPE>
#endif
{
};

#ifdef SORTEDARRAY_ATL_ONLY
template<typename TYPE, class COMPARE_TYPE, class ARG_TYPE = const TYPE&, class ETraits = CElementTraits<TYPE>>
class CSortedArrayX : public CSortedArrayBase<TYPE, CSortedArrayCompareFunctor<COMPARE_TYPE>, ARG_TYPE, ETraits>
#else
template<class TYPE, class COMPARE_TYPE, class ARG_TYPE = const TYPE&>
class CSortedArrayX : public CSortedArrayBase<TYPE, CSortedArrayCompareFunctor<COMPARE_TYPE>, ARG_TYPE>
#endif
{
};
