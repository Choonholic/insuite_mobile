// FontManagerX.cpp

#include "StdAfx.h"
#include "FontManagerX.h"

typedef struct _TTC_HEADER
{
	BYTE	TTCTag[OTF_TAG_LENGTH];	// TrueType Collection ID string: 'ttcf'
	DWORD	Version;				// Version of the TTC Header
	DWORD	numFonts;				// Number of fonts in TTC
} TTC_HEADER;

typedef struct _TTC_OFFSETTABLE
{
	DWORD	ulOffsetTable;			// Array of offsets to the OffsetTable for each font from the beginning of the file
} TTC_OFFSETTABLE;

typedef struct _TTC_DSIGHEADER
{
	DWORD	ulDsigTag;				// Tag indicating that a DSIG table exists, 0x44534947 ('DSIG') (null if no signature)
	DWORD	ulDsigLength;			// The length (in bytes) of the DSIG table (null if no signature)
	DWORD	ulDsigOffset;			// The offset (in bytes) of the DSIG table from the beginning of the TTC file (null if no signature)
} TTC_DSIGHEADER;

typedef struct _OTF_OFFSETTABLE
{
	DWORD	sfntVersion;			// 0x00010000 for version 1.0
	WORD	numTables;				// Number of tables
	WORD	searchRange;			// (Maximum power of 2 <= numTables) X 16
	WORD	entrySelector;			// Log2(maximum power of 2 <= numTables)
	WORD	rangeShift;				// NumTables X 16 - searchRange
} OTF_OFFSETTABLE;

typedef struct _OTF_TABLEDIRECTORY
{
	BYTE	DirTag[OTF_TAG_LENGTH];	// 4-byte identifier
	DWORD	checkSum;				// CheckSum for this table
	DWORD	offset;					// Offset from beginning of TrueType font file
	DWORD	length;					// Length of this table
} OTF_TABLEDIRECTORY;

CFontManagerX::CFontManagerX(void)
{
	m_ftFontType	= FTX_UNKNOWN;
	m_dwFontNum		= 0;

	ZeroMemory(m_szFontFace, (sizeof(TCHAR) * MAX_TTC_FONTS * LF_FULLFACESIZE));
	ZeroMemory(m_szFontVersion, (sizeof(TCHAR) * MAX_TTC_FONTS * OTF_VERSION_LEN));
	ZeroMemory(m_szFontCopyrights, (sizeof(TCHAR) * MAX_TTC_FONTS * OTF_COPYRIGHT_LEN));
	ZeroMemory(m_nEncodingID, (sizeof(int) * MAX_TTC_FONTS));

	m_bClearTypeOptimized	= FALSE;
	m_bDigitallySigned		= FALSE;
}

CFontManagerX::~CFontManagerX(void)
{
}

BOOL CFontManagerX::IsValidFont(void)
{
	return static_cast<BOOL>(m_ftFontType != FTX_UNKNOWN);
}

DWORD CFontManagerX::GetFontCount(void)
{
	if (IsValidFont())
	{
		return m_dwFontNum;
	}

	return 0;
}

