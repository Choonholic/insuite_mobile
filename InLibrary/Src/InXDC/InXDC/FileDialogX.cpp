// FileDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "FileDialogX.h"
#include "PropertiesDialogX.h"
#include "DeleteDialogX.h"
#include "ShellX.h"
#include "DeviceInfoX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CFileDialogX, COptionsDialogX)

enum ITEM_TYPE
{
	ITEM_NEW,
	ITEM_FOLDER,
	ITEM_FOLDER_NOITEM,
	ITEM_FILE
};

static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CTreeCtrlX*	pTreeCtrl	= (CTreeCtrlX*)lParamSort;

	if ((lParam1 == ITEM_FILE) && (lParam2 != ITEM_FILE))
	{
		return 1;
	}

	if ((lParam1 != ITEM_FILE) && (lParam2 == ITEM_FILE))
	{
		return -1;
	}

	return 0;
}

CFileDialogX::CFileDialogX(CWnd* pParent /*=NULL*/)	: COptionsDialogX(IDD_TREE_DIALOG, pParent)
{
	m_arrFilters.Add(_T("*.*"));
	m_strFolder.Empty();
	m_strFile.Empty();

	m_strDefaultItem	= _T("\\");
	m_strEdit			= _T("");
	m_bEnableLabelEdit	= TRUE;
	m_bFileOnly			= TRUE;
	m_bUsePopup			= TRUE;

	SetHeader(IDS_FILE_HEADER);
}

CFileDialogX::~CFileDialogX()
{
}

void CFileDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TREE, m_ctlTree);
}

BEGIN_MESSAGE_MAP(CFileDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE_TREE, &CFileDialogX::OnTvnGetdispinfoTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_TREE, &CFileDialogX::OnTvnItemexpandingTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE_TREE, &CFileDialogX::OnTvnBeginlabeleditTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_TREE, &CFileDialogX::OnTvnEndlabeleditTree)
	ON_COMMAND(ID_FILE_SELECT, &CFileDialogX::OnSelectTree)
	ON_COMMAND(ID_FILE_EXPAND, &CFileDialogX::OnExpandTree)
	ON_COMMAND(ID_FILE_COLLAPSE, &CFileDialogX::OnCollapseTree)
	ON_COMMAND(ID_FILE_EXECUTE, &CFileDialogX::OnExecuteTree)
	ON_COMMAND(ID_FILE_NEW_FOLDER, &CFileDialogX::OnNewFolderTree)
	ON_COMMAND(ID_FILE_RENAME, &CFileDialogX::OnRenameTree)
	ON_COMMAND(ID_FILE_DELETE, &CFileDialogX::OnDeleteTree)
	ON_COMMAND(ID_FILE_REFRESH, &CFileDialogX::OnRefreshTree)
	ON_COMMAND(ID_FILE_PROPERTIES, &CFileDialogX::OnPropertiesTree)
	ON_MESSAGE(WM_TCX_TAPANDHOLD, &CFileDialogX::OnTapAndHoldTree)
END_MESSAGE_MAP()

void CFileDialogX::SetDefaultPath(LPCTSTR lpszPath)
{
	m_strDefaultItem	= _T("");

	if (lpszPath == NULL)
	{
		m_strDefaultItem	= _T("\\");
	}
	else
	{
		if (lpszPath[0] != _T('\\'))
		{
			m_strDefaultItem = _T("\\");
		}

		m_strDefaultItem += lpszPath;
	}

	if ((m_strDefaultItem.Right(1) == _T("\\")) && (m_strDefaultItem.GetLength() > 1))
	{
		m_strDefaultItem	= m_strDefaultItem.Left(m_strDefaultItem.GetLength() - 1);
	}
}

