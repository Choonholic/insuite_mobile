// ImageDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "ImageDialogX.h"
#include "PropertiesDialogX.h"
#include "DeleteDialogX.h"
#include "ShellX.h"
#include "DeviceInfoX.h"
#include "FontX.h"
#include "DisplayX.h"

IMPLEMENT_DYNAMIC(CImageDialogX, COptionsDialogX)

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

CImageDialogX::CImageDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_IMAGE_DIALOG, pParent)
{
	m_arrFilters.Add(_T("*.*"));
	m_strFolder.Empty();
	m_strFile.Empty();

	m_strDefaultItem	= _T("\\");
	m_strEdit			= _T("");
	m_bEnableLabelEdit	= TRUE;
	m_bUsePopup			= TRUE;
	m_bImageOnly		= TRUE;
	m_nWidth			= (-1);
	m_nHeight			= (-1);
	m_hBitmap			= NULL;

	SetHeader(IDS_IMGE_HEADER);
}

CImageDialogX::~CImageDialogX()
{
	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);

		m_hBitmap	= NULL;
		m_nWidth	= (-1);
		m_nHeight	= (-1);
	}
}

void CImageDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMGE_TREE, m_ctlTree);
}

BEGIN_MESSAGE_MAP(CImageDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY(TVN_GETDISPINFO, IDC_IMGE_TREE, &CImageDialogX::OnTvnGetdispinfoTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_IMGE_TREE, &CImageDialogX::OnTvnItemexpandingTree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_IMGE_TREE, &CImageDialogX::OnTvnBeginlabeleditTree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_IMGE_TREE, &CImageDialogX::OnTvnEndlabeleditTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_IMGE_TREE, &CImageDialogX::OnTvnSelchangedTree)
	ON_COMMAND(ID_IMGE_NEW_FOLDER, &CImageDialogX::OnNewFolderTree)
	ON_COMMAND(ID_IMGE_RENAME, &CImageDialogX::OnRenameTree)
	ON_COMMAND(ID_IMGE_DELETE, &CImageDialogX::OnDeleteTree)
	ON_COMMAND(ID_IMGE_REFRESH, &CImageDialogX::OnRefreshTree)
	ON_COMMAND(ID_IMGE_PROPERTIES, &CImageDialogX::OnPropertiesTree)
	ON_MESSAGE(WM_TCX_TAPANDHOLD, &CImageDialogX::OnTapAndHoldTree)
END_MESSAGE_MAP()

void CImageDialogX::SetDefaultPath(LPCTSTR lpszPath)
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

void CImageDialogX::SetFilters(LPCTSTR lpszFilters)
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

void CImageDialogX::SetFilters(UINT nID)
{
	CString	strFilters;

	strFilters.LoadString(nID);
	SetFilters(strFilters);
}

void CImageDialogX::SetSelectImageOnly(BOOL bSelect)
{
	m_bImageOnly	= bSelect;
}

void CImageDialogX::SetEnableLabelEdit(BOOL bEnable)
{
	m_bEnableLabelEdit	= bEnable;
}

void CImageDialogX::SetUsePopup(BOOL bUse)
{
	m_bUsePopup	= bUse;
}

CString CImageDialogX::GetFullPath()
{
	CString	strFullPath;

	strFullPath	= m_strFolder + m_strFile;

	return strFullPath;
}

CString	CImageDialogX::GetPath()
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

CString CImageDialogX::GetFile()
{
	return m_strFile;
}

CString	CImageDialogX::GetTitle()
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

CString	CImageDialogX::GetExtension()
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

BOOL CImageDialogX::SearchDefaultItem(HTREEITEM hParentItem)
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

CString	CImageDialogX::SearchNextItem(CString& strItem)
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

int CImageDialogX::SearchItems(HTREEITEM hParentItem, LPCTSTR lpszFolder)
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

int CImageDialogX::SearchSpecifiedItems(HTREEITEM hParentItem, LPCTSTR lpszFolder, LPCTSTR lpszSpec, HTREEITEM& hSpecItem)
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

CString CImageDialogX::SearchFullPath(HTREEITEM hItem, BOOL bSelf)
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

		strFullPath = _T("\\") + strFullPath;
	}
	else
	{
		strFullPath	= _T("\\");
	}

	return strFullPath;
}

void CImageDialogX::BuildRoot()
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

void CImageDialogX::ExpandSubFolder(HTREEITEM hItem)
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

