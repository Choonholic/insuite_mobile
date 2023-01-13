// DualView.h

#pragma once

#include "TreeCtrlX.h"
#include "ListCtrlX.h"

#ifndef VIEW_STANDARD
#define VIEW_STANDARD	0
#define VIEW_FOLDERONLY	1
#define VIEW_FILEONLY	2
#endif

#ifndef HDR_MAX_ITEMS
#define HDR_MAX_ITEMS	5
#define HDR_NAME		0
#define HDR_SIZE		1
#define HDR_TYPE		2
#define HDR_ATTRIBUTE	3
#define HDR_MODIFIED	4
#endif

#ifndef DVIEW_MAX_AUTOWIDTH
#define DVIEW_MAX_AUTOWIDTH	200
#endif

#ifndef EXPINFOX_DEFINED
#define EXPINFOX_DEFINED
typedef struct tagEXPINFOX
{
	WIN32_FIND_DATA	fd;
	TCHAR			szFolder[MAX_PATH];
}
EXPINFOX, *PEXPINFOX, NEAR* NPEXPINFOX, FAR* LPEXPINFOX;
#endif

class CDualView : public CFormView
{
	DECLARE_DYNCREATE(CDualView)

public:
	CDualView();
	virtual	~CDualView();

	void	LoadGeneralOptions();
	void	LoadViewOptions();
	void	LoadBrowserOptions();
	void	SetDefaultPath(LPCTSTR lpszPath = NULL);
	void	SetFilters(LPCTSTR lpszFilters);
	void	SetFilters(UINT nID);
	void	SetHeaderState(int nIndex, BOOL bUse);
	void	SetLayout(BOOL bVertical = TRUE);
	void	SetShowMode(int nMode = VIEW_STANDARD);
	void	SetShowFolderBar(BOOL bShow = TRUE);
	void	SetShowStatusBar(BOOL bShow = TRUE);
	void	SetAutoWidth(BOOL bAuto = TRUE);
	void	SetMaxAutoWidth(int nMax = DVIEW_MAX_AUTOWIDTH);
	void	SetFullRowSelect(BOOL bSet = TRUE);
	void	SetNameEditor(BOOL bUse = FALSE);
	void	SetDoubleTapActivation(BOOL bSet = FALSE);
	void	SetMultiSelect(BOOL bSet = FALSE);
	void	SetDetailedDateFormat(BOOL bFormat = TRUE);
	void	SetDetailedSizeFormat(BOOL bFormat = TRUE);
	void	SetShowExtension(BOOL bShow = TRUE);
	void	SetShowROM(BOOL bShow = TRUE);
	void	SetShowSystem(BOOL bShow = TRUE);
	void	SetShowHidden(BOOL bShow = TRUE);
	void	RefreshLayout();