void CFileDialogX::SetFilters(LPCTSTR lpszFilters)
{
	m_arrFilters.RemoveAll();

	if (lpszFilters != NULL)
	{
		CString	strBase, strParsed;
		int		nPos;

		strBase	= lpszFilters;

		while (strBase.GetLength())
		{
			if ((nPos = strBase.Find(_T('|'))) != -1)
			{
				strParsed	= strBase.Left(nPos);
				strBase		= strBase.Mid(nPos + 1);
			}
			else
			{
				strParsed	= strBase;
				strBase		= _T("");
			}

			if (strParsed.GetLength())
			{
				m_arrFilters.Add(strParsed);
			}
		}
	}
	else
	{
		m_arrFilters.Add(_T("*.*"));
	}
}

void CFileDialogX::SetFilters(UINT nID)
{
	CString	strFilters;

	strFilters.LoadString(nID);
	SetFilters(strFilters);
}

void CFileDialogX::SetSelectFileOnly(BOOL bSelect)
{
	m_bFileOnly	= bSelect;
}

void CFileDialogX::SetEnableLabelEdit(BOOL bEnable)
{
	m_bEnableLabelEdit	= bEnable;
}

void CFileDialogX::SetUsePopup(BOOL bUse)
{
	m_bUsePopup	= bUse;
}

CString CFileDialogX::GetFullPath()
{
	CString	strFullPath;

	strFullPath	= m_strFolder + m_strFile;

	return strFullPath;
}

CString	CFileDialogX::GetPath()
{
	CString	strPath;

	if (m_strFolder != _T("\\"))
	{
		strPath	= m_strFolder.Left(m_strFolder.GetLength() - 1);
	}
	else
	{
		strPath	= m_strFolder;
	}

	return strPath;
}

CString CFileDialogX::GetFile()
{
	return m_strFile;
}

CString	CFileDialogX::GetTitle()
{
	int		nPos;
	CString	strTitle;

	if ((nPos = m_strFile.ReverseFind(_T('.'))) != -1)
	{
		strTitle	= m_strFile.Left(nPos);
	}
	else
	{
		strTitle	= m_strFile;
	}

	return strTitle;
}

CString	CFileDialogX::GetExtension()
{
	int		nPos;
	CString	strExt;

	if ((nPos = m_strFile.ReverseFind(_T('.'))) != -1)
	{
		strExt	= m_strFile.Mid(nPos + 1);
	}
	else
	{
		strExt	= _T("");
	}

	return strExt;
}

BOOL CFileDialogX::SearchDefaultItem(HTREEITEM hParentItem)
{
	CFileStatus	fs;
	CString		strParsed;
	CString		strItem;
	BOOL		bExist	= FALSE;
	BOOL		bFound	= FALSE;
	HTREEITEM	hItem;

	if (m_strDefaultItem == _T("\\"))
	{
		ExpandSubFolder(hParentItem);
		m_ctlTree.Select(hParentItem, TVGN_CARET);

		bFound	= TRUE;

		return bFound;
	}

	if (!CFile::GetStatus(m_strDefaultItem, fs))
	{
		return bFound;
	}

	strItem		= m_strDefaultItem;
	strParsed	= SearchNextItem(strItem);
	bExist		= (BOOL)(strParsed.GetLength() > 0);

	while (bExist)
	{
		TVITEM	item;

		item.mask	= TVIF_HANDLE | TVIF_PARAM;
		item.hItem	= hParentItem;

		CString	strFullPath	= SearchFullPath(item.hItem);
		RECT	rc;

		if (SearchSpecifiedItems(item.hItem, strFullPath, strParsed, hItem))
		{
			item.lParam	= ITEM_FOLDER;
		}
		else
		{
			item.lParam = ITEM_FOLDER_NOITEM;
		}

		TVSORTCB	tvs;

		tvs.hParent		= item.hItem;
		tvs.lpfnCompare	= CompareProc;
		tvs.lParam		= 0;

		m_ctlTree.SortChildrenCB(&tvs);
		m_ctlTree.SetItem(&item);
		m_ctlTree.GetItemRect(item.hItem, &rc, FALSE);
		m_ctlTree.InvalidateRect(&rc);

		if (hItem == NULL)
		{
			bExist	= FALSE;
		}
		else
		{
			hParentItem	= hItem;
			strParsed	= SearchNextItem(strItem);

			if ((bExist = (BOOL)(strParsed.GetLength() > 0)) == FALSE)
			{
				m_ctlTree.Select(hItem, TVGN_CARET);

				bFound	= TRUE;
			}
		}
	}

	m_ctlTree.UpdateWindow();
	return bFound;
}

