// FoundDialog.h

#pragma once

#include "OptionsDialogX.h"
#include "ListCtrlX.h"

class CFoundDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFoundDialog)

public:
	CFoundDialog(CWnd* pParent = NULL);
	virtual	~CFoundDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedSelectAll();
	afx_msg void	OnBnClickedUnselectAll();
	afx_msg void	OnClean();

	enum { IDD = IDD_FOUND };

	CListCtrlX	m_ctlItems;
	CButton		m_ctlSelectAll;
	CButton		m_ctlUnselectAll;

protected:
	void	SetHeaders();
	void	SetItems();
	void	Clean();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
