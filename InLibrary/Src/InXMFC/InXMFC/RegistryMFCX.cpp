// RegistryMFCX.cpp

#include "StdAfx.h"
#include "RegistryMFCX.h"

CRegistryMFCX::CRegistryMFCX(void)
{
	SetFindOptions();
}

CRegistryMFCX::CRegistryMFCX(HKEY hKeyParent, LPCTSTR lpszSubKey, BOOL bCreateIfNew) : CRegistryX(hKeyParent, lpszSubKey, bCreateIfNew)
{
	SetFindOptions();
}

CRegistryMFCX::CRegistryMFCX(CRegistryMFCX& rSrc) : CRegistryX(rSrc)
{
	SetFindOptions();
}

CRegistryMFCX::~CRegistryMFCX(void)
{
	CRegistryX::~CRegistryX();
}

CString CRegistryMFCX::ReadString(LPCTSTR lpszKey, LPCTSTR lpszDefault)
{
	if (!AfxIsValidString(lpszKey))
	{
		return _T("");
	}

	DWORD	dwDataSize	= 2048;
	DWORD	dwType		= REG_SZ;
	TCHAR	szValue[2048];

	memset(szValue, 0, sizeof(TCHAR) * 2048);

	if (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, (LPBYTE)szValue, &dwDataSize) == ERROR_SUCCESS)
	{
		CString	strValue	= (LPCTSTR)szValue;

		return strValue;
	}

	if (AfxIsValidString(lpszDefault))
	{
		WriteString(lpszKey, lpszDefault);
		return lpszDefault;
	}

	return _T("");
}

BOOL CRegistryMFCX::ReadMultiString(LPCTSTR lpszKey, CStringArray& arrValues)
{
	arrValues.RemoveAll();

	if (!AfxIsValidString(lpszKey))
	{
		return FALSE;
	}

	DWORD	dwDataSize	= 0;
	DWORD	dwType		= REG_MULTI_SZ;

	if (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, (LPBYTE)NULL, &dwDataSize) == ERROR_SUCCESS)
	{
		TCHAR	cData[4096];

		memset(cData, 0, sizeof(TCHAR) * 4096);

		if (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, (LPBYTE)cData, &dwDataSize) == ERROR_SUCCESS)
		{
			CString	strItem	= _T("");

			dwDataSize	= dwDataSize / sizeof(TCHAR);

			for (int i = 0; i < (int)(dwDataSize - 1); i++)
			{
				if (cData[i] != NULL)
				{
					strItem	+= cData[i];
				}
				else
				{
					arrValues.Add(strItem);

					strItem	= _T("");
				}
			}

			return TRUE;
		}
	}

	return FALSE;;
}

CString CRegistryMFCX::ReadExpandString(LPCTSTR lpszKey, LPCTSTR lpszDefault)
{
	if (!AfxIsValidString(lpszKey))
	{
		return _T("");
	}

	DWORD	dwDataSize	= 2048;
	DWORD	dwType		= REG_EXPAND_SZ;
	TCHAR	szValue[2048];

	memset(szValue, 0, sizeof(TCHAR) * 2048);

	if (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, (LPBYTE)szValue, &dwDataSize) == ERROR_SUCCESS)
	{
		CString	strValue	= (LPCTSTR)szValue;

		return strValue;
	}

	if (AfxIsValidString(lpszDefault))
	{
		WriteExpandString(lpszKey, lpszDefault);
		return lpszDefault;
	}

	return _T("");
}

BOOL CRegistryMFCX::WriteMultiString(LPCTSTR lpszKey, CStringArray& arrValues)
{
	if (!AfxIsValidString(lpszKey))
	{
		return FALSE;
	}

	BYTE	cData[4096];
	DWORD	cbData	= 0;
	DWORD	dwSize;

	memset(cData, 0, sizeof(BYTE) * 4096);

	for (int i = 0; i < arrValues.GetSize(); i++)
	{
		dwSize	= arrValues.GetAt(i).GetLength() * sizeof(TCHAR);

		memcpy_s(cData + cbData, 4096 - cbData, (LPBYTE)(arrValues.GetAt(i).GetBuffer(arrValues.GetAt(i).GetLength())), dwSize);

		cbData	+= dwSize + sizeof(TCHAR);
	}

	cbData	+= sizeof(TCHAR);

	return (RegSetValueEx(m_hKey, lpszKey, 0, REG_MULTI_SZ, (LPBYTE)cData, cbData) == ERROR_SUCCESS);
}