CString	CFileDialogX::SearchNextItem(CString &strItem)
{
	CString	strParsed	= strItem;

	if (strParsed.Left(1) == _T("\\"))
	{
		strParsed	= strParsed.Mid(1);
	}

	int	nFind	= strParsed.Find(_T("\\"));

	if (nFind >= 0)
	{
		strItem		= strParsed.Mid(nFind);
		strParsed	= strParsed.Left(nFind);
	}
	else
	{
		strItem	= _T("");
	}

	return strParsed;
}

int CFileDialogX::SearchItems(HTREEITEM hParentItem, LPCTSTR lpszFolder)
{
	CString			strFilter;
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	BOOL			bFind;
	int				iIcon;
	TVINSERTSTRUCT	tvi;
	CString			strFile;
	int				nCount	= 0;

	BeginWaitCursor();
	strFilter.Format(_T("%s*.*"), lpszFolder);

	hFind	= FindFirstFile(strFilter, &fd);
	bFind	= (hFind != INVALID_HANDLE_VALUE);

	while (bFind)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strFile.Format(_T("%s%s"), lpszFolder, fd.cFileName);

			iIcon					= GetSystemIconIndex(strFile);
			tvi.hParent				= hParentItem;
			tvi.hInsertAfter		= TVI_SORT;
			tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
			tvi.item.pszText		= fd.cFileName;
			tvi.item.iImage			= iIcon;
			tvi.item.iSelectedImage	= iIcon;
			tvi.item.cChildren		= I_CHILDRENCALLBACK;
			tvi.item.lParam			= ITEM_NEW;

			m_ctlTree.InsertItem(&tvi);

			nCount++;
		}

		bFind	= FindNextFile(hFind, &fd);
	}

	FindClose(hFind);

	for (int i = 0; i < m_arrFilters.GetSize(); i++)
	{
		strFilter.Format(_T("%s%s"), lpszFolder, m_arrFilters.GetAt(i));

		hFind	= FindFirstFile(strFilter, &fd);
		bFind	= (hFind != INVALID_HANDLE_VALUE);

		while (bFind)
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				strFile.Format(_T("%s%s"), lpszFolder, fd.cFileName);

				iIcon					= GetSystemIconIndex(strFile);
				tvi.hParent				= hParentItem;
				tvi.hInsertAfter		= TVI_SORT;
				tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				tvi.item.pszText		= fd.cFileName;
				tvi.item.iImage			= iIcon;
				tvi.item.iSelectedImage	= iIcon;
				tvi.item.cChildren		= I_CHILDRENCALLBACK;
				tvi.item.lParam			= ITEM_FILE;
				m_ctlTree.InsertItem(&tvi);

				nCount++;
			}

			bFind	= FindNextFile(hFind, &fd);
		}

		FindClose(hFind);
	}

	EndWaitCursor();
	return nCount;
}

