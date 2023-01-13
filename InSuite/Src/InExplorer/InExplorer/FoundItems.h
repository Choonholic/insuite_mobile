// FoundItems.h

#pragma once

#include "OptionsDialogX.h"
#include "ListCtrlX.h"

class CFoundItems : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFoundItems)

public:
	CFoundItems(CWnd* pParent = NULL);
	virtual	~CFoundItems();

	void	SetHeaders();
	void	SetHeaderDefault();
	void	SetItems();
	void	ClearItems();
	void	SetButtons();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedFoundClear();
	afx_msg void	OnNMClickFoundList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT	OnTapAndHoldFoundList(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnLvnItemchangedFoundList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnFindClearAll();
	afx_msg void	OnFindLocate();

	enum { IDD = IDD_FOUND_ITEMS };

	CListCtrlX	m_ctlFoundItems;
	CButton		m_ctlClear;
	BOOL		m_bAutoWidth;
	int			m_nMaxAutoWidth;

	UINT			m_nSelected;
	CStringArray	m_arrFiles;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();

	CImageList	m_iSysSmall;

	DECLARE_MESSAGE_MAP()
};