BOOL CRegistryMFCX::GetFirstSubKey(CString& strSubKey)
{
	DWORD	dwNameSize	= MAX_PATH;

	m_nSubKeyIndex	= 0;

	if (RegEnumKeyEx(m_hKey, m_nSubKeyIndex, strSubKey.GetBuffer(dwNameSize), &dwNameSize, 0, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		strSubKey.ReleaseBuffer();

		strSubKey	= _T("");

		return FALSE;
	}

	strSubKey.ReleaseBuffer();
	return TRUE;
}

BOOL CRegistryMFCX::GetNextSubKey(CString& strSubKey)
{
	DWORD	dwNameSize	= MAX_PATH;

	m_nSubKeyIndex++;

	if (RegEnumKeyEx(m_hKey, m_nSubKeyIndex, strSubKey.GetBuffer(dwNameSize), &dwNameSize, 0, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		strSubKey.ReleaseBuffer();

		strSubKey	= _T("");

		return FALSE;
	}

	strSubKey.ReleaseBuffer();
	return TRUE;
}

BOOL CRegistryMFCX::GetSubKey(int nIndex, CString& strSubKey)
{
	DWORD	dwNameSize	= MAX_PATH;
	LONG	rc;

	m_nSubKeyIndex	= nIndex;
	rc				= RegEnumKeyEx(m_hKey, m_nSubKeyIndex, strSubKey.GetBuffer(dwNameSize), &dwNameSize, 0, NULL, NULL, NULL);

	strSubKey.ReleaseBuffer();

	if (rc != ERROR_SUCCESS)
	{
		if (rc != ERROR_NO_MORE_ITEMS)
		{
		}

		strSubKey	= _T("");

		return FALSE;
	}

	return TRUE;
}

BOOL CRegistryMFCX::GetFirstValue(CString& strValueName, LPDWORD lpdwType)
{
	DWORD	dwNameSize	= MAX_PATH;

	m_nValueIndex	= 0;

	if (RegEnumValue(m_hKey, m_nValueIndex, strValueName.GetBuffer(dwNameSize), &dwNameSize, 0, lpdwType, NULL, NULL) != ERROR_SUCCESS)
	{
		strValueName.ReleaseBuffer();

		strValueName	= _T("");

		if (lpdwType != NULL)
		{
			*lpdwType	= 0;
		}

		return FALSE;
	}

	strValueName.ReleaseBuffer();
	return TRUE;
}

BOOL CRegistryMFCX::GetNextValue(CString& strValueName, LPDWORD lpdwType)
{
	DWORD	dwNameSize	= MAX_PATH;

	m_nValueIndex++;

	if (RegEnumValue(m_hKey, m_nValueIndex, strValueName.GetBuffer(dwNameSize), &dwNameSize, 0, lpdwType, NULL, NULL) != ERROR_SUCCESS)
	{
		strValueName.ReleaseBuffer();

		strValueName	= _T("");

		if (lpdwType != NULL)
		{
			*lpdwType	= 0;
		}

		return FALSE;
	}

	strValueName.ReleaseBuffer();
	return TRUE;
}

BOOL CRegistryMFCX::GetValue(int nIndex, CString& strValueName, LPDWORD lpdwType)
{
	DWORD	dwNameSize	= MAX_PATH;
	LONG	rc;

	m_nValueIndex	= nIndex;
	rc				= RegEnumValue(m_hKey, m_nValueIndex, strValueName.GetBuffer(dwNameSize), &dwNameSize, 0, NULL, NULL, NULL);

	strValueName.ReleaseBuffer();

	if (rc != ERROR_SUCCESS)
	{
		if (rc != ERROR_NO_MORE_ITEMS)
		{
		}

		strValueName	= _T("");

		if (lpdwType != NULL)
		{
			*lpdwType	= 0;
		}

		return FALSE;
	}

	return TRUE;
}

BOOL CRegistryMFCX::GetFirstOrderedSubKey(CString& strSubKey)
{
	CString	strInternalSubKey;
	int		i;
	int		nCount;

	ClearOrderedKeyInfo();

	if (GetFirstSubKey(strInternalSubKey))
	{
		do
		{
			nCount	= m_arrOrderedKeyName.GetSize();

			for (i = 0; i < nCount; i++)
			{
				if (strInternalSubKey.Compare(m_arrOrderedKeyName.GetAt(i)) < 0)
				{
					break;
				}
			}

			if (i < nCount)
			{
				m_arrOrderedKeyName.InsertAt(i, strInternalSubKey);
			}
			else
			{
				m_arrOrderedKeyName.Add(strInternalSubKey);
			}
		} while (GetNextSubKey(strInternalSubKey));
	}
	else
	{
		strSubKey	= _T("");

		return FALSE;
	}

	strSubKey	= m_arrOrderedKeyName.GetAt(m_nSubKeyIndex);

	return TRUE;
}

BOOL CRegistryMFCX::GetNextOrderedSubKey(CString& strSubKey)
{
	m_nSubKeyIndex++;

	if ((m_nSubKeyIndex < 0) || (m_nSubKeyIndex >= m_arrOrderedKeyName.GetSize()))
	{
		strSubKey	= _T("");

		return FALSE;
	}

	strSubKey	= m_arrOrderedKeyName.GetAt(m_nSubKeyIndex);

	return TRUE;
}

BOOL CRegistryMFCX::GetOrderedSubKey(int nIndex, CString& strSubKey)
{
	m_nSubKeyIndex	= nIndex;

	if ((m_nSubKeyIndex < 0) || (m_nSubKeyIndex >= m_arrOrderedKeyName.GetSize()))
	{
		strSubKey	= _T("");

		return FALSE;
	}

	strSubKey	= m_arrOrderedKeyName.GetAt(m_nSubKeyIndex);

	return TRUE;
}

void CRegistryMFCX::ClearOrderedKeyInfo(void)
{
	m_arrOrderedKeyName.RemoveAll();

	m_nSubKeyIndex	= 0;
}

BOOL CRegistryMFCX::GetFirstOrderedValue(CString& strValueName, LPDWORD lpdwType)
{
	CString	strInternalValueName;
	DWORD	dwType;
	int		i;
	int		nCount;

	ClearOrderedValueInfo();

	if (GetFirstValue(strInternalValueName, &dwType))
	{
		do
		{
			nCount	= m_arrOrderedValueName.GetSize();

			for (i = 0; i < nCount; i++)
			{
				if (strInternalValueName.Compare(m_arrOrderedValueName.GetAt(i)) < 0)
				{
					break;
				}
			}

			if (i < nCount)
			{
				m_arrOrderedValueName.InsertAt(i, strInternalValueName);
				m_arrOrderedValueType.InsertAt(i, dwType);
			}
			else
			{
				m_arrOrderedValueName.Add(strInternalValueName);
				m_arrOrderedValueType.Add(dwType);
			}
		} while (GetNextValue(strInternalValueName, &dwType));
	}
	else
	{
		strValueName	= _T("");

		if (lpdwType != NULL)
		{
			*lpdwType	= 0;
		}

		return FALSE;
	}

	strValueName	= m_arrOrderedValueName.GetAt(m_nValueIndex);

	if (lpdwType != NULL)
	{
		*lpdwType	= m_arrOrderedValueType.GetAt(m_nValueIndex);
	}

	return TRUE;
}

BOOL CRegistryMFCX::GetNextOrderedValue(CString& strValueName, LPDWORD lpdwType)
{
	m_nValueIndex++;

	if ((m_nValueIndex < 0) || (m_nValueIndex >= m_arrOrderedValueName.GetSize()))
	{
		strValueName	= _T("");

		if (lpdwType != NULL)
		{
			*lpdwType	= 0;
		}

		return FALSE;
	}

	strValueName	= m_arrOrderedValueName.GetAt(m_nValueIndex);

	if (lpdwType != NULL)
	{
		*lpdwType	= m_arrOrderedValueType.GetAt(m_nValueIndex);
	}

	return TRUE;
}

BOOL CRegistryMFCX::GetOrderedValue(int nIndex, CString& strValueName, LPDWORD lpdwType)
{
	m_nValueIndex	= nIndex;

	if ((m_nValueIndex < 0) || (m_nValueIndex >= m_arrOrderedValueName.GetSize()))
	{
		strValueName	= _T("");

		if (lpdwType != NULL)
		{
			*lpdwType	= 0;
		}

		return FALSE;
	}

	strValueName	= m_arrOrderedValueName.GetAt(m_nValueIndex);

	if (lpdwType != NULL)
	{
		*lpdwType	= m_arrOrderedValueType.GetAt(m_nValueIndex);
	}

	return TRUE;
}

void CRegistryMFCX::ClearOrderedValueInfo(void)
{
	m_arrOrderedValueName.RemoveAll();
	m_arrOrderedValueType.RemoveAll();

	m_nValueIndex	= 0;
}

void CRegistryMFCX::SetFindOptions(BOOL bKeys, BOOL bValues, BOOL bData, BOOL bMatchOnly, BOOL bCaseSensitive)
{
	m_bFOKeys			= bKeys;
	m_bFOValues			= bValues;
	m_bFOData			= bData;
	m_bFOMatchOnly		= bMatchOnly;
	m_bFOCaseSensitive	= bCaseSensitive;
}

LPCTSTR CRegistryMFCX::GetFindString(void)
{
	return (LPCTSTR)(m_strFindString);
}

int CRegistryMFCX::BuildFindList(LPCTSTR lpszString)
{
	HKEY	hKey;
	CString	strSubKey;

	ResetFindResults();

	m_strFindString			= lpszString;
	m_strFindStringUpper	= lpszString;

	m_strFindStringUpper.MakeUpper();

	// Find in HKEY_CLASSES_ROOT
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, _T(""), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		strSubKey	= _T("HKEY_CLASSES_ROOT");

		FindKeys(hKey, strSubKey);
		RegCloseKey(hKey);
	}

	// Find in HKEY_CURRENT_USER
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T(""), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		strSubKey	= _T("HKEY_CURRENT_USER");

		FindKeys(hKey, strSubKey);
		RegCloseKey(hKey);
	}

	// Find in HKEY_LOCAL_MACHINE
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(""), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		strSubKey	= _T("HKEY_LOCAL_MACHINE");

		FindKeys(hKey, strSubKey);
		RegCloseKey(hKey);
	}

	// Find in HKEY_USERS
	if (RegOpenKeyEx(HKEY_USERS, _T(""), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		strSubKey	= _T("HKEY_USERS");

		FindKeys(hKey, strSubKey);
		RegCloseKey(hKey);
	}

	return m_arrFRType.GetSize();
}

