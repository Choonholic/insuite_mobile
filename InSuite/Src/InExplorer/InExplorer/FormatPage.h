// FormatPage.h

#pragma once

#include "PropertyPageX.h"

class CFormatPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CFormatPage)

public:
	CFormatPage();
	virtual	~CFormatPage();

	void	LoadSettings();
	void	SaveSettings();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_FORMATPAGE };

	BOOL	m_bSimpleDate;
	BOOL	m_bSimpleSize;
	CStatic	m_ctlFormatH;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
