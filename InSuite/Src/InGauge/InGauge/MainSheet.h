// MainSheet.h

#pragma once

#include "PropertySheetX.h"
#include "StoragePage.h"
#include "PowerPage.h"
#include "SystemPage.h"
#include "AboutPage.h"

class CMainSheet : public CPropertySheetX
{
	DECLARE_DYNAMIC(CMainSheet)

public:
	CMainSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual	~CMainSheet();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnRefresh();

	CStoragePage	m_dStorage;
	CPowerPage		m_dPower;
	CSystemPage		m_dSystem;
	CAboutPage		m_dAbout;

protected:
	DECLARE_MESSAGE_MAP()
};
