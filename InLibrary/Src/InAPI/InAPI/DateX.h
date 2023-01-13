// DateX.h

#pragma once

#include "InAPI.h"

#ifndef DX_MONTHS
#define DX_MONTHS		12
#define DX_FIRST_YEAR	1841
#define DX_LAST_YEAR	2049
#define DX_TOTAL_YEARS	(DX_LAST_YEAR - DX_FIRST_YEAR + 1)
#endif

// The Sexagenary Cycle - Sky : Gap, Eul, Byoung, Jeong, Mu, Gi, Gyoung, Shin, Im, Gye.
// The Sexagenary Cycle - Land: Ja, Chuk, In, Myo, Jin, Sa, Wo, Mi, Shin, You, Sul, Hae.
// The Zodiac(al) Sign        : Mouse, Cattle, Tiger, Rabbit, Dragon, Snake, Horse, Sheep, Monkey, Chicken, Dog, Pig.

class INAPI_CLASS CDateX
{
public:
	CDateX(void);
	virtual	~CDateX(void);

	// Set Information
	BOOL	SetGregorianDate(int nYear, int nMonth, int nDay);
	BOOL	SetLunarDate(int nYear, int nMonth, int nDay, BOOL bLeap = FALSE);

	// Retreive Information
	BOOL	GetGregorianDate(int& nYear, int& nMonth, int& nDay, int& nWeekdays);
	BOOL	GetLunarDate(int& nYear, int& nMonth, int& nDay, BOOL& bLeap);

	// Sexagenary Cycle
	BOOL	GetSexagenaryCycleYear(int& nSky, int& nLand);
	BOOL	GetSexagenaryCycleDay(int& nSky, int& nLand);

	// Zodiac Sign
	BOOL	GetZodiacSign(int& nZodiac);

protected:
	int		m_nMonthDays[DX_MONTHS];
	int		m_nGY, m_nGM, m_nGD;
	int		m_nLY, m_nLM, m_nLD;
	int		m_nWeekdays;

	BOOL	m_bLeap;

	int		m_nYearSCS, m_nYearSCL, m_nZS;
	int		m_nDaySCS, m_nDaySCL;

	BOOL	m_bValid;

	void	AllReset(void);
	void	FebReset(int nYear);

	BOOL	GregorianToLunar(void);
	BOOL	LunarToGregorian(void);
};
