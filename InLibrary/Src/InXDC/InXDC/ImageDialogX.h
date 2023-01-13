// ImageDialogX.h

#pragma once

#include "OptionsDialogX.h"
#include "TreeCtrlX.h"

class AFX_EXT_CLASS CImageDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CImageDialogX)

public:
	CImageDialogX(CWnd* pParent = NULL);
	virtual	~CImageDialogX();

	void	SetDefaultPath(LPCTSTR lpszPath = NULL);
	void	SetFilters(LPCTSTR lpszFilters);
	void	SetFilters(UINT nID);
	void	SetSelectImageOnly(BOOL bSelect = TRUE);
	void	SetEnableLabelEdit(BOOL bEnable = TRUE);
	void	SetUsePopup(BOOL bUse = TRUE);

	CString	GetFullPath();
	CString	GetPath();
	CString	GetFile();
	CString	GetTitle();
	CString	GetExtension();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg void	OnTvnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnBeginlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSelectTree();
	afx_msg void	OnExpandTree();
	afx_msg void	OnCollapseTree();
	afx_msg void	OnNewFolderTree();
	afx_msg void	OnRenameTree();
	afx_msg void	OnDeleteTree();
	afx_msg void	OnRefreshTree();
	afx_msg void	OnPropertiesTree();
	afx_msg LRESULT	OnTapAndHoldTree(WPARAM wParam, LPARAM lParam);

protected:
	BOOL	SearchDefaultItem(HTREEITEM hParentItem);
	CString	SearchNextItem(CString& strItem);

	int		SearchItems(HTREEITEM hParentItem, LPCTSTR lpszFolder);
	int		SearchSpecifiedItems(HTREEITEM hParentItem, LPCTSTR lpszFolder, LPCTSTR lpszSpec, HTREEITEM& hSpecItem);
	CString SearchFullPath(HTREEITEM hItem, BOOL bSelf = TRUE);

	void	BuildRoot();
	void	ExpandSubFolder(HTREEITEM hItem);

	void	LayoutControls();
	void	Refresh(LPCTSTR lpszDefaultPath = NULL);

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();
	virtual void	OnCancel();

	CString			m_strFolder;
	CString			m_strFile;
	CString			m_strDefaultItem;
	CStringArray	m_arrFilters;
	BOOL			m_bImageOnly;
	CImageList		m_iSysIcons;
	BOOL			m_bEnableLabelEdit;
	CString			m_strEdit;
	BOOL			m_bUsePopup;
	RECT			m_rcTree;
	HBITMAP			m_hBitmap;
	int				m_nWidth;
	int				m_nHeight;

	CTreeCtrlX	m_ctlTree;

	DECLARE_MESSAGE_MAP()
};