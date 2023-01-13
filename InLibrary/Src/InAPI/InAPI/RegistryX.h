// RegistryX.h

#pragma once

#include "InAPI.h"

#ifndef REG_MAX_DEPTH
#define REG_MAX_DEPTH		16
#define REG_MAX_KEYLEN		280
#define	REG_MAX_VALUELEN	4096
#endif

class INAPI_CLASS CRegistryX
{
public:
	CRegistryX(void);
	CRegistryX(HKEY hKeyParent, LPCTSTR lpszSubKey, BOOL bCreateIfNew = TRUE);
	CRegistryX(CRegistryX& rSrc);
	virtual	~CRegistryX(void);

	BOOL	Open(HKEY hKeyParent, LPCTSTR lpszSubKey, BOOL bCreateIfNew = TRUE);
	BOOL	Flush(void);
	void	FlushAll(void);
	BOOL	Close(void);

	BOOL	ReadBinary(LPCTSTR lpszKey, LPBYTE lpData, LPDWORD lpcbData);
	DWORD	ReadDWORD(LPCTSTR lpszKey, DWORD dwDefault = 0xDEF0);
	BOOL	ReadString(LPCTSTR lpszKey, LPTSTR lpszValue, LPDWORD lpcbValue = NULL);
	BOOL	ReadMultiString(LPCTSTR lpszKey, LPBYTE lpValue, LPDWORD lpcbValue);
	BOOL	ReadExpandString(LPCTSTR lpszKey, LPTSTR lpszValue, LPDWORD lpcbValue = NULL);

	BOOL	WriteBinary(LPCTSTR lpszKey, LPBYTE lpData, DWORD cbData);
	BOOL	WriteDWORD(LPCTSTR lpszKey, DWORD dwValue);
	BOOL	WriteString(LPCTSTR lpszKey, LPCTSTR lpszValue);
	BOOL	WriteMultiString(LPCTSTR lpszKey, LPBYTE lpData, DWORD cbData);
	BOOL	WriteExpandString(LPCTSTR lpszKey, LPCTSTR lpszValue);

	int		GetSubKeyCount(void);

	BOOL	GetFirstSubKey(LPTSTR lpszSubKey, LPDWORD lpcbName);
	BOOL	GetNextSubKey(LPTSTR lpszSubKey, LPDWORD lpcbName);
	BOOL	GetSubKey(int nIndex, LPTSTR lpszSubKey, LPDWORD lpcbName);

	BOOL	IsSubKeyExist(LPCTSTR lpszSubKey);

	BOOL	DeleteKey(HKEY hKeyParent, LPCTSTR lpszSubKey);
	BOOL	DeleteSubKey(LPCTSTR lpszSubKey);

	int		GetValueCount(void);

	BOOL	GetFirstValue(LPTSTR lpszValueName, LPDWORD lpcbName, LPDWORD lpdwType = NULL);
	BOOL	GetNextValue(LPTSTR lpszValueName, LPDWORD lpcbName, LPDWORD lpdwType = NULL);
	BOOL	GetValue(int nIndex, LPTSTR lpszValueName, LPDWORD lpcbName, LPDWORD lpdwType = NULL);

	BOOL	IsValueExist(LPCTSTR lpszValueName);
	BOOL	GetValueType(LPCTSTR lpszValueName, LPDWORD lpdwType);

	BOOL	DeleteValue(LPCTSTR lpszValueName);

	HKEY	GetKey(void);

	BOOL	Copy(HKEY hKeyRootFrom, LPCTSTR lpszSubkeyFrom, HKEY hKeyRootTo, LPCTSTR lpszSubKeyTo);
	BOOL	Move(HKEY hKeyRootFrom, LPCTSTR lpszSubkeyFrom, HKEY hKeyRootTo, LPCTSTR lpszSubKeyTo);

	DWORD	GetExportType(LPCTSTR lpszFileName);

	BOOL	IRI1Export(LPCTSTR lpszFileName, BOOL bRecursiveSubKeys = TRUE, BOOL bAlwaysNew = TRUE);
	BOOL	IRI1Import(LPCTSTR lpszFileName);

	BOOL	REG5Export(LPCTSTR lpszFileName, BOOL bRecursiveSubKeys = TRUE, BOOL bAlwaysNew = TRUE);
	BOOL	REG5Import(LPCTSTR lpszFileName);

protected:
	BOOL	DeleteSubKeyInternal(HKEY hKey, LPCTSTR lpszSubKey);

	void	CopyKeys(HKEY hKeyFrom, HKEY hKeyTo);
	void	CopyValues(HKEY hKeyFrom, HKEY hKeyTo);

	void	IRI1ExportKeys(HKEY hKey, LPCTSTR lpszPath, HANDLE hHandle, BOOL bRecursiveSubKeys);
	void	IRI1ExportValues(HKEY hKey, HANDLE hHandle);
	void	IRI1FormatValue(LPBYTE lpValue, DWORD dwValueLen, DWORD dwType, LPTSTR szProcessed, int nSize);

	void	REG5ExportKeys(HKEY hKey, LPCTSTR szPath, HANDLE hHandle, BOOL bRecursiveSubKeys);
	void	REG5ExportValues(HKEY hKey, HANDLE hHandle);
	void	REG5FormatValue(LPBYTE lpValue, DWORD dwValueLen, DWORD dwType, LPTSTR szProcessed, int nSize);

	HKEY	m_hKey;
	HKEY	m_hKeyParent;
	DWORD	m_dwDisposition;

	int		m_nSubKeyIndex;
	int		m_nValueIndex;

	TCHAR	m_szSubKey[REG_MAX_KEYLEN + 1];
	TCHAR	m_szSubKeyTop[REG_MAX_KEYLEN + 1];
};