int CFileDialogX::SearchSpecifiedItems(HTREEITEM hParentItem, LPCTSTR lpszFolder, LPCTSTR lpszSpec, HTREEITEM &hSpecItem)
{
	CString			strFilter;
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	BOOL			bFind;
	int				iIcon;
	TVINSERTSTRUCT	tvi;
	CString			strFile;
	int				nCount	= 0;

	BeginWaitCursor();

	hSpecItem	= NULL;

	strFilter.Format(_T("%s*.*"), lpszFolder);

	hFind	= FindFirstFile(strFilter, &fd);
	bFind	= (hFind != INVALID_HANDLE_VALUE);

	while (bFind)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strFile.Format(_T("%s%s"), lpszFolder, fd.cFileName);

			iIcon					= GetSystemIconIndex(strFile);
			tvi.hParent				= hParentItem;
			tvi.hInsertAfter		= TVI_SORT;
			tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
			tvi.item.pszText		= fd.cFileName;
			tvi.item.iImage			= iIcon;
			tvi.item.iSelectedImage	= iIcon;
			tvi.item.cChildren		= I_CHILDRENCALLBACK;
			tvi.item.lParam			= ITEM_NEW;

			HTREEITEM	hItem	= m_ctlTree.InsertItem(&tvi);

			nCount++;

			if (!_tcscmp(fd.cFileName, lpszSpec))
			{
				hSpecItem	= hItem;
			}
		}

		bFind	= FindNextFile(hFind, &fd);
	}

	FindClose(hFind);

	for (int i = 0; i < m_arrFilters.GetSize(); i++)
	{
		strFilter.Format(_T("%s%s"), lpszFolder, m_arrFilters.GetAt(i));

		hFind	= FindFirstFile(strFilter, &fd);
		bFind	= (hFind != INVALID_HANDLE_VALUE);

		while (bFind)
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				strFile.Format(_T("%s%s"), lpszFolder, fd.cFileName);

				iIcon					= GetSystemIconIndex(strFile);
				tvi.hParent				= hParentItem;
				tvi.hInsertAfter		= TVI_SORT;
				tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				tvi.item.pszText		= fd.cFileName;
				tvi.item.iImage			= iIcon;
				tvi.item.iSelectedImage	= iIcon;
				tvi.item.cChildren		= I_CHILDRENCALLBACK;
				tvi.item.lParam			= ITEM_FILE;

				HTREEITEM	hItem	= m_ctlTree.InsertItem(&tvi);

				nCount++;

				if (!_tcscmp(fd.cFileName, lpszSpec))
				{
					hSpecItem	= hItem;
				}
			}

			bFind	= FindNextFile(hFind, &fd);
		}

		FindClose(hFind);
	}

	EndWaitCursor();
	return nCount;
}

CString CFileDialogX::SearchFullPath(HTREEITEM hItem, BOOL bSelf)
{
	CString	strFullPath;
	CString	strFolder;

	if (hItem)
	{
		if (bSelf == FALSE)
		{
			hItem	= m_ctlTree.GetParentItem(hItem);
		}

		while (hItem)
		{
			if (m_ctlTree.GetItemData(hItem) != ITEM_FILE)
			{
				if (hItem == m_ctlTree.GetRootItem())
				{
					strFolder	= _T("\\");
				}
				else
				{
					strFolder	= m_ctlTree.GetItemText(hItem);
				}

				if (strFolder != _T("\\"))
				{
					strFullPath	= strFolder + _T("\\") + strFullPath;
				}
			}

			hItem	= m_ctlTree.GetParentItem(hItem);
		}

		strFullPath	= _T("\\") + strFullPath;
	}
	else
	{
		strFullPath	= _T("\\");
	}

	return strFullPath;
}

void CFileDialogX::BuildRoot()
{
	HTREEITEM		hItemRoot	= TVI_ROOT;
	int				iIcon		= GetSystemIconIndex(_T("\\"));
	TCHAR			szRoot[64];
	TVINSERTSTRUCT	tvi;

	GetDeviceIDX(szRoot, 64);

	tvi.hParent				= hItemRoot;
	tvi.hInsertAfter		= TVI_LAST;
	tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText		= (LPTSTR)szRoot;
	tvi.item.iImage			= iIcon;
	tvi.item.iSelectedImage	= iIcon;
	tvi.item.cChildren		= I_CHILDRENCALLBACK;
	tvi.item.lParam			= ITEM_NEW;

	HTREEITEM	hItem	= m_ctlTree.InsertItem(&tvi);

	SearchDefaultItem(hItem);
}

