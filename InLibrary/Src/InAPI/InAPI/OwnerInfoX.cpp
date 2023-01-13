// OwnerInfoX.cpp

#include "StdAfx.h"
#include "OwnerInfoX.h"
#include "RegistryX.h"

COwnerInfoX::COwnerInfoX(void)
{
}

COwnerInfoX::~COwnerInfoX(void)
{
}

BOOL COwnerInfoX::IsDisplayOwnerTurnedOn(void)
{
	GetOwnerInformation();
	return m_bDisplayOwner;
}

BOOL COwnerInfoX::IsDisplayNotesTurnedOn(void)
{
	GetOwnerInformation();
	return m_bDisplayNotes;
}

void COwnerInfoX::DisplayOwnerTurnedOn(BOOL bDisplay)
{
	GetOwnerInformation();

	m_bDisplayOwner	= bDisplay;

	SetOwnerInformation();
}

void COwnerInfoX::DisplayNotesTurnedOn(BOOL bDisplay)
{
	GetOwnerInformation();

	m_bDisplayNotes	= bDisplay;

	SetOwnerInformation();
}

LPCTSTR	COwnerInfoX::GetName(void)
{
	GetOwnerInformation();
	return m_szName;
}

LPCTSTR	COwnerInfoX::GetCompany(void)
{
	GetOwnerInformation();
	return m_szCompany;
}

LPCTSTR	COwnerInfoX::GetAddress(void)
{
	GetOwnerInformation();
	return m_szAddress;
}

LPCTSTR	COwnerInfoX::GetTelephone(void)
{
	GetOwnerInformation();
	return m_szTelephone;
}

LPCTSTR	COwnerInfoX::GetEMail(void)
{
	GetOwnerInformation();
	return m_szEMail;
}

LPCTSTR	COwnerInfoX::GetNotes(void)
{
	GetOwnerInformation();
	return m_szNotes;
}

void COwnerInfoX::SetName(LPCTSTR lpszName)
{
	GetOwnerInformation();
	wcscpy_s(m_szName, OIX_LEN_NAME, lpszName);
	SetOwnerInformation();
}

void COwnerInfoX::SetCompany(LPCTSTR lpszCompany)
{
	GetOwnerInformation();
	wcscpy_s(m_szCompany, OIX_LEN_COMPANY, lpszCompany);
	SetOwnerInformation();
}

void COwnerInfoX::SetAddress(LPCTSTR lpszAddress)
{
	GetOwnerInformation();
	wcscpy_s(m_szAddress, OIX_LEN_ADDRESS, lpszAddress);
	SetOwnerInformation();
}

void COwnerInfoX::SetTelephone(LPCTSTR lpszTelephone)
{
	GetOwnerInformation();
	wcscpy_s(m_szTelephone, OIX_LEN_TELEPHONE, lpszTelephone);
	SetOwnerInformation();
}

void COwnerInfoX::SetEMail(LPCTSTR lpszEMail)
{
	GetOwnerInformation();
	wcscpy_s(m_szEMail, OIX_LEN_EMAIL, lpszEMail);
	SetOwnerInformation();
}

void COwnerInfoX::SetNotes(LPCTSTR lpszNotes)
{
	GetOwnerInformation();
	wcscpy_s(m_szNotes, OIX_LEN_NOTES, lpszNotes);
	SetOwnerInformation();
}

#define OWNER_ROOT			_T("ControlPanel\\Owner")
#define OWNER_VALUE			_T("Owner")
#define NOTES_VALUE			_T("Owner Notes")
#define NAME_STRING			_T("Name")
#define TELEPHONE_STRING	_T("Telephone")
#define EMAIL_STRING		_T("E-mail")
#define NOTES_STRING		_T("Notes")

