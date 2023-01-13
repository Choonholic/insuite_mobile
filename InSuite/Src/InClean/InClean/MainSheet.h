// MainSheet.h

#pragma once

#include "PropertySheetX.h"
#include "FilesPage.h"
#include "RegistryPage.h"
#include "AboutPage.h"

class CMainSheet : public CPropertySheetX
{
	DECLARE_DYNAMIC(CMainSheet)

public:
	CMainSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual	~CMainSheet();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnScan();

	CFilesPage		m_dFiles;
	CRegistryPage	m_dRegistry;
	CAboutPage		m_dAbout;

protected:
	DECLARE_MESSAGE_MAP()
};