void CFileDialogX::ExpandSubFolder(HTREEITEM hItem)
{
	if (m_ctlTree.GetItemData(hItem) == ITEM_NEW)
	{
		TVITEM	item;

		item.mask	= TVIF_HANDLE | TVIF_PARAM;
		item.hItem	= hItem;

		CString	strFullPath = SearchFullPath(item.hItem);
		RECT	rc;

		if (SearchItems(item.hItem, strFullPath))
		{
			item.lParam	= ITEM_FOLDER;
		}
		else
		{
			item.lParam = ITEM_FOLDER_NOITEM;
		}

		TVSORTCB	tvs;

		tvs.hParent		= item.hItem;
		tvs.lpfnCompare	= CompareProc;
		tvs.lParam		= 0;

		m_ctlTree.SortChildrenCB(&tvs);
		m_ctlTree.SetItem(&item);
		m_ctlTree.GetItemRect(item.hItem, &rc, FALSE);
		m_ctlTree.InvalidateRect(&rc);
		m_ctlTree.UpdateWindow();
	}

	m_ctlTree.Expand(hItem, TVE_EXPAND);
	m_ctlTree.EnsureVisible(hItem);
}

void CFileDialogX::LayoutControls()
{
	RECT	rc;

	GetClientRect(&rc);

	rc.top	= GetHeaderHeight();

	m_ctlTree.MoveWindow(&rc);

	rc.top	= DRA::SCALEY(0);

	InvalidateRect(&rc);
	UpdateWindow();
}

void CFileDialogX::Refresh(LPCTSTR lpszDefaultPath)
{
	BeginWaitCursor();

	if (lpszDefaultPath == NULL)
	{
		SetDefaultPath(SearchFullPath(m_ctlTree.GetSelectedItem()));
	}
	else
	{
		SetDefaultPath(lpszDefaultPath);
	}

	m_ctlTree.DeleteAllItems();
	BuildRoot();
	EndWaitCursor();
}

void CFileDialogX::ExecuteItem()
{
	HTREEITEM	hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		if (m_ctlTree.GetItemData(hItem) == ITEM_FILE)
		{
			CString				strFile;
			SHELLEXECUTEINFO	shei;

			strFile	= SearchFullPath(hItem);
			strFile	+= m_ctlTree.GetItemText(hItem);

			memset(&shei, 0, sizeof(SHELLEXECUTEINFO));

			shei.cbSize			= sizeof(SHELLEXECUTEINFO);
			shei.fMask			= 0;
			shei.hwnd			= this->GetSafeHwnd();
			shei.lpVerb			= _T("open");
			shei.lpFile			= strFile;
			shei.lpDirectory	= SearchFullPath(hItem);
			shei.nShow			= SW_SHOW;

			ShellExecuteEx(&shei);
		}
	}
}

BOOL CFileDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_OPEN);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_MENU_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	HIMAGELIST	hSysIcons	= GetSystemIconList();
	LOGFONT		lfFont;

	m_iSysIcons.Attach(hSysIcons);
	m_ctlTree.SetImageList(&m_iSysIcons, TVSIL_NORMAL);
	m_iSysIcons.Detach();
	GetDefaultFont(&lfFont);
	SetItemFont(CreateFontIndirect(&lfFont));
	LayoutControls();
	BuildRoot();
	return TRUE;
}

void CFileDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);
	LayoutControls();
}

void CFileDialogX::OnTvnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if (pTVDispInfo->item.mask & TVIF_CHILDREN)
	{
		if ((pTVDispInfo->item.lParam == ITEM_NEW) || (pTVDispInfo->item.lParam == ITEM_FOLDER))
		{
			pTVDispInfo->item.cChildren	= 1;
		}
		else
		{
			pTVDispInfo->item.cChildren	= 0;
		}
	}

	*pResult	= 0;
}

