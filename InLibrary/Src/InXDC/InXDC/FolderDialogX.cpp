// FolderDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "FolderDialogX.h"
#include "PropertiesDialogX.h"
#include "DeleteDialogX.h"
#include "ShellX.h"
#include "DeviceInfoX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CFolderDialogX, COptionsDialogX)

enum ITEM_TYPE
{
	ITEM_NEW,
	ITEM_FOLDER,
	ITEM_FOLDER_NOITEM
};

CFolderDialogX::CFolderDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_TREE_DIALOG, pParent)
{
	m_strFolder.Empty();

	m_strDefaultItem	= _T("\\");
	m_strEdit			= _T("");
	m_bEnableLabelEdit	= TRUE;
	m_bUsePopup			= TRUE;

	SetHeader(IDS_FOLD_HEADER);
}

CFolderDialogX::~CFolderDialogX()
{
}

void CFolderDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_TREE, m_ctlTree);
}

BEGIN_MESSAGE_MAP(CFolderDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE_TREE, &CFolderDialogX::OnTvnGetdispinfoTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_TREE, &CFolderDialogX::OnTvnItemexpandingTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE_TREE, &CFolderDialogX::OnTvnBeginlabeleditTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_TREE, &CFolderDialogX::OnTvnEndlabeleditTree)
	ON_COMMAND(ID_FOLD_SELECT, &CFolderDialogX::OnSelectTree)
	ON_COMMAND(ID_FOLD_EXPAND, &CFolderDialogX::OnExpandTree)
	ON_COMMAND(ID_FOLD_COLLAPSE, &CFolderDialogX::OnCollapseTree)
	ON_COMMAND(ID_FOLD_NEW_FOLDER, &CFolderDialogX::OnNewFolderTree)
	ON_COMMAND(ID_FOLD_RENAME, &CFolderDialogX::OnRenameTree)
	ON_COMMAND(ID_FOLD_DELETE, &CFolderDialogX::OnDeleteTree)
	ON_COMMAND(ID_FOLD_REFRESH, &CFolderDialogX::OnRefreshTree)
	ON_COMMAND(ID_FOLD_PROPERTIES, &CFolderDialogX::OnPropertiesTree)
	ON_MESSAGE(WM_TCX_TAPANDHOLD, &CFolderDialogX::OnTapAndHoldTree)
END_MESSAGE_MAP()

void CFolderDialogX::SetDefaultPath(LPCTSTR lpszPath)
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
			m_strDefaultItem	= _T("\\");
		}

		m_strDefaultItem	+= lpszPath;
	}

	if ((m_strDefaultItem.Right(1) == _T("\\")) && (m_strDefaultItem.GetLength() > 1))
	{
		m_strDefaultItem	= m_strDefaultItem.Left(m_strDefaultItem.GetLength() - 1);
	}
}

void CFolderDialogX::SetEnableLabelEdit(BOOL bEnable)
{
	m_bEnableLabelEdit	= bEnable;
}

void CFolderDialogX::SetUsePopup(BOOL bUse)
{
	m_bUsePopup	= bUse;
}

CString CFolderDialogX::GetFullPath()
{
	return GetPath();
}

CString	CFolderDialogX::GetPath()
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

BOOL CFolderDialogX::SearchDefaultItem(HTREEITEM hParentItem)
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
			item.lParam	= ITEM_FOLDER_NOITEM;
		}

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

CString	CFolderDialogX::SearchNextItem(CString &strItem)
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

int CFolderDialogX::SearchItems(HTREEITEM hParentItem, LPCTSTR lpszFolder)
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
	EndWaitCursor();
	return nCount;
}

int CFolderDialogX::SearchSpecifiedItems(HTREEITEM hParentItem, LPCTSTR lpszFolder, LPCTSTR lpszSpec, HTREEITEM &hSpecItem)
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
	EndWaitCursor();
	return nCount;
}

CString CFolderDialogX::SearchFullPath(HTREEITEM hItem, BOOL bSelf)
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

void CFolderDialogX::BuildRoot()
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

void CFolderDialogX::ExpandSubFolder(HTREEITEM hItem)
{
	if (m_ctlTree.GetItemData(hItem) == ITEM_NEW)
	{
		TVITEM	item;

		item.mask	= TVIF_HANDLE | TVIF_PARAM;
		item.hItem	= hItem;

		CString	strFullPath	= SearchFullPath(item.hItem);
		RECT	rc;

		if (SearchItems(item.hItem, strFullPath))
		{
			item.lParam	= ITEM_FOLDER;
		}
		else
		{
			item.lParam = ITEM_FOLDER_NOITEM;
		}

		m_ctlTree.SetItem(&item);
		m_ctlTree.GetItemRect(item.hItem, &rc, FALSE);
		m_ctlTree.InvalidateRect(&rc);
		m_ctlTree.UpdateWindow();
	}

	m_ctlTree.Expand(hItem, TVE_EXPAND);
	m_ctlTree.EnsureVisible(hItem);
}

