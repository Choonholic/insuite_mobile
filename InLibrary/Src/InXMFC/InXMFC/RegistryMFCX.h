// RegistryMFCX.h

#pragma once

#ifndef REGFIND_NONE
#define REGFIND_NONE	0x0000
#define REGFIND_KEY		0x0001
#define REGFIND_VALUE	0x0002
#define REGFIND_DATA	0x0003
#endif

#include "RegistryX.h"

class AFX_EXT_CLASS CRegistryMFCX : public CRegistryX
{
public:
	CRegistryMFCX(void);
	CRegistryMFCX(HKEY hKeyParent, LPCTSTR lpszSubKey, BOOL bCreateIfNew = TRUE);
	CRegistryMFCX(CRegistryMFCX& rSrc);
	virtual	~CRegistryMFCX(void);

	CString	ReadString(LPCTSTR lpszKey, LPCTSTR lpszDefault = _T(""));
	BOOL	ReadMultiString(LPCTSTR lpszKey, CStringArray& arrValues);
	CString	ReadExpandString(LPCTSTR lpszKey, LPCTSTR lpszDefault = _T(""));

	BOOL	WriteMultiString(LPCTSTR lpszKey, CStringArray& arrValues);

	BOOL	GetFirstSubKey(CString& strSubKey);
	BOOL	GetNextSubKey(CString& strSubKey);
	BOOL	GetSubKey(int nIndex, CString& strSubKey);

	BOOL	GetFirstValue(CString& strValueName, LPDWORD lpdwType = NULL);
	BOOL	GetNextValue(CString& strValueName, LPDWORD lpdwType = NULL);
	BOOL	GetValue(int nIndex, CString& strValueName, LPDWORD lpdwType = NULL);

	BOOL	GetFirstOrderedSubKey(CString& strSubKey);
	BOOL	GetNextOrderedSubKey(CString& strSubKey);
	BOOL	GetOrderedSubKey(int nIndex, CString& strSubKey);
	void	ClearOrderedKeyInfo(void);

	BOOL	GetFirstOrderedValue(CString& strValueName, LPDWORD lpdwType = NULL);
	BOOL	GetNextOrderedValue(CString& strValueName, LPDWORD lpdwType = NULL);
	BOOL	GetOrderedValue(int nIndex, CString& strValueName, LPDWORD lpdwType = NULL);
	void	ClearOrderedValueInfo(void);

	void	SetFindOptions(BOOL bKeys = TRUE, BOOL bValues = TRUE, BOOL bData = TRUE, BOOL bMatchOnly = FALSE, BOOL bCaseSensitive = FALSE);
	LPCTSTR	GetFindString(void);
	int		BuildFindList(LPCTSTR lpszString);
	void	ResetFindResults(void);

	CDWordArray		m_arrFRType;
	CStringArray	m_arrFRSubKey;
	CStringArray	m_arrFRValue;
	CStringArray	m_arrFRData;

protected:
	void	FindKeys(HKEY hKey, LPCTSTR lpszPath);
	void	FindValues(HKEY hKey, LPCTSTR lpszPath);
	void	FindFormatValue(LPBYTE lpValue, DWORD dwValueLen, DWORD dwType, LPTSTR szProcessed, int nSize);

	CStringArray	m_arrOrderedKeyName;
	CStringArray	m_arrOrderedValueName;
	CDWordArray		m_arrOrderedValueType;

	CString			m_strFindString;
	CString			m_strFindStringUpper;

	BOOL	m_bFOKeys;
	BOOL	m_bFOValues;
	BOOL	m_bFOData;
	BOOL	m_bFOMatchOnly;
	BOOL	m_bFOCaseSensitive;
};
