// RegistryX.cpp

#include "StdAfx.h"
#include "RegistryX.h"
#include "ConvertersX.h"
#include "StringX.h"
#include "FileX.h"

#define IRI1_HEADER_LEN	27
#define REG5_HEADER_LEN	36

#define IRI1_HEADER		_T("InRegistry Information 1.00")
#define REG5_HEADER		_T("Windows Registry Editor Version 5.00")

CRegistryX::CRegistryX(void) : m_hKey(NULL), m_hKeyParent(NULL), m_nSubKeyIndex(0)
{
}

CRegistryX::CRegistryX(HKEY hKeyParent, LPCTSTR lpszSubKey, BOOL bCreateIfNew) : m_nSubKeyIndex(0)
{
	if (bCreateIfNew)
	{
		if (RegCreateKeyEx(hKeyParent, lpszSubKey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_hKey, &m_dwDisposition) == ERROR_SUCCESS)
		{
			m_hKeyParent	= hKeyParent;

			_tcscpy_s(m_szSubKeyTop, REG_MAX_KEYLEN + 1, lpszSubKey);
		}
	}
	else
	{
		if (RegOpenKeyEx(hKeyParent, lpszSubKey, 0, KEY_ALL_ACCESS, &m_hKey) != ERROR_SUCCESS)
		{
			m_hKey	= 0;
		}
		else
		{
			m_hKeyParent	= hKeyParent;

			_tcscpy_s(m_szSubKeyTop, REG_MAX_KEYLEN + 1, lpszSubKey);
		}
	}
}

CRegistryX::CRegistryX(CRegistryX& rSrc)
{
	if (rSrc.m_hKey)
	{
		if (RegCreateKeyEx(rSrc.m_hKey, rSrc.m_szSubKey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_hKey, &m_dwDisposition) != ERROR_SUCCESS)
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		m_hKey	= 0;
	}

	m_nSubKeyIndex	= 0;
	m_hKeyParent	= rSrc.m_hKeyParent;

	_tcscpy_s(m_szSubKey, REG_MAX_KEYLEN + 1, rSrc.m_szSubKey);
	_tcscpy_s(m_szSubKeyTop, REG_MAX_KEYLEN + 1, rSrc.m_szSubKeyTop);
}

CRegistryX::~CRegistryX(void)
{
	Close();
}

BOOL CRegistryX::Open(HKEY hKeyParent, LPCTSTR lpszSubKey, BOOL bCreateIfNew)
{
	Close();

	m_nSubKeyIndex	= 0;

	if (bCreateIfNew)
	{
		if (RegCreateKeyEx(hKeyParent, lpszSubKey, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_hKey, &m_dwDisposition) != ERROR_SUCCESS)
		{
			return FALSE;
		}
		else
		{
			m_hKeyParent	= hKeyParent;

			_tcscpy_s(m_szSubKeyTop, REG_MAX_KEYLEN + 1, lpszSubKey);
		}
	}
	else
	{
		if (RegOpenKeyEx(hKeyParent, lpszSubKey, 0, KEY_ALL_ACCESS, &m_hKey) != ERROR_SUCCESS)
		{
			m_hKey	= 0;

			return FALSE;
		}
		else
		{
			m_hKeyParent	= hKeyParent;

			_tcscpy_s(m_szSubKeyTop, REG_MAX_KEYLEN + 1, lpszSubKey);
		}
	}

	return TRUE;
}

BOOL CRegistryX::Flush(void)
{
	if (m_hKey)
	{
		return (RegFlushKey(m_hKey) == ERROR_SUCCESS);
	}

	return FALSE;
}

void CRegistryX::FlushAll(void)
{
	RegFlushKey(HKEY_CLASSES_ROOT);
	RegFlushKey(HKEY_CURRENT_USER);
	RegFlushKey(HKEY_LOCAL_MACHINE);
	RegFlushKey(HKEY_USERS);
}

BOOL CRegistryX::Close(void)
{
	if (m_hKey)
	{
		RegCloseKey(m_hKey);

		m_hKey	= 0;

		return TRUE;
	}

	return FALSE;
}

BOOL CRegistryX::ReadBinary(LPCTSTR lpszKey, LPBYTE lpData, LPDWORD lpcbData)
{
	DWORD	dwType	= REG_BINARY;

	return (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, lpData, lpcbData) == ERROR_SUCCESS);
}

DWORD CRegistryX::ReadDWORD(LPCTSTR lpszKey, DWORD dwDefault)
{
	DWORD	dwValue;
	DWORD	dwValueSize	= sizeof(DWORD);
	DWORD	dwType		= REG_DWORD;

	if (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &dwValueSize) == ERROR_SUCCESS)
	{
		return dwValue;
	}

	if (dwDefault != 0xDEF0)
	{
		RegSetValueEx(m_hKey, lpszKey, 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwDefault), sizeof(DWORD));
	}

	return dwDefault;
}

BOOL CRegistryX::ReadString(LPCTSTR lpszKey, LPTSTR lpszValue, LPDWORD lpcbValue)
{
	DWORD	dwDataSize	= 2048;
	DWORD	dwType		= REG_SZ;
	TCHAR	szValue[2048];

	memset(szValue, 0, sizeof(TCHAR) * 2048);

	if (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, reinterpret_cast<LPBYTE>(szValue), &dwDataSize) == ERROR_SUCCESS)
	{
		if (lpcbValue != NULL)
		{
			if (_tcslen(szValue) > *lpcbValue)
			{
				return FALSE;
			}
		}
		else
		{
			*lpcbValue	= (_tcslen(szValue) + 1);
		}

		_tcscpy_s(lpszValue, *lpcbValue, szValue);
		return TRUE;
	}

	return FALSE;
}

BOOL CRegistryX::ReadMultiString(LPCTSTR lpszKey, LPBYTE lpValue, LPDWORD lpcbValue)
{
	DWORD	dwType	= REG_MULTI_SZ;

	return (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, lpValue, lpcbValue) == ERROR_SUCCESS);
}

BOOL CRegistryX::ReadExpandString(LPCTSTR lpszKey, LPTSTR lpszValue, LPDWORD lpcbValue)
{
	DWORD	dwDataSize	= 2048;
	DWORD	dwType		= REG_EXPAND_SZ;
	TCHAR	szValue[2048];

	memset(szValue, 0, sizeof(TCHAR) * 2048);

	if (RegQueryValueEx(m_hKey, lpszKey, 0, &dwType, reinterpret_cast<LPBYTE>(szValue), &dwDataSize) == ERROR_SUCCESS)
	{
		if (lpcbValue != NULL)
		{
			if (_tcslen(szValue) > *lpcbValue)
			{
				return FALSE;
			}
		}
		else
		{
			*lpcbValue	= (_tcslen(szValue) + 1);
		}

		_tcscpy_s(lpszValue, *lpcbValue, szValue);
		return TRUE;
	}

	return FALSE;
}

