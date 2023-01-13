// BackgroundPage.h

#pragma once

#include "PropertyPageX.h"

class CBackgroundPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CBackgroundPage)

public:
	CBackgroundPage();
	virtual	~CBackgroundPage();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_BACKGROUNDPAGE };

	int		m_nType;
	CButton	m_ctlConfigure;
	CStatic	m_ctlMenuBackgroundH;

protected:
	void	LoadSettings();
	void	SaveSettings();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};