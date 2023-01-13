// GeneralPage.h

#pragma once

#include "PropertyPageX.h"

class CGeneralPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CGeneralPage)

public:
	CGeneralPage();
	virtual	~CGeneralPage();

	void	LoadSettings();
	void	SaveSettings();
	void	UpdateControls();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg void	OnBnClickedGeLastopened();
	afx_msg void	OnBnClickedGeSpecificH();
	afx_msg void	OnBnClickedGeSpecific();

	enum { IDD = IDD_GENERALPAGE };

	int		m_nStartup;
	CString	m_strSpecific;
	BOOL	m_bDoubleTap;
	CStatic	m_ctlStartupH;
	CStatic	m_ctlMiscH;
	CButton	m_ctlSpecific;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
