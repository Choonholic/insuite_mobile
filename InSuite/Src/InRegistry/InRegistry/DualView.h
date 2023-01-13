// DualView.h

#pragma once

#include "TreeCtrlX.h"
#include "ListCtrlX.h"

#ifndef VIEW_STANDARD
#define VIEW_STANDARD		0
#define VIEW_KEYONLY		1
#define VIEW_VALUEONLY		2
#endif

#ifndef TID_KEY_REFRESH
#define TID_KEY_REFRESH	0x0803
#define TID_VAL_REFRESH	0x0804
#define TID_RESIZE		0x0805
#define MINIMUM_RESIZE	200
#endif

class CDualView : public CFormView
{
	DECLARE_DYNCREATE(CDualView)

public:
	CDualView();
	virtual	~CDualView();

	void	LoadLayoutOptions();

	void	SetDefaultKey(LPCTSTR lpszKey = NULL);
	void	SetLayout(BOOL bVertical = TRUE);
	void	SetShowMode(int nMode = VIEW_STANDARD);
	void	SetShowDataType(BOOL bShow = TRUE);
	void	SetAutoWidth(BOOL bAuto = FALSE);
	void	SetFullRowSelect(BOOL bSet = TRUE);
	void	SetNameEditor(BOOL bUse = TRUE);
	void	RefreshLayout(BOOL bHeader = FALSE);

	virtual void	OnInitialUpdate();
	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTvnSelchangedKeytree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnItemexpandedKeytree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnItemexpandingKeytree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnGetdispinfoKeytree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnBeginlabeleditKeytree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnEndlabeleditKeytree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT	OnTapAndHoldKeytree(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnLvnGetdispinfoValuelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLvnBeginlabeleditValuelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLvnEndlabeleditValuelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLvnKeydownValuelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnNMDblclkValuelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnNMSetfocusValuelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT	OnTapAndHoldValuelist(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);

#ifdef _DEBUG
	virtual void	AssertValid() const;
#ifndef _WIN32_WCE
	virtual void	Dump(CDumpContext& dc) const;
#endif
#endif

	enum { IDD = IDD_DUALVIEW };

	CTreeCtrlX	m_ctlKeyTree;
	CListCtrlX	m_ctlValueList;
	CImageList	m_iKeyImages;
	CImageList	m_iValueImages;

protected:
	void	RecalcLayout();
	void	LayoutControls();
	void	Refresh(LPCTSTR lpszDefaultKey = NULL);

	void	BuildHeaders();

	void	BuildRoot();
	BOOL	GetKeys(PHKEY phKey, CString& strRootKey, LPCTSTR lpszKey);
	BOOL	GetPath(CString& strFullPath, HKEY hKey, LPCTSTR lpszSubKey);
	int		SearchKeys(HTREEITEM hParentItem, LPCTSTR lpszKey);
	int		SearchKeysSpec(HTREEITEM hParentItem, LPCTSTR lpszKey, LPCTSTR lpszSpec, HTREEITEM& hSpecItem);
	void	SearchValues();

	BOOL	SearchDefaultKey(HTREEITEM hParentItem);
	CString	SearchNextKey(CString& strKey);
	CString	SearchFullKeyPath(HTREEITEM hItem, BOOL bIncludeSelf = TRUE);

	void	ExpandSubKeys(HTREEITEM hItem, BOOL bRefresh = FALSE);
	void	GoParentKey();

	void	ModeStandard();
	void	ModeKeyOnly();
	void	ModeValueOnly();

	void	EditKeyName(HTREEITEM hItem);
	void	EditValueName(int nItem);

	void	EditStringValue(CString strValue);
	void	EditBinaryValue(CString strValue);
	void	EditDWORDValue(CString strValue);
	void	EditMultiStringValue(CString strValue);
	void	EditExpandStringValue(CString strValue);

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void	OnDualImport();
	afx_msg void	OnDualExport();
	afx_msg void	OnDualFind();
	afx_msg void	OnDualOpenFoundItems();
	afx_msg void	OnDualModeStandard();
	afx_msg void	OnDualModeKeyOnly();
	afx_msg void	OnDualModeValueOnly();
	afx_msg void	OnDualLayout();
	afx_msg void	OnDualNameEditor();
	afx_msg void	OnDualNewKey();
	afx_msg void	OnDualNewString();
	afx_msg void	OnDualNewBinary();
	afx_msg void	OnDualNewDWORD();
	afx_msg void	OnDualNewMultiString();
	afx_msg void	OnDualNewExpandString();
	afx_msg void	OnDualRefresh();
	afx_msg void	OnDualOptions();
	afx_msg void	OnDualSwitch();
	afx_msg void	OnKeyExpand();
	afx_msg void	OnKeyCollapse();
	afx_msg void	OnKeyDelete();
	afx_msg void	OnKeyRename();
	afx_msg void	OnKeyCopyname();
	afx_msg void	OnValueModify();
	afx_msg void	OnValueDelete();
	afx_msg void	OnValueRename();
	afx_msg void	OnValueShowDataType();
	afx_msg void	OnValueAutoWidth();
	afx_msg void	OnValueFullRowSelect();
	afx_msg void	OnAbout();
	afx_msg void	OnUpdateDualModeStandard(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualModeKeyOnly(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualModeValueOnly(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualLayout(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualNameEditor(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualSwitch(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateValueShowDataType(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateValueAutoWidth(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateValueFullRowSelect(CCmdUI* pCmdUI);

	HKEY	m_hRootKey;
	CString	m_strSubKey;

	CString	m_strDefaultKey;

	BOOL	m_bVertical;
	int		m_nShowMode;
	BOOL	m_bShowDataType;
	BOOL	m_bAutoWidth;
	BOOL	m_bFullRowSelect;
	BOOL	m_bNameEditor;

	CString	m_strEdit;

	CString	m_strSearch;
	BOOL	m_bSearchKeys;
	BOOL	m_bSearchValues;
	BOOL	m_bSearchData;

	HKEY	m_hSearchStartRootKey;
	CString	m_strSearchStartSubKey;

	HKEY	m_hSearchRootKey;
	CString	m_strSearchSubKey;
	int		m_nSearchSubKeyIndex;
	int		m_nSearchValueIndex;
	int		m_nSearchType;

	int		m_nSplit;
	int		m_nSplitDistance;
	int		m_nTrackDistance;
	int		m_nTracking;
	BOOL	m_bTracking;

	CRect	m_rcKeyTree;
	CRect	m_rcValueList;

	UINT	m_nKeyTimerID;
	UINT	m_nValueTimerID;
	UINT	m_nResizeTimerID;
	DWORD	m_dwLastResizeTicks;

	CDWordArray		m_arrFindType;
	CStringArray	m_arrFindSubKey;
	CStringArray	m_arrFindValue;
	CStringArray	m_arrFindData;

	DECLARE_MESSAGE_MAP()
};
