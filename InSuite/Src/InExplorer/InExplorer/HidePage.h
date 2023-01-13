// HidePage.h

#pragma once

#include "PropertyPageX.h"

class CHidePage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CHidePage)

public:
	CHidePage();
	virtual	~CHidePage();

	void	LoadSettings();
	void	SaveSettings();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_HIDEPAGE };

	BOOL	m_bHideSystem;
	BOOL	m_bHideHidden;
	BOOL	m_bHideROM;
	BOOL	m_bHideExtension;
	CStatic	m_ctlHideH;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