void CImageDialogX::LayoutControls()
{
	RECT	rc;
	int		nLength;

	GetClientRect(&rc);

	m_rcTree.left	= rc.left;
	m_rcTree.top	= GetHeaderHeight();

	switch (GetScreenResolutionMode())
	{
	case SRMODE_PORTRAIT:
		{
			m_rcTree.right	= rc.right;
			nLength			= ((rc.bottom - m_rcTree.top) / 2);
			m_rcTree.bottom	= m_rcTree.top + nLength;

			m_ctlTree.MoveWindow(&m_rcTree);
		}
		break;
	case SRMODE_LANDSCAPE:
		{
			nLength			= ((rc.right - rc.left) / 2);
			m_rcTree.right	= rc.left + nLength;
			m_rcTree.bottom	= rc.bottom;

			m_ctlTree.MoveWindow(&m_rcTree);
		}
		break;
	case SRMODE_SQUARE:
	default:
		{
			m_rcTree.right	= rc.right;
			m_rcTree.bottom	= rc.bottom;

			m_ctlTree.MoveWindow(&m_rcTree);
		}
		break;
	}

	InvalidateRect(&rc);
	UpdateWindow();
}

void CImageDialogX::Refresh(LPCTSTR lpszDefaultPath)
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

BOOL CImageDialogX::OnInitDialog()
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

void CImageDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);
	LayoutControls();
}

