// VersionInfoX.cpp

#include "StdAfx.h"
#include "VersionInfoX.h"

CVersionInfoX::CVersionInfoX(void)
{
	m_lpVIB	= NULL;

	Close();
}

CVersionInfoX::~CVersionInfoX(void)
{
	Close();
}

LPCTSTR	CVersionInfoX::m_arrlpszVS[]	=
{
	_T("Comments"),
	_T("CompanyName"),
	_T("FileDescription"),
	_T("FileVersion"),
	_T("InternalName"),
	_T("LegalCopyright"),
	_T("LegalTrademarks"),
	_T("OriginalFilename"),
	_T("PrivateBuild"),
	_T("ProductName"),
	_T("ProductVersion"),
	_T("SpecialBuild"),
	_T("OLESelfRegister")
};

BOOL CVersionInfoX::Open(HINSTANCE hInstance)
{
	if (hInstance == NULL)
	{
		return FALSE;
	}

	TCHAR	szFileName[MAX_PATH]	= { 0 };

	if (::GetModuleFileName(hInstance, szFileName, MAX_PATH))
	{
		return Open(szFileName);
	}

	return FALSE;
}

BOOL CVersionInfoX::Open(LPCTSTR lpszFileName)
{
	if (lpszFileName == NULL)
	{
		return FALSE;
	}

	Close();

	if ((!GetVersionInfo(lpszFileName)) || (!QueryVersionTrans()))
	{
		Close();
	}

	return m_bValid;
}

void CVersionInfoX::Close()
{
	m_bValid	= FALSE;
	m_nTransCnt	= 0;
	m_nTransCur	= 0;
	m_lpdwTrans	= NULL;

	::ZeroMemory(&m_vsffi, sizeof(VS_FIXEDFILEINFO));

	if (m_lpVIB != NULL)
	{
		free(m_lpVIB);

		m_lpVIB	= NULL;
	}
}

BOOL CVersionInfoX::QueryStringValue(LPCTSTR lpszItem, LPTSTR lpszValue, INT nBuf) const
{
	if ((m_bValid  == FALSE) || (lpszItem == NULL))
	{
		return FALSE;
	}

	if ((lpszValue != NULL) && (nBuf <= 0))
	{
		return FALSE;
	}

	::ZeroMemory(lpszValue, nBuf * sizeof(TCHAR));

	TCHAR	szSFI[MAX_PATH]	= { 0 };

	::wsprintf(szSFI, _T("\\StringFileInfo\\%04X%04X\\%s"), GetCurLID(), GetCurCP(), lpszItem);

	BOOL	bRes	= FALSE;
	UINT	uLen	= 0;
	LPTSTR	lpszBuf	= NULL;

	if (::VerQueryValue(m_lpVIB, static_cast<LPTSTR>(szSFI), reinterpret_cast<LPVOID*>(&lpszBuf), &uLen))
	{
		if ((lpszValue != NULL) && (nBuf > 0))
		{
			bRes	= static_cast<BOOL>(::wcsncpy(lpszValue, lpszBuf, nBuf) != NULL);
		}
		else
		{
			bRes	= TRUE;
		}
	}

	return bRes;
}

BOOL CVersionInfoX::QueryStringValue(INT nIndex, LPTSTR lpszValue, INT nBuf) const
{
	if ((nIndex < VI_STR_COMMENTS) || (nIndex > VI_STR_OLESELFREGISTER))
	{
		return FALSE;
	}

	return QueryStringValue(m_arrlpszVS[nIndex], lpszValue, nBuf);
}

LPCTSTR CVersionInfoX::GetVerStringName(INT nIndex)
{
	if ((nIndex < VI_STR_COMMENTS) || (nIndex > VI_STR_OLESELFREGISTER))
	{
		return NULL;
	}

	return (LPCTSTR)m_arrlpszVS[nIndex];
}

INT CVersionInfoX::FindTrans(LANGID wLID, WORD wCP) const
{
	if (m_bValid == FALSE)
	{
		return -1;
	}

	for (UINT n = 0; n < m_nTransCnt; n++)
	{
		if ((LOWORD(m_lpdwTrans[n]) == wLID) && (HIWORD(m_lpdwTrans[n]) == wCP))
		{
			return n;
		}
	}

	return -1;
}

BOOL CVersionInfoX::SetTrans(LANGID wLID, WORD wCP)
{
	if (m_bValid == FALSE)
	{
		return FALSE;
	}

	if ((GetCurLID() == wLID) && (GetCurCP() == wCP))
	{
		return TRUE;
	}

	INT	nPos	= FindTrans(wLID, wCP);

	if (nPos != -1)
	{
		m_nTransCur	= nPos;
	}

	return (m_nTransCur == (UINT)nPos);
}