void CFileDialogX::OnTvnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView	= reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (pNMTreeView->action & TVE_EXPAND)
	{
		if (pNMTreeView->itemNew.lParam == ITEM_NEW)
		{
			TVITEM	item;

			item.mask	= TVIF_HANDLE | TVIF_PARAM;
			item.hItem	= pNMTreeView->itemNew.hItem;

			CString	strFullPath = SearchFullPath(item.hItem);
			RECT	rc;

			if (SearchItems(item.hItem, strFullPath))
			{
				item.lParam	= ITEM_FOLDER;
			}
			else
			{
				item.lParam	= ITEM_FOLDER_NOITEM;
			}

			TVSORTCB	tvs;

			tvs.hParent		= item.hItem;
			tvs.lpfnCompare	= CompareProc;
			tvs.lParam		= 0;

			m_ctlTree.SortChildrenCB(&tvs);
			m_ctlTree.SetItem(&item);
			m_ctlTree.GetItemRect(item.hItem, &rc, FALSE);
			m_ctlTree.InvalidateRect(&rc);
			m_ctlTree.UpdateWindow();
		}
	}

	*pResult	= 0;
}

void CFileDialogX::OnTvnBeginlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if (m_bEnableLabelEdit)
	{
		if (pTVDispInfo->item.hItem == m_ctlTree.GetRootItem())
		{
			*pResult	= 1;
		}
		else
		{
			m_strEdit	= m_ctlTree.GetItemText(pTVDispInfo->item.hItem);

			::SHSipPreference(this->GetSafeHwnd(), SIP_UP);
			m_ctlTree.EnsureVisible(pTVDispInfo->item.hItem);

			*pResult	= 0;
		}
	}
	else
	{
		*pResult	= 1;
	}
}

void CFileDialogX::OnTvnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	CString		strOldPath;
	CString		strNewPath;

	::SHSipPreference(this->GetSafeHwnd(), SIP_DOWN);

	if (!pTVDispInfo->item.pszText)
	{
		*pResult	= 0;

		return;
	}

	if (!CheckValidFileName(pTVDispInfo->item.pszText))
	{
		CString	strTitle;
		CString	strMessage;

		strTitle.LoadString(IDS_FILE_TITLEERROR);
		strMessage.LoadString(IDS_FILE_INVALIDNAME);

		strMessage	+= _T("\n\n");
		strMessage	+= _T("\\ / : * ? \" < > |");

		MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);

		*pResult	= 0;

		return;
	}

	switch (pTVDispInfo->item.lParam)
	{
	case ITEM_FILE:
		{
			strOldPath	= SearchFullPath(pTVDispInfo->item.hItem) + m_strEdit;
			strNewPath	= SearchFullPath(pTVDispInfo->item.hItem) + pTVDispInfo->item.pszText;
		}
		break;
	case ITEM_FOLDER:
	case ITEM_FOLDER_NOITEM:
	case ITEM_NEW:
		{
			strOldPath	= SearchFullPath(pTVDispInfo->item.hItem, FALSE) + m_strEdit;
			strNewPath	= SearchFullPath(pTVDispInfo->item.hItem, FALSE) + pTVDispInfo->item.pszText;
		}
		break;
	}

	m_strEdit	= _T("");
	*pResult	= (BOOL)MoveFile(strOldPath, strNewPath);
}

void CFileDialogX::OnSelectTree()
{
	OnOK();
}

void CFileDialogX::OnExpandTree()
{
	HTREEITEM	hItem	= m_ctlTree.GetSelectedItem();

	m_ctlTree.Expand(hItem, TVE_EXPAND);
	m_ctlTree.EnsureVisible(hItem);
}

void CFileDialogX::OnCollapseTree()
{
	HTREEITEM	hItem	= m_ctlTree.GetSelectedItem();

	m_ctlTree.Expand(hItem, TVE_COLLAPSE);
	m_ctlTree.EnsureVisible(hItem);
}

void CFileDialogX::OnExecuteTree()
{
	ExecuteItem();
}

