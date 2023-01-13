// DateX.cpp

#include "StdAfx.h"
#include "DateX.h"

// Converting Table is based on the table of Korea Astronomy and Space Science Institute. (http://www.kasi.re.kr/)
// 1: Normal Small Month (NSM)
// 2: Normal Large Month (NLM)
// 3: NSM + Leap Small Month
// 4: NSM + Leap Large Month
// 5: NLM + Leap Small Month
// 6: NLM + Leap Large Month

BYTE	nLunarTable[DX_TOTAL_YEARS][DX_MONTHS]	=
{
	1, 2, 4, 1, 1, 2,	1, 2, 1, 2, 2, 1,	// 1841
	2, 2, 1, 2, 1, 1,	2, 1, 2, 1, 2, 1,
	2, 2, 2, 1, 2, 1,	4, 1, 2, 1, 2, 1,
	2, 2, 1, 2, 1, 2,	1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 2, 1,	2, 1, 2, 1, 2, 1,
	2, 1, 2, 1, 5, 2,	1, 2, 2, 1, 2, 1,
	2, 1, 1, 2, 1, 2,	1, 2, 2, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	2, 1, 2, 2, 2, 1,
	2, 1, 2, 3, 2, 1,	2, 1, 2, 1, 2, 2,
	2, 1, 2, 1, 1, 2,	1, 1, 2, 2, 1, 2,

	2, 2, 1, 2, 1, 1,	2, 1, 2, 1, 5, 2,	// 1851
	2, 1, 2, 2, 1, 1,	2, 1, 2, 1, 1, 2,
	2, 1, 2, 2, 1, 2,	1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2,	5, 2, 1, 2, 1, 2,
	1, 1, 2, 1, 2, 2,	1, 2, 2, 1, 2, 1,
	2, 1, 1, 2, 1, 2,	1, 2, 2, 2, 1, 2,
	1, 2, 1, 1, 5, 2,	1, 2, 1, 2, 2, 2,
	1, 2, 1, 1, 2, 1,	1, 2, 2, 1, 2, 2,
	2, 1, 2, 1, 1, 2,	1, 1, 2, 1, 2, 2,
	2, 1, 6, 1, 1, 2,	1, 1, 2, 1, 2, 2,

	1, 2, 2, 1, 2, 1,	2, 1, 2, 1, 1, 2,	// 1861
	2, 1, 2, 1, 2, 2,	1, 2, 2, 3, 1, 2,
	1, 2, 2, 1, 2, 1,	2, 2, 1, 2, 1, 2,
	1, 1, 2, 1, 2, 1,	2, 2, 1, 2, 2, 1,
	2, 1, 1, 2, 4, 1,	2, 2, 1, 2, 2, 1,
	2, 1, 1, 2, 1, 1,	2, 2, 1, 2, 2, 2,
	1, 2, 1, 1, 2, 1,	1, 2, 1, 2, 2, 2,
	1, 2, 2, 3, 2, 1,	1, 2, 1, 2, 2, 1,
	2, 2, 2, 1, 1, 2,	1, 1, 2, 1, 2, 1,
	2, 2, 2, 1, 2, 1,	2, 1, 1, 5, 2, 1,

	2, 2, 1, 2, 2, 1,	2, 1, 2, 1, 1, 2,	// 1871
	1, 2, 1, 2, 2, 1,	2, 1, 2, 2, 1, 2,
	1, 1, 2, 1, 2, 4,	2, 1, 2, 2, 1, 2,
	1, 1, 2, 1, 2, 1,	2, 1, 2, 2, 2, 1,
	2, 1, 1, 2, 1, 1,	2, 1, 2, 2, 2, 1,
	2, 2, 1, 1, 5, 1,	2, 1, 2, 2, 1, 2,
	2, 2, 1, 1, 2, 1,	1, 2, 1, 2, 1, 2,
	2, 2, 1, 2, 1, 2,	1, 1, 2, 1, 2, 1,
	2, 2, 4, 2, 1, 2,	1, 1, 2, 1, 2, 1,
	2, 1, 2, 2, 1, 2,	2, 1, 2, 1, 1, 2,

	1, 2, 1, 2, 1, 2,	5, 2, 2, 1, 2, 1,	// 1881
	1, 2, 1, 2, 1, 2,	1, 2, 2, 1, 2, 2,
	1, 1, 2, 1, 1, 2,	1, 2, 2, 2, 1, 2,
	2, 1, 1, 2, 3, 2,	1, 2, 2, 1, 2, 2,
	2, 1, 1, 2, 1, 1,	2, 1, 2, 1, 2, 2,
	2, 1, 2, 1, 2, 1,	1, 2, 1, 2, 1, 2,
	2, 2, 1, 5, 2, 1,	1, 2, 1, 2, 1, 2,
	2, 1, 2, 2, 1, 2,	1, 1, 2, 1, 2, 1,
	2, 1, 2, 2, 1, 2,	1, 2, 1, 2, 1, 2,
	1, 5, 2, 1, 2, 2,	1, 2, 1, 2, 1, 2,

	1, 2, 1, 2, 1, 2,	1, 2, 2, 1, 2, 2,	// 1891
	1, 1, 2, 1, 1, 5,	2, 2, 1, 2, 2, 2,
	1, 1, 2, 1, 1, 2,	1, 2, 1, 2, 2, 2,
	1, 2, 1, 2, 1, 1,	2, 1, 2, 1, 2, 2,
	2, 1, 2, 1, 5, 1,	2, 1, 2, 1, 2, 1,
	2, 2, 2, 1, 2, 1,	1, 2, 1, 2, 1, 2,
	1, 2, 2, 1, 2, 1,	2, 1, 2, 1, 2, 1,
	2, 1, 5, 2, 2, 1,	2, 1, 2, 1, 2, 1,
	2, 1, 2, 1, 2, 1,	2, 2, 1, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	2, 5, 2, 2, 1, 2,

	1, 2, 1, 1, 2, 1,	2, 1, 2, 2, 2, 1,	// 1901
	2, 1, 2, 1, 1, 2,	1, 2, 1, 2, 2, 2,
	1, 2, 1, 2, 3, 2,	1, 1, 2, 2, 1, 2,
	2, 2, 1, 2, 1, 1,	2, 1, 1, 2, 2, 1,
	2, 2, 1, 2, 2, 1,	1, 2, 1, 2, 1, 2,
	1, 2, 2, 4, 1, 2,	1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2,	2, 1, 2, 1, 2, 1,
	2, 1, 1, 2, 2, 1,	2, 1, 2, 2, 1, 2,
	1, 5, 1, 2, 1, 2,	1, 2, 2, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	2, 1, 2, 2, 2, 1,

	2, 1, 2, 1, 1, 5,	1, 2, 2, 1, 2, 2,	// 1911
	2, 1, 2, 1, 1, 2,	1, 1, 2, 2, 1, 2,
	2, 2, 1, 2, 1, 1,	2, 1, 1, 2, 1, 2,
	2, 2, 1, 2, 5, 1,	2, 1, 2, 1, 1, 2,
	2, 1, 2, 2, 1, 2,	1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2,	2, 1, 2, 1, 2, 1,
	2, 3, 2, 1, 2, 2,	1, 2, 2, 1, 2, 1,
	2, 1, 1, 2, 1, 2,	1, 2, 2, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	5, 2, 2, 1, 2, 2,
	1, 2, 1, 1, 2, 1,	1, 2, 2, 1, 2, 2,

	2, 1, 2, 1, 1, 2,	1, 1, 2, 1, 2, 2,	// 1921
	2, 1, 2, 2, 3, 2,	1, 1, 2, 1, 2, 2,
	1, 2, 2, 1, 2, 1,	2, 1, 2, 1, 1, 2,
	2, 1, 2, 1, 2, 2,	1, 2, 1, 2, 1, 1,
	2, 1, 2, 5, 2, 1,	2, 2, 1, 2, 1, 2,
	1, 1, 2, 1, 2, 1,	2, 2, 1, 2, 2, 1,
	2, 1, 1, 2, 1, 2,	1, 2, 2, 1, 2, 2,
	1, 5, 1, 2, 1, 1,	2, 2, 1, 2, 2, 2,
	1, 2, 1, 1, 2, 1,	1, 2, 1, 2, 2, 2,
	1, 2, 2, 1, 1, 5,	1, 2, 1, 2, 2, 1,

	2, 2, 2, 1, 1, 2,	1, 1, 2, 1, 2, 1,	// 1931
	2, 2, 2, 1, 2, 1,	2, 1, 1, 2, 1, 2,
	1, 2, 2, 1, 6, 1,	2, 1, 2, 1, 1, 2,
	1, 2, 1, 2, 2, 1,	2, 2, 1, 2, 1, 2,
	1, 1, 2, 1, 2, 1,	2, 2, 1, 2, 2, 1,
	2, 1, 4, 1, 2, 1,	2, 1, 2, 2, 2, 1,
	2, 1, 1, 2, 1, 1,	2, 1, 2, 2, 2, 1,
	2, 2, 1, 1, 2, 1,	4, 1, 2, 2, 1, 2,
	2, 2, 1, 1, 2, 1,	1, 2, 1, 2, 1, 2,
	2, 2, 1, 2, 1, 2,	1, 1, 2, 1, 2, 1,

	2, 2, 1, 2, 2, 4,	1, 1, 2, 1, 2, 1,	// 1941
	2, 1, 2, 2, 1, 2,	2, 1, 2, 1, 1, 2,
	1, 2, 1, 2, 1, 2,	2, 1, 2, 2, 1, 2,
	1, 1, 2, 4, 1, 2,	1, 2, 2, 1, 2, 2,
	1, 1, 2, 1, 1, 2,	1, 2, 2, 2, 1, 2,
	2, 1, 1, 2, 1, 1,	2, 1, 2, 2, 1, 2,
	2, 5, 1, 2, 1, 1,	2, 1, 2, 1, 2, 2,
	2, 1, 2, 1, 2, 1,	1, 2, 1, 2, 1, 2,
	2, 2, 1, 2, 1, 2,	3, 2, 1, 2, 1, 2,
	2, 1, 2, 2, 1, 2,	1, 1, 2, 1, 2, 1,

	2, 1, 2, 2, 1, 2,	1, 2, 1, 2, 1, 2,	// 1951
	1, 2, 1, 2, 4, 2,	1, 2, 1, 2, 1, 2,
	1, 2, 1, 1, 2, 2,	1, 2, 2, 1, 2, 2,
	1, 1, 2, 1, 1, 2,	1, 2, 2, 1, 2, 2,
	2, 1, 4, 1, 1, 2,	1, 2, 1, 2, 2, 2,
	1, 2, 1, 2, 1, 1,	2, 1, 2, 1, 2, 2,
	2, 1, 2, 1, 2, 1,	1, 5, 2, 1, 2, 2,
	1, 2, 2, 1, 2, 1,	1, 2, 1, 2, 1, 2,
	1, 2, 2, 1, 2, 1,	2, 1, 2, 1, 2, 1,
	2, 1, 2, 1, 2, 5,	2, 1, 2, 1, 2, 1,

	2, 1, 2, 1, 2, 1,	2, 2, 1, 2, 1, 2,	// 1961
	1, 2, 1, 1, 2, 1,	2, 2, 1, 2, 2, 1,
	2, 1, 2, 3, 2, 1,	2, 1, 2, 2, 2, 1,
	2, 1, 2, 1, 1, 2,	1, 2, 1, 2, 2, 2,
	1, 2, 1, 2, 1, 1,	2, 1, 1, 2, 2, 1,
	2, 2, 5, 2, 1, 1,	2, 1, 1, 2, 2, 1,
	2, 2, 1, 2, 2, 1,	1, 2, 1, 2, 1, 2,
	1, 2, 2, 1, 2, 1,	5, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2,	2, 1, 2, 1, 2, 1,
	2, 1, 1, 2, 2, 1,	2, 1, 2, 2, 1, 2,

	1, 2, 1, 1, 5, 2,	1, 2, 2, 2, 1, 2,	// 1971
	1, 2, 1, 1, 2, 1,	2, 1, 2, 2, 2, 1,
	2, 1, 2, 1, 1, 2,	1, 1, 2, 2, 2, 1,
	2, 2, 1, 5, 1, 2,	1, 1, 2, 2, 1, 2,
	2, 2, 1, 2, 1, 1,	2, 1, 1, 2, 1, 2,
	2, 2, 1, 2, 1, 2,	1, 5, 2, 1, 1, 2,
	2, 1, 2, 2, 1, 2,	1, 2, 1, 2, 1, 1,
	2, 2, 1, 2, 1, 2,	2, 1, 2, 1, 2, 1,
	2, 1, 1, 2, 1, 6,	1, 2, 2, 1, 2, 1,
	2, 1, 1, 2, 1, 2,	1, 2, 2, 1, 2, 2,

	1, 2, 1, 1, 2, 1,	1, 2, 2, 1, 2, 2,	// 1981
	2, 1, 2, 3, 2, 1,	1, 2, 2, 1, 2, 2,
	2, 1, 2, 1, 1, 2,	1, 1, 2, 1, 2, 2,
	2, 1, 2, 2, 1, 1,	2, 1, 1, 5, 2, 2,
	1, 2, 2, 1, 2, 1,	2, 1, 1, 2, 1, 2,
	1, 2, 2, 1, 2, 2,	1, 2, 1, 2, 1, 1,
	2, 1, 2, 2, 1, 5,	2, 2, 1, 2, 1, 2,
	1, 1, 2, 1, 2, 1,	2, 2, 1, 2, 2, 1,
	2, 1, 1, 2, 1, 2,	1, 2, 2, 1, 2, 2,
	1, 2, 1, 1, 5, 1,	2, 1, 2, 2, 2, 2,

	1, 2, 1, 1, 2, 1,	1, 2, 1, 2, 2, 2,	// 1991
	1, 2, 2, 1, 1, 2,	1, 1, 2, 1, 2, 2,
	1, 2, 5, 2, 1, 2,	1, 1, 2, 1, 2, 1,
	2, 2, 2, 1, 2, 1,	2, 1, 1, 2, 1, 2,
	1, 2, 2, 1, 2, 2,	1, 5, 2, 1, 1, 2,
	1, 2, 1, 2, 2, 1,	2, 1, 2, 2, 1, 2,
	1, 1, 2, 1, 2, 1,	2, 2, 1, 2, 2, 1,
	2, 1, 1, 2, 3, 2,	2, 1, 2, 2, 2, 1,
	2, 1, 1, 2, 1, 1,	2, 1, 2, 2, 2, 1,
	2, 2, 1, 1, 2, 1,	1, 2, 1, 2, 2, 1,

	2, 2, 2, 3, 2, 1,	1, 2, 1, 2, 1, 2,	// 2001
	2, 2, 1, 2, 1, 2,	1, 1, 2, 1, 2, 1,
	2, 2, 1, 2, 2, 1,	2, 1, 1, 2, 1, 2,
	1, 5, 2, 2, 1, 2,	1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2,	2, 1, 2, 2, 1, 1,
	2, 1, 2, 1, 2, 1,	5, 2, 2, 1, 2, 2,
	1, 1, 2, 1, 1, 2,	1, 2, 2, 2, 1, 2,
	2, 1, 1, 2, 1, 1,	2, 1, 2, 2, 1, 2,
	2, 2, 1, 1, 5, 1,	2, 1, 2, 1, 2, 2,
	2, 1, 2, 1, 2, 1,	1, 2, 1, 2, 1, 2,

	2, 1, 2, 2, 1, 2,	1, 1, 2, 1, 2, 1,	// 2011
	2, 1, 6, 2, 1, 2,	1, 1, 2, 1, 2, 1,
	2, 1, 2, 2, 1, 2,	1, 2, 1, 2, 1, 2,
	1, 2, 1, 2, 1, 2,	1, 2, 5, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	2, 2, 2, 1, 2, 2,
	1, 1, 2, 1, 1, 2,	1, 2, 2, 1, 2, 2,
	2, 1, 1, 2, 3, 2,	1, 2, 1, 2, 2, 2,
	1, 2, 1, 2, 1, 1,	2, 1, 2, 1, 2, 2,
	2, 1, 2, 1, 2, 1,	1, 2, 1, 2, 1, 2,
	2, 1, 2, 5, 2, 1,	1, 2, 1, 2, 1, 2,

	1, 2, 2, 1, 2, 1,	2, 1, 2, 1, 2, 1,	// 2021
	2, 1, 2, 1, 2, 2,	1, 2, 1, 2, 1, 2,
	1, 5, 2, 1, 2, 1,	2, 2, 1, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	2, 2, 1, 2, 2, 1,
	2, 1, 2, 1, 1, 5,	2, 1, 2, 2, 2, 1,
	2, 1, 2, 1, 1, 2,	1, 2, 1, 2, 2, 2,
	1, 2, 1, 2, 1, 1,	2, 1, 1, 2, 2, 2,
	1, 2, 2, 1, 5, 1,	2, 1, 1, 2, 2, 1,
	2, 2, 1, 2, 2, 1,	1, 2, 1, 1, 2, 2,
	1, 2, 1, 2, 2, 1,	2, 1, 2, 1, 2, 1,

	2, 1, 5, 2, 1, 2,	2, 1, 2, 1, 2, 1,	// 2031
	2, 1, 1, 2, 1, 2,	2, 1, 2, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	5, 2, 2, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	2, 1, 2, 2, 2, 1,
	2, 1, 2, 1, 1, 2,	1, 1, 2, 2, 1, 2,
	2, 2, 1, 2, 1, 4,	1, 1, 2, 1, 2, 2,
	2, 2, 1, 2, 1, 1,	2, 1, 1, 2, 1, 2,
	2, 2, 1, 2, 1, 2,	1, 2, 1, 1, 2, 1,
	2, 2, 1, 2, 5, 2,	1, 2, 1, 2, 1, 1,
	2, 1, 2, 2, 1, 2,	2, 1, 2, 1, 2, 1,

	2, 1, 1, 2, 1, 2,	2, 1, 2, 2, 1, 2,	// 2041
	1, 5, 1, 2, 1, 2,	1, 2, 2, 2, 1, 2,
	1, 2, 1, 1, 2, 1,	1, 2, 2, 1, 2, 2,
	2, 1, 2, 1, 1, 2,	3, 2, 1, 2, 2, 2,
	2, 1, 2, 1, 1, 2,	1, 1, 2, 1, 2, 2,
	2, 1, 2, 2, 1, 1,	2, 1, 1, 2, 1, 2,
	2, 1, 2, 2, 4, 1,	2, 1, 1, 2, 1, 2,
	1, 2, 2, 1, 2, 2,	1, 2, 1, 2, 1, 1,
	2, 1, 2, 1, 2, 2,	1, 2, 2, 1, 2, 1,
};

