// ComponentsPage.h

#pragma once

#include "PropertyPageX.h"

class CComponentsPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CComponentsPage)

public:
	CComponentsPage();
	virtual	~CComponentsPage();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_COMPONENTSPAGE };

	BOOL	m_bVertical;
	BOOL	m_bDataType;
	BOOL	m_bFullRowSelect;
	CStatic	m_ctlComponentsH;

protected:
	void	LoadSettings();
	void	SaveSettings();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