BOOL CFontManagerX::LoadFontFile(LPCTSTR szFontFile)
{
	HANDLE	hFile;
	DWORD	i;
	DWORD	j;

	// Initialization
	m_ftFontType			= FTX_UNKNOWN;
	m_dwFontNum				= 0;
	m_bClearTypeOptimized	= FALSE;
	m_bDigitallySigned		= FALSE;

	// Opening File
	hFile	= CreateFile(szFontFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Failed to load file
		return FALSE;
	}

	// Checking TTC Header
	DWORD			dwRead;
	TTC_HEADER		ttcHeader;
	TTC_OFFSETTABLE	ttcOffsetTable[MAX_TTC_FONTS];
	TTC_DSIGHEADER	ttcDsigHeader;
	OTF_OFFSETTABLE	otfOffsetTable[MAX_TTC_FONTS];

	ReadFile(hFile, &ttcHeader, sizeof(TTC_HEADER), &dwRead, NULL);

	if (strncmp(reinterpret_cast<const char *>(ttcHeader.TTCTag), "ttcf", OTF_TAG_LENGTH)) // Not Unicode
	{
		// Not TTC, so check sfnt version temporarily
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		ReadFile(hFile, &otfOffsetTable[0], sizeof(OTF_OFFSETTABLE), &dwRead, NULL);

		otfOffsetTable[0].sfntVersion	= SWAPDWORD(otfOffsetTable[0].sfntVersion);
		otfOffsetTable[0].numTables		= SWAPWORD(otfOffsetTable[0].numTables);
		otfOffsetTable[0].searchRange	= SWAPWORD(otfOffsetTable[0].searchRange);
		otfOffsetTable[0].entrySelector	= SWAPWORD(otfOffsetTable[0].entrySelector);
		otfOffsetTable[0].rangeShift	= SWAPWORD(otfOffsetTable[0].rangeShift);

		if (otfOffsetTable[0].sfntVersion != 0x00010000)
		{
			// Not OpenType or TrueType Font
			m_ftFontType	= FTX_UNKNOWN;
			m_dwFontNum		= 0;

			CloseHandle(hFile);
			return FALSE;
		}

		m_ftFontType					= FTX_TTF;
		m_dwFontNum						= 1;
		ttcOffsetTable[0].ulOffsetTable	= 0;
	}
	else
	{
		ttcHeader.Version	= SWAPDWORD(ttcHeader.Version);
		ttcHeader.numFonts	= SWAPDWORD(ttcHeader.numFonts);

		if ((ttcHeader.Version != 0x00010000) && (ttcHeader.Version != 0x00020000))
		{
			// Unknown TTC Version
			m_ftFontType	= FTX_UNKNOWN;
			m_dwFontNum		= 0;

			CloseHandle(hFile);
			return FALSE;
		}
		else
		{
			// TTC Fonts
			m_ftFontType	= FTX_TTC;
			m_dwFontNum		= ttcHeader.numFonts;

			for (i = 0; i < ttcHeader.numFonts; i++)
			{
				ReadFile(hFile, &ttcOffsetTable[i], sizeof(TTC_OFFSETTABLE), &dwRead, NULL);

				ttcOffsetTable[i].ulOffsetTable	= SWAPDWORD(ttcOffsetTable[i].ulOffsetTable);
			}

			if (ttcHeader.Version == 0x00020000)
			{
				ReadFile(hFile, &ttcDsigHeader, sizeof(TTC_DSIGHEADER), &dwRead, NULL);
			}
		}
	}

	// Reading Table Directories
	OTF_TABLEDIRECTORY	otfTableDirectory;
	DWORD				dwCurrentPos;

	for (i = 0; i < m_dwFontNum; i++)
	{
		// Reading Offset Table
		SetFilePointer(hFile, ttcOffsetTable[i].ulOffsetTable, NULL, FILE_BEGIN);
		ReadFile(hFile, &otfOffsetTable[i], sizeof(OTF_OFFSETTABLE), &dwRead, NULL);

		otfOffsetTable[i].sfntVersion	= SWAPDWORD(otfOffsetTable[i].sfntVersion);
		otfOffsetTable[i].numTables		= SWAPWORD(otfOffsetTable[i].numTables);
		otfOffsetTable[i].searchRange	= SWAPWORD(otfOffsetTable[i].searchRange);
		otfOffsetTable[i].entrySelector	= SWAPWORD(otfOffsetTable[i].entrySelector);
		otfOffsetTable[i].rangeShift	= SWAPWORD(otfOffsetTable[i].rangeShift);

		if (otfOffsetTable[i].sfntVersion != 0x00010000)
		{
			// Not OpenType or TrueType Font
			m_ftFontType	= FTX_UNKNOWN;
			m_dwFontNum		= 0;

			CloseHandle(hFile);
			return FALSE;
		}

		for (j = 0; j < otfOffsetTable[i].numTables; j++)
		{
			ReadFile(hFile, &otfTableDirectory, sizeof(OTF_TABLEDIRECTORY), &dwRead, NULL);

			// Tags are not Unicode string, so we have to use standard str... functions
			otfTableDirectory.checkSum	= SWAPDWORD(otfTableDirectory.checkSum);
			otfTableDirectory.offset	= SWAPDWORD(otfTableDirectory.offset);
			otfTableDirectory.length	= SWAPDWORD(otfTableDirectory.length);

			dwCurrentPos	= SetFilePointer(hFile, 0, NULL, FILE_CURRENT);

			if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "cmap", OTF_TAG_LENGTH))
			{
				// Character to glyph mapping
				Process_cmap(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "head", OTF_TAG_LENGTH))
			{
				// Font header
				Process_head(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "hhea", OTF_TAG_LENGTH))
			{
				// Horizontal header
				Process_hhea(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "hmtx", OTF_TAG_LENGTH))
			{
				// Horizontal metrics
				Process_hmtx(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "maxp", OTF_TAG_LENGTH))
			{
				// Maximum profile
				Process_maxp(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "name", OTF_TAG_LENGTH))
			{
				// Naming table
				Process_name(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "OS/2", OTF_TAG_LENGTH))
			{
				// OS/2 and Windows specific metrics
				Process_OS_2(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "post", OTF_TAG_LENGTH))
			{
				// PostScript information
				Process_post(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "cvt ", OTF_TAG_LENGTH))
			{
				// Control Value Table
				Process_cvt_(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "fpgm", OTF_TAG_LENGTH))
			{
				// Font program
				Process_fpgm(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "glyf", OTF_TAG_LENGTH))
			{
				// Glyph data
				Process_glyf(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "loca", OTF_TAG_LENGTH))
			{
				// Index to location
				Process_loca(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "prep", OTF_TAG_LENGTH))
			{
				// CVT program
				Process_prep(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "CFF ", OTF_TAG_LENGTH))
			{
				// PostScript font program (compact font format)
				Process_CFF_(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "VORG", OTF_TAG_LENGTH))
			{
				// Vertical Origin
				Process_VORG(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "EBDT", OTF_TAG_LENGTH))
			{
				// Embedded bitmap data
				Process_EBDT(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "EBLC", OTF_TAG_LENGTH))
			{
				// Embbeded bitmap location data
				Process_EBLC(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "EBSC", OTF_TAG_LENGTH))
			{
				// Embedded bitmap scaling data
				Process_EBSC(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "BASE", OTF_TAG_LENGTH))
			{
				// Baseline data
				Process_BASE(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "GDEF", OTF_TAG_LENGTH))
			{
				// Glyph definition data
				Process_GDEF(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "GPOS", OTF_TAG_LENGTH))
			{
				// Glyph positioning data
				Process_GPOS(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "GSUB", OTF_TAG_LENGTH))
			{
				// Glyph substitution data
				Process_GSUB(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "JSTF", OTF_TAG_LENGTH))
			{
				// Justification data
				Process_JSTF(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "DSIG", OTF_TAG_LENGTH))
			{
				// Digital signature
				Process_DSIG(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "gasp", OTF_TAG_LENGTH))
			{
				// Grid-fitting/Scan-conversion
				Process_gasp(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "hdmx", OTF_TAG_LENGTH))
			{
				// Horizontal device metrics
				Process_hdmx(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "kern", OTF_TAG_LENGTH))
			{
				// Kerning
				Process_kern(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "LTSH", OTF_TAG_LENGTH))
			{
				// Linear threshold data
				Process_LTSH(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "PCLT", OTF_TAG_LENGTH))
			{
				// PCL 5 Data
				Process_PCLT(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "VDMX", OTF_TAG_LENGTH))
			{
				// Vertical device metrics
				Process_VDMX(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "vhea", OTF_TAG_LENGTH))
			{
				// Vertical Metrics Header
				Process_vhea(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}
			else if (!strncmp(reinterpret_cast<const char *>(otfTableDirectory.DirTag), "vmtx", OTF_TAG_LENGTH))
			{
				// Vertical Metrics
				Process_vmtx(i, otfTableDirectory.offset, otfTableDirectory.length, hFile);
			}

			SetFilePointer(hFile, dwCurrentPos, NULL, FILE_BEGIN);
		}
	}

	CloseHandle(hFile);
	return TRUE;
}

