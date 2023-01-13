// OptionsSheet.h

#pragma once

#include "PropertySheetX.h"
#include "GeneralPage.h"
#include "ComponentsPage.h"
#include "FormatPage.h"
#include "HidePage.h"
#include "FeaturesPage.h"
#include "FindPage.h"
#include "ZIPPage.h"

class COptionsSheet : public CPropertySheetX
{
	DECLARE_DYNAMIC(COptionsSheet)

public:
	COptionsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual	~COptionsSheet();

	virtual BOOL	OnInitDialog();

	CGeneralPage	m_dGeneral;
	CComponentsPage	m_dComponents;
	CFormatPage		m_dFormat;
	CHidePage		m_dHide;
	CFeaturesPage	m_dFeatures;
	CFindPage		m_dFind;
	CZIPPage		m_dZIP;

protected:
	DECLARE_MESSAGE_MAP()
};
