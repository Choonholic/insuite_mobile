// MainSheet.h

#pragma once

#include "PropertySheetX.h"
#include "GeneralPage.h"
#include "ScreenPage.h"
#include "ActionsPage.h"
#include "BackgroundPage.h"
#include "MenuPage.h"
#include "ExcludedPage.h"
#include "ShortcutsPage.h"
#include "AboutPage.h"

class CMainSheet : public CPropertySheetX
{
	DECLARE_DYNAMIC(CMainSheet)

public:
	CMainSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual	~CMainSheet();

	virtual BOOL	OnInitDialog();

	CGeneralPage	m_dGeneral;
	CScreenPage		m_dScreen;
	CActionsPage	m_dActions;
	CBackgroundPage	m_dBackground;
	CMenuPage		m_dMenu;
	CExcludedPage	m_dExcluded;
	CShortcutsPage	m_dShortcuts;
	CAboutPage		m_dAbout;

protected:
	DECLARE_MESSAGE_MAP()
};
