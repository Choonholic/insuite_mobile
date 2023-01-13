// StringX.cpp

#include "stdafx.h"
#include "StringX.h"

BOOL LeftX(LPTSTR lpszString, int nCount)
{
	ASSERT(lpszString != NULL);
	ASSERT(nCount >= 0);

	int	nLength	= _tcslen(lpszString);

	if (nCount > nLength)
	{
		nCount	= nLength;
	}

	LPTSTR	lpszSub	= new TCHAR[nCount + 1];

	if (lpszSub != NULL)
	{
		_tcsncpy_s(lpszSub, nCount + 1, lpszString, nCount);

		lpszSub[nCount]	= NULL;

		_tcscpy_s(lpszString, nLength + 1, lpszSub);
		delete [] lpszSub;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL RightX(LPTSTR lpszString, int nCount)
{
	ASSERT(lpszString != NULL);
	ASSERT(nCount >= 0);

	int	nLength	= _tcslen(lpszString);

	if (nCount > nLength)
	{
		nCount	= nLength;
	}

	LPTSTR	lpszSub	= new TCHAR[nCount + 1];

	if (lpszSub != NULL)
	{
		_tcscpy_s(lpszSub, nCount + 1, lpszString + (nLength - nCount));
		_tcscpy_s(lpszString, nLength + 1, lpszSub);
		delete [] lpszSub;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL MidX(LPTSTR lpszString, int iFirst, int nCount)
{
	ASSERT(lpszString != NULL);
	ASSERT(iFirst >= 0);
	ASSERT(nCount >= 0);

	int	nLength	= _tcslen(lpszString);

	if (iFirst >= nLength)
	{
		iFirst	= 0;
	}

	if ((iFirst + nCount) > nLength)
	{
		nCount	= nLength - iFirst;
	}

	LPTSTR	lpszSub	= new TCHAR[nCount + 1];

	if (lpszSub != NULL)
	{
		_tcsncpy_s(lpszSub, nCount + 1, lpszString + iFirst, nCount);

		lpszSub[nCount]	= NULL;

		_tcscpy_s(lpszString, nLength + 1, lpszSub);
		delete [] lpszSub;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL MidX(LPTSTR lpszString, int iFirst)
{
	ASSERT(lpszString != NULL);
	ASSERT(iFirst >= 0);

	int	nLength	= _tcslen(lpszString);

	if (iFirst >= nLength)
	{
		iFirst	= 0;
	}

	LPTSTR	lpszSub	= new TCHAR[nLength - iFirst + 1];

	if (lpszSub != NULL)
	{
		_tcscpy_s(lpszSub, nLength - iFirst + 1, lpszString + iFirst);
		_tcscpy_s(lpszString, nLength + 1, lpszSub);
		delete [] lpszSub;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL LeftX(LPTSTR lpszTarget, int nSize, LPCTSTR lpszString, int nCount)
{
	ASSERT(lpszTarget != NULL);
	ASSERT(lpszString != NULL);
	ASSERT(nCount >= 0);

	int	nLength	= _tcslen(lpszString);

	if (nCount > nLength)
	{
		nCount	= nLength;
	}

	LPTSTR	lpszSub	= new TCHAR[nCount + 1];

	if (lpszSub != NULL)
	{
		_tcsncpy_s(lpszSub, nCount + 1, lpszString, nCount);

		lpszSub[nCount]	= NULL;

		_tcscpy_s(lpszTarget, nSize, lpszSub);
		delete [] lpszSub;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL RightX(LPTSTR lpszTarget, int nSize, LPCTSTR lpszString, int nCount)
{
	ASSERT(lpszTarget != NULL);
	ASSERT(lpszString != NULL);
	ASSERT(nCount >= 0);

	int	nLength	= _tcslen(lpszString);

	if (nCount > nLength)
	{
		nCount	= nLength;
	}

	LPTSTR	lpszSub	= new TCHAR[nCount + 1];

	if (lpszSub != NULL)
	{
		_tcscpy_s(lpszSub, nCount + 1, lpszString + (nLength - nCount));
		_tcscpy_s(lpszTarget, nSize, lpszSub);
		delete [] lpszSub;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL MidX(LPTSTR lpszTarget, int nSize, LPCTSTR lpszString, int iFirst, int nCount)
{
	ASSERT(lpszTarget != NULL);
	ASSERT(lpszString != NULL);
	ASSERT(iFirst >= 0);
	ASSERT(nCount >= 0);

	int	nLength	= _tcslen(lpszString);

	if (iFirst >= nLength)
	{
		iFirst	= 0;
	}

	if ((iFirst + nCount) > nLength)
	{
		nCount	= nLength - iFirst;
	}

	LPTSTR	lpszSub	= new TCHAR[nCount + 1];

	if (lpszSub != NULL)
	{
		_tcsncpy_s(lpszSub, nCount + 1, lpszString + iFirst, nCount);

		lpszSub[nCount]	= NULL;

		_tcscpy_s(lpszTarget, nSize, lpszSub);
		delete [] lpszSub;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL MidX(LPTSTR lpszTarget, int nSize, LPCTSTR lpszString, int iFirst)
{
	ASSERT(lpszTarget != NULL);
	ASSERT(lpszString != NULL);
	ASSERT(iFirst >= 0);

	int	nLength	= _tcslen(lpszString);

	if (iFirst >= nLength)
	{
		iFirst	= 0;
	}

	LPTSTR	lpszSub	= new TCHAR[nLength - iFirst + 1];

	if (lpszSub != NULL)
	{
		_tcscpy_s(lpszSub, nLength - iFirst + 1, lpszString + iFirst);
		_tcscpy_s(lpszTarget, nSize, lpszSub);
		delete [] lpszSub;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

int InsertX(LPTSTR lpszString, int nSize, int iIndex, LPCTSTR lpszSub)
{
	ASSERT(lpszString != NULL);
	ASSERT(iIndex >= 0);

	int	nLength		= _tcslen(lpszString);
	int	nLengthSub	= _tcslen(lpszSub);

	if (iIndex > nLength)
	{
		return nLength;
	}

	LPTSTR	lpszBuffer	= new TCHAR[nLength + nLengthSub + 1];

	if (lpszBuffer != NULL)
	{
		_tcsncpy_s(lpszBuffer, nLength + nLengthSub + 1, lpszString, iIndex);

		lpszBuffer[iIndex]	= NULL;

		_tcscat_s(lpszBuffer, nLength + nLengthSub + 1, lpszSub);
		_tcscat_s(lpszBuffer, nLength + nLengthSub + 1, lpszString + iIndex);

		lpszBuffer[nLength + nLengthSub]	= NULL;

		_tcscpy_s(lpszString, nSize, lpszBuffer);
	}
	else
	{
		return nLength;
	}

	return _tcslen(lpszString);
}

int InsertX(LPTSTR lpszString, int nSize, int iIndex, TCHAR ch)
{
	ASSERT(lpszString != NULL);
	ASSERT(iIndex >= 0);

	int	nLength	= _tcslen(lpszString);

	if (iIndex > nLength)
	{
		return nLength;
	}

	LPTSTR	lpszBuffer	= new TCHAR[nLength + 2];

	if (lpszBuffer != NULL)
	{
		_tcsncpy_s(lpszBuffer, nLength + 2, lpszString, iIndex);

		lpszBuffer[iIndex]		= ch;
		lpszBuffer[iIndex + 1]	= NULL;

		_tcscat_s(lpszBuffer, nLength + 2, lpszString + iIndex);

		lpszBuffer[nLength + 1]	= NULL;

		_tcscpy_s(lpszString, nSize, lpszBuffer);
	}
	else
	{
		return nLength;
	}

	return _tcslen(lpszString);
}

int DeleteX(LPTSTR lpszString, int iIndex, int nCount)
{
	ASSERT(lpszString != NULL);
	ASSERT(iIndex >= 0);
	ASSERT(nCount >= 0);

	int	nLength	= _tcslen(lpszString);

	if (iIndex >= nLength)
	{
		return nLength;
	}

	if ((iIndex + nCount) > nLength)
	{
		return nLength;
	}

	LPTSTR	lpszBuffer	= new TCHAR[nLength - nCount + 1];

	if (lpszBuffer != NULL)
	{
		if (iIndex == 0)
		{
			_tcscpy_s(lpszBuffer, nLength - nCount + 1, lpszString + (iIndex + nCount));
		}
		else
		{
			_tcsncpy_s(lpszBuffer, nLength - nCount + 1, lpszString, iIndex);

			lpszBuffer[iIndex]	= NULL;

			_tcscat_s(lpszBuffer, nLength - nCount + 1, lpszString + (iIndex + nCount));
		}

		lpszBuffer[nLength - nCount]	= NULL;

		_tcscpy_s(lpszString, nLength + 1, lpszBuffer);
		delete lpszBuffer;
	}
	else
	{
		return nLength;
	}

	return _tcslen(lpszString);
}

int ReplaceX(LPTSTR lpszString, int nSize, LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	ASSERT(lpszString != NULL);

	int	nCount		= 0;
	int	nPos		= 0;
	int	nLengthOld	= _tcslen(lpszOld);
	int	nLengthNew	= _tcslen(lpszNew);

	while (FindX(lpszString, lpszOld, nPos) >= 0)
	{
		nPos	= FindX(lpszString, lpszOld, nPos);

		DeleteX(lpszString, nPos, nLengthOld);
		InsertX(lpszString, nSize, nPos, lpszNew);

		nPos	+= nLengthNew;
		nCount++;
	}

	return nCount;
}

int	ReplaceX(LPTSTR lpszString, TCHAR chOld, TCHAR chNew)
{
	ASSERT(lpszString != NULL);

	int	nLength	= _tcslen(lpszString);
	int	nCount	= 0;

	for (int i = 0; i < nLength; i++)
	{
		if (lpszString[i] == chOld)
		{
			lpszString[i]	= chNew;
			nCount++;
		}
	}

	return nCount;
}

int FindX(LPCTSTR lpszString, LPCTSTR lpszSub, int iStart)
{
	ASSERT(lpszString != NULL);
	ASSERT(iStart >= 0);

	int	nLength	= _tcslen(lpszString);

	if (iStart >= nLength)
	{
		iStart	= 0;
	}

	LPTSTR	lpszFound	= _tcsstr(lpszString + iStart, lpszSub);
	int		nPos;

	if (lpszFound == NULL)
	{
		nPos	= -1;
	}
	else
	{
		nPos	= lpszFound - lpszString;
	}

	return nPos;
}

int FindX(LPCTSTR lpszString, TCHAR ch, int iStart)
{
	ASSERT(lpszString != NULL);
	ASSERT(iStart >= 0);

	int	nLength	= _tcslen(lpszString);

	if (iStart >= nLength)
	{
		iStart	= 0;
	}

	LPTSTR	lpszFound	= _tcschr(lpszString + iStart, ch);
	int		nPos;

	if (lpszFound == NULL)
	{
		nPos	= -1;
	}
	else
	{
		nPos	= lpszFound - lpszString;
	}

	return nPos;
}

int ReverseFindX(LPCTSTR lpszString, LPCTSTR lpszSub)
{
	ASSERT(lpszString != NULL);

	int		nLength		= _tcslen(lpszString);
	int		nLengthSub	= _tcslen(lpszSub);
	BOOL	bFound		= FALSE;

	ASSERT(nLengthSub > 0);

	for (int i = (nLength - nLengthSub); i >= 0; i--)
	{
		if (lpszString[i] == lpszSub[0])
		{
			int	nPos	= 0;

			for (int j = i; j < (i + nLengthSub); j++)
			{
				if (lpszString[j] == lpszSub[nPos++])
				{
					bFound	= TRUE;
				}
				else
				{
					bFound	= FALSE;

					break;
				}
			}

			if (bFound == TRUE)
			{
				return i;
			}
		}
	}

	return -1;
}

int ReverseFindX(LPCTSTR lpszString, TCHAR ch)
{
	ASSERT(lpszString != NULL);

	int	nLength	= _tcslen(lpszString);

	LPTSTR	lpszFound	= _tcsrchr(lpszString, ch);
	int		nPos;

	if (lpszFound == NULL)
	{
		nPos	= -1;
	}
	else
	{
		nPos	= lpszFound - lpszString;
	}

	return nPos;
}

void TrimX(LPTSTR lpszString, TCHAR chTarget)
{
	ASSERT(lpszString != NULL);
	TrimLeftX(lpszString, chTarget);
	TrimRightX(lpszString, chTarget);
}

void TrimX(LPTSTR lpszString, LPCTSTR lpszTargets)
{
	ASSERT(lpszString != NULL);
	TrimLeftX(lpszString, lpszTargets);
	TrimRightX(lpszString, lpszTargets);
}

void TrimX(LPTSTR lpszString)
{
	ASSERT(lpszString != NULL);
	TrimLeftX(lpszString);
	TrimRightX(lpszString);
}

void TrimLeftX(LPTSTR lpszString, TCHAR chTarget)
{
	ASSERT(lpszString != NULL);

	while (FindX(lpszString, chTarget) == 0)
	{
		DeleteX(lpszString, FindX(lpszString, chTarget));
	}
}

void TrimLeftX(LPTSTR lpszString, LPCTSTR lpszTargets)
{
	ASSERT(lpszString != NULL);

	int	nLength	= _tcslen(lpszTargets);

	while (FindX(lpszString, lpszTargets) == 0)
	{
		DeleteX(lpszString, 0, nLength);
	}
}

void TrimLeftX(LPTSTR lpszString)
{
	ASSERT(lpszString != NULL);
	TrimLeftX(lpszString, _T(' '));
	TrimLeftX(lpszString, _T('\t'));
	TrimLeftX(lpszString, _T('\r'));
	TrimLeftX(lpszString, _T('\n'));
}

void TrimRightX(LPTSTR lpszString, TCHAR chTarget)
{
	ASSERT(lpszString != NULL);

	if (_tcslen(lpszString))
	{
		while (ReverseFindX(lpszString, chTarget) == (_tcslen(lpszString) - 1))
		{
			DeleteX(lpszString, ReverseFindX(lpszString, chTarget));
		}
	}
}

void TrimRightX(LPTSTR lpszString, LPCTSTR lpszTargets)
{
	ASSERT(lpszString != NULL);
	ASSERT(lpszTargets != NULL);

	int	nLength		= _tcslen(lpszString);
	int	nLengthSub	= _tcslen(lpszTargets);

	if (nLengthSub > nLength)
	{
		return;
	}

	while (ReverseFindX(lpszString, lpszTargets) == (nLength - nLengthSub))
	{
		DeleteX(lpszString, ReverseFindX(lpszString, lpszTargets), nLengthSub);
	}
}

void TrimRightX(LPTSTR lpszString)
{
	ASSERT(lpszString != NULL);
	TrimRightX(lpszString, _T('\n'));
	TrimRightX(lpszString, _T('\r'));
	TrimRightX(lpszString, _T('\t'));
	TrimRightX(lpszString, _T(' '));
}
