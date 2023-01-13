// GeneralPage.h

#pragma once

#include "PropertyPageX.h"

class CGeneralPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CGeneralPage)

public:
	CGeneralPage();
	virtual	~CGeneralPage();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_GENERALPAGE };

	BOOL	m_bFontLinkMethod;
	int		m_nResetMethod;
	CStatic	m_ctlFontLinkMethodH;
	CStatic	m_ctlResetMethodH;

protected:
	void	LoadSettings();
	void	SaveSettings();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
