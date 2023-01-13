// FontManagerX.h

#pragma once

#include "InAPI.h"

#ifndef MAX_TTC_FONTS
#define MAX_TTC_FONTS	32
#endif

#ifndef OTF_TAG_LENGTH
#define OTF_TAG_LENGTH	4
#endif

#ifndef OTF_VERSION_LEN
#define OTF_VERSION_LEN		64
#define OTF_COPYRIGHT_LEN	512
#endif

#ifndef SWAPWORD
#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#endif

#ifndef SWAPLONG
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))
#endif

#ifndef SWAPDWORD
#define SWAPDWORD(x)	(DWORD)(MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x))))
#endif

class INAPI_CLASS CFontManagerX
{
public:
	CFontManagerX(void);
	virtual	~CFontManagerX(void);

	enum FontTypeX
	{
		FTX_UNKNOWN,	// Unknown
		FTX_TTF,		// TrueType Font including Agfa AC3 Compressed Font
		FTX_OTF,		// OpenType Font including Agfa AC3 Compressed Font
		FTX_TTC,		// TrueType Collections including Agfa AC3 Compressed Font
		FTX_RESERVED	// Reserved
	};

	BOOL	IsValidFont(void);
	DWORD	GetFontCount(void);

	// Font Loader
	BOOL	LoadFontFile(LPCTSTR szFontFile);

	// Additional Information
	TCHAR	m_szFontFace[MAX_TTC_FONTS][LF_FULLFACESIZE];
	TCHAR	m_szFontVersion[MAX_TTC_FONTS][OTF_VERSION_LEN];
	TCHAR	m_szFontCopyrights[MAX_TTC_FONTS][OTF_COPYRIGHT_LEN];
	int		m_nEncodingID[MAX_TTC_FONTS];
	BOOL	m_bClearTypeOptimized;
	BOOL	m_bDigitallySigned;

protected:
	// Processors for Required Tables
	virtual void	Process_cmap(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_head(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_hhea(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_hmtx(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_maxp(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_name(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_OS_2(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_post(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);

	// Processors for Tables Related to TrueType Outlines
	virtual void	Process_cvt_(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_fpgm(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_glyf(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_loca(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_prep(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);

	// Processors for Tables Related to PostScript Outlines
	virtual void	Process_CFF_(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_VORG(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);

	// Processors for Tables Related to Bitmap Glyphs
	virtual void	Process_EBDT(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_EBLC(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_EBSC(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);

	// Processors for Advanced Typographic Tables
	virtual void	Process_BASE(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_GDEF(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_GPOS(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_GSUB(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_JSTF(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);

	// Processors for Other OpenType Tables
	virtual void	Process_DSIG(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_gasp(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_hdmx(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_kern(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_LTSH(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_PCLT(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_VDMX(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_vhea(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);
	virtual void	Process_vmtx(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile);

	// Base Information
	FontTypeX	m_ftFontType;
	DWORD		m_dwFontNum;
};
