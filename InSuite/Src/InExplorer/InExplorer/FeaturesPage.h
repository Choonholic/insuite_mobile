// FeaturesPage.h

#pragma once

#include "PropertyPageX.h"

class CFeaturesPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CFeaturesPage)

public:
	CFeaturesPage();
	virtual	~CFeaturesPage();

	void	LoadSettings();
	void	SaveSettings();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_FEATURESPAGE };

	BOOL	m_bAutoColumnWidth;
	BOOL	m_bNameEditor;
	BOOL	m_bMultiSelect;
	CStatic	m_ctlFeaturesH;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
