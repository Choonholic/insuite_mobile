// ExcludedPage.h

#pragma once

#include "PropertyPageX.h"

class CExcludedPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CExcludedPage)

public:
	CExcludedPage();
	virtual	~CExcludedPage();

	enum { IDD = IDD_EXCLUDEDPAGE };

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnBnClickedExAdd();
	afx_msg void	OnBnClickedExRemove();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	CListBox	m_ctlList;
	CButton		m_ctlAdd;
	CButton		m_ctlRemove;

protected:
	void	LoadSettings();
	void	SaveSettings();
	void	SetList();

	virtual void	DoDataExchange(CDataExchange* pDX);

	CStringArray	m_arrModule;
	CDWordArray		m_arrType;

	DECLARE_MESSAGE_MAP()
};