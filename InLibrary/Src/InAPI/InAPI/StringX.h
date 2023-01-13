// StringX.h

#pragma once

#include "InAPI.h"

// String Manipulating Functions
BOOL	INAPI_API	LeftX(LPTSTR lpszString, int nCount);
BOOL	INAPI_API	RightX(LPTSTR lpszString, int nCount);
BOOL	INAPI_API	MidX(LPTSTR lpszString, int iFirst, int nCount);
BOOL	INAPI_API	MidX(LPTSTR lpszString, int iFirst);

BOOL	INAPI_API	LeftX(LPTSTR lpszTarget, int nSize, LPCTSTR lpszString, int nCount);
BOOL	INAPI_API	RightX(LPTSTR lpszTarget, int nSize, LPCTSTR lpszString, int nCount);
BOOL	INAPI_API	MidX(LPTSTR lpszTarget, int nSize, LPCTSTR lpszString, int iFirst, int nCount);
BOOL	INAPI_API	MidX(LPTSTR lpszTarget, int nSize, LPCTSTR lpszString, int iFirst);

int		INAPI_API	InsertX(LPTSTR lpszString, int nSize, int iIndex, LPCTSTR lpszSub);
int		INAPI_API	InsertX(LPTSTR lpszString, int nSize, int iIndex, TCHAR ch);
int		INAPI_API	DeleteX(LPTSTR lpszString, int iIndex, int nCount = 1);

int		INAPI_API	ReplaceX(LPTSTR lpszString, int nSize, LPCTSTR lpszOld, LPCTSTR lpszNew);
int		INAPI_API	ReplaceX(LPTSTR lpszString, TCHAR chOld, TCHAR chNew);

// String Finding Functions
int		INAPI_API	FindX(LPCTSTR lpszString, LPCTSTR lpszSub, int iStart = 0);
int		INAPI_API	FindX(LPCTSTR lpszString, TCHAR ch, int iStart = 0);
int		INAPI_API	ReverseFindX(LPCTSTR lpszString, LPCTSTR lpszSub);
int		INAPI_API	ReverseFindX(LPCTSTR lpszString, TCHAR ch);

// String Trimming Functions
void	INAPI_API	TrimX(LPTSTR lpszString, TCHAR chTarget);
void	INAPI_API	TrimX(LPTSTR lpszString, LPCTSTR lpszTargets);
void	INAPI_API	TrimX(LPTSTR lpszString);

void	INAPI_API	TrimLeftX(LPTSTR lpszString, TCHAR chTarget);
void	INAPI_API	TrimLeftX(LPTSTR lpszString, LPCTSTR lpszTargets);
void	INAPI_API	TrimLeftX(LPTSTR lpszString);

void	INAPI_API	TrimRightX(LPTSTR lpszString, TCHAR chTarget);
void	INAPI_API	TrimRightX(LPTSTR lpszString, LPCTSTR lpszTargets);
void	INAPI_API	TrimRightX(LPTSTR lpszString);
