// AntialiasingPage.h

#pragma once

#include "PropertyPageX.h"

class CAntialiasingPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CAntialiasingPage)

public:
	CAntialiasingPage();
	virtual	~CAntialiasingPage();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_ANTIALIASINGPAGE };

	int		m_nType;
	BOOL	m_bApplyLS;
	CStatic	m_ctlTypeH;
	CStatic	m_ctlAddOptionsH;

protected:
	void	LoadSettings();
	void	SaveSettings();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