CDateX::CDateX(void)
{
	AllReset();
}

CDateX::~CDateX(void)
{
}

BOOL CDateX::SetGregorianDate(int nYear, int nMonth, int nDay)
{
	AllReset();

	m_nGY	= nYear;
	m_nGM	= nMonth;
	m_nGD	= nDay;

	return GregorianToLunar();
}

BOOL CDateX::SetLunarDate(int nYear, int nMonth, int nDay, BOOL bLeap)
{
	AllReset();

	m_nLY	= nYear;
	m_nLM	= nMonth;
	m_nLD	= nDay;
	m_bLeap	= bLeap;

	return LunarToGregorian();
}

BOOL CDateX::GetGregorianDate(int& nYear, int& nMonth, int& nDay, int& nWeekdays)
{
	nYear		= m_nGY;
	nMonth		= m_nGM;
	nDay		= m_nGD;
	nWeekdays	= m_nWeekdays;

	return m_bValid;
}

BOOL CDateX::GetLunarDate(int& nYear, int& nMonth, int& nDay, BOOL& bLeap)
{
	nYear	= m_nLY;
	nMonth	= m_nLM;
	nDay	= m_nLD;
	bLeap	= m_bLeap;

	return m_bValid;
}

BOOL CDateX::GetSexagenaryCycleYear(int& nSky, int& nLand)
{
	nSky	= m_nYearSCS;
	nLand	= m_nYearSCL;

	return m_bValid;
}

