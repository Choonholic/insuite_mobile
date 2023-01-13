// ComponentsDialog.h

#pragma once

#include "OptionsDialogX.h"
#include "ComponentList.h"

class CComponentsDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CComponentsDialog)

public:
	CComponentsDialog(CWnd* pParent = NULL);
	virtual	~CComponentsDialog();

	void	SetItems();
	void	Refresh();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnModifyComponent();

	enum { IDD = IDD_COMPONENTS_DIALOG };

	CComponentList	m_ctlList;
	CStringArray	m_arrItems;
	BOOL			m_bModified;

protected:
	void	ModifyComponent(int nComponent);

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();

	DECLARE_MESSAGE_MAP()
};
