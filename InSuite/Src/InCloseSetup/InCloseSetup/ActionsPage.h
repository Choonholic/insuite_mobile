// ActionsPage.h

#pragma once

#include "PropertyPageX.h"
#include "ListCtrlX.h"

#define MAX_ACTIONS	6

class CActionsPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CActionsPage)

public:
	CActionsPage();
	virtual	~CActionsPage();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnLvnItemchangedAcSelect(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnCbnSelchangeAcAssign();

	enum { IDD = IDD_ACTIONSPAGE };

	CStatic		m_ctlSelectH;
	CStatic		m_ctlAssignH;
	CListCtrlX	m_ctlSelect;
	CComboBox	m_ctlAssign;
	CImageList	m_iActionImages;

	int	m_nAssignments[MAX_ACTIONS];
	int	m_nSelectedAction;

protected:
	void	LoadSettings();
	void	SaveSettings();
	void	SetHeader();
	void	SetMenu();
	void	SetItems();

	virtual void	DoDataExchange(CDataExchange* pDX);

	int	m_nMaxActions;

	DECLARE_MESSAGE_MAP()
};