void CRegistryMFCX::ResetFindResults(void)
{
	m_strFindString		= _T("");

	if (m_arrFRType.GetSize() > 0)
	{
		m_arrFRType.RemoveAll();
	}

	if (m_arrFRSubKey.GetSize() > 0)
	{
		m_arrFRSubKey.RemoveAll();
	}

	if (m_arrFRValue.GetSize() > 0)
	{
		m_arrFRValue.RemoveAll();
	}

	if (m_arrFRData.GetSize() > 0)
	{
		m_arrFRData.RemoveAll();
	}
}

void CRegistryMFCX::FindKeys(HKEY hKey, LPCTSTR lpszPath)
{
	DWORD	dwDepth;
	HKEY	arrKey[REG_MAX_DEPTH];
	DWORD	arrIndex[REG_MAX_DEPTH];
	TCHAR	arrPath[REG_MAX_DEPTH][REG_MAX_KEYLEN];
	CString	strFindData;
	int		nPos;

	dwDepth				= 0;
	arrKey[dwDepth]		= hKey;
	arrIndex[dwDepth]	= 0;

	wcscpy_s(arrPath[dwDepth], REG_MAX_KEYLEN, lpszPath);

	do
	{
		if (arrIndex[dwDepth] == 0)
		{
			if (m_bFOKeys == TRUE)
			{
				strFindData	= arrPath[dwDepth];
				nPos		= strFindData.ReverseFind(_T('\\'));

				if (nPos >= 0)
				{
					strFindData	= strFindData.Mid(nPos + 1);
				}

				if (m_bFOMatchOnly == TRUE)
				{
					if (m_bFOCaseSensitive == TRUE)
					{
						if (strFindData.Compare(m_strFindString) == 0)
						{
							m_arrFRType.Add(REGFIND_KEY);
							m_arrFRSubKey.Add(arrPath[dwDepth]);
							m_arrFRValue.Add(_T(""));
							m_arrFRData.Add(_T(""));
						}
					}
					else
					{
						if (strFindData.CompareNoCase(m_strFindString) == 0)
						{
							m_arrFRType.Add(REGFIND_KEY);
							m_arrFRSubKey.Add(arrPath[dwDepth]);
							m_arrFRValue.Add(_T(""));
							m_arrFRData.Add(_T(""));
						}
					}
				}
				else
				{
					if (m_bFOCaseSensitive == TRUE)
					{
						if (strFindData.Find(m_strFindString) >= 0)
						{
							m_arrFRType.Add(REGFIND_KEY);
							m_arrFRSubKey.Add(arrPath[dwDepth]);
							m_arrFRValue.Add(_T(""));
							m_arrFRData.Add(_T(""));
						}
					}
					else
					{
						strFindData.MakeUpper();

						if (strFindData.Find(m_strFindStringUpper) >= 0)
						{
							m_arrFRType.Add(REGFIND_KEY);
							m_arrFRSubKey.Add(arrPath[dwDepth]);
							m_arrFRValue.Add(_T(""));
							m_arrFRData.Add(_T(""));
						}
					}
				}
			}

			FindValues(arrKey[dwDepth], arrPath[dwDepth]);
		}

		TCHAR	szSubKey[REG_MAX_KEYLEN + 1];
		DWORD	dwSize	= REG_MAX_KEYLEN;
		LONG	lResult	= RegEnumKeyEx(arrKey[dwDepth], arrIndex[dwDepth], szSubKey, &dwSize, NULL, NULL, NULL, NULL);

		if (lResult == ERROR_NO_MORE_ITEMS)
		{
			RegCloseKey(arrKey[dwDepth]);

			if (dwDepth == 0)
			{
				break;
			}

			dwDepth--;
		}
		else
		{
			wsprintf(arrPath[dwDepth + 1], _T("%s\\%s"), arrPath[dwDepth], szSubKey);
			RegOpenKeyEx(arrKey[dwDepth], szSubKey, 0, 0, &arrKey[dwDepth + 1]);

			arrIndex[dwDepth]++;
			dwDepth++;
			arrIndex[dwDepth] = 0;
		}
	} while (TRUE);
}

