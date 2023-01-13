// VersionInfoX.h

#pragma once

#include "InAPI.h"

typedef enum _VI_CP
{
	VI_CP_ASCII		= 0,
	VI_CP_JAPAN		= 932,
	VI_CP_KOREA		= 949,
	VI_CP_TAIWAN	= 950,
	VI_CP_UNICODE	= 1200,
	VI_CP_LATIN2	= 1250,
	VI_CP_CYRILLIC	= 1251,
	VI_CP_MULTILNG	= 1252,
	VI_CP_GREEK		= 1253,
	VI_CP_TURKISH	= 1254,
	VI_CP_HEBREW	= 1255,
	VI_CP_ARABIC	= 1256
} VI_CP;

typedef enum _VI_STR
{
	VI_STR_COMMENTS			= 0,
	VI_STR_COMPANYNAME		= 1,
	VI_STR_FILEDESCRIPTION	= 2,
	VI_STR_FILEVERSION		= 3,
	VI_STR_INTERNALNAME		= 4,
	VI_STR_LEGALCOPYRIGHT	= 5,
	VI_STR_LEGALTRADEMARKS	= 6,
	VI_STR_ORIGINALFILENAME	= 7,
	VI_STR_PRIVATEBUILD		= 8,
	VI_STR_PRODUCTNAME		= 9,
	VI_STR_PRODUCTVERSION	= 10,
	VI_STR_SPECIALBUILD		= 11,
	VI_STR_OLESELFREGISTER	= 12
} VI_STR;

class INAPI_CLASS CVersionInfoX
{
public:
	CVersionInfoX(void);
	virtual	~CVersionInfoX(void);

	BOOL	Open(LPCTSTR lpszFileName);
	BOOL	Open(HINSTANCE hInstance);
	void	Close(void);

	BOOL	QueryStringValue(INT nIndex, LPTSTR lpszValue, INT nBuf) const;
	BOOL	QueryStringValue(LPCTSTR lpszItem, LPTSTR lpszValue, INT nBuf) const;
	LPCTSTR	GetVerStringName(INT nIndex);

	BOOL	SetTrans(LANGID wLID = LANG_NEUTRAL, WORD wCP = VI_CP_UNICODE);
	BOOL	SetTransIndex(UINT nIndex = 0);
	INT		FindTrans(LANGID wLID, WORD wCP) const;
	DWORD	GetTransByIndex(UINT nIndex) const;

	inline const	VS_FIXEDFILEINFO &GetVSFFI(void) const;
	inline BOOL		IsValid(void) const;

	inline WORD	GetFileVersionMajor(void) const;
	inline WORD	GetFileVersionMinor(void) const;
	inline WORD	GetFileVersionBuild(void) const;
	inline WORD	GetFileVersionQFE(void) const;

	inline WORD	GetProductVersionMajor(void) const;
	inline WORD	GetProductVersionMinor(void) const;
	inline WORD	GetProductVersionBuild(void) const;
	inline WORD	GetProductVersionQFE(void) const;

	inline UINT		GetTransCount(void) const;
	inline UINT		GetCurTransIndex(void) const;
	inline LANGID	GetLIDByIndex(UINT nIndex) const;
	inline WORD		GetCPByIndex(UINT nIndex) const;
	inline DWORD	GetCurTrans(void) const;
	inline LANGID	GetCurLID(void) const;
	inline WORD		GetCurCP(void) const;

protected:
	BOOL	GetVersionInfo(LPCTSTR lpszFileName);
	BOOL	QueryVersionTrans(void);

	static LPCTSTR		m_arrlpszVS[13];
	VS_FIXEDFILEINFO	m_vsffi;

	LPBYTE	m_lpVIB;
	BOOL	m_bValid;

	LPDWORD	m_lpdwTrans;
	UINT	m_nTransCur;
	UINT	m_nTransCnt;
};
