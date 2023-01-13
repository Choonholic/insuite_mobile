// OptionsSheet.h

#pragma once

#include "PropertySheetX.h"
#include "GeneralPage.h"
#include "AntialiasingPage.h"
#include "FolderPage.h"

class COptionsSheet : public CPropertySheetX
{
	DECLARE_DYNAMIC(COptionsSheet)

public:
	COptionsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual	~COptionsSheet();

	virtual BOOL	OnInitDialog();

	CGeneralPage		m_dGeneralPage;
	CAntialiasingPage	m_dAntialiasingPage;
	CFolderPage			m_dFolderPage;

protected:
	DECLARE_MESSAGE_MAP()
};