void COwnerInfoX::GetOwnerInformation(void)
{
	CRegistryX	reg;
	WCHAR*		pOwner	= new WCHAR[OIX_BUF_OWNER / sizeof(WCHAR)];
	DWORD		dwOwner	= OIX_BUF_OWNER;
	WCHAR*		pNotes	= new WCHAR[OIX_BUF_NOTES / sizeof(WCHAR)];
	DWORD		dwNotes	= OIX_BUF_NOTES;

	reg.Open(HKEY_CURRENT_USER, OWNER_ROOT);
	memset(pOwner, 0, OIX_BUF_OWNER);
	_tcscpy_s(m_szName, OIX_LEN_NAME, _T(""));
	_tcscpy_s(m_szCompany, OIX_LEN_COMPANY, _T(""));
	_tcscpy_s(m_szAddress, OIX_LEN_ADDRESS, _T(""));
	_tcscpy_s(m_szTelephone, OIX_LEN_ADDRESS, _T(""));
	_tcscpy_s(m_szEMail, OIX_LEN_EMAIL, _T(""));

	if (reg.ReadBinary(OWNER_VALUE, (LPBYTE)pOwner, &dwOwner))
	{
		if ((TCHAR)(pOwner + OIX_POS_NAME))
		{
			wcscpy_s(m_szName, OIX_LEN_NAME, static_cast<LPCTSTR>(pOwner + OIX_POS_NAME));
		}

		if ((TCHAR)(pOwner + OIX_POS_COMPANY))
		{
			wcscpy_s(m_szCompany, OIX_LEN_COMPANY, static_cast<LPCTSTR>(pOwner + OIX_POS_COMPANY));
		}

		if ((TCHAR)(pOwner + OIX_POS_ADDRESS))
		{
			wcscpy_s(m_szAddress, OIX_LEN_ADDRESS, static_cast<LPCTSTR>(pOwner + OIX_POS_ADDRESS));
		}

		if ((TCHAR)(pOwner + OIX_POS_TELEPHONE))
		{
			wcscpy_s(m_szTelephone, OIX_LEN_TELEPHONE, static_cast<LPCTSTR>(pOwner + OIX_POS_TELEPHONE));
		}

		if ((TCHAR)(pOwner + OIX_POS_EMAIL))
		{
			wcscpy_s(m_szEMail, OIX_LEN_EMAIL, static_cast<LPCTSTR>(pOwner + OIX_POS_EMAIL));
		}

		m_bDisplayOwner	= static_cast<BOOL>(reinterpret_cast<WCHAR>(pOwner + OIX_POS_DISPOWNER));
	}

	memset(pNotes, 0, OIX_BUF_NOTES);
	_tcscpy_s(m_szNotes, OIX_LEN_NOTES, _T(""));

	if (reg.ReadBinary(NOTES_VALUE, reinterpret_cast<LPBYTE>(pNotes), &dwNotes))
	{
		if (reinterpret_cast<TCHAR>(pNotes + OIX_POS_NOTES))
		{
			wcscpy_s(m_szNotes, OIX_LEN_NOTES, static_cast<LPCTSTR>(pNotes + OIX_POS_NOTES));
		}

		m_bDisplayNotes	= static_cast<BOOL>(reinterpret_cast<WCHAR>(pNotes + OIX_POS_DISPNOTES));
	}

	delete [] pOwner;
	delete [] pNotes;

	reg.Close();
}

void COwnerInfoX::SetOwnerInformation(void)
{
	CRegistryX	reg;
	WCHAR*		pOwner	= new WCHAR[OIX_BUF_OWNER / sizeof(WCHAR)];
	DWORD		dwOwner	= OIX_BUF_OWNER;
	WCHAR*		pNotes	= new WCHAR[OIX_BUF_NOTES / sizeof(WCHAR)];
	DWORD		dwNotes	= OIX_BUF_NOTES;

	reg.Open(HKEY_CURRENT_USER, OWNER_ROOT);
	memset(pOwner, 0, OIX_BUF_OWNER);
	memcpy_s((pOwner + OIX_POS_NAME), OIX_BUF_OWNER - OIX_POS_NAME, m_szName, sizeof(TCHAR) * wcslen(m_szName));
	memcpy_s((pOwner + OIX_POS_COMPANY), OIX_BUF_OWNER - OIX_POS_COMPANY, m_szCompany, sizeof(TCHAR) * wcslen(m_szCompany));
	memcpy_s((pOwner + OIX_POS_ADDRESS), OIX_BUF_OWNER - OIX_POS_ADDRESS, m_szAddress, sizeof(TCHAR) * wcslen(m_szAddress));
	memcpy_s((pOwner + OIX_POS_TELEPHONE), OIX_BUF_OWNER - OIX_POS_TELEPHONE, m_szTelephone, sizeof(TCHAR) * wcslen(m_szTelephone));
	memcpy_s((pOwner + OIX_POS_EMAIL), OIX_BUF_OWNER - OIX_POS_EMAIL, m_szEMail, sizeof(TCHAR) * wcslen(m_szEMail));
	memcpy_s((pOwner + OIX_POS_DISPOWNER), OIX_BUF_OWNER - OIX_POS_DISPOWNER, reinterpret_cast<const void*>((WORD)m_bDisplayOwner), sizeof(WORD));
	reg.WriteBinary(OWNER_VALUE, (LPBYTE)pOwner, dwOwner);
	memset(pNotes, 0, OIX_BUF_NOTES);
	memcpy_s((pNotes + OIX_POS_NOTES), OIX_BUF_NOTES - OIX_POS_NOTES, m_szNotes, sizeof(TCHAR) * wcslen(m_szNotes));
	memcpy_s((pOwner + OIX_POS_DISPNOTES), OIX_BUF_OWNER - OIX_POS_DISPNOTES, reinterpret_cast<const void*>((WORD)m_bDisplayNotes), sizeof(WORD));
	reg.WriteBinary(NOTES_VALUE, (LPBYTE)pNotes, dwNotes);

	delete [] pOwner;
	delete [] pNotes;

	reg.WriteString(NAME_STRING, m_szName);
	reg.WriteString(TELEPHONE_STRING, m_szTelephone);
	reg.WriteString(EMAIL_STRING, m_szEMail);
	reg.WriteString(NOTES_STRING, m_szNotes);
	reg.Close();
}