DWORD CVersionInfoX::GetTransByIndex(UINT nIndex) const
{
	if ((m_bValid == FALSE) || (nIndex < 0) || (nIndex > m_nTransCnt))
	{
		return 0;
	}

	return m_lpdwTrans[nIndex];
}

BOOL CVersionInfoX::SetTransIndex(UINT nIndex)
{
	if (m_bValid == FALSE)
	{
		return FALSE;
	}

	if (m_nTransCur == nIndex)
	{
		return TRUE;
	}

	if ((nIndex >= 0) && (nIndex <= m_nTransCnt))
	{
		m_nTransCur = nIndex;
	}

	return (m_nTransCur == nIndex);
}

inline BOOL CVersionInfoX::IsValid(void) const
{
	return m_bValid;
}

inline const VS_FIXEDFILEINFO &CVersionInfoX::GetVSFFI(void) const
{
	return m_vsffi;
}

inline WORD CVersionInfoX::GetFileVersionMajor(void) const
{
	ASSERT(m_bValid);
	return HIWORD(m_vsffi.dwFileVersionMS);
}

inline WORD CVersionInfoX::GetFileVersionMinor(void) const
{
	ASSERT(m_bValid);
	return LOWORD(m_vsffi.dwFileVersionMS);
}

inline WORD CVersionInfoX::GetFileVersionBuild(void) const
{
	ASSERT(m_bValid);
	return HIWORD(m_vsffi.dwFileVersionLS);
}

inline WORD CVersionInfoX::GetFileVersionQFE(void) const
{
	ASSERT(m_bValid);
	return LOWORD(m_vsffi.dwFileVersionLS);
}

inline WORD CVersionInfoX::GetProductVersionMajor(void) const
{
	ASSERT(m_bValid);
	return HIWORD(m_vsffi.dwProductVersionMS);
}

inline WORD CVersionInfoX::GetProductVersionMinor(void) const
{
	ASSERT(m_bValid);
	return LOWORD(m_vsffi.dwProductVersionMS);
}

inline WORD CVersionInfoX::GetProductVersionBuild(void) const
{
	ASSERT(m_bValid);
	return HIWORD(m_vsffi.dwProductVersionLS);
}

inline WORD CVersionInfoX::GetProductVersionQFE(void) const
{
	ASSERT(m_bValid);
	return LOWORD(m_vsffi.dwProductVersionLS);
}

inline UINT CVersionInfoX::GetTransCount(void) const
{
	ASSERT(m_bValid);
	return m_nTransCnt;
}

inline UINT CVersionInfoX::GetCurTransIndex(void) const
{
	ASSERT(m_bValid);
	return m_nTransCur;
}

inline LANGID CVersionInfoX::GetLIDByIndex(UINT nIndex) const
{
	return LOWORD(GetTransByIndex(nIndex));
}

inline WORD CVersionInfoX::GetCPByIndex(UINT nIndex) const
{
	return HIWORD(GetTransByIndex(nIndex));
}

inline DWORD CVersionInfoX::GetCurTrans(void) const
{
	return GetTransByIndex(GetCurTransIndex());
}

inline LANGID CVersionInfoX::GetCurLID(void) const
{
	return GetLIDByIndex(GetCurTransIndex());
}

inline WORD CVersionInfoX::GetCurCP(void) const
{
	return GetCPByIndex(GetCurTransIndex());
}

BOOL CVersionInfoX::GetVersionInfo(LPCTSTR lpszFileName)
{
	DWORD	dwDummy	= 0;
	DWORD	dwSize	= ::GetFileVersionInfoSize(const_cast<LPTSTR>(lpszFileName), &dwDummy);

	if (dwSize > 0)
	{
		m_lpVIB	= reinterpret_cast<LPBYTE>(malloc(dwSize));

		if ((m_lpVIB != NULL) && (::GetFileVersionInfo(const_cast<LPTSTR>(lpszFileName), 0, dwSize, m_lpVIB)))
		{
			UINT	uLen	= 0;
			LPVOID	lpVSFFI	= NULL;

			if (::VerQueryValue(m_lpVIB, _T("\\"), (LPVOID*)&lpVSFFI, &uLen))
			{
				::CopyMemory(&m_vsffi, lpVSFFI, sizeof(VS_FIXEDFILEINFO));

				m_bValid	= (m_vsffi.dwSignature == VS_FFI_SIGNATURE);
			}
		}
	}

	return m_bValid;
}

BOOL CVersionInfoX::QueryVersionTrans(void)
{
	if (m_bValid)
	{
		return FALSE;
	}

	UINT	uLen	= 0;
	LPVOID	lpBuf	= NULL;

	if (::VerQueryValue(m_lpVIB, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpBuf, &uLen))
	{
		m_lpdwTrans	= reinterpret_cast<LPDWORD>(lpBuf);
		m_nTransCnt	= (uLen / sizeof(DWORD));
	}

	return (BOOL)(m_lpdwTrans != NULL);
}