void CFolderDialogX::LayoutControls()
{
	RECT	rc;

	GetClientRect(&rc);

	rc.top	= GetHeaderHeight();

	m_ctlTree.MoveWindow(&rc);

	rc.top	= DRA::SCALEY(0);

	InvalidateRect(&rc);
	UpdateWindow();
}

void CFolderDialogX::Refresh(LPCTSTR lpszDefaultPath)
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

BOOL CFolderDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_SELECT);

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

void CFolderDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);
	LayoutControls();
}

void CFolderDialogX::OnTvnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult)
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

void CFolderDialogX::OnTvnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
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
				item.lParam = ITEM_FOLDER_NOITEM;
			}

			m_ctlTree.SetItem(&item);
			m_ctlTree.GetItemRect(item.hItem, &rc, FALSE);
			m_ctlTree.InvalidateRect(&rc);
			m_ctlTree.UpdateWindow();
		}
	}

	*pResult	= 0;
}

void CFolderDialogX::OnTvnBeginlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult)
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

void CFolderDialogX::OnTvnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	CString			strOldPath;
	CString			strNewPath;

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

	strOldPath	= SearchFullPath(pTVDispInfo->item.hItem, FALSE) + m_strEdit;
	strNewPath	= SearchFullPath(pTVDispInfo->item.hItem, FALSE) + pTVDispInfo->item.pszText;
	m_strEdit	= _T("");
	*pResult	= (BOOL)MoveFile(strOldPath, strNewPath);
}

void CFolderDialogX::OnSelectTree()
{
	OnOK();
}

void CFolderDialogX::OnExpandTree()
{
	HTREEITEM	hItem	= m_ctlTree.GetSelectedItem();

	m_ctlTree.Expand(hItem, TVE_EXPAND);
	m_ctlTree.EnsureVisible(hItem);
}

void CFolderDialogX::OnCollapseTree()
{
	HTREEITEM	hItem	= m_ctlTree.GetSelectedItem();

	m_ctlTree.Expand(hItem, TVE_COLLAPSE);
	m_ctlTree.EnsureVisible(hItem);
}

void CFolderDialogX::OnNewFolderTree()
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

void CFolderDialogX::OnRenameTree()
{
	m_ctlTree.EditLabel(m_ctlTree.GetSelectedItem());
}

void CFolderDialogX::OnDeleteTree()
{
	CDeleteDialogX	dDelete;
	HTREEITEM		hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		dDelete.ClearFiles();
		dDelete.AddFile(SearchFullPath(hItem));

		if (dDelete.DoModal() == IDOK)
		{
			Refresh(SearchFullPath(hItem, FALSE));
		}
	}
}

void CFolderDialogX::OnRefreshTree()
{
	Refresh();
}

void CFolderDialogX::OnPropertiesTree()
{
	CPropertiesDialogX	dProperties;
	HTREEITEM			hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		dProperties.ClearFiles();
		dProperties.AddFile(SearchFullPath(hItem));
		dProperties.DoModal();

		if (dProperties.GetChangeFlags())
		{
			Refresh();
		}
	}
}

LRESULT CFolderDialogX::OnTapAndHoldTree(WPARAM wParam, LPARAM lParam)
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

		if ((m_ctlTree.GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) != TVIS_EXPANDED)
		{
			if (m_ctlTree.GetItemData(hItem) != ITEM_FOLDER_NOITEM)
			{
				strMenu.LoadString(IDS_MENU_EXPAND);
				ctlMenu.AppendMenu(MF_ENABLED, ID_FOLD_EXPAND, strMenu);
			}
		}
		else
		{
			strMenu.LoadString(IDS_MENU_COLLAPSE);
			ctlMenu.AppendMenu(MF_ENABLED, ID_FOLD_COLLAPSE, strMenu);
		}

		strMenu.LoadString(IDS_MENU_SELECT);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLD_SELECT, strMenu);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_NEW_FOLDER);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLD_NEW_FOLDER, strMenu);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_RENAME);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLD_RENAME, strMenu);
		strMenu.LoadString(IDS_MENU_DELETE);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLD_DELETE, strMenu);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLD_REFRESH, strMenu);
		strMenu.LoadString(IDS_MENU_PROPERTIES);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLD_PROPERTIES, strMenu);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}

	return 0;
}

void CFolderDialogX::OnOK()
{
	HTREEITEM	hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		m_strFolder	= SearchFullPath(hItem);

		COptionsDialogX::OnOK();
	}
}

void CFolderDialogX::OnCancel()
{
	COptionsDialogX::OnCancel();
}
