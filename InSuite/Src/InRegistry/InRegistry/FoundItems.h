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

	BOOL	GetKeys(PHKEY phKey, CString& strRootKey, LPCTSTR lpszKey);

	void	SetHeaders();
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
	afx_msg void	OnFindDelete();
	afx_msg void	OnFindExport();

	enum { IDD = IDD_FOUND_ITEMS };

	CListCtrlX	m_ctlFoundItems;
	CButton		m_ctlClear;
	UINT		m_nSelected;

	CDWordArray		m_arrFindType;
	CStringArray	m_arrFindSubKey;
	CStringArray	m_arrFindValue;
	CStringArray	m_arrFindData;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();

	DECLARE_MESSAGE_MAP()
};
