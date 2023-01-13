// FindPage.h

#pragma once

#include "PropertyPageX.h"

class CFindPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CFindPage)

public:
	CFindPage();
	virtual	~CFindPage();

	void	LoadSettings();
	void	SaveSettings();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_FINDPAGE };

	BOOL	m_bAutoColumnWidth;
	CStatic	m_ctlFoundItemsH;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
