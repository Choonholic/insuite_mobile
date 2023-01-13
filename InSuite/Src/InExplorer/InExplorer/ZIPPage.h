// ZIPPage.h

#pragma once

#include "PropertyPageX.h"

class CZIPPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CZIPPage)

public:
	CZIPPage();
	virtual	~CZIPPage();

	void	SetMenus();
	void	LoadSettings();
	void	SaveSettings();
	void	UpdateControls();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg void	OnBnClickedZpSystemdefault();
	afx_msg void	OnBnClickedZpSpecificCpH();

	enum { IDD = IDD_ZIPPAGE };

	int	m_nLevel;
	int	m_nCodePageSet;
	int	m_nCodePage;

	CStatic		m_ctlLevelH;
	CComboBox	m_ctlLevel;
	CStatic		m_ctlCodePageH;
	CComboBox	m_ctlCodePage;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
