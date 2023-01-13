// ShortcutsPage.h

#pragma once

#include "PropertyPageX.h"

typedef struct _tagSHORTCUTINFO
{
	TCHAR	szOption[3];
	DWORD	dwStringID;
	DWORD	dwIconID;
} SHORTCUTINFO, FAR* LPSHORTCUTINFO;

class CShortcutsPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CShortcutsPage)

public:
	CShortcutsPage();
	virtual	~CShortcutsPage();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedScCreate();

	enum { IDD = IDD_SHORTCUTSPAGE };

	CStatic		m_ctlCommandH;
	CComboBox	m_ctlCommand;
	CButton		m_ctlCreate;

protected:
	void	SetItems();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