void CImageDialogX::OnPaint()
{
	CPaintDC	dc(this);
	CString		strMessage;
	CRect		rc;
	CRect		rcSplit;
	int			nRatioImage;
	int			nRatioCanvas;
	int			nDispWidth;
	int			nDispHeight;
	int			x;
	int			y;

	GetClientRect(&rc);

	switch (GetScreenResolutionMode())
	{
	case SRMODE_PORTRAIT:
		{
			rcSplit.SetRect(rc.left, m_rcTree.bottom, rc.right, m_rcTree.bottom + DRA::SCALEY(4));
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));
			rcSplit.SetRect(rc.left, m_rcTree.bottom, rc.right, m_rcTree.bottom + DRA::SCALEY(1));
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_WINDOWTEXT));
			rcSplit.SetRect(rc.left, m_rcTree.bottom + DRA::SCALEY(3), rc.right, m_rcTree.bottom + DRA::SCALEY(4));
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_WINDOWTEXT));
			rcSplit.SetRect(rc.left, m_rcTree.bottom + DRA::SCALEY(4), rc.right, rc.bottom);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_WINDOW));

			if (m_hBitmap == NULL)
			{
				strMessage.LoadString(IDS_IMGE_NOIMAGE);
				dc.DrawText(strMessage, -1, &rcSplit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				rcSplit.right	= rc.right - DRA::SCALEX(90);

				rcSplit.InflateRect(-DRA::SCALEX(3), -DRA::SCALEY(3));
				dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));
				rcSplit.InflateRect(-DRA::SCALEX(1), -DRA::SCALEY(1));
				dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_WINDOW));

				nRatioCanvas	= (rcSplit.Height() * 100) / rcSplit.Width();
				nRatioImage		= (m_nHeight * 100) / m_nWidth;

				if ((m_nWidth < rcSplit.Width()) && (m_nHeight < rcSplit.Height()))
				{
					nDispWidth	= m_nWidth;
					nDispHeight	= m_nHeight;
					x			= rcSplit.left + ((rcSplit.Width() - nDispWidth) / 2);
					y			= rcSplit.top + ((rcSplit.Height() - nDispHeight) / 2);
				}
				else if (nRatioCanvas > nRatioImage)
				{
					nRatioImage	= (m_nHeight * 100) / m_nWidth;
					nDispWidth	= rcSplit.Width();
					nDispHeight	= (nDispWidth * nRatioImage) / 100;
					x			= rcSplit.left;
					y			= rcSplit.top + ((rcSplit.Height() - nDispHeight) / 2);
				}
				else
				{
					nRatioImage	= (m_nWidth * 100) / m_nHeight;
					nDispHeight	= rcSplit.Height();
					nDispWidth	= (nDispHeight * nRatioImage) / 100;
					x			= rcSplit.left + ((rcSplit.Width() - nDispWidth) / 2);
					y			= rcSplit.top;
				}

				::TransparentImage(dc.GetSafeHdc(), x, y, nDispWidth, nDispHeight, m_hBitmap, 0, 0, m_nWidth, m_nHeight, GetSysColor(COLOR_WINDOW));

				rcSplit.SetRect(rc.right - DRA::SCALEX(90), m_rcTree.bottom + DRA::SCALEY(4), rc.right, rc.bottom);
				rcSplit.InflateRect(-DRA::SCALEX(4), -DRA::SCALEY(4));

				rcSplit.bottom	= rcSplit.top + (rcSplit.Height() / 2);

				strMessage.LoadString(IDS_IMGE_WIDTH);
				dc.DrawText(strMessage, -1, &rcSplit, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
				strMessage.Format(_T("%d"), m_nWidth);
				dc.DrawText(strMessage, -1, &rcSplit, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

				rcSplit.SetRect(rc.right - DRA::SCALEX(90), m_rcTree.bottom + DRA::SCALEY(4), rc.right, rc.bottom);
				rcSplit.InflateRect(-DRA::SCALEX(4), -DRA::SCALEY(4));

				rcSplit.top	+= (rcSplit.Height() / 2);

				strMessage.LoadString(IDS_IMGE_HEIGHT);
				dc.DrawText(strMessage, -1, &rcSplit, DT_LEFT | DT_TOP | DT_SINGLELINE);
				strMessage.Format(_T("%d"), m_nHeight);
				dc.DrawText(strMessage, -1, &rcSplit, DT_RIGHT | DT_TOP | DT_SINGLELINE);
			}
		}
		break;
	case SRMODE_LANDSCAPE:
		{
			rcSplit.SetRect(m_rcTree.right, GetHeaderHeight(), m_rcTree.right + DRA::SCALEX(4), rc.bottom);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));
			rcSplit.SetRect(m_rcTree.right, GetHeaderHeight(), m_rcTree.right + DRA::SCALEX(1), rc.bottom);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_WINDOWTEXT));
			rcSplit.SetRect(m_rcTree.right + DRA::SCALEX(3), GetHeaderHeight(), m_rcTree.right + DRA::SCALEX(4), rc.bottom);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_WINDOWTEXT));
			rcSplit.SetRect(m_rcTree.right + DRA::SCALEX(4), GetHeaderHeight(), rc.right, rc.bottom);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_WINDOW));

			if (m_hBitmap == NULL)
			{
				strMessage.LoadString(IDS_IMGE_NOIMAGE);
				dc.DrawText(strMessage, -1, &rcSplit, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				rcSplit.bottom	= rc.bottom - DRA::SCALEY(40);

				rcSplit.InflateRect(-DRA::SCALEX(3), -DRA::SCALEY(3));
				dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));
				rcSplit.InflateRect(-DRA::SCALEX(1), -DRA::SCALEY(1));
				dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_WINDOW));

				nRatioCanvas	= (rcSplit.Height() * 100) / rcSplit.Width();
				nRatioImage		= (m_nHeight * 100) / m_nWidth;

				if ((m_nWidth < rcSplit.Width()) && (m_nHeight < rcSplit.Height()))
				{
					nDispWidth	= m_nWidth;
					nDispHeight	= m_nHeight;
					x			= rcSplit.left + ((rcSplit.Width() - nDispWidth) / 2);
					y			= rcSplit.top + ((rcSplit.Height() - nDispHeight) / 2);
				}
				else if (nRatioCanvas > nRatioImage)
				{
					nRatioImage	= (m_nHeight * 100) / m_nWidth;
					nDispWidth	= rcSplit.Width();
					nDispHeight	= (nDispWidth * nRatioImage) / 100;
					x			= rcSplit.left;
					y			= rcSplit.top + ((rcSplit.Height() - nDispHeight) / 2);
				}
				else
				{
					nRatioImage	= (m_nWidth * 100) / m_nHeight;
					nDispHeight	= rcSplit.Height();
					nDispWidth	= (nDispHeight * nRatioImage) / 100;
					x			= rcSplit.left + ((rcSplit.Width() - nDispWidth) / 2);
					y			= rcSplit.top;
				}

				::TransparentImage(dc.GetSafeHdc(), x, y, nDispWidth, nDispHeight, m_hBitmap, 0, 0, m_nWidth, m_nHeight, GetSysColor(COLOR_WINDOW));

				rcSplit.SetRect(m_rcTree.right + DRA::SCALEX(4), rc.bottom - DRA::SCALEY(40), rc.right, rc.bottom);
				rcSplit.InflateRect(-DRA::SCALEX(4), -DRA::SCALEY(4));

				rcSplit.bottom	= rcSplit.top + (rcSplit.Height() / 2);

				strMessage.LoadString(IDS_IMGE_WIDTH);
				dc.DrawText(strMessage, -1, &rcSplit, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
				strMessage.Format(_T("%d"), m_nWidth);
				dc.DrawText(strMessage, -1, &rcSplit, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

				rcSplit.SetRect(m_rcTree.right + DRA::SCALEX(4), rc.bottom - DRA::SCALEY(40), rc.right, rc.bottom);
				rcSplit.InflateRect(-DRA::SCALEX(4), -DRA::SCALEY(4));

				rcSplit.top	+= (rcSplit.Height() / 2);

				strMessage.LoadString(IDS_IMGE_HEIGHT);
				dc.DrawText(strMessage, -1, &rcSplit, DT_LEFT | DT_TOP | DT_SINGLELINE);
				strMessage.Format(_T("%d"), m_nHeight);
				dc.DrawText(strMessage, -1, &rcSplit, DT_RIGHT | DT_TOP | DT_SINGLELINE);
			}
		}
		break;
	case SRMODE_SQUARE:
	default:
		break;
	}

	COptionsDialogX::OnPaint();
}