BOOL CDateX::GetSexagenaryCycleDay(int& nSky, int& nLand)
{
	nSky	= m_nDaySCS;
	nLand	= m_nDaySCL;

	return m_bValid;
}

BOOL CDateX::GetZodiacSign(int& nZodiac)
{
	nZodiac	= m_nZS;

	return m_bValid;
}

BOOL CDateX::GregorianToLunar(void)
{
	int		i, j;
	int		nGY, nGM, nGD;
	int		nLY, nLM, nLD, nLL;
	int		nMM, nM1, nM2;
	DWORD	dwYearTotal[DX_TOTAL_YEARS];
	DWORD	dwTotalDays, dwTotalDays1, dwTotalDays2;

	// Check Date Validation
	if ((m_nGY < DX_FIRST_YEAR) || (m_nGY > DX_LAST_YEAR))
	{
		return FALSE;
	}

	if ((m_nGM < 1) || (m_nGM > 12))
	{
		return FALSE;
	}

	FebReset(m_nGY);

	if ((m_nGD < 1) || (m_nGD > m_nMonthDays[m_nGM - 1]))
	{
		return FALSE;
	}

	if ((m_nGY == DX_FIRST_YEAR) && (m_nGM == 1) && (m_nGD < 23))
	{
		return FALSE;
	}

	// Set Date
	nGY	= m_nGY;
	nGM	= m_nGM;
	nGD	= m_nGD;

	// Calculate first year
	dwTotalDays1	= ((DX_FIRST_YEAR - 1) * 365) + ((DX_FIRST_YEAR - 1) / 4) - ((DX_FIRST_YEAR - 1) / 100) + ((DX_FIRST_YEAR - 1) / 400) + 23;

	// Calculate last year
	dwTotalDays2	= ((nGY - 1) * 365) + ((nGY - 1) / 4) - ((nGY - 1) / 100) + ((nGY - 1) / 400) + nGD;

	for (i = 0; i < nGM - 1; i++)
	{
		dwTotalDays2	+= m_nMonthDays[i];
	}

	// Calculate total days
	dwTotalDays	= dwTotalDays2 - dwTotalDays1 + 1;

	for (i = 0; i <= (nGY - DX_FIRST_YEAR); i++)
	{
		dwYearTotal[i]	= 0;

		for (j = 0; j < 12; j++)
		{
			switch (nLunarTable[i][j])
			{
			case 1:
				{
					nMM	= 29;
				}
				break;
			case 2:
				{
					nMM	= 30;
				}
				break;
			case 3:
				{
					nMM	= 58;
				}
				break;
			case 4:
				{
					nMM	= 59;
				}
				break;
			case 5:
				{
					nMM	= 59;
				}
				break;
			case 6:
				{
					nMM	= 60;
				}
				break;
			}

			dwYearTotal[i]	+= nMM;
		}
	}

	// Calculate Lunar Year
	nLY	= 0;

	while (TRUE)
	{
		if (dwTotalDays > dwYearTotal[nLY])
		{
			dwTotalDays	-= dwYearTotal[nLY];
			nLY++;
		}
		else
		{
			break;
		}
	}

	// Calculate Lunar Month
	nLM	= 0;
	nLL	= 0;

	while (TRUE)
	{
		if (nLunarTable[nLY][nLM] <= 2)
		{
			nMM	= nLunarTable[nLY][nLM] + 28;

			if (dwTotalDays > static_cast<DWORD>(nMM))
			{
				dwTotalDays	-= nMM;
				nLM++;
			}
			else
			{
				break;
			}
		}
		else
		{
			switch (nLunarTable[nLY][nLM])
			{
			case 3:
				{
					nM1	= 29;
					nM2	= 29;
				}
				break;
			case 4:
				{
					nM1	= 29;
					nM2	= 30;
				}
				break;
			case 5:
				{
					nM1	= 30;
					nM2	= 29;
				}
				break;
			case 6:
				{
					nM1	= 30;
					nM2	= 30;
				}
				break;
			}

			if (dwTotalDays > static_cast<DWORD>(nM1))
			{
				dwTotalDays	-= nM1;

				if (dwTotalDays > static_cast<DWORD>(nM2))
				{
					dwTotalDays	-= nM2;
					nLM++;
				}
				else
				{
					nLL	= 1;
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	// Calculate Lunar Date
	nLY	+= DX_FIRST_YEAR;
	nLM	+= 1;
	nLD	= static_cast<int>(dwTotalDays);

	m_nLY	= nLY;
	m_nLM	= nLM;
	m_nLD	= nLD;

	m_nWeekdays	= (dwTotalDays2 % 7);
	m_bLeap		= static_cast<BOOL>(nLL);
	m_nDaySCS	= ((dwTotalDays2 + 4) % 10);
	m_nDaySCL	= ((dwTotalDays2 + 2) % 12);
	m_nYearSCS	= ((nLY + 6) % 10);
	m_nYearSCL	= ((nLY + 8) % 12);
	m_nZS		= ((nLY + 8) % 12);
	m_bValid	= TRUE;

	return TRUE;
}

BOOL CDateX::LunarToGregorian(void)
{
	int		nGY, nGM, nGD;
	int		nLY, nLM, nLD, nLL;
	int		nY1, nY2;
	int		nMM, nM1;
	int		i, j;
	DWORD	dwTotalDays;

	// Check Date Validation
	if ((m_nLY < DX_FIRST_YEAR) || (m_nLY > DX_LAST_YEAR))
	{
		return FALSE;
	}

	if ((m_nLM < 1) || (m_nLM > 12))
	{
		return FALSE;
	}

	nY1	= m_nLY - DX_FIRST_YEAR;
	nM1	= m_nLM - 1;
	nLL	= 0;

	if (nLunarTable[nY1][nM1] > 2)
	{
		nLL	= m_bLeap;
	}

	m_bLeap	= static_cast<BOOL>(nLL);

	if (nLL)
	{
		switch (nLunarTable[nY1][nM1])
		{
		case 3:
		case 5:
			{
				nMM	= 29;
			}
			break;
		case 4:
		case 6:
			{
				nMM	= 30;
			}
			break;
		}
	}
	else
	{
		switch (nLunarTable[nY1][nM1])
		{
		case 1:
		case 3:
		case 4:
			{
				nMM	= 29;
			}
			break;
		case 2:
		case 5:
		case 6:
			{
				nMM	= 30;
			}
			break;
		}
	}

	if ((m_nLD < 1) || (m_nLD > nMM))
	{
		return FALSE;
	}

	// Set Date
	nLY	= m_nLY;
	nLM	= m_nLM;
	nLD	= m_nLD;

	// Calculate Total Days
	dwTotalDays	= 0;

	for (i = 0; i < nY1; i++)
	{
		for (j = 0; j < 12; j++)
		{
			switch (nLunarTable[i][j])
			{
			case 1:
				{
					dwTotalDays	+= 29;
				}
				break;
			case 2:
				{
					dwTotalDays	+= 30;
				}
				break;
			case 3:
				{
					dwTotalDays	+= 58;
				}
				break;
			case 4:
				{
					dwTotalDays	+= 59;
				}
				break;
			case 5:
				{
					dwTotalDays	+= 59;
				}
				break;
			case 6:
				{
					dwTotalDays	+= 60;
				}
				break;
			}
		}
	}

	for (j = 0; j < nM1; j++)
	{
		switch (nLunarTable[nY1][j])
		{
		case 1:
			{
				dwTotalDays	+= 29;
			}
			break;
		case 2:
			{
				dwTotalDays	+= 30;
			}
			break;
		case 3:
			{
				dwTotalDays	+= 58;
			}
			break;
		case 4:
			{
				dwTotalDays	+= 59;
			}
			break;
		case 5:
			{
				dwTotalDays	+= 59;
			}
			break;
		case 6:
			{
				dwTotalDays	+= 60;
			}
			break;
		}
	}

	if (nLL)
	{
		switch (nLunarTable[nY1][nM1])
		{
		case 3:
		case 4:
			{
				dwTotalDays	+= 29;
			}
			break;
		case 5:
		case 6:
			{
				dwTotalDays	+= 30;
			}
			break;
		}
	}

	dwTotalDays	+= nLD + 22;
	nY1			= DX_FIRST_YEAR - 1;

	do
	{
		nY1++;

		if (!(nY1 % 400) || (nY1 % 100) && !(nY1 % 4))
		{
			nY2	= 366;
		}
		else
		{
			nY2	= 365;
		}

		if (dwTotalDays <= (DWORD)nY2)
		{
			break;
		}

		dwTotalDays	-= nY2;
	} while (TRUE);

	nGY				= nY1;
	m_nMonthDays[1]	= nY2 - 337;
	nM1				= 0;

	do
	{
		nM1++;

		if (dwTotalDays <= (DWORD)m_nMonthDays[nM1 - 1])
		{
			break;
		}

		dwTotalDays	-= m_nMonthDays[nM1 - 1];
	} while (TRUE);

	nGM			= nM1;
	nGD			= dwTotalDays;
	nY1			= nGY - 1;
	dwTotalDays	= (nY1 * 365) + (nY1 / 4) - (nY1 / 100) + (nY1 / 400);

	for (i = 0; i < nGM - 1; i++)
	{
		dwTotalDays	+= m_nMonthDays[i];
	}

	dwTotalDays	+= nGD;

	// Set Gregorian Date
	m_nGY		= nGY;
	m_nGM		= nGM;
	m_nGD		= nGD;
	m_nWeekdays	= dwTotalDays % 7;

	m_nDaySCS	= (dwTotalDays + 4) % 10;
	m_nDaySCL	= (dwTotalDays + 2) % 12;
	m_nYearSCS	= (nLY + 6) % 10;
	m_nYearSCL	= (nLY + 8) % 12;
	m_nZS		= (nLY + 8) % 12;
	m_bValid	= TRUE;

	return TRUE;
}

void CDateX::AllReset(void)
{
	m_nMonthDays[0x00]	= 31;
	m_nMonthDays[0x01]	= 0;
	m_nMonthDays[0x02]	= 31;
	m_nMonthDays[0x03]	= 30;
	m_nMonthDays[0x04]	= 31;
	m_nMonthDays[0x05]	= 30;
	m_nMonthDays[0x06]	= 31;
	m_nMonthDays[0x07]	= 31;
	m_nMonthDays[0x08]	= 30;
	m_nMonthDays[0x09]	= 31;
	m_nMonthDays[0x0A]	= 30;
	m_nMonthDays[0x0B]	= 31;

	m_nGY	= 0;
	m_nGM	= 0;
	m_nGD	= 0;
	m_nLY	= 0;
	m_nLM	= 0;
	m_nLD	= 0;

	m_nWeekdays	= 0;
	m_bLeap		= FALSE;
	m_nDaySCS	= 0;
	m_nDaySCL	= 0;
	m_nYearSCS	= 0;
	m_nYearSCL	= 0;
	m_nZS		= 0;
	m_bValid	= FALSE;
}

void CDateX::FebReset(int nYear)
{
	BOOL	bLeap	= FALSE;

	if ((nYear % 4) == 0)
	{
		bLeap	= TRUE;
	}

	if ((nYear % 100) == 0)
	{
		bLeap	= FALSE;
	}

	if ((nYear % 400) == 0)
	{
		bLeap	= TRUE;
	}

	if (bLeap == TRUE)
	{
		m_nMonthDays[0x01]	= 29;
	}
	else
	{
		m_nMonthDays[0x01]	= 28;
	}
}