void CFontManagerX::Process_cmap(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

// Font Header (Partial)
typedef struct _OTF_FONT_HEADER_PARTIAL
{
	DWORD	TableVersion;			// 0x00010000 for version 1.0
	DWORD	fontRevision;			// Set by font manufacturer
	DWORD	checkSumAdjustment;		// To compute: set it to 0, sum the entire font as ULONG, then store 0xB1B0AFBA - sum
	DWORD	magicNumber;			// Set to 0x5F0F3CF5
	WORD	flags;					// Flags
/*
	Bit 0: Baseline for font at y = 0
	Bit 1: Left sidebearing point at x=0
	Bit 2: Instructions may depend on point size
	Bit 3: Force ppem to integer values for all internal scaler math; may use fractional ppem sizes if this bit is clear
	Bit 4: Instructions may alter advance width (the advance widths might not scale linearly)
	Bits 5-10: These should be set according to Apple's specification . However, they are not implemented in OpenType
	Bit 11: Font data is 'lossless,' as a result of having been compressed and decompressed with the Agfa MicroType Express engine
	Bit 12: Font converted (produce compatible metrics)
	Bit 13: Font optimised for ClearType
	Bit 14: Reserved, set to 0
	Bit 15: Reserved, set to 0
*/
} OTF_FONT_HEADER_PARTIAL;

void CFontManagerX::Process_head(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
	DWORD					dwRead;
	OTF_FONT_HEADER_PARTIAL	otfFontHeaderPartial;

	// Reading Font Header
	SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
	ReadFile(hFile, &otfFontHeaderPartial, sizeof(OTF_FONT_HEADER_PARTIAL), &dwRead, NULL);

	if ((otfFontHeaderPartial.TableVersion != 0x00000100) || (otfFontHeaderPartial.magicNumber != 0xF53C0F5F))
	{
		// Invalid Table
		return;
	}

	otfFontHeaderPartial.flags	= SWAPWORD(otfFontHeaderPartial.flags);
	m_bClearTypeOptimized		= static_cast<BOOL>((otfFontHeaderPartial.flags & 0x04) == 0x04);
}

void CFontManagerX::Process_hhea(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_hmtx(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_maxp(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

// Naming Table
typedef struct _OTF_NAMING_TABLE
{
	WORD	format;					// Format selector (= 0)
	WORD	count;					// Number of name records
	WORD	stringOffset;			// Offset to start of string storage (from start of table)
} OTF_NAMING_TABLE;

// Name Record
typedef struct _OTF_NAME_RECORD
{
	WORD	platformID;				// Platform ID
	WORD	encodingID;				// Platform-specific encoding ID
	WORD	languageID;				// Language ID
	WORD	nameID;					// Name ID
	WORD	length;					// String length (in bytes)
	WORD	offset;					// String offset from start of storage area (in bytes)
} OTF_NAME_RECORD;

void CFontManagerX::Process_name(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
	DWORD				i, j;
	DWORD				dwRead;
	BYTE				nSwap;
	OTF_NAMING_TABLE	otfNamingTable;
	OTF_NAME_RECORD		otfNameRecord;

	// Reading Naming Table
	SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
	ReadFile(hFile, &otfNamingTable, sizeof(OTF_NAMING_TABLE), &dwRead, NULL);

	if (otfNamingTable.format != 0)
	{
		// Invalid Table
		return;
	}

	otfNamingTable.count		= SWAPWORD(otfNamingTable.count);
	otfNamingTable.stringOffset	= SWAPWORD(otfNamingTable.stringOffset);

	LANGID	LanguageID			= GetSystemDefaultLangID();
	BOOL	bFound				= FALSE;
	DWORD	dwStartingOffset	= dwOffset + otfNamingTable.stringOffset;
	DWORD	dwPos				= SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	BYTE	cBuffer[1024];

	for (i = 0; i < otfNamingTable.count; i++)
	{
		ReadFile(hFile, &otfNameRecord, sizeof(OTF_NAME_RECORD), &dwRead, NULL);

		otfNameRecord.platformID	= SWAPWORD(otfNameRecord.platformID);
		otfNameRecord.encodingID	= SWAPWORD(otfNameRecord.encodingID);
		otfNameRecord.languageID	= SWAPWORD(otfNameRecord.languageID);

		if (otfNameRecord.platformID != 3)
		{
			// We will track items which are 'Windows encoding'.
			continue;
		}

		if ((otfNameRecord.encodingID != 0) && (otfNameRecord.encodingID != 1))
		{
			// We will track items which are 'Symbol' and 'Unicode BMP (UCS-2)'.
			continue;
		}

		if (otfNameRecord.languageID == LanguageID)
		{
			bFound	= TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		LanguageID	= MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
	}

	SetFilePointer(hFile, dwPos, NULL, FILE_BEGIN);

	for (i = 0; i < otfNamingTable.count; i++)
	{
		ReadFile(hFile, &otfNameRecord, sizeof(OTF_NAME_RECORD), &dwRead, NULL);

		otfNameRecord.platformID	= SWAPWORD(otfNameRecord.platformID);
		otfNameRecord.encodingID	= SWAPWORD(otfNameRecord.encodingID);
		otfNameRecord.languageID	= SWAPWORD(otfNameRecord.languageID);
		otfNameRecord.nameID		= SWAPWORD(otfNameRecord.nameID);
		otfNameRecord.length		= SWAPWORD(otfNameRecord.length);
		otfNameRecord.offset		= SWAPWORD(otfNameRecord.offset);

		if (otfNameRecord.platformID != 3)
		{
			// We will track items which are 'Microsoft encoding'.
			continue;
		}

		if ((otfNameRecord.encodingID != 0) && (otfNameRecord.encodingID != 1))
		{
			// We will track items which are 'Symbol' and 'Unicode BMP (UCS-2)'.
			continue;
		}

		if (otfNameRecord.languageID != LanguageID)
		{
			// We will track items which are 'English (United States)'.
			continue;
		}

		m_nEncodingID[nIndex]	= otfNameRecord.encodingID;
		dwPos					= SetFilePointer(hFile, 0, NULL, FILE_CURRENT);

		switch (otfNameRecord.nameID)
		{
		case 0:		// Copyright notice
			{
				SetFilePointer(hFile, dwStartingOffset + otfNameRecord.offset, NULL, FILE_BEGIN);
				ZeroMemory(cBuffer, 1024);
				ReadFile(hFile, cBuffer, otfNameRecord.length, &dwRead, NULL);

				for (j = 0; j < otfNameRecord.length; j += sizeof(WCHAR))
				{
					nSwap			= cBuffer[j];
					cBuffer[j]		= cBuffer[j + 1];
					cBuffer[j + 1]	= nSwap;
				}

				_tcscpy_s(m_szFontCopyrights[nIndex], OTF_COPYRIGHT_LEN, reinterpret_cast<LPCWSTR>(cBuffer));
			}
			break;
		case 4:		// Full font name
			{
				SetFilePointer(hFile, dwStartingOffset + otfNameRecord.offset, NULL, FILE_BEGIN);
				ZeroMemory(cBuffer, 1024);
				ReadFile(hFile, cBuffer, otfNameRecord.length, &dwRead, NULL);

				for (j = 0; j < otfNameRecord.length; j += sizeof(WCHAR))
				{
					nSwap			= cBuffer[j];
					cBuffer[j]		= cBuffer[j + 1];
					cBuffer[j + 1]	= nSwap;
				}

				_tcscpy_s(m_szFontFace[nIndex], LF_FULLFACESIZE, reinterpret_cast<LPCWSTR>(cBuffer));
			}
			break;
		case 5:		// Version string
			{
				SetFilePointer(hFile, dwStartingOffset + otfNameRecord.offset, NULL, FILE_BEGIN);
				ZeroMemory(cBuffer, 1024);
				ReadFile(hFile, cBuffer, otfNameRecord.length, &dwRead, NULL);

				for (j = 0; j < otfNameRecord.length; j += sizeof(WCHAR))
				{
					nSwap			= cBuffer[j];
					cBuffer[j]		= cBuffer[j + 1];
					cBuffer[j + 1]	= nSwap;
				}

				_tcscpy_s(m_szFontVersion[nIndex], OTF_VERSION_LEN, reinterpret_cast<LPCWSTR>(cBuffer));
			}
			break;
		}

		SetFilePointer(hFile, dwPos, NULL, FILE_BEGIN);
	}
}

void CFontManagerX::Process_OS_2(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_post(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_cvt_(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_fpgm(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_glyf(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_loca(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_prep(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_CFF_(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_VORG(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_EBDT(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_EBLC(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_EBSC(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_BASE(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_GDEF(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_GPOS(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_GSUB(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_JSTF(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

// DSIG Header
typedef struct _OTF_DSIG_HEADER
{
	DWORD	ulVersion;				// Version number of the DSIG table (0x00000001)
	WORD	usNumSigs;				// Number of signatures in the table
	WORD	usFlag;					// Permission flags (Bit 0: cannot be resigned, Bits 1-7: Reserved (Set to 0))
} OTF_DSIG_HEADER;

void CFontManagerX::Process_DSIG(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
	DWORD			dwRead;
	OTF_DSIG_HEADER	otfDSIGHeader;

	// Reading DSIG Header
	SetFilePointer(hFile, dwOffset, NULL, FILE_BEGIN);
	ReadFile(hFile, &otfDSIGHeader, sizeof(OTF_DSIG_HEADER), &dwRead, NULL);

	if (otfDSIGHeader.ulVersion != 0x01000000)
	{
		// Invalid Table
		return;
	}

	otfDSIGHeader.usNumSigs	= SWAPWORD(otfDSIGHeader.usNumSigs);
	m_bDigitallySigned		= static_cast<BOOL>(otfDSIGHeader.usNumSigs > 0);
}

void CFontManagerX::Process_gasp(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_hdmx(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_kern(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_LTSH(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_PCLT(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_VDMX(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_vhea(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}

void CFontManagerX::Process_vmtx(int nIndex, DWORD dwOffset, DWORD dwLength, HANDLE hFile)
{
}