BOOL CRegistryX::WriteBinary(LPCTSTR lpszKey, LPBYTE lpData, DWORD cbData)
{
	return (RegSetValueEx(m_hKey, lpszKey, 0, REG_BINARY, lpData, cbData) == ERROR_SUCCESS);
}

BOOL CRegistryX::WriteDWORD(LPCTSTR lpszKey, DWORD dwValue)
{
	return (RegSetValueEx(m_hKey, lpszKey, 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwValue), sizeof(DWORD)) == ERROR_SUCCESS);
}

BOOL CRegistryX::WriteString(LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	return (RegSetValueEx(m_hKey, lpszKey, 0, REG_SZ, reinterpret_cast<LPBYTE>(const_cast<LPTSTR>(lpszValue)), (_tcslen(lpszValue) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS);
}

BOOL CRegistryX::WriteMultiString(LPCTSTR lpszKey, LPBYTE lpData, DWORD cbData)
{
	return (RegSetValueEx(m_hKey, lpszKey, 0, REG_MULTI_SZ, lpData, cbData) == ERROR_SUCCESS);
}

BOOL CRegistryX::WriteExpandString(LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	return (RegSetValueEx(m_hKey, lpszKey, 0, REG_EXPAND_SZ, reinterpret_cast<LPBYTE>(const_cast<LPTSTR>(lpszValue)), (_tcslen(lpszValue) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS);
}

int CRegistryX::GetSubKeyCount(void)
{
	int		nIndex	= 0;
	DWORD	dwSize	= MAX_PATH;
	TCHAR	szSubKey[MAX_PATH + 1];

	while (RegEnumKeyEx(m_hKey, nIndex, szSubKey, &dwSize, 0, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		_tcscpy_s(szSubKey, MAX_PATH + 1, _T(""));

		nIndex++;
		dwSize	= MAX_PATH;
	}

	return nIndex;
}

BOOL CRegistryX::GetFirstSubKey(LPTSTR lpszSubKey, LPDWORD lpcbName)
{
	m_nSubKeyIndex	= 0;

	if (RegEnumKeyEx(m_hKey, m_nSubKeyIndex, lpszSubKey, lpcbName, 0, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRegistryX::GetNextSubKey(LPTSTR lpszSubKey, LPDWORD lpcbName)
{
	m_nSubKeyIndex++;

	if (RegEnumKeyEx(m_hKey, m_nSubKeyIndex, lpszSubKey, lpcbName, 0, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRegistryX::GetSubKey(int nIndex, LPTSTR lpszSubKey, LPDWORD lpcbName)
{
	LONG	rc;

	m_nSubKeyIndex	= nIndex;
	rc				= RegEnumKeyEx(m_hKey, m_nSubKeyIndex, lpszSubKey, lpcbName, 0, NULL, NULL, NULL);

	if (rc != ERROR_SUCCESS)
	{
		if (rc != ERROR_NO_MORE_ITEMS)
		{
			// Key Eumulating Error
		}

		return FALSE;
	}

	return TRUE;
}

BOOL CRegistryX::IsSubKeyExist(LPCTSTR lpszSubKey)
{
	HKEY	hKeySubKey;
	DWORD	dwRC		= RegOpenKeyEx(m_hKey, lpszSubKey, 0, KEY_ALL_ACCESS, &hKeySubKey);

	if (dwRC != ERROR_SUCCESS)
	{
		return FALSE;
	}

	RegCloseKey(hKeySubKey);
	return TRUE;
}

BOOL CRegistryX::DeleteKey(HKEY hKey, LPCTSTR lpszSubKey)
{
	return (DeleteSubKeyInternal(hKey, lpszSubKey));
}

BOOL CRegistryX::DeleteSubKey(LPCTSTR lpszSubKey)
{
	return (DeleteSubKeyInternal(m_hKey, lpszSubKey));
}

int CRegistryX::GetValueCount(void)
{
	int		nIndex	= 0;
	DWORD	dwSize	= MAX_PATH;
	TCHAR	szValue[MAX_PATH + 1];

	while (RegEnumValue(m_hKey, nIndex, szValue, &dwSize, 0, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		_tcscpy_s(szValue, MAX_PATH + 1, _T(""));

		nIndex++;
		dwSize	= MAX_PATH;
	}

	return nIndex;
}

BOOL CRegistryX::GetFirstValue(LPTSTR lpszValueName, LPDWORD lpcbName, LPDWORD lpdwType)
{
	m_nValueIndex	= 0;

	if (RegEnumValue(m_hKey, m_nValueIndex, lpszValueName, lpcbName, 0, lpdwType, NULL, NULL) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRegistryX::GetNextValue(LPTSTR lpszValueName, LPDWORD lpcbName, LPDWORD lpdwType)
{
	m_nValueIndex++;

	if (RegEnumValue(m_hKey, m_nValueIndex, lpszValueName, lpcbName, 0, lpdwType, NULL, NULL) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CRegistryX::GetValue(int nIndex, LPTSTR lpszValueName, LPDWORD lpcbName, LPDWORD lpdwType)
{
	LONG	rc;

	m_nValueIndex	= nIndex;
	rc				= RegEnumValue(m_hKey, m_nValueIndex, lpszValueName, lpcbName, 0, lpdwType, NULL, NULL);

	if (rc != ERROR_SUCCESS)
	{
		if (rc != ERROR_NO_MORE_ITEMS)
		{
			// Value Eumulating Error
		}

		return FALSE;
	}

	return TRUE;
}

BOOL CRegistryX::IsValueExist(LPCTSTR lpszValueName)
{
	return (RegQueryValueEx(m_hKey, lpszValueName, 0, NULL, NULL, NULL) == ERROR_SUCCESS);
}

BOOL CRegistryX::GetValueType(LPCTSTR lpszValueName, LPDWORD lpdwType)
{
	return (RegQueryValueEx(m_hKey, lpszValueName, 0, lpdwType, NULL, NULL) == ERROR_SUCCESS);
}

BOOL CRegistryX::DeleteValue(LPCTSTR lpszValueName)
{
	return (RegDeleteValue(m_hKey, lpszValueName) == ERROR_SUCCESS);
}

HKEY CRegistryX::GetKey(void)
{
	return m_hKey;
}

BOOL CRegistryX::Copy(HKEY hKeyRootFrom, LPCTSTR lpszSubKeyFrom, HKEY hKeyRootTo, LPCTSTR lpszSubKeyTo)
{
	HKEY	hKeyFrom;
	HKEY	hKeyTo;
	DWORD	dwDisposition;

	if (RegOpenKeyEx(hKeyRootFrom, lpszSubKeyFrom, 0, KEY_ALL_ACCESS, &hKeyFrom) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	RegCreateKeyEx(hKeyRootTo, lpszSubKeyTo, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKeyTo, &dwDisposition);
	CopyKeys(hKeyFrom, hKeyTo);
	return TRUE;
}

BOOL CRegistryX::Move(HKEY hKeyRootFrom, LPCTSTR lpszSubKeyFrom, HKEY hKeyRootTo, LPCTSTR lpszSubKeyTo)
{
	HKEY	hKeyFrom;
	HKEY	hKeyTo;
	DWORD	dwDisposition;

	if (RegOpenKeyEx(hKeyRootFrom, lpszSubKeyFrom, 0, KEY_ALL_ACCESS, &hKeyFrom) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	RegCreateKeyEx(hKeyRootTo, lpszSubKeyTo, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKeyTo, &dwDisposition);
	CopyKeys(hKeyFrom, hKeyTo);
	DeleteKey(hKeyRootFrom, lpszSubKeyFrom);
	return TRUE;
}

DWORD CRegistryX::GetExportType(LPCTSTR lpszFileName)
{
	DWORD	dwRetCode	= 0;
	HANDLE	hFile;

	hFile	= CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return (static_cast<DWORD>(-1));
	}

	int		nUnicodeHeader	= CheckUnicodeBOM(hFile);
	BYTE	szBuffer[MAX_PATH * sizeof(TCHAR)];
	TCHAR	szHeader[MAX_PATH];
	DWORD	dwRead;

	memset(szBuffer, 0, MAX_PATH * sizeof(TCHAR));

	if (nUnicodeHeader == static_cast<WORD>(0xFEFF))
	{
		ReadFile(hFile, szBuffer, (IRI1_HEADER_LEN * sizeof(TCHAR)), &dwRead, NULL);
		SetFilePointer(hFile, (static_cast<LONG>(IRI1_HEADER_LEN * sizeof(TCHAR)) * (-1)), NULL, FILE_CURRENT);

		_tcscpy_s(szHeader, MAX_PATH, reinterpret_cast<LPCTSTR>(szBuffer));

		if (!_tcscmp(szHeader, IRI1_HEADER))
		{
			dwRetCode	= 0x00000003;
		}

		ReadFile(hFile, szBuffer, (REG5_HEADER_LEN * sizeof(TCHAR)), &dwRead, NULL);
		SetFilePointer(hFile, (static_cast<LONG>(REG5_HEADER_LEN * sizeof(TCHAR)) * (-1)), NULL, FILE_CURRENT);

		_tcscpy_s(szHeader, MAX_PATH, reinterpret_cast<LPCTSTR>(szBuffer));

		if (!_tcscmp(szHeader, REG5_HEADER))
		{
			dwRetCode	= 0x00000050;
		}
	}

	CloseHandle(hFile);
	return dwRetCode;
}

BOOL CRegistryX::IRI1Export(LPCTSTR lpszFileName, BOOL bRecursiveSubKeys, BOOL bAlwaysNew)
{
	TCHAR	szKeyRoot[MAX_PATH + 1];
	HANDLE	hFile;

	if (m_hKeyParent == HKEY_CLASSES_ROOT)
	{
		_tcscpy_s(szKeyRoot, MAX_PATH + 1, _T("HKEY_CLASSES_ROOT"));
	}

	if (m_hKeyParent == HKEY_CURRENT_USER)
	{
		_tcscpy_s(szKeyRoot, MAX_PATH + 1, _T("HKEY_CURRENT_USER"));
	}

	if (m_hKeyParent == HKEY_LOCAL_MACHINE)
	{
		_tcscpy_s(szKeyRoot, MAX_PATH + 1, _T("HKEY_LOCAL_MACHINE"));
	}

	if (m_hKeyParent == HKEY_USERS)
	{
		_tcscpy_s(szKeyRoot, MAX_PATH + 1, _T("HKEY_USERS"));
	}

	hFile	= CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Failed to Create Export File, lpszFileName
		return FALSE;
	}

	if (bAlwaysNew == FALSE)
	{
		WORD	wUnicodeHeader	= 0x0000;
		TCHAR	szHeader[IRI1_HEADER_LEN + 1];
		DWORD	dwSize;

		ReadFile(hFile, &wUnicodeHeader, sizeof(WORD), &dwSize, NULL);
		ReadFile(hFile, reinterpret_cast<LPTSTR>(szHeader), (IRI1_HEADER_LEN * sizeof(TCHAR)), &dwSize, NULL);
		SetFilePointer(hFile, 0, NULL, FILE_END);

		szHeader[IRI1_HEADER_LEN]	= NULL;

		if ((wUnicodeHeader != static_cast<WORD>(0xFEFF)) || (_tcscmp(szHeader, IRI1_HEADER)))
		{
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

			bAlwaysNew	= TRUE;
		}
	}

	if (bAlwaysNew == TRUE)
	{
		WORD	wUnicodeHeader	= 0xFEFF;
		TCHAR	szHeader[IRI1_HEADER_LEN + 3];
		DWORD	dwSize;

		_tcscpy_s(szHeader, IRI1_HEADER_LEN + 3, IRI1_HEADER);
		_tcscat_s(szHeader, IRI1_HEADER_LEN + 3, _T("\r\n"));
		WriteFile(hFile, &wUnicodeHeader, sizeof(WORD), &dwSize, NULL);
		WriteFile(hFile, szHeader, _tcslen(szHeader) * sizeof(TCHAR), &dwSize, NULL);
	}

	TCHAR	szFullPath[MAX_PATH + 1];

	wsprintf(szFullPath, _T("%s%s%s"), szKeyRoot, ((_tcslen(m_szSubKeyTop) > 0) ? _T("\\") : _T("")), m_szSubKeyTop);
	IRI1ExportKeys(m_hKey, szFullPath, hFile, bRecursiveSubKeys);
	CloseHandle(hFile);
	return TRUE;
}

BOOL CRegistryX::IRI1Import(LPCTSTR lpszFileName)
{
	HKEY	hKey;
	BOOL	bOpened	= FALSE;
	HANDLE	hFile;
	TCHAR	szBuffer[2048];

	if (GetExportType(lpszFileName) != 0x00000003)
	{
		//	Invalid Type Detected.
		return FALSE;
	}

	hFile	= CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Failed to Open File, lpszFileName
		return FALSE;
	}

	CheckUnicodeBOM(hFile);
	memset(szBuffer, 0, sizeof(TCHAR) * 2048);

	while (ReadStringX(hFile, szBuffer, 2048, TRUE))
	{
		TrimX(szBuffer);

		if (_tcslen(szBuffer) < 1)
		{
			// Blank Line
			memset(szBuffer, 0, sizeof(TCHAR) * 2048);
			continue;
		}
		else if (!_tcscmp(szBuffer, IRI1_HEADER))
		{
			// Header
			memset(szBuffer, 0, sizeof(TCHAR) * 2048);
			continue;
		}
		else if (szBuffer[0] == _T('['))
		{
			// Key Header
			TrimLeftX(szBuffer, _T('['));
			TrimRightX(szBuffer, _T(']'));

			if (bOpened)
			{
				RegCloseKey(hKey);

				bOpened	= FALSE;
			}

			BOOL	bDelete	= FALSE;

			if (szBuffer[0] == _T('-'))
			{
				bDelete		= TRUE;

				MidX(szBuffer, 1);
			}

			TCHAR	szKeyRoot[MAX_PATH + 1];
			HKEY	hKeyRoot;
			DWORD	rc;

			szKeyRoot[0]	= NULL;

			LeftX(szKeyRoot, MAX_PATH + 1, szBuffer, FindX(szBuffer, _T('\\')));
			MidX(szBuffer, FindX(szBuffer, _T('\\')) + 1);

			if (!_tcscmp(szKeyRoot, _T("HKEY_CLASSES_ROOT")))
			{
				hKeyRoot	= HKEY_CLASSES_ROOT;
			}

			if (!_tcscmp(szKeyRoot, _T("HKEY_CURRENT_USER")))
			{
				hKeyRoot	= HKEY_CURRENT_USER;
			}

			if (!_tcscmp(szKeyRoot, _T("HKEY_LOCAL_MACHINE")))
			{
				hKeyRoot	= HKEY_LOCAL_MACHINE;
			}

			if (!_tcscmp(szKeyRoot, _T("HKEY_USERS")))
			{
				hKeyRoot	= HKEY_USERS;
			}

			if (!bDelete)
			{
				RegCreateKeyEx(hKeyRoot, szBuffer, 0, _T(""), 0, 0, NULL, &hKey, &rc);

				bOpened	= TRUE;
			}
			else
			{
				DeleteKey(hKeyRoot, szBuffer);

				bDelete	= FALSE;
			}

			memset(szBuffer, 0, sizeof(TCHAR) * 2048);
		}
		else if ((szBuffer[0] == _T('\"')) || (szBuffer[0] == _T('@')))
		{
			if (bOpened == FALSE)
			{
				memset(szBuffer, 0, sizeof(TCHAR) * 2048);
				continue;
			}

			// Value
			TCHAR	szKey[MAX_PATH + 1];
			TCHAR	szValue[2048];
			TCHAR	szType[6];

			LeftX(szKey, MAX_PATH + 1, szBuffer, FindX(szBuffer, _T(" = ")));
			MidX(szValue, 2048, szBuffer, FindX(szBuffer, _T(" = ")) + 3);
			LeftX(szType, 6, szValue, 5);
			MidX(szValue, 6);

			if (!_tcscmp(szKey, _T("@")))
			{
				// Default Value
				szKey[0]	= NULL;
			}
			else
			{
				TrimX(szKey, _T('\"'));
			}

			if (!_tcscmp(szType, _T("ERASE")))
			{
				RegDeleteValue(hKey, szKey);
			}

			if (!_tcscmp(szType, _T("STRGZ")))
			{
				// String
				TCHAR	szMark[2];

				RightX(szMark, 2, szValue, 1);

				while (szMark[0] != _T('\"'))
				{
					ReadStringX(hFile, szBuffer, 2048, TRUE);
					TrimX(szBuffer);
					_tcscat_s(szValue, 2048, _T("\r\n"));
					_tcscat_s(szValue, 2048, szBuffer);
					RightX(szMark, 2, szValue, 1);
				}

				TrimX(szValue, _T('\"'));

				int nPos	= 0;

				while (nPos < static_cast<int>(_tcslen(szValue)))
				{
					if ((szValue[nPos] == _T('\\')) && (nPos < static_cast<int>(_tcslen(szValue) - 1)))
					{
						switch (szValue[nPos + 1])
						{
						case _T('\\'):
							szValue[nPos]	= _T('\\');

							DeleteX(szValue, nPos + 1);
							break;
						case _T('r'):
							szValue[nPos]	= _T('\r');

							DeleteX(szValue, nPos + 1);
							break;
						case _T('n'):
							szValue[nPos]	= _T('\n');

							DeleteX(szValue, nPos + 1);
							break;
						case _T('t'):
							szValue[nPos]	= _T('\t');

							DeleteX(szValue, nPos + 1);
							break;
						}
					}

					nPos++;
				}

				RegSetValueEx(hKey, szKey, 0, REG_SZ, reinterpret_cast<LPBYTE>(szValue), sizeof(TCHAR) * (_tcslen(szValue) + 1));
			}

			if (!_tcscmp(szType, _T("DWORD")))
			{
				// DWord
				DWORD	dwValue;

				HexStringToDWORD(szValue, dwValue);
				RegSetValueEx(hKey, szKey, 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwValue), sizeof(DWORD));
			}

			if (!_tcsncmp(szType, _T("HEX"), 3))
			{
				// Hex
				BYTE	cValue[4096];
				int		nPos	= 0;
				TCHAR	szHex[3];

				while (_tcslen(szValue))
				{
					if (szValue[0] == _T('+'))
					{
						ReadStringX(hFile, szBuffer, 2048, TRUE);
						TrimX(szBuffer);
						_tcscpy_s(szValue, 2048, szBuffer);
					}
					else
					{
						LeftX(szHex, 3, szValue, 2);

						if (_tcslen(szValue) < 3)
						{
							szValue[0]	= NULL;
						}
						else
						{
							MidX(szValue, 3);
						}

						DWORD	dwValue;

						HexStringToDWORD(szHex, dwValue);

						cValue[nPos++] = static_cast<BYTE>(dwValue);
					}
				}

				if (!_tcscmp(szType, _T("HEX03")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_BINARY, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX07")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_MULTI_SZ, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX02")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_EXPAND_SZ, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX09")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_FULL_RESOURCE_DESCRIPTOR, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX08")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_RESOURCE_LIST, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX0A")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_RESOURCE_REQUIREMENTS_LIST, static_cast<LPBYTE>(cValue), nPos);
				}
			}
		}

		memset(szBuffer, 0, sizeof(TCHAR) * 2048);
	}

	if (bOpened)
	{
		RegCloseKey(hKey);

		bOpened	= FALSE;
	}

	CloseHandle(hFile);
	return TRUE;
}

BOOL CRegistryX::REG5Export(LPCTSTR lpszFileName, BOOL bRecursiveSubKeys, BOOL bAlwaysNew)
{
	TCHAR	szKeyRoot[MAX_PATH + 1];
	HANDLE	hFile;

	if (m_hKeyParent == HKEY_CLASSES_ROOT)
	{
		_tcscpy_s(szKeyRoot, MAX_PATH + 1, _T("HKEY_CLASSES_ROOT"));
	}

	if (m_hKeyParent == HKEY_CURRENT_USER)
	{
		_tcscpy_s(szKeyRoot, MAX_PATH + 1, _T("HKEY_CURRENT_USER"));
	}

	if (m_hKeyParent == HKEY_LOCAL_MACHINE)
	{
		_tcscpy_s(szKeyRoot, MAX_PATH + 1, _T("HKEY_LOCAL_MACHINE"));
	}

	if (m_hKeyParent == HKEY_USERS)
	{
		_tcscpy_s(szKeyRoot, MAX_PATH + 1, _T("HKEY_USERS"));
	}

	hFile	= CreateFile(lpszFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Failed to Create Export File, lpszFileName
		return FALSE;
	}

	if (bAlwaysNew == FALSE)
	{
		WORD	wUnicodeHeader	= 0x0000;
		TCHAR	szHeader[REG5_HEADER_LEN + 1];
		DWORD	dwSize;

		ReadFile(hFile, &wUnicodeHeader, sizeof(WORD), &dwSize, NULL);
		ReadFile(hFile, static_cast<LPTSTR>(szHeader), (REG5_HEADER_LEN * sizeof(TCHAR)), &dwSize, NULL);
		SetFilePointer(hFile, 0, NULL, FILE_END);

		szHeader[REG5_HEADER_LEN]	= NULL;

		if ((wUnicodeHeader != static_cast<WORD>(0xFEFF)) || (_tcscmp(szHeader, REG5_HEADER)))
		{
			SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

			bAlwaysNew	= TRUE;
		}
	}

	if (bAlwaysNew == TRUE)
	{
		WORD	wUnicodeHeader	= 0xFEFF;
		TCHAR	szHeader[REG5_HEADER_LEN + 3];
		DWORD	dwSize;

		_tcscpy_s(szHeader, REG5_HEADER_LEN + 3, REG5_HEADER);
		_tcscat_s(szHeader, REG5_HEADER_LEN + 3, _T("\r\n"));
		WriteFile(hFile, &wUnicodeHeader, sizeof(WORD), &dwSize, NULL);
		WriteFile(hFile, szHeader, _tcslen(szHeader) * sizeof(TCHAR), &dwSize, NULL);
	}

	TCHAR	szFullPath[MAX_PATH + 1];

	wsprintf(szFullPath, _T("%s%s%s"), szKeyRoot, ((_tcslen(m_szSubKeyTop) > 0) ? _T("\\") : _T("")), m_szSubKeyTop);
	REG5ExportKeys(m_hKey, szFullPath, hFile, bRecursiveSubKeys);
	CloseHandle(hFile);
	return TRUE;
}

BOOL CRegistryX::REG5Import(LPCTSTR lpszFileName)
{
	HKEY	hKey;
	BOOL	bOpened	= FALSE;
	HANDLE	hFile;
	TCHAR	szBuffer[2048];

	if (GetExportType(lpszFileName) != 0x00000050)
	{
		//	Invalid Type Detected.
		return FALSE;
	}

	hFile	= CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Failed to Open File, lpszFileName
		return FALSE;
	}

	CheckUnicodeBOM(hFile);

	memset(szBuffer, 0, sizeof(TCHAR) * 2048);

	while (ReadStringX(hFile, szBuffer, 2048, TRUE))
	{
		TrimX(szBuffer);

		if (_tcslen(szBuffer) < 1)
		{
			// Blank Line
			memset(szBuffer, 0, sizeof(TCHAR) * 2048);
			continue;
		}
		else if (!_tcscmp(szBuffer, REG5_HEADER))
		{
			// Header
			memset(szBuffer, 0, sizeof(TCHAR) * 2048);
			continue;
		}
		else if (szBuffer[0] == _T('['))
		{
			// Key Header
			TrimLeftX(szBuffer, _T('['));
			TrimRightX(szBuffer, _T(']'));

			if (bOpened)
			{
				RegCloseKey(hKey);

				bOpened	= FALSE;
			}

			BOOL	bDelete;

			if (szBuffer[0] == _T('-'))
			{
				bDelete		= TRUE;

				MidX(szBuffer, 1);
			}

			TCHAR	szKeyRoot[MAX_PATH + 1];
			HKEY	hKeyRoot;
			DWORD	rc;

			szKeyRoot[0]	= NULL;

			LeftX(szKeyRoot, MAX_PATH + 1, szBuffer, FindX(szBuffer, _T('\\')));
			MidX(szBuffer, FindX(szBuffer, _T('\\')) + 1);

			if (!_tcscmp(szKeyRoot, _T("HKEY_CLASSES_ROOT")))
			{
				hKeyRoot	= HKEY_CLASSES_ROOT;
			}

			if (!_tcscmp(szKeyRoot, _T("HKEY_CURRENT_USER")))
			{
				hKeyRoot	= HKEY_CURRENT_USER;
			}

			if (!_tcscmp(szKeyRoot, _T("HKEY_LOCAL_MACHINE")))
			{
				hKeyRoot	= HKEY_LOCAL_MACHINE;
			}

			if (!_tcscmp(szKeyRoot, _T("HKEY_USERS")))
			{
				hKeyRoot	= HKEY_USERS;
			}

			if (!bDelete)
			{
				RegCreateKeyEx(hKeyRoot, szBuffer, 0, _T(""), 0, 0, NULL, &hKey, &rc);

				bOpened	= TRUE;
			}
			else
			{
				DeleteKey(hKeyRoot, szBuffer);

				bDelete	= FALSE;
			}
		}
		else if ((szBuffer[0] == _T('\"')) || (szBuffer[0] == _T('@')))
		{
			if (bOpened == FALSE)
			{
				memset(szBuffer, 0, sizeof(TCHAR) * 2048);
				continue;
			}

			// Value
			TCHAR	szKey[MAX_PATH + 1];
			TCHAR	szValue[2048];
			TCHAR	szType[6];

			LeftX(szKey, MAX_PATH + 1, szBuffer, FindX(szBuffer, _T("=")));
			MidX(szValue, 2048, szBuffer, FindX(szBuffer, _T("=")) + 1);

			if (szValue[0] == _T('-'))
			{
				_tcscpy_s(szType, 6, _T("ERASE"));
			}
			else if (szValue[0] == _T('\"'))
			{
				_tcscpy_s(szType, 6, _T("STRGZ"));
			}
			else if (!_tcsncmp(szValue, _T("dword:"), 6))
			{
				_tcscpy_s(szType, 6, _T("DWORD"));
				MidX(szValue, 6);
			}
			else if (!_tcsncmp(szValue, _T("hex:"), 4))
			{
				_tcscpy_s(szType, 6, _T("HEX03"));
				MidX(szValue, 4);
			}
			else if (!_tcsncmp(szValue, _T("hex(7):"), 7))
			{
				_tcscpy_s(szType, 6, _T("HEX07"));
				MidX(szValue, 7);
			}
			else if (!_tcsncmp(szValue, _T("hex(2):"), 7))
			{
				_tcscpy_s(szType, 6, _T("HEX02"));
				MidX(szValue, 7);
			}
			else if (!_tcsncmp(szValue, _T("hex(9):"), 7))
			{
				_tcscpy_s(szType, 6, _T("HEX09"));
				MidX(szValue, 7);
			}
			else if (!_tcsncmp(szValue, _T("hex(8):"), 7))
			{
				_tcscpy_s(szType, 6, _T("HEX08"));
				MidX(szValue, 7);
			}
			else if (!_tcsncmp(szValue, _T("hex(10):"), 8))
			{
				_tcscpy_s(szType, 6, _T("HEX0A"));
				MidX(szValue, 8);
			}

			if (!_tcscmp(szKey, _T("@")))
			{
				// Default Value
				szKey[0]	= NULL;
			}
			else
			{
				TrimX(szKey, _T('\"'));
			}

			if (!_tcscmp(szType, _T("ERASE")))
			{
				RegDeleteValue(hKey, szKey);
			}

			if (!_tcscmp(szType, _T("STRGZ")))
			{
				// String
				TCHAR	szMark[2];

				RightX(szMark, 2, szValue, 1);

				while (szMark[0] != _T('\"'))
				{
					ReadStringX(hFile, szBuffer, 2048, TRUE);
					TrimX(szBuffer);
					_tcscat_s(szValue, 2048, _T("\r\n"));
					_tcscat_s(szValue, 2048, szBuffer);
					RightX(szMark, 2, szValue, 1);
				}

				TrimX(szValue, _T('\"'));

				int nPos	= 0;

				while (nPos < static_cast<int>(_tcslen(szValue)))
				{
					if ((szValue[nPos] == _T('\\')) && (nPos < static_cast<int>(_tcslen(szValue) - 1)))
					{
						switch (szValue[nPos + 1])
						{
						case _T('\\'):
							szValue[nPos]	= _T('\\');

							DeleteX(szValue, nPos + 1);

							break;
						}
					}

					nPos++;
				}

				RegSetValueEx(hKey, szKey, 0, REG_SZ, reinterpret_cast<LPBYTE>(szValue), sizeof(TCHAR) * (_tcslen(szValue) + 1));
			}

			if (!_tcscmp(szType, _T("DWORD")))
			{
				// DWord
				DWORD	dwValue;

				HexStringToDWORD(szValue, dwValue);
				RegSetValueEx(hKey, szKey, 0, REG_DWORD, reinterpret_cast<LPBYTE>(&dwValue), sizeof(DWORD));
			}

			if (!_tcsncmp(szType, _T("HEX"), 3))
			{
				// Hex
				BYTE	cValue[4096];
				int		nPos	= 0;
				TCHAR	szHex[3];

				while (_tcslen(szValue))
				{
					if (szValue[0] == _T('\\'))
					{
						ReadStringX(hFile, szBuffer, 2048, TRUE);
						TrimX(szBuffer);
						_tcscpy_s(szValue, 2048, szBuffer);
					}
					else
					{
						LeftX(szHex, 3, szValue, 2);

						if (_tcslen(szValue) < 3)
						{
							szValue[0]	= NULL;
						}
						else
						{
							MidX(szValue, 3);
						}

						DWORD	dwValue;

						HexStringToDWORD(szHex, dwValue);

						cValue[nPos++]	= static_cast<BYTE>(dwValue);
					}
				}

				if (!_tcscmp(szType, _T("HEX03")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_BINARY, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX07")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_MULTI_SZ, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX02")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_EXPAND_SZ, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX09")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_FULL_RESOURCE_DESCRIPTOR, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX08")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_RESOURCE_LIST, static_cast<LPBYTE>(cValue), nPos);
				}

				if (!_tcscmp(szType, _T("HEX0A")))
				{
					RegSetValueEx(hKey, szKey, 0, REG_RESOURCE_REQUIREMENTS_LIST, static_cast<LPBYTE>(cValue), nPos);
				}
			}
		}

		memset(szBuffer, 0, sizeof(TCHAR) * 2048);
	}

	if (bOpened)
	{
		RegCloseKey(hKey);

		bOpened	= FALSE;
	}

	CloseHandle(hFile);
	return TRUE;
}

BOOL CRegistryX::DeleteSubKeyInternal(HKEY hKey, LPCTSTR lpszSubKey)
{
#ifndef _WIN32_WCE
	if (!lpszSubKey)
	{
		return FALSE;
	}
	else
	{
		HKEY	hKeySubKey;
		TCHAR	szSubKey[MAX_PATH + 1];
		TCHAR	szValue[MAX_PATH + 1];
		DWORD	dwRC	= RegOpenKeyEx(hKey, lpszSubKey, 0, KEY_ALL_ACCESS, &hKeySubKey);
		DWORD	dwNameSize;

		if (dwRC != ERROR_SUCCESS)
		{
			ASSERT(FALSE);
			return FALSE;
		}

		dwNameSize	= MAX_PATH;

		while (RegEnumValue(hKeySubKey, 0, szValue, &dwNameSize, 0, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			if (RegDeleteValue(hKeySubKey, szValue) != ERROR_SUCCESS)
			{
				RegCloseKey(hKeySubKey);
				return FALSE;
			}

			dwNameSize	= MAX_PATH;
		}

		dwNameSize	= MAX_PATH;

		while (RegEnumKeyEx(hKeySubKey, 0, szSubKey, &dwNameSize, 0, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			if (!DeleteSubKeyInternal(hKeySubKey, szSubKey))
			{
				RegCloseKey(hKeySubKey);
				return FALSE;
			}

			dwNameSize	= MAX_PATH;
		}

		RegCloseKey(hKeySubKey);
	}
#endif

	return (BOOL)(RegDeleteKey(hKey, lpszSubKey) == ERROR_SUCCESS);
}

void CRegistryX::CopyKeys(HKEY hKeyFrom, HKEY hKeyTo)
{
	DWORD	dwDepth;
	HKEY	arrKey[REG_MAX_DEPTH];
	HKEY	arrKeyTo[REG_MAX_DEPTH];
	DWORD	arrIndex[REG_MAX_DEPTH];

	dwDepth				= 0;
	arrKey[dwDepth]		= hKeyFrom;
	arrKeyTo[dwDepth]	= hKeyTo;
	arrIndex[dwDepth]	= 0;

	do
	{
		TCHAR	szSubKey[260];
		TCHAR	szClass[260];
		DWORD	dwKeySize		= 260;
		DWORD	dwClassSize		= 260;
		LONG	lResult			= RegEnumKeyEx(arrKey[dwDepth], arrIndex[dwDepth], szSubKey, &dwKeySize, NULL, szClass, &dwClassSize, NULL);
		DWORD	dwDisposition;

		if (lResult == ERROR_NO_MORE_ITEMS)
		{
			CopyValues(arrKey[dwDepth], arrKeyTo[dwDepth]);
			RegCloseKey(arrKey[dwDepth]);
			RegCloseKey(arrKeyTo[dwDepth]);

			if (dwDepth == 0)
			{
				break;
			}

			dwDepth--;
		}
		else
		{
			RegCreateKeyEx(arrKeyTo[dwDepth], szSubKey, 0, szClass, 0, KEY_ALL_ACCESS, 0, &arrKeyTo[dwDepth + 1], &dwDisposition);
			RegOpenKeyEx(arrKey[dwDepth], szSubKey, 0, 0, &arrKey[dwDepth + 1]);

			arrIndex[dwDepth]++;
			dwDepth++;
			arrIndex[dwDepth]	= 0;
		}
	} while (TRUE);
}

void CRegistryX::CopyValues(HKEY hKeyFrom, HKEY hKeyTo)
{
	TCHAR	lpszName[MAX_PATH];
	BYTE	lpValue[4096];
	int		nIndex	= 0;

	while (TRUE)
	{
		DWORD	dwType;
		DWORD	dwNameSize	= MAX_PATH;
		DWORD	dwValueSize	= 4096;

		memset(lpValue, 0, 4096 * sizeof(BYTE));

		if (RegEnumValue(hKeyFrom, nIndex, lpszName, &dwNameSize, 0, &dwType, lpValue, &dwValueSize) == ERROR_NO_MORE_ITEMS)
		{
			break;
		}

		RegSetValueEx(hKeyTo, lpszName, 0, dwType, lpValue, dwValueSize);

		nIndex++;
	}
}

#define	PROCESS_SIZE	(24 * 1024)		// 24 KB

void CRegistryX::IRI1ExportKeys(HKEY hKey, LPCTSTR lpszPath, HANDLE hFile, BOOL bRecursiveSubKeys)
{
	DWORD	dwDepth;
	HKEY	arrKey[REG_MAX_DEPTH];
	DWORD	arrIndex[REG_MAX_DEPTH];
	TCHAR	arrPath[REG_MAX_DEPTH][REG_MAX_KEYLEN];

	dwDepth				= 0;
	arrKey[dwDepth]		= hKey;
	arrIndex[dwDepth]	= 0;

	_tcscpy_s(arrPath[dwDepth], REG_MAX_KEYLEN, lpszPath);

	do
	{
		if (arrIndex[dwDepth] == 0)
		{
			TCHAR	szBuffer[2048];
			DWORD	dwWritten;

			wsprintf(szBuffer, _T("\r\n[%s]\r\n"), arrPath[dwDepth]);
			WriteFile(hFile, szBuffer, _tcslen(szBuffer) * sizeof(TCHAR), &dwWritten, NULL);
			IRI1ExportValues(arrKey[dwDepth], hFile);
		}

		TCHAR	szSubKey[REG_MAX_KEYLEN + 1];
		DWORD	dwSize	= REG_MAX_KEYLEN;
		LONG	lResult	= (bRecursiveSubKeys ? RegEnumKeyEx(arrKey[dwDepth], arrIndex[dwDepth], szSubKey, &dwSize, NULL, NULL, NULL, NULL) : ERROR_NO_MORE_ITEMS);

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
			arrIndex[dwDepth]	= 0;
		}
	} while (TRUE);
}

void CRegistryX::IRI1ExportValues(HKEY hKey, HANDLE hFile)
{
	LONG	lResult;
	DWORD	dwValues, dwMaxNameLen, dwMaxValueLen;

	lResult	= RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &dwValues, &dwMaxNameLen, &dwMaxValueLen, NULL, NULL);

	if (lResult != ERROR_SUCCESS)
	{
		return;
	}

	LPTSTR	lpszName		= new TCHAR[dwMaxNameLen + 1];
	LPBYTE	lpValue			= new BYTE[dwMaxValueLen + 1];
	LPTSTR	lpszProcessed	= new TCHAR[PROCESS_SIZE];
	LPTSTR	lpszBuffer		= new TCHAR[PROCESS_SIZE];
	DWORD	dwType;
	DWORD	dwNameLen;
	DWORD	dwValueLen;
	DWORD	dwWritten;

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
		memset(lpszBuffer, 0, sizeof(TCHAR) * PROCESS_SIZE);
		IRI1FormatValue(lpValue, dwValueLen, dwType, lpszProcessed, PROCESS_SIZE);

		if ((_tcslen(lpszName) < 1) || (!_tcscmp(lpszName, _T("Default"))))
		{
			_tcscpy_s(lpszBuffer, PROCESS_SIZE, _T("@ = "));
		}
		else
		{
			wsprintf(lpszBuffer, _T("\"%s\" = "), lpszName);
		}

		_tcscat_s(lpszBuffer, PROCESS_SIZE, lpszProcessed);
		WriteFile(hFile, lpszBuffer, _tcslen(lpszBuffer) * sizeof(TCHAR), &dwWritten, NULL);
	}

	delete lpszBuffer;
	delete lpszProcessed;
	delete lpValue;
	delete lpszName;
}

void CRegistryX::IRI1FormatValue(LPBYTE lpValue, DWORD dwValueLen, DWORD dwType, LPTSTR szProcessed, int nSize)
{
	switch (dwType)
	{
	case REG_SZ:
		{
			_tcscpy_s(szProcessed, nSize, _T("STRGZ:\""));

			for (DWORD dwIndex = 0; dwIndex < dwValueLen; dwIndex += 2)
			{
				TCHAR	szValue[2];

				szValue[0]	= lpValue[dwIndex] + (lpValue[dwIndex + 1] << 8);
				szValue[1]	= NULL;

				switch (szValue[0])
				{
				case _T('\\'):
					_tcscat_s(szProcessed, nSize, _T("\\\\"));
					break;
				case _T('\r'):
					_tcscat_s(szProcessed, nSize, _T("\\r"));
					break;
				case _T('\n'):
					_tcscat_s(szProcessed, nSize, _T("\\n"));
					break;
				case _T('\t'):
					_tcscat_s(szProcessed, nSize, _T("\\t"));
					break;
				default:
					_tcscat_s(szProcessed, nSize, szValue);
					break;
				}
			}

			_tcscat_s(szProcessed, nSize, _T("\"\r\n"));
		}

		break;
	case REG_DWORD:
		{
			DWORD	dwValue;

			memcpy_s(&dwValue, sizeof(DWORD), lpValue, sizeof(DWORD));
			wsprintf(szProcessed, _T("DWORD:%08lX\r\n"), dwValue);
		}

		break;
	case REG_BINARY:
	case REG_MULTI_SZ:
	case REG_EXPAND_SZ:
	case REG_FULL_RESOURCE_DESCRIPTOR:
	case REG_RESOURCE_LIST:
	case REG_RESOURCE_REQUIREMENTS_LIST:
		{
			wsprintf(szProcessed, _T("HEX%02X:"), dwType);

			for (DWORD dwIndex = 0; dwIndex < dwValueLen; dwIndex++)
			{
				TCHAR	szValue[8];

				wsprintf(szValue, _T("%02X%s"), lpValue[dwIndex], ((dwIndex == (dwValueLen - 1)) ? _T("") : _T(",")));
				_tcscat_s(szProcessed, nSize, szValue);

				if ((dwIndex != 0) && ((dwIndex % 0x15) == 0) && (dwIndex != (dwValueLen - 1)))
				{
					_tcscat_s(szProcessed, nSize, _T("+\r\n    "));
				}
			}

			_tcscat_s(szProcessed, nSize, _T("\r\n"));
		}

		break;
	case REG_NONE:
	case REG_DWORD_BIG_ENDIAN:
	case REG_LINK:
		break;
	}
}

void CRegistryX::REG5ExportKeys(HKEY hKey, LPCTSTR lpszPath, HANDLE hFile, BOOL bRecursiveSubKeys)
{
	DWORD	dwDepth;
	HKEY	arrKey[REG_MAX_DEPTH];
	DWORD	arrIndex[REG_MAX_DEPTH];
	TCHAR	arrPath[REG_MAX_DEPTH][REG_MAX_KEYLEN];

	dwDepth				= 0;
	arrKey[dwDepth]		= hKey;
	arrIndex[dwDepth]	= 0;

	_tcscpy_s(arrPath[dwDepth], REG_MAX_KEYLEN, lpszPath);

	do
	{
		if (arrIndex[dwDepth] == 0)
		{
			TCHAR	szBuffer[2048];
			DWORD	dwWritten;

			wsprintf(szBuffer, _T("\r\n[%s]\r\n"), arrPath[dwDepth]);
			WriteFile(hFile, szBuffer, _tcslen(szBuffer) * sizeof(TCHAR), &dwWritten, NULL);
			REG5ExportValues(arrKey[dwDepth], hFile);
		}

		TCHAR	szSubKey[REG_MAX_KEYLEN + 1];
		DWORD	dwSize	= REG_MAX_KEYLEN;
		LONG	lResult	= (bRecursiveSubKeys ? RegEnumKeyEx(arrKey[dwDepth], arrIndex[dwDepth], szSubKey, &dwSize, NULL, NULL, NULL, NULL) : ERROR_NO_MORE_ITEMS);

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
			arrIndex[dwDepth]	= 0;
		}
	} while (TRUE);
}

void CRegistryX::REG5ExportValues(HKEY hKey, HANDLE hFile)
{
	LONG	lResult;
	DWORD	dwValues, dwMaxNameLen, dwMaxValueLen;

	lResult	= RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &dwValues, &dwMaxNameLen, &dwMaxValueLen, NULL, NULL);

	if (lResult != ERROR_SUCCESS)
	{
		return;
	}

	LPTSTR	lpszName		= new TCHAR[dwMaxNameLen + 1];
	LPBYTE	lpValue			= new BYTE[dwMaxValueLen + 1];
	LPTSTR	lpszProcessed	= new TCHAR[PROCESS_SIZE];
	LPTSTR	lpszBuffer		= new TCHAR[PROCESS_SIZE];
	DWORD	dwType;
	DWORD	dwNameLen;
	DWORD	dwValueLen;
	DWORD	dwWritten;

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
		memset(lpszBuffer, 0, sizeof(TCHAR) * PROCESS_SIZE);
		REG5FormatValue(lpValue, dwValueLen, dwType, lpszProcessed, PROCESS_SIZE);

		if ((_tcslen(lpszName) < 1) || (!_tcscmp(lpszName, _T("Default"))))
		{
			_tcscpy_s(lpszBuffer, PROCESS_SIZE, _T("@="));
		}
		else
		{
			wsprintf(lpszBuffer, _T("\"%s\"="), lpszName);
		}

		_tcscat_s(lpszBuffer, PROCESS_SIZE, lpszProcessed);
		WriteFile(hFile, lpszBuffer, _tcslen(lpszBuffer) * sizeof(TCHAR), &dwWritten, NULL);
	}

	delete lpszBuffer;
	delete lpszProcessed;
	delete lpValue;
	delete lpszName;
}

void CRegistryX::REG5FormatValue(LPBYTE lpValue, DWORD dwValueLen, DWORD dwType, LPTSTR szProcessed, int nSize)
{
	switch (dwType)
	{
	case REG_SZ:
		{
			_tcscpy_s(szProcessed, nSize, _T("\""));

			for (DWORD dwIndex = 0; dwIndex < dwValueLen; dwIndex += 2)
			{
				TCHAR	szValue[2];

				szValue[0]	= lpValue[dwIndex] + (lpValue[dwIndex + 1] << 8);
				szValue[1]	= NULL;

				switch (szValue[0])
				{
				case _T('\\'):
					_tcscat_s(szProcessed, nSize, _T("\\\\"));
					break;
				default:
					_tcscat_s(szProcessed, nSize, szValue);
					break;
				}
			}

			_tcscat_s(szProcessed, nSize, _T("\"\r\n"));
		}

		break;
	case REG_DWORD:
		{
			DWORD	dwValue;

			memcpy_s(&dwValue, sizeof(DWORD), lpValue, sizeof(DWORD));
			wsprintf(szProcessed, _T("dword:%08lx\r\n"), dwValue);
		}

		break;
	case REG_BINARY:
	case REG_MULTI_SZ:
	case REG_EXPAND_SZ:
	case REG_FULL_RESOURCE_DESCRIPTOR:
	case REG_RESOURCE_LIST:
	case REG_RESOURCE_REQUIREMENTS_LIST:
		{
			if (dwType == REG_BINARY)
			{
				_tcscpy_s(szProcessed, nSize, _T("hex:"));
			}
			else
			{
				wsprintf(szProcessed, _T("hex(%d):"), dwType);
			}

			for (DWORD dwIndex = 0; dwIndex < dwValueLen; dwIndex++)
			{
				TCHAR	szValue[8];

				wsprintf(szValue, _T("%02X%s"), lpValue[dwIndex], ((dwIndex == (dwValueLen - 1)) ? _T("") : _T(",")));
				_tcscat_s(szProcessed, nSize, szValue);

				if ((dwIndex != 0) && ((dwIndex % 0x15) == 0) && (dwIndex != (dwValueLen - 1)))
				{
					_tcscat_s(szProcessed, nSize, _T("\\\r\n  "));
				}
			}

			_tcscat_s(szProcessed, nSize, _T("\r\n"));
		}

		break;
	case REG_NONE:
	case REG_DWORD_BIG_ENDIAN:
	case REG_LINK:
		break;
	}
}