void CImageDialogX::OnTvnGetdispinfoTree(NMHDR* pNMHDR, LRESULT* pResult)
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

	*pResult = 0;
}

void CImageDialogX::OnTvnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
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

void CImageDialogX::OnTvnBeginlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult)
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

void CImageDialogX::OnTvnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	CString	strOldPath;
	CString	strNewPath;

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

void CImageDialogX::OnTvnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView	= reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);

		m_hBitmap	= NULL;
		m_nWidth	= (-1);
		m_nHeight	= (-1);
	}

	HTREEITEM	hItem	= pNMTreeView->itemNew.hItem;

	if (hItem)
	{
		if (m_ctlTree.GetItemData(hItem) == ITEM_FILE)
		{
			CString	strFile	= SearchFullPath(hItem) + m_ctlTree.GetItemText(hItem);

			if ((m_hBitmap = SHLoadImageFile(strFile)) != NULL)
			{
				BITMAP	ObjBmp;

				GetObject(m_hBitmap, sizeof(BITMAP), (LPVOID)&ObjBmp);

				m_nWidth	= ObjBmp.bmWidth;
				m_nHeight	= ObjBmp.bmHeight;
			}
		}
	}

	RECT	rc;

	GetClientRect(&rc);
	InvalidateRect(&rc);
	UpdateWindow();

	*pResult	= 0;
}

void CImageDialogX::OnSelectTree()
{
	OnOK();
}

void CImageDialogX::OnExpandTree()
{
	HTREEITEM	hItem	= m_ctlTree.GetSelectedItem();

	m_ctlTree.Expand(hItem, TVE_EXPAND);
	m_ctlTree.EnsureVisible(hItem);
}

void CImageDialogX::OnCollapseTree()
{
	HTREEITEM	hItem	= m_ctlTree.GetSelectedItem();

	m_ctlTree.Expand(hItem, TVE_COLLAPSE);
	m_ctlTree.EnsureVisible(hItem);
}

void CImageDialogX::OnNewFolderTree()
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

void CImageDialogX::OnRenameTree()
{
	m_ctlTree.EditLabel(m_ctlTree.GetSelectedItem());
}

void CImageDialogX::OnDeleteTree()
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

void CImageDialogX::OnRefreshTree()
{
	Refresh();
}

void CImageDialogX::OnPropertiesTree()
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

LRESULT CImageDialogX::OnTapAndHoldTree(WPARAM wParam, LPARAM lParam)
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

			if (m_bImageOnly == FALSE)
			{
				strMenu.LoadString(IDS_MENU_SELECT);
				ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_SELECT, strMenu);
			}
		}

		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_NEW_FOLDER);
		ctlMenu.AppendMenu(MF_ENABLED, ID_IMGE_NEW_FOLDER, strMenu);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_RENAME);
		ctlMenu.AppendMenu(MF_ENABLED, ID_IMGE_RENAME, strMenu);
		strMenu.LoadString(IDS_MENU_DELETE);
		ctlMenu.AppendMenu(MF_ENABLED, ID_IMGE_DELETE, strMenu);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strMenu.LoadString(IDS_MENU_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_IMGE_REFRESH, strMenu);
		strMenu.LoadString(IDS_MENU_PROPERTIES);
		ctlMenu.AppendMenu(MF_ENABLED, ID_IMGE_PROPERTIES, strMenu);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}

	return 0;
}

void CImageDialogX::OnOK()
{
	HTREEITEM	hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) == NULL)
	{
		return;
	}

	if (m_bImageOnly)
	{
		if ((m_ctlTree.GetItemData(hItem) != ITEM_FILE) || (m_hBitmap == NULL))
		{
			return;
		}
	}

	m_strFolder	= SearchFullPath(hItem);
	m_strFile	= m_ctlTree.GetItemText(hItem);

	COptionsDialogX::OnOK();
}

void CImageDialogX::OnCancel()
{
	COptionsDialogX::OnCancel();
}