	virtual void	OnInitialUpdate();
	afx_msg void	OnDestroy();
	virtual BOOL	OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnTvnSelchangedFoldertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnItemexpandedFoldertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnItemexpandingFoldertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnGetdispinfoFoldertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnBeginlabeleditFoldertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnTvnEndlabeleditFoldertree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT	OnTapAndHoldFoldertree(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnHeaderClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLvnGetdispinfoFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLvnBeginlabeleditFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnLvnEndlabeleditFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnNMClickFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnNMDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnNMSetfocusFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT	OnTapAndHoldFilelist(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnTimer(UINT_PTR nIDEvent);

#ifdef _DEBUG
	virtual void	AssertValid() const;
#ifndef _WIN32_WCE
	virtual void	Dump(CDumpContext& dc) const;
#endif
#endif

	enum { IDD = IDD_DUALVIEW };

	CTreeCtrlX	m_ctlTree;
	CListCtrlX	m_ctlList;

protected:
	void	RecalcLayout();
	void	LayoutControls();
	void	Refresh(LPCTSTR lpszDefaultPath = NULL);

	void	BuildHeaders();
	void	SetHeaderDefault();
	void	SetColumnIcons();
	void	RefreshHeaderState();
	int		GetHeaderCount();
	int		GetHeaderIndex(int nItem);

	void	BuildRoot();
	int		SearchFolders(HTREEITEM hParentItem, LPCTSTR lpszFolder);
	int		SearchFoldersSpec(HTREEITEM hParentItem, LPCTSTR lpszFolder, LPCTSTR lpszSpec, HTREEITEM& hSpecItem);
	void	SearchFiles();
	void	SortFiles();

	void	SetCurrentFolder(LPCTSTR lpszFolder);
	BOOL	SearchDefaultItem(HTREEITEM hParentItem);
	CString	SearchNextItem(CString& strItem);
	CString	SearchFullPath(HTREEITEM hItem, BOOL bIncludeSelf = TRUE);

	void	ExpandSubFolder(HTREEITEM hItem, BOOL bRefresh = FALSE);
	void	GoParentFolder();

	CString	GetSizeString(WIN32_FIND_DATA fd);
	CString	GetAttributesString(WIN32_FIND_DATA fd);

	void	UpdateSingleTap();
	void	UpdateDoubleTap();

	void	AddClipboard(LPCTSTR lpszItem);
	void	ClearClipboard();

	void	ModeStandard();
	void	ModeFolderOnly();
	void	ModeFileOnly();

	void	ModeTiles();
	void	ModeIcons();
	void	ModeList();
	void	ModeDetails();

	void	EditFolderName(HTREEITEM hItem);
	void	EditFileName(int nItem);

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void	OnDualRun();
	afx_msg void	OnDualReceive();
	afx_msg void	OnDualFind();
	afx_msg void	OnDualFoundItems();
	afx_msg void	OnDualModeStandard();
	afx_msg void	OnDualModeFolderOnly();
	afx_msg void	OnDualModeFileOnly();
	afx_msg void	OnDualLayout();
	afx_msg void	OnDualFolderBar();
	afx_msg void	OnDualStatusBar();
	afx_msg void	OnDualNameEditor();
	afx_msg void	OnDualRefresh();
	afx_msg void	OnDualSwitch();
	afx_msg void	OnDualOptions();
	afx_msg void	OnFolderExpand();
	afx_msg void	OnFolderCollapse();
	afx_msg void	OnFolderCut();
	afx_msg void	OnFolderCopy();
	afx_msg void	OnFolderPaste();
	afx_msg void	OnFolderPasteShortcut();
	afx_msg void	OnFolderCopyTo();
	afx_msg void	OnFolderMoveTo();
	afx_msg void	OnFolderDelete();
	afx_msg void	OnFolderRename();
	afx_msg void	OnFolderNewFolder();
	afx_msg void	OnFolderProperties();
	afx_msg void	OnFileOpen();
	afx_msg void	OnFileOpenWith();
	afx_msg void	OnFileSendToIR();
	afx_msg void	OnFileZIPAdd();
	afx_msg void	OnFileZIPExtract();
	afx_msg void	OnFileCut();
	afx_msg void	OnFileCopy();
	afx_msg void	OnFilePaste();
	afx_msg void	OnFilePasteShortcut();
	afx_msg void	OnFileAutoWidth();
	afx_msg void	OnFileFullRowSelect();
	afx_msg void	OnFileSelectAll();
	afx_msg void	OnFileInvertSelection();
	afx_msg void	OnFileMultiSelect();
	afx_msg void	OnFileTapActivation();
	afx_msg void	OnFileCopyTo();
	afx_msg void	OnFileMoveTo();
	afx_msg void	OnFileDelete();
	afx_msg void	OnFileRename();
	afx_msg void	OnFileNewFolder();
	afx_msg void	OnFileProperties();
	afx_msg void	OnFileViewTiles();
	afx_msg void	OnFileViewIcons();
	afx_msg void	OnFileViewList();
	afx_msg void	OnFileViewDetails();
	afx_msg void	OnAbout();
	afx_msg void	OnUpdateDualModeStandard(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualModeFolderOnly(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualModeFileOnly(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualLayout(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualFolderBar(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualStatusBar(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualNameEditor(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateDualSwitch(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateFileViewTiles(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateFileViewIcons(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateFileViewList(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateFileViewDetails(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateFileAutoWidth(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateFileFullRowSelect(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateFileMultiSelect(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateFileTapActivation(CCmdUI* pCmdUI);

	CArray<EXPINFOX, EXPINFOX>	m_arrInfo;

	int				m_nCBMode;
	BOOL			m_bCBSingle;
	CString			m_strCBFolder;
	CStringArray	m_arrCBItems;

	CString			m_strFolder;
	CStringArray	m_arrFiles;

	CStringArray	m_arrFoundFiles;

	CString			m_strDefaultItem;
	CStringArray	m_arrFilters;

	BOOL	m_bVertical;
	int		m_nShowMode;
	BOOL	m_bShowFolderBar;
	BOOL	m_bShowStatusBar;
	BOOL	m_bAutoWidth;
	int		m_nMaxAutoWidth;
	BOOL	m_bFullRowSelect;
	BOOL	m_bNameEditor;
	BOOL	m_bDoubleTap;
	BOOL	m_bMultiSelect;
	BOOL	m_bDateFormat;
	BOOL	m_bSizeFormat;
	BOOL	m_bShowExt;
	BOOL	m_bShowROM;
	BOOL	m_bShowSystem;
	BOOL	m_bShowHidden;

	int	m_nSortItem;

	CString	m_strEdit;
	CString	m_strFind;

	int		m_nSplit;
	int		m_nSplitDistance;
	int		m_nTrackDistance;
	int		m_nTracking;
	BOOL	m_bTracking;
	BOOL	m_bFolderBarOpen;

	CImageList	m_iSysSmall;
	CImageList	m_iSysLarge;
	CImageList	m_iHdrIcons;

	CRect	m_rcTree;
	CRect	m_rcList;

	UINT	m_nRenameTimerID;
	UINT	m_nUpdateTimerID;
	UINT	m_nResizeTimerID;
	DWORD	m_dwLastUpdateTicks;
	DWORD	m_dwLastResizeTicks;
	UINT	m_nSizeType;
	int		m_nSizecx;
	int		m_nSizecy;
	BOOL	m_bUseItems[HDR_MAX_ITEMS];

	DECLARE_MESSAGE_MAP()
};