#define PROCESS_SIZE	(24 * 1024)		// 24 KB

void CRegistryMFCX::FindValues(HKEY hKey, LPCTSTR lpszPath)
{
	LONG	lResult;
	DWORD	dwValues, dwMaxNameLen, dwMaxValueLen;
	CString	strFindData;

	lResult	= RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &dwValues, &dwMaxNameLen, &dwMaxValueLen, NULL, NULL);

	if (lResult != ERROR_SUCCESS)
	{
		return;
	}

	LPTSTR	lpszName		= new TCHAR[dwMaxNameLen + 1];
	LPBYTE	lpValue			= new BYTE[dwMaxValueLen + 1];
	LPTSTR	lpszProcessed	= new TCHAR[PROCESS_SIZE];
	DWORD	dwType;
	DWORD	dwNameLen;
	DWORD	dwValueLen;

	for (DWORD dwIndex = 0; dwIndex < dwValues; dwIndex++)
	{
		dwType		= 0;
		dwNameLen	= dwMaxNameLen + 1;
		dwValueLen	= dwMaxValueLen + 1;

		memset(lpszName, 0, sizeof(TCHAR) * (dwMaxNameLen + 1));
		memset(lpValue, 0, sizeof(BYTE) * (dwMaxValueLen + 1));

		lResult	= RegEnumValue(hKey, dwIndex, lpszName, &dwNameLen, NULL, &dwType, lpValue, &dwValueLen);

		if (lResult == ERROR_NO_MORE_ITEMS)
		{
			break;
		}

		memset(lpszProcessed, 0, sizeof(TCHAR) * PROCESS_SIZE);
		FindFormatValue(lpValue, dwValueLen, dwType, lpszProcessed, PROCESS_SIZE);

		if ((_tcslen(lpszName) < 1) || (!_tcscmp(lpszName, _T("Default"))))
		{
			_tcscpy_s(lpszName, dwMaxNameLen + 1, _T(""));
		}

		if (m_bFOValues == TRUE)
		{
			strFindData	= lpszName;

			if (m_bFOMatchOnly == TRUE)
			{
				if (m_bFOCaseSensitive == TRUE)
				{
					if (strFindData.Compare(m_strFindString) == 0)
					{
						m_arrFRType.Add(REGFIND_VALUE);
						m_arrFRSubKey.Add(lpszPath);
						m_arrFRValue.Add(lpszName);
						m_arrFRData.Add(lpszProcessed);
					}
				}
				else
				{
					if (strFindData.CompareNoCase(m_strFindString) == 0)
					{
						m_arrFRType.Add(REGFIND_VALUE);
						m_arrFRSubKey.Add(lpszPath);
						m_arrFRValue.Add(lpszName);
						m_arrFRData.Add(lpszProcessed);
					}
				}
			}
			else
			{
				if (m_bFOCaseSensitive == TRUE)
				{
					if (strFindData.Find(m_strFindString) >= 0)
					{
						m_arrFRType.Add(REGFIND_VALUE);
						m_arrFRSubKey.Add(lpszPath);
						m_arrFRValue.Add(lpszName);
						m_arrFRData.Add(lpszProcessed);
					}
				}
				else
				{
					strFindData.MakeUpper();

					if (strFindData.Find(m_strFindStringUpper) >= 0)
					{
						m_arrFRType.Add(REGFIND_VALUE);
						m_arrFRSubKey.Add(lpszPath);
						m_arrFRValue.Add(lpszName);
						m_arrFRData.Add(lpszProcessed);
					}
				}
			}
		}

		if (m_bFOData == TRUE)
		{
			strFindData	= lpszProcessed;

			if (m_bFOMatchOnly == TRUE)
			{
				if (m_bFOCaseSensitive == TRUE)
				{
					if (strFindData.Compare(m_strFindString) == 0)
					{
						m_arrFRType.Add(REGFIND_DATA);
						m_arrFRSubKey.Add(lpszPath);
						m_arrFRValue.Add(lpszName);
						m_arrFRData.Add(lpszProcessed);
					}
				}
				else
				{
					if (strFindData.CompareNoCase(m_strFindString) == 0)
					{
						m_arrFRType.Add(REGFIND_DATA);
						m_arrFRSubKey.Add(lpszPath);
						m_arrFRValue.Add(lpszName);
						m_arrFRData.Add(lpszProcessed);
					}
				}
			}
			else
			{
				if (m_bFOCaseSensitive == TRUE)
				{
					if (strFindData.Find(m_strFindString) >= 0)
					{
						m_arrFRType.Add(REGFIND_DATA);
						m_arrFRSubKey.Add(lpszPath);
						m_arrFRValue.Add(lpszName);
						m_arrFRData.Add(lpszProcessed);
					}
				}
				else
				{
					strFindData.MakeUpper();

					if (strFindData.Find(m_strFindStringUpper) >= 0)
					{
						m_arrFRType.Add(REGFIND_DATA);
						m_arrFRSubKey.Add(lpszPath);
						m_arrFRValue.Add(lpszName);
						m_arrFRData.Add(lpszProcessed);
					}
				}
			}
		}
	}

	delete lpszProcessed;
	delete lpValue;
	delete lpszName;
}

