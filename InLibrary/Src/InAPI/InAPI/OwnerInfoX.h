// OwnerInfoX.h

#pragma once

#include "InAPI.h"

#ifndef OIX_BUF_OWNER
#define OIX_BUF_OWNER		640
#define OIX_BUF_NOTES		388
#endif

#ifndef OIX_LEN_NAME
#define OIX_LEN_NAME		36
#define OIX_LEN_COMPANY		36
#define OIX_LEN_ADDRESS		186
#define OIX_LEN_TELEPHONE	25
#define OIX_LEN_EMAIL		36
#define OIX_LEN_DISPOWNER	1
#define OIX_LEN_NOTES		193
#define OIX_LEN_DISPNOTES	1
#endif

#ifndef OIX_POS_NAME
#define OIX_POS_NAME		0
#define OIX_POS_COMPANY		(OIX_POS_NAME + OIX_LEN_NAME)
#define OIX_POS_ADDRESS		(OIX_POS_COMPANY + OIX_LEN_COMPANY)
#define OIX_POS_TELEPHONE	(OIX_POS_ADDRESS + OIX_LEN_ADDRESS)
#define OIX_POS_EMAIL		(OIX_POS_TELEPHONE + OIX_LEN_TELEPHONE)
#define OIX_POS_DISPOWNER	(OIX_POS_EMAIL + OIX_LEN_EMAIL)
#define OIX_POS_NOTES		0
#define OIX_POS_DISPNOTES	(OIX_POS_NOTES + OIX_LEN_NOTES)
#endif

class INAPI_CLASS COwnerInfoX
{
public:
	COwnerInfoX(void);
	virtual	~COwnerInfoX(void);

	BOOL	IsDisplayOwnerTurnedOn(void);
	BOOL	IsDisplayNotesTurnedOn(void);
	void	DisplayOwnerTurnedOn(BOOL bDisplay);
	void	DisplayNotesTurnedOn(BOOL bDisplay);

	LPCTSTR	GetName(void);
	LPCTSTR	GetCompany(void);
	LPCTSTR	GetAddress(void);
	LPCTSTR	GetTelephone(void);
	LPCTSTR	GetEMail(void);
	LPCTSTR	GetNotes(void);

	void	SetName(LPCTSTR lpszName);
	void	SetCompany(LPCTSTR lpszCompany);
	void	SetAddress(LPCTSTR lpszAddress);
	void	SetTelephone(LPCTSTR lpszTelephone);
	void	SetEMail(LPCTSTR lpszEMail);
	void	SetNotes(LPCTSTR lpszNotes);

protected:
	void	GetOwnerInformation(void);
	void	SetOwnerInformation(void);

	TCHAR	m_szName[OIX_LEN_NAME];
	TCHAR	m_szCompany[OIX_LEN_COMPANY];
	TCHAR	m_szAddress[OIX_LEN_ADDRESS];
	TCHAR	m_szTelephone[OIX_LEN_TELEPHONE];
	TCHAR	m_szEMail[OIX_LEN_EMAIL];
	TCHAR	m_szNotes[OIX_LEN_NOTES];

	BOOL	m_bDisplayOwner;
	BOOL	m_bDisplayNotes;
};