void CFileDialogX::OnNewFolderTree()
{
	HTREEITEM	hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		CFileStatus	fs;
		CString		strPath;
		CString		strNewFolder;
		int			nCount	= 0;

		strNewFolder.LoadString(IDS_FILE_NEW_FOLDER);
		strPath.Format(_T("%s%s"), SearchFullPath(hItem), strNewFolder);

		while (CFile::GetStatus(strPath, fs))
		{
			strPath.Format(_T("%s%s #%d"), SearchFullPath(hItem), strNewFolder, ++nCount);
		}

		if (CreateDirectory(strPath, NULL))
		{
			Refresh(strPath);

			if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
			{
				m_ctlTree.EditLabel(hItem);
			}
		}
	}
}

void CFileDialogX::OnRenameTree()
{
	m_ctlTree.EditLabel(m_ctlTree.GetSelectedItem());
}

void CFileDialogX::OnDeleteTree()
{
	CDeleteDialogX	dDelete;
	HTREEITEM		hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		CString	strFile;

		strFile	= SearchFullPath(hItem);

		if (m_ctlTree.GetItemData(hItem) == ITEM_FILE)
		{
			strFile	+= m_ctlTree.GetItemText(hItem);
		}

		dDelete.ClearFiles();
		dDelete.AddFile(strFile);

		if (dDelete.DoModal() == IDOK)
		{
			Refresh(SearchFullPath(hItem, FALSE));
		}
	}
}

void CFileDialogX::OnRefreshTree()
{
	Refresh();
}

void CFileDialogX::OnPropertiesTree()
{
	CPropertiesDialogX	dProperties;
	HTREEITEM			hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		CString	strFile;

		strFile	= SearchFullPath(hItem);

		if (m_ctlTree.GetItemData(hItem) == ITEM_FILE)
		{
			strFile	+= m_ctlTree.GetItemText(hItem);
		}

		dProperties.ClearFiles();
		dProperties.AddFile(strFile);
		dProperties.DoModal();

		if (dProperties.GetChangeFlags())
		{
			Refresh();
		}
	}
}

LRESULT CFileDialogX::OnTapAndHoldTree(WPARAM wParam, LPARAM lParam)
{
	if (m_bUsePopup)
	{
		HTREEITEM	hItem	= (HTREEITEM)wParam;
		CMenu		ctlMenu;
		CString		strMenu;

		if (hItem == NULL)
		{
			return 0;
		}

		m_ctlTree.Select(hItem, TVGN_CARET);
		ctlMenu.CreatePopupMenu();

		if (m_ctlTree.GetItemData(hItem) == ITEM_FILE)
		{
			strMenu.LoadString(IDS_MENU_SELECT);
			ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_SELECT, strMenu);
			strMenu.LoadString(IDS_MENU_EXECUTE);
			ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_EXECUTE, strMenu);
		}
		else
		{
			if ((m_ctlTree.GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) != TVIS_EXPANDED)
			{
				if (m_ctlTree.GetItemData(hItem) != ITEM_FOLDER_NOITEM)
				{
					strMenu.LoadString(IDS_MENU_EXPAND);
					ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_EXPAND, strMenu);
				}
			}
			else
			{
				strMenu.LoadString(IDS_MENU_COLLAPSE);
				ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_COLLAPSE, strMenu);
			}

			if (m_bFileOnly == FALSE)
			{
				strMenu.LoadString(IDS_MENU_SELECT);
				ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_SELECT, strMenu);
			}
		}

		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_NEW_FOLDER);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_NEW_FOLDER, strMenu);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_RENAME);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_RENAME, strMenu);
		strMenu.LoadString(IDS_MENU_DELETE);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_DELETE, strMenu);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_REFRESH, strMenu);
		strMenu.LoadString(IDS_MENU_PROPERTIES);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_PROPERTIES, strMenu);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}

	return 0;
}

void CFileDialogX::OnOK()
{
	HTREEITEM	hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) == NULL)
	{
		return;
	}

	if ((m_bFileOnly) && (m_ctlTree.GetItemData(hItem) != ITEM_FILE))
	{
		return;
	}

	m_strFolder	= SearchFullPath(hItem);
	m_strFile	= m_ctlTree.GetItemText(hItem);

	COptionsDialogX::OnOK();
}

void CFileDialogX::OnCancel()
{
	COptionsDialogX::OnCancel();
}
