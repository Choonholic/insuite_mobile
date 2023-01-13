// OptionsSheet.h

#pragma once

#include "PropertySheetX.h"
#include "GeneralPage.h"
#include "ComponentsPage.h"
#include "FeaturesPage.h"

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
	CFeaturesPage	m_dFeatures;

protected:
	DECLARE_MESSAGE_MAP()
};