void CRegistryMFCX::FindFormatValue(LPBYTE lpValue, DWORD dwValueLen, DWORD dwType, LPTSTR szProcessed, int nSize)
{
	switch (dwType)
	{
	case REG_SZ:
		{
			for (DWORD dwIndex = 0; dwIndex < dwValueLen; dwIndex += 2)
			{
				TCHAR	szValue[2];

				szValue[0] = lpValue[dwIndex] + (lpValue[dwIndex + 1] << 8);
				szValue[1] = NULL;

				_tcscat_s(szProcessed, nSize, szValue);
			}
		}
		break;
	case REG_DWORD:
		{
			DWORD	dwValue;

			memcpy_s(&dwValue, sizeof(DWORD), lpValue, sizeof(DWORD));
			wsprintf(szProcessed, _T("%08lx (%d)"), dwValue, dwValue);
		}
		break;
	case REG_BINARY:
	case REG_MULTI_SZ:
	case REG_EXPAND_SZ:
	case REG_FULL_RESOURCE_DESCRIPTOR:
	case REG_RESOURCE_LIST:
	case REG_RESOURCE_REQUIREMENTS_LIST:
		{
			for (DWORD dwIndex = 0; dwIndex < dwValueLen; dwIndex++)
			{
				TCHAR	szValue[8];

				wsprintf(szValue, _T("%02X%s"), lpValue[dwIndex], ((dwIndex == (dwValueLen - 1)) ? _T("") : _T(" ")));
				_tcscat_s(szProcessed, nSize, szValue);
			}
		}
		break;
	case REG_NONE:
	case REG_DWORD_BIG_ENDIAN:
	case REG_LINK:
		{
		}
		break;
	}
}
