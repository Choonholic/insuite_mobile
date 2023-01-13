// DualView.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "InRegistry_Settings.h"
#include "MainFrm.h"
#include "DisplayX.h"
#include "ShellX.h"
#include "ClipboardX.h"
#include "DeviceInfoX.h"
#include "RegistryMFCX.h"
#include "ExportRange.h"
#include "FileDialogX.h"
#include "FileSaveDialogX.h"
#include "EditorName.h"
#include "EditorSZ.h"
#include "EditorMultiSZ.h"
#include "EditorDWORD.h"
#include "EditorBinary.h"
#include "FindDialog.h"
#include "FoundItems.h"
#include "OptionsSheet.h"
#include "AboutDialog.h"
#include "DualView.h"

enum ITEM_TYPE
{
	ITEM_NEW,
	ITEM_KEY,
	ITEM_KEY_NOSUB
};

IMPLEMENT_DYNCREATE(CDualView, CFormView)

CDualView::CDualView() : CFormView(CDualView::IDD)
{
	m_bVertical				= TRUE;
	m_nShowMode				= VIEW_STANDARD;
	m_nSplit				= 0;
	m_nSplitDistance		= DRA::SCALEY(4);
	m_nTrackDistance		= 0;
	m_nTracking				= 0;
	m_bTracking				= FALSE;
	m_bShowDataType			= TRUE;
	m_bAutoWidth			= FALSE;
	m_bFullRowSelect		= TRUE;
	m_bNameEditor			= TRUE;

	m_hRootKey				= NULL;
	m_strSubKey				= _T("");

	m_strSearch				= _T("");
	m_bSearchKeys			= TRUE;
	m_bSearchValues			= TRUE;
	m_bSearchData			= TRUE;

	m_hSearchRootKey		= NULL;
	m_strSearchSubKey		= _T("");
	m_nSearchSubKeyIndex	= (-1);
	m_nSearchValueIndex		= (-1);
	m_nSearchType			= (-1);

	m_nKeyTimerID			= 0;
	m_nValueTimerID			= 0;
	m_nResizeTimerID		= 0;
	m_dwLastResizeTicks		= 0;
}

CDualView::~CDualView()
{
}

void CDualView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KEYTREE, m_ctlKeyTree);
	DDX_Control(pDX, IDC_VALUELIST, m_ctlValueList);
}

BEGIN_MESSAGE_MAP(CDualView, CFormView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_KEYTREE, &CDualView::OnTvnSelchangedKeytree)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_KEYTREE, &CDualView::OnTvnItemexpandedKeytree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_KEYTREE, &CDualView::OnTvnItemexpandingKeytree)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_KEYTREE, &CDualView::OnTvnGetdispinfoKeytree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_KEYTREE, &CDualView::OnTvnBeginlabeleditKeytree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_KEYTREE, &CDualView::OnTvnEndlabeleditKeytree)
	ON_MESSAGE(WM_TCX_TAPANDHOLD, &CDualView::OnTapAndHoldKeytree)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_VALUELIST, &CDualView::OnLvnGetdispinfoValuelist)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_VALUELIST, &CDualView::OnLvnBeginlabeleditValuelist)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_VALUELIST, &CDualView::OnLvnEndlabeleditValuelist)
	ON_NOTIFY(LVN_KEYDOWN, IDC_VALUELIST, &CDualView::OnLvnKeydownValuelist)
	ON_NOTIFY(NM_DBLCLK, IDC_VALUELIST, &CDualView::OnNMDblclkValuelist)
	ON_NOTIFY(NM_SETFOCUS, IDC_VALUELIST, &CDualView::OnNMSetfocusValuelist)
	ON_MESSAGE(WM_LCX_TAPANDHOLD, &CDualView::OnTapAndHoldValuelist)
	ON_COMMAND(ID_DUAL_IMPORT, &CDualView::OnDualImport)
	ON_COMMAND(ID_DUAL_EXPORT, &CDualView::OnDualExport)
	ON_COMMAND(ID_DUAL_FIND, &CDualView::OnDualFind)
	ON_COMMAND(ID_DUAL_OPEN_FOUND_ITEMS, &CDualView::OnDualOpenFoundItems)
	ON_COMMAND(ID_DUAL_MODE_STANDARD, &CDualView::OnDualModeStandard)
	ON_COMMAND(ID_DUAL_MODE_KEYONLY, &CDualView::OnDualModeKeyOnly)
	ON_COMMAND(ID_DUAL_MODE_VALUEONLY, &CDualView::OnDualModeValueOnly)
	ON_COMMAND(ID_DUAL_LAYOUT, &CDualView::OnDualLayout)
	ON_COMMAND(ID_DUAL_NAMEEDITOR, &CDualView::OnDualNameEditor)
	ON_COMMAND(ID_DUAL_NEW_KEY, &CDualView::OnDualNewKey)
	ON_COMMAND(ID_DUAL_NEW_STRING, &CDualView::OnDualNewString)
	ON_COMMAND(ID_DUAL_NEW_BINARY, &CDualView::OnDualNewBinary)
	ON_COMMAND(ID_DUAL_NEW_DWORD, &CDualView::OnDualNewDWORD)
	ON_COMMAND(ID_DUAL_NEW_MSTRING, &CDualView::OnDualNewMultiString)
	ON_COMMAND(ID_DUAL_NEW_ESTRING, &CDualView::OnDualNewExpandString)
	ON_COMMAND(ID_DUAL_REFRESH, &CDualView::OnDualRefresh)
	ON_COMMAND(ID_DUAL_OPTIONS, &CDualView::OnDualOptions)
	ON_COMMAND(ID_DUAL_SWITCH, &CDualView::OnDualSwitch)
	ON_COMMAND(ID_KEY_EXPAND, &CDualView::OnKeyExpand)
	ON_COMMAND(ID_KEY_COLLAPSE, &CDualView::OnKeyCollapse)
	ON_COMMAND(ID_KEY_DELETE, &CDualView::OnKeyDelete)
	ON_COMMAND(ID_KEY_RENAME, &CDualView::OnKeyRename)
	ON_COMMAND(ID_KEY_COPYNAME, &CDualView::OnKeyCopyname)
	ON_COMMAND(ID_VALUE_MODIFY, &CDualView::OnValueModify)
	ON_COMMAND(ID_VALUE_DELETE, &CDualView::OnValueDelete)
	ON_COMMAND(ID_VALUE_RENAME, &CDualView::OnValueRename)
	ON_COMMAND(ID_VALUE_SHOWDATATYPE, &CDualView::OnValueShowDataType)
	ON_COMMAND(ID_VALUE_AUTOWIDTH, &CDualView::OnValueAutoWidth)
	ON_COMMAND(ID_VALUE_FULLROWSELECT, &CDualView::OnValueFullRowSelect)
	ON_COMMAND(ID_ABOUT, &CDualView::OnAbout)
	ON_UPDATE_COMMAND_UI(ID_DUAL_MODE_STANDARD, &CDualView::OnUpdateDualModeStandard)
	ON_UPDATE_COMMAND_UI(ID_DUAL_MODE_KEYONLY, &CDualView::OnUpdateDualModeKeyOnly)
	ON_UPDATE_COMMAND_UI(ID_DUAL_MODE_VALUEONLY, &CDualView::OnUpdateDualModeValueOnly)
	ON_UPDATE_COMMAND_UI(ID_DUAL_LAYOUT, &CDualView::OnUpdateDualLayout)
	ON_UPDATE_COMMAND_UI(ID_DUAL_NAMEEDITOR, &CDualView::OnUpdateDualNameEditor)
	ON_UPDATE_COMMAND_UI(ID_DUAL_SWITCH, &CDualView::OnUpdateDualSwitch)
	ON_UPDATE_COMMAND_UI(ID_VALUE_SHOWDATATYPE, &CDualView::OnUpdateValueShowDataType)
	ON_UPDATE_COMMAND_UI(ID_VALUE_AUTOWIDTH, &CDualView::OnUpdateValueAutoWidth)
	ON_UPDATE_COMMAND_UI(ID_VALUE_FULLROWSELECT, &CDualView::OnUpdateValueFullRowSelect)
	ON_WM_TIMER()
END_MESSAGE_MAP()

#ifdef _DEBUG
void CDualView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDualView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CDualView::LoadLayoutOptions()
{
	CRegistryX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	SetLayout((BOOL)(reg.ReadDWORD(RV_RG_LAYOUT, 1) == 1));
	SetShowMode(reg.ReadDWORD(RV_RG_MODE, VIEW_STANDARD));
	SetShowDataType((BOOL)(reg.ReadDWORD(RV_RG_SHOWDATATYPE, 1) == 1));
	SetAutoWidth((BOOL)(reg.ReadDWORD(RV_RG_AUTOWIDTH, 0) == 1));
	SetFullRowSelect((BOOL)(reg.ReadDWORD(RV_RG_FULLROWSELECT, 1) == 1));
	SetNameEditor((BOOL)(reg.ReadDWORD(RV_RG_NAMEEDITOR, 1) == 1));
	reg.Close();
}

void CDualView::SetDefaultKey(LPCTSTR lpszKey)
{
	m_strDefaultKey	= _T("");

	if (lpszKey != NULL)
	{
		m_strDefaultKey = lpszKey;
	}

	if (m_strDefaultKey.Right(1) == _T("\\"))
	{
		m_strDefaultKey	= m_strDefaultKey.Left(m_strDefaultKey.GetLength() - 1);
	}
}

void CDualView::SetLayout(BOOL bVertical)
{
	m_bVertical	= bVertical;
}

void CDualView::SetShowMode(int nMode)
{
	m_nShowMode	= nMode;
}

void CDualView::SetShowDataType(BOOL bShow)
{
	m_bShowDataType	= bShow;
}

void CDualView::SetAutoWidth(BOOL bAuto)
{
	m_bAutoWidth	= bAuto;
}

void CDualView::SetFullRowSelect(BOOL bSet)
{
	m_bFullRowSelect	= bSet;
}

void CDualView::SetNameEditor(BOOL bUse)
{
	m_bNameEditor	= bUse;
}

void CDualView::RefreshLayout(BOOL bHeader)
{
	RecalcLayout();
	LayoutControls();

	if (bHeader)
	{
		BuildHeaders();
	}
}

void CDualView::RecalcLayout()
{
	RECT	rc;

	GetClientRect(&rc);

	if (m_bVertical == TRUE)
	{
		m_nSplit	= ((rc.bottom - rc.top - m_nSplitDistance) / 2) + rc.top;

		GetClientRect(&m_rcKeyTree);

		m_rcKeyTree.top		= DRA::SCALEY(0);
		m_rcKeyTree.left	= DRA::SCALEX(0);
		m_rcKeyTree.bottom	= m_nSplit;

		GetClientRect(m_rcValueList);

		m_rcValueList.top	= m_nSplit + m_nSplitDistance + DRA::SCALEY(1);
	}
	else
	{
		m_nSplit	= ((rc.right - rc.left - m_nSplitDistance) / 2) + rc.left;

		GetClientRect(&m_rcKeyTree);

		m_rcKeyTree.top		= DRA::SCALEY(0);
		m_rcKeyTree.left	= DRA::SCALEX(0);
		m_rcKeyTree.right	= m_nSplit;

		GetClientRect(m_rcValueList);

		m_rcValueList.top	= DRA::SCALEY(0);
		m_rcValueList.left	= m_nSplit + m_nSplitDistance + DRA::SCALEY(1);
	}
}

void CDualView::LayoutControls()
{
	switch (m_nShowMode)
	{
	case VIEW_STANDARD:
		{
			ModeStandard();
		}
		break;
	case VIEW_KEYONLY:
		{
			ModeKeyOnly();
		}
		break;
	case VIEW_VALUEONLY:
		{
			ModeValueOnly();
		}
		break;
	}
}

void CDualView::Refresh(LPCTSTR lpszDefaultKey)
{
	BeginWaitCursor();

	if (lpszDefaultKey == NULL)
	{
		SetDefaultKey(SearchFullKeyPath(m_ctlKeyTree.GetSelectedItem()));
	}
	else
	{
		SetDefaultKey(lpszDefaultKey);
	}

	BuildRoot();
	EndWaitCursor();
}

void CDualView::BuildHeaders()
{
	CString	strItem;
	int		nCount	= m_ctlValueList.GetHeaderCtrl()->GetItemCount();

	for (int i = 0; i < nCount; i++)
	{
		m_ctlValueList.DeleteColumn(0);
	}

	strItem.LoadString(IDS_HEADER_NAME);
	m_ctlValueList.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(100), 0);

	if (m_bShowDataType)
	{
		strItem.LoadString(IDS_HEADER_TYPE);
		m_ctlValueList.InsertColumn(1, strItem, LVCFMT_LEFT, DRA::SCALEX(80), 1);
		strItem.LoadString(IDS_HEADER_DATA);
		m_ctlValueList.InsertColumn(2, strItem, LVCFMT_LEFT, DRA::SCALEX(120), 2);
	}
	else
	{
		strItem.LoadString(IDS_HEADER_DATA);
		m_ctlValueList.InsertColumn(1, strItem, LVCFMT_LEFT, DRA::SCALEX(120), 1);
	}
}

void CDualView::BuildRoot()
{
	HTREEITEM		hItemRoot	= TVI_ROOT;
	TCHAR			szRoot[64];
	TVINSERTSTRUCT	tvi;

	GetDeviceIDX(szRoot, 64);
	m_ctlKeyTree.ModifyStyle(TVS_EDITLABELS, 0);

	if (!m_bNameEditor)
	{
		m_ctlKeyTree.ModifyStyle(0, TVS_EDITLABELS);
	}

	if (m_ctlKeyTree.GetCount())
	{
		m_ctlKeyTree.DeleteAllItems();
	}

	tvi.hParent				= hItemRoot;
	tvi.hInsertAfter		= TVI_LAST;
	tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText		= (LPTSTR)szRoot;
	tvi.item.iImage			= 0;
	tvi.item.iSelectedImage	= 0;
	tvi.item.cChildren		= I_CHILDRENCALLBACK;
	tvi.item.lParam			= ITEM_KEY;

	HTREEITEM	hItem		= m_ctlKeyTree.InsertItem(&tvi);

	tvi.hParent				= hItem;
	tvi.hInsertAfter		= TVI_LAST;
	tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText		= _T("HKEY_CLASSES_ROOT");
	tvi.item.iImage			= 1;
	tvi.item.iSelectedImage	= 1;
	tvi.item.cChildren		= I_CHILDRENCALLBACK;
	tvi.item.lParam			= ITEM_NEW;

	m_ctlKeyTree.InsertItem(&tvi);

	tvi.hParent				= hItem;
	tvi.hInsertAfter		= TVI_LAST;
	tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText		= _T("HKEY_CURRENT_USER");
	tvi.item.iImage			= 1;
	tvi.item.iSelectedImage	= 1;
	tvi.item.cChildren		= I_CHILDRENCALLBACK;
	tvi.item.lParam			= ITEM_NEW;

	m_ctlKeyTree.InsertItem(&tvi);

	tvi.hParent				= hItem;
	tvi.hInsertAfter		= TVI_LAST;
	tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText		= _T("HKEY_LOCAL_MACHINE");
	tvi.item.iImage			= 1;
	tvi.item.iSelectedImage	= 1;
	tvi.item.cChildren		= I_CHILDRENCALLBACK;
	tvi.item.lParam			= ITEM_NEW;

	m_ctlKeyTree.InsertItem(&tvi);

	tvi.hParent				= hItem;
	tvi.hInsertAfter		= TVI_LAST;
	tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText		= _T("HKEY_USERS");
	tvi.item.iImage			= 1;
	tvi.item.iSelectedImage	= 1;
	tvi.item.cChildren		= I_CHILDRENCALLBACK;
	tvi.item.lParam			= ITEM_NEW;

	m_ctlKeyTree.InsertItem(&tvi);

	SearchDefaultKey(hItem);
	SearchValues();
}

BOOL CDualView::GetKeys(PHKEY phKey, CString &strRootKey, LPCTSTR lpszKey)
{
	CString	strKey	= lpszKey;
	int		nPos	= -1;

	*phKey		= NULL;
	strRootKey	= _T("");

	if (strKey.GetLength() == 0)
	{
		return TRUE;
	}

	if (strKey.Left(17) == _T("HKEY_CLASSES_ROOT"))
	{
		*phKey	= HKEY_CLASSES_ROOT;
		nPos	= 17;
	}
	else if (strKey.Left(17) == _T("HKEY_CURRENT_USER"))
	{
		*phKey	= HKEY_CURRENT_USER;
		nPos	= 17;
	}
	else if (strKey.Left(18) == _T("HKEY_LOCAL_MACHINE"))
	{
		*phKey	= HKEY_LOCAL_MACHINE;
		nPos	= 18;
	}
	else if (strKey.Left(10) == _T("HKEY_USERS"))
	{
		*phKey	= HKEY_USERS;
		nPos	= 10;
	}

	if (nPos < 0)
	{
		return FALSE;
	}

	if (strKey.GetLength() == nPos)
	{
		strRootKey	= _T("");
	}
	else if ((strKey.GetLength() > (nPos + 1)) && (strKey[nPos] == _T('\\')))
	{
		strRootKey	= strKey.Mid(nPos + 1);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDualView::GetPath(CString &strFullPath, HKEY hKey, LPCTSTR lpszSubKey)
{
	if (hKey == HKEY_CLASSES_ROOT)
	{
		strFullPath	= _T("HKEY_CLASSES_ROOT\\");
	}
	else if (hKey == HKEY_CURRENT_USER)
	{
		strFullPath	= _T("HKEY_CURRENT_USER\\");
	}
	else if (hKey == HKEY_LOCAL_MACHINE)
	{
		strFullPath	= _T("HKEY_LOCAL_MACHINE\\");
	}
	else if (hKey == HKEY_USERS)
	{
		strFullPath	= _T("HKEY_USERS\\");
	}
	else
	{
		return FALSE;
	}

	strFullPath	+= lpszSubKey;

	return TRUE;
}

int CDualView::SearchKeys(HTREEITEM hParentItem, LPCTSTR lpszKey)
{
	CRegistryMFCX	reg;
	HKEY			hKey;
	CString			strRootKey;
	CString			strSubKey;
	TVINSERTSTRUCT	tvi;
	BOOL			bFind;
	int				nCount	= 0;

	BeginWaitCursor();

	if (!GetKeys(&hKey, strRootKey, lpszKey))
	{
		return nCount;
	}

	if ((hKey == NULL) && (strRootKey == _T("")))
	{
		HTREEITEM	hItem	= m_ctlKeyTree.GetChildItem(m_ctlKeyTree.GetRootItem());

		while (hItem != NULL)
		{
			nCount++;
			hItem	= m_ctlKeyTree.GetNextSiblingItem(hItem);
		}
	}
	else
	{
		reg.Open(hKey, strRootKey, FALSE);

		bFind	= reg.GetFirstSubKey(strSubKey);

		while (bFind)
		{
			tvi.hParent				= hParentItem;
			tvi.hInsertAfter		= TVI_SORT;
			tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
			tvi.item.pszText		= strSubKey.GetBuffer(strSubKey.GetLength());
			tvi.item.iImage			= 1;
			tvi.item.iSelectedImage	= 1;
			tvi.item.cChildren		= I_CHILDRENCALLBACK;
			tvi.item.lParam			= ITEM_NEW;

			m_ctlKeyTree.InsertItem(&tvi);
			strSubKey.ReleaseBuffer();

			nCount++;
			bFind	= reg.GetNextSubKey(strSubKey);
		}

		reg.Close();
	}

	EndWaitCursor();
	return nCount;
}

int CDualView::SearchKeysSpec(HTREEITEM hParentItem, LPCTSTR lpszKey, LPCTSTR lpszSpec, HTREEITEM &hSpecItem)
{
	CRegistryMFCX	reg;
	HKEY			hKey;
	CString			strRootKey;
	CString			strSubKey;
	TVINSERTSTRUCT	tvi;
	BOOL			bFind;
	int				nCount	= 0;
	HTREEITEM		hItem;

	BeginWaitCursor();

	if (!GetKeys(&hKey, strRootKey, lpszKey))
	{
		return nCount;
	}

	hSpecItem	= NULL;

	if ((hKey == NULL) && (strRootKey == _T("")))
	{
		hItem	= m_ctlKeyTree.GetChildItem(m_ctlKeyTree.GetRootItem());

		while (hItem != NULL)
		{
			nCount++;

			if (m_ctlKeyTree.GetItemText(hItem) == lpszSpec)
			{
				hSpecItem	= hItem;
			}

			hItem	= m_ctlKeyTree.GetNextSiblingItem(hItem);
		}
	}
	else
	{
		reg.Open(hKey, strRootKey, FALSE);

		bFind	= reg.GetFirstSubKey(strSubKey);

		while (bFind)
		{
			tvi.hParent				= hParentItem;
			tvi.hInsertAfter		= TVI_SORT;
			tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
			tvi.item.pszText		= strSubKey.GetBuffer(strSubKey.GetLength());
			tvi.item.iImage			= 1;
			tvi.item.iSelectedImage	= 1;
			tvi.item.cChildren		= I_CHILDRENCALLBACK;
			tvi.item.lParam			= ITEM_NEW;
			hItem					= m_ctlKeyTree.InsertItem(&tvi);

			strSubKey.ReleaseBuffer();

			nCount++;

			if (strSubKey == lpszSpec)
			{
				hSpecItem	= hItem;
			}

			bFind	= reg.GetNextSubKey(strSubKey);
		}

		reg.Close();
	}

	EndWaitCursor();
	return nCount;
}

void CDualView::SearchValues()
{
	CRegistryMFCX	reg;
	CString			strValueName;
	DWORD			dwType;
	BOOL			bFind;
	int				nCount;
	LVITEM			lvi;
	DWORD			dwStyle;
	BOOL			bDefaultExist;
	CString			strDefault;
	int				i;

	BeginWaitCursor();
	m_ctlValueList.SetRedraw(FALSE);
	m_ctlValueList.ModifyStyle(LVS_EDITLABELS, 0);

	if (!m_bNameEditor)
	{
		m_ctlValueList.ModifyStyle(0, LVS_EDITLABELS);
	}

	dwStyle	= m_ctlValueList.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT);

	if (m_bFullRowSelect)
	{
		dwStyle	|= LVS_EX_FULLROWSELECT;
	}

	m_ctlValueList.SetExtendedStyle(dwStyle);

	if (m_ctlValueList.GetItemCount() > 0)
	{
		m_ctlValueList.DeleteAllItems();
	}

	nCount	= 0;

	if (m_hRootKey != NULL)
	{
		strDefault.LoadString(IDS_DEFAULT);
		reg.Open(m_hRootKey, m_strSubKey, FALSE);

		bDefaultExist	= FALSE;
		bFind			= reg.GetFirstValue(strValueName, &dwType);

		while (bFind)
		{
			if (strValueName.CompareNoCase(strDefault) == 0)
			{
				strValueName.LoadString(IDS_DEFAULT_NAME);

				bDefaultExist	= TRUE;
			}

			for (i = 0; i < m_ctlValueList.GetItemCount(); i++)
			{
				if (strValueName.Compare(m_ctlValueList.GetItemText(i, 0)) < 0)
				{
					break;
				}
			}

			lvi.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem		= i;
			lvi.iSubItem	= 0;
			lvi.iImage		= I_IMAGECALLBACK;
			lvi.lParam		= dwType;
			lvi.pszText		= strValueName.GetBuffer(strValueName.GetLength());

			m_ctlValueList.InsertItem(&lvi);
			strValueName.ReleaseBuffer();

			if (m_bShowDataType)
			{
				m_ctlValueList.SetItemText(nCount, 1, LPSTR_TEXTCALLBACK);
				m_ctlValueList.SetItemText(nCount, 2, LPSTR_TEXTCALLBACK);
			}
			else
			{
				m_ctlValueList.SetItemText(nCount, 1, LPSTR_TEXTCALLBACK);
			}

			nCount++;
			bFind	= reg.GetNextValue(strValueName, &dwType);
		}

		reg.Close();

		if (bDefaultExist == FALSE)
		{
			strValueName.LoadString(IDS_DEFAULT_NAME);

			for (i = 0; i < m_ctlValueList.GetItemCount(); i++)
			{
				if (strValueName.Compare(m_ctlValueList.GetItemText(i, 0)) < 0)
				{
					break;
				}
			}

			lvi.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem		= i;
			lvi.iSubItem	= 0;
			lvi.iImage		= I_IMAGECALLBACK;
			lvi.lParam		= REG_SZ;
			lvi.pszText		= strValueName.GetBuffer(strValueName.GetLength());

			m_ctlValueList.InsertItem(&lvi);
			strValueName.ReleaseBuffer();

			if (m_bShowDataType)
			{
				m_ctlValueList.SetItemText(nCount, 1, LPSTR_TEXTCALLBACK);
				m_ctlValueList.SetItemText(nCount, 2, LPSTR_TEXTCALLBACK);
			}
			else
			{
				m_ctlValueList.SetItemText(nCount, 1, LPSTR_TEXTCALLBACK);
			}

			nCount++;
		}
	}

	if (m_bAutoWidth)
	{
		m_ctlValueList.SetAutoColumnWidth();
	}

	m_ctlValueList.SetRedraw(TRUE);
	EndWaitCursor();
}

BOOL CDualView::SearchDefaultKey(HTREEITEM hParentItem)
{
	CRegistryMFCX	reg;
	HKEY			hKey;
	CString			strRootKey;
	CString			strKey;
	CString			strParsed;
	HTREEITEM		hItem;
	BOOL			bFound	= FALSE;
	BOOL			bExist	= FALSE;

	if (!GetKeys(&hKey, strRootKey, m_strDefaultKey))
	{
		return bFound;
	}

	if ((hKey == NULL) && (m_strDefaultKey == _T("")))
	{
		ExpandSubKeys(hParentItem);
		m_ctlKeyTree.Select(hParentItem, TVGN_CARET);

		bFound	= TRUE;

		return bFound;
	}

	if (!reg.Open(hKey, strRootKey, FALSE))
	{
		return bFound;
	}

	strKey		= m_strDefaultKey;
	strParsed	= SearchNextKey(strKey);
	bExist		= (BOOL)(strParsed.GetLength() > 0);

	while (bExist)
	{
		TVITEM	item;

		item.mask	= TVIF_HANDLE | TVIF_PARAM;
		item.hItem	= hParentItem;

		CString	strFullKey	= SearchFullKeyPath(item.hItem);
		RECT	rc;

		if (SearchKeysSpec(item.hItem, strFullKey, strParsed, hItem))
		{
			item.lParam	= ITEM_KEY;
		}
		else
		{
			item.lParam	= ITEM_KEY_NOSUB;
		}

		m_ctlKeyTree.SetItem(&item);
		m_ctlKeyTree.GetItemRect(item.hItem, &rc, FALSE);
		m_ctlKeyTree.InvalidateRect(&rc);

		if (hItem == NULL)
		{
			bExist	= FALSE;
		}
		else
		{
			hParentItem	= hItem;
			strParsed	= SearchNextKey(strKey);

			if ((bExist = (BOOL)(strParsed.GetLength() > 0)) == FALSE)
			{
				m_ctlKeyTree.Select(hItem, TVGN_CARET);

				bFound	= TRUE;
			}
		}
	}

	m_ctlKeyTree.UpdateWindow();
	return bFound;
}

CString	CDualView::SearchNextKey(CString &strKey)
{
	CString strParsed	= strKey;
	int		nFind		= strParsed.Find(_T("\\"));

	if (nFind >= 0)
	{
		strKey		= strParsed.Mid(nFind + 1);
		strParsed	= strParsed.Left(nFind);
	}
	else
	{
		strKey	= _T("");
	}

	return strParsed;
}

CString CDualView::SearchFullKeyPath(HTREEITEM hItem, BOOL bIncludeSelf)
{
	CString	strFullKeyPath	= _T("");
	CString	strKey;

	if (hItem)
	{
		if (bIncludeSelf == FALSE)
		{
			hItem	= m_ctlKeyTree.GetParentItem(hItem);
		}
		while (hItem)
		{
			if (hItem == m_ctlKeyTree.GetRootItem())
			{
				strKey	= _T("");
			}
			else
			{
				strKey	= m_ctlKeyTree.GetItemText(hItem);
			}

			strFullKeyPath	= strKey + _T("\\") + strFullKeyPath;
			hItem			= m_ctlKeyTree.GetParentItem(hItem);
		}
	}
	else
	{
		strFullKeyPath	= _T("");
	}

	strFullKeyPath.Trim(_T('\\'));
	return strFullKeyPath;
}

void CDualView::ExpandSubKeys(HTREEITEM hItem, BOOL bRefresh)
{
	HTREEITEM	hChildItem;
	HTREEITEM	hNextItem;

	if (bRefresh == TRUE)
	{
		if (m_ctlKeyTree.GetItemData(hItem) != ITEM_NEW)
		{
			hChildItem	= m_ctlKeyTree.GetChildItem(hItem);

			if (hChildItem != NULL)
			{
				do
				{
					hNextItem	= m_ctlKeyTree.GetNextSiblingItem(hChildItem);

					m_ctlKeyTree.DeleteItem(hChildItem);

					hChildItem	= hNextItem;
				}
				while (hChildItem != NULL);
			}

			m_ctlKeyTree.SetItemData(hItem, ITEM_NEW);
		}

		if (m_ctlKeyTree.GetItemData(hItem) == ITEM_NEW)
		{
			TVITEM	item;

			item.mask	= TVIF_HANDLE | TVIF_PARAM;
			item.hItem	= hItem;

			CString	strFullKeyPath = SearchFullKeyPath(item.hItem);
			RECT	rc;

			if (SearchKeys(item.hItem, strFullKeyPath))
			{
				item.lParam	= ITEM_KEY;
			}
			else
			{
				item.lParam	= ITEM_KEY_NOSUB;
			}

			m_ctlKeyTree.SetItem(&item);
			m_ctlKeyTree.GetItemRect(item.hItem, &rc, FALSE);
			m_ctlKeyTree.InvalidateRect(&rc);
			m_ctlKeyTree.UpdateWindow();
		}
	}

	m_ctlKeyTree.Expand(hItem, TVE_EXPAND);
	m_ctlKeyTree.EnsureVisible(hItem);
}

void CDualView::GoParentKey()
{
	if (m_ctlKeyTree.GetSelectedItem() != m_ctlKeyTree.GetRootItem())
	{
		BeginWaitCursor();
		SetDefaultKey(SearchFullKeyPath(m_ctlKeyTree.GetParentItem(m_ctlKeyTree.GetSelectedItem())));
		BuildRoot();
		EndWaitCursor();
	}
}

void CDualView::ModeStandard()
{
	RECT			rc;
	TBBUTTONINFO	tbButtonInfo;
	CString			strButton;
	CRegistryMFCX	reg;

	if ((!::IsWindow(m_ctlKeyTree.m_hWnd)) || (!::IsWindow(m_ctlValueList.m_hWnd)))
	{
		return;
	}

	m_ctlKeyTree.ShowWindow(SW_SHOW);
	m_ctlKeyTree.MoveWindow(&m_rcKeyTree);
	m_ctlValueList.ShowWindow(SW_SHOW);
	m_ctlValueList.MoveWindow(&m_rcValueList);

	if (m_nShowMode == VIEW_KEYONLY)
	{
		SearchValues();
	}

	m_nShowMode	= VIEW_STANDARD;

	strButton.LoadString(IDS_SWITCH);
	memset(&tbButtonInfo, 0, sizeof(TBBUTTONINFO));

	tbButtonInfo.cbSize		= sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask		= TBIF_TEXT;
	tbButtonInfo.pszText	= strButton.GetBuffer();

	static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd)->m_wndCommandBar.SendMessage(TB_SETBUTTONINFO, ID_DUAL_SWITCH, (LPARAM)&tbButtonInfo);
	GetClientRect(&rc);
	InvalidateRect(&rc);
	UpdateWindow();
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_MODE, m_nShowMode);
	reg.Close();
}

void CDualView::ModeKeyOnly()
{
	RECT			rc;
	TBBUTTONINFO	tbButtonInfo;
	CString			strButton;
	CRegistryMFCX	reg;

	if ((!::IsWindow(m_ctlKeyTree.m_hWnd)) || (!::IsWindow(m_ctlValueList.m_hWnd)))
	{
		return;
	}

	m_ctlKeyTree.ShowWindow(SW_SHOW);
	m_ctlKeyTree.MoveWindow(m_rcKeyTree.left, m_rcKeyTree.top, m_rcValueList.right - m_rcKeyTree.left, m_rcValueList.bottom - m_rcKeyTree.top);
	m_ctlValueList.ShowWindow(SW_HIDE);

	m_nShowMode	= VIEW_KEYONLY;

	strButton.LoadString(IDS_UP);
	memset(&tbButtonInfo, 0, sizeof(TBBUTTONINFO));

	tbButtonInfo.cbSize		= sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask		= TBIF_TEXT;
	tbButtonInfo.pszText	= strButton.GetBuffer();

	static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd)->m_wndCommandBar.SendMessage(TB_SETBUTTONINFO, ID_DUAL_SWITCH, (LPARAM)&tbButtonInfo);
	GetClientRect(&rc);
	InvalidateRect(&rc);
	UpdateWindow();
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_MODE, m_nShowMode);
	reg.Close();
}

void CDualView::ModeValueOnly()
{
	RECT			rc;
	TBBUTTONINFO	tbButtonInfo;
	CString			strButton;
	CRegistryMFCX	reg;

	if ((!::IsWindow(m_ctlKeyTree.m_hWnd)) || (!::IsWindow(m_ctlValueList.m_hWnd)))
	{
		return;
	}

	m_ctlKeyTree.ShowWindow(SW_HIDE);
	m_ctlValueList.ShowWindow(SW_SHOW);
	m_ctlValueList.MoveWindow(m_rcKeyTree.left, m_rcKeyTree.top, m_rcValueList.right - m_rcKeyTree.left, m_rcValueList.bottom - m_rcKeyTree.top);

	if (m_nShowMode == VIEW_KEYONLY)
	{
		SearchValues();
	}

	m_nShowMode	= VIEW_VALUEONLY;

	strButton.LoadString(IDS_UP);
	memset(&tbButtonInfo, 0, sizeof(TBBUTTONINFO));

	tbButtonInfo.cbSize		= sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask		= TBIF_TEXT;
	tbButtonInfo.pszText	= strButton.GetBuffer();

	static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd)->m_wndCommandBar.SendMessage(TB_SETBUTTONINFO, ID_DUAL_SWITCH, (LPARAM)&tbButtonInfo);
	GetClientRect(&rc);
	InvalidateRect(&rc);
	UpdateWindow();
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_MODE, m_nShowMode);
	reg.Close();
}

void CDualView::EditKeyName(HTREEITEM hItem)
{
	if ((m_ctlKeyTree.GetRootItem() == hItem) || (m_ctlKeyTree.GetRootItem() == m_ctlKeyTree.GetParentItem(hItem)))
	{
		return;
	}
	else
	{
		CEditorName	dName;

		dName.SetHeader(IDS_KEY_RENAME);

		m_strEdit		= m_ctlKeyTree.GetItemText(hItem);
		dName.m_strName	= m_ctlKeyTree.GetItemText(hItem);

		if (dName.DoModal() == IDOK)
		{
			CRegistryMFCX	reg;
			HKEY			hOldKey;
			HKEY			hNewKey;
			CString			strOldSubKey;
			CString			strNewSubKey;
			CString			strOldKey;
			CString			strNewKey;
			CString			strValidName;

			if (!dName.m_strName.GetLength())
			{
				return;
			}

			strValidName	= dName.m_strName;

			if (!strValidName.Compare(m_strEdit))
			{
				return;
			}

			if (strValidName.Find(_T('\\')) != -1)
			{
				CString	strTitle;
				CString	strMessage;

				strTitle.LoadString(IDR_MAINFRAME);
				strMessage.LoadString(IDS_INVALID_KEY_NAME);

				strMessage	+= _T("\n\n");
				strMessage	+= _T("\\");

				MessageBox(strMessage, strTitle, MB_OK | MB_ICONWARNING);
				return;
			}

			strOldKey.Format(_T("%s\\%s"), SearchFullKeyPath(hItem, FALSE), m_strEdit);
			strNewKey.Format(_T("%s\\%s"), SearchFullKeyPath(hItem, FALSE), strValidName);
			GetKeys(&hOldKey, strOldSubKey, strOldKey);
			GetKeys(&hNewKey, strNewSubKey, strNewKey);
			reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
			reg.WriteString(RV_RG_CURRENTKEY, strNewKey);
			reg.Close();
			reg.Move(hOldKey, strOldSubKey, hNewKey, strNewSubKey);
			m_ctlKeyTree.SetItemText(hItem, strValidName);

			m_strEdit		= _T("");
			m_nKeyTimerID	= SetTimer(TID_KEY_REFRESH, 500, NULL);
		}
	}
}

void CDualView::EditValueName(int nItem)
{
	CString	strDefault;

	strDefault.LoadString(IDS_DEFAULT_NAME);

	if (!strDefault.CompareNoCase(m_ctlValueList.GetItemText(nItem, 0)))
	{
		return;
	}
	else
	{
		CEditorName	dName;

		dName.SetHeader(IDS_VALUE_RENAME);

		m_strEdit		= m_ctlValueList.GetItemText(nItem, 0);
		dName.m_strName	= m_ctlValueList.GetItemText(nItem, 0);

		if (dName.DoModal() == IDOK)
		{
			CRegistryMFCX	reg;
			CString			strValidName;
			CString			strOldKeyPath;
			CString			strNewPath;
			DWORD			dwType	= (DWORD)m_ctlValueList.GetItemData(nItem);

			if (!dName.m_strName.GetLength())
			{
				return;
			}

			strValidName	= dName.m_strName;

			if (!strValidName.Compare(m_strEdit))
			{
				return;
			}

			if (strValidName.Find(_T('\\')) != -1)
			{
				CString	strTitle;
				CString	strMessage;

				strTitle.LoadString(IDR_MAINFRAME);
				strMessage.LoadString(IDS_INVALID_VALUE_NAME);

				strMessage	+= _T("\n\n");
				strMessage	+= _T("\\");

				MessageBox(strMessage, strTitle, MB_OK | MB_ICONWARNING);

				return;
			}

			reg.Open(m_hRootKey, m_strSubKey);

			switch (dwType)
			{
			case REG_SZ:
				{
					CString	strValue;

					strValue	= reg.ReadString(m_strEdit);

					reg.WriteString(strValidName, strValue);
				}
				break;
			case REG_EXPAND_SZ:
				{
					CString	strValue;

					strValue	= reg.ReadExpandString(m_strEdit);

					reg.WriteExpandString(strValidName, strValue);
				}
				break;
			case REG_BINARY:
				{
					BYTE	cBuffer[4096];
					DWORD	dwSize	= 4096;

					reg.ReadBinary(m_strEdit, cBuffer, &dwSize);
					reg.WriteBinary(strValidName, cBuffer, dwSize);
				}
				break;
			case REG_DWORD:
				{
					DWORD	dwValue;

					dwValue	= reg.ReadDWORD(m_strEdit);

					reg.WriteDWORD(strValidName, dwValue);
				}
				break;
			case REG_MULTI_SZ:
				{
					CStringArray	arrValue;

					reg.ReadMultiString(m_strEdit, arrValue);
					reg.WriteMultiString(strValidName, arrValue);
				}
				break;
			}

			reg.DeleteValue(m_strEdit);
			reg.Close();
			m_ctlValueList.SetItemText(nItem, 0, strValidName);

			m_strEdit		= _T("");
			m_nValueTimerID	= SetTimer(TID_VAL_REFRESH, 500, NULL);
		}
	}
}

void CDualView::EditStringValue(CString strValue)
{
	CRegistryMFCX	reg;
	CEditorSZ		dSZ;
	CString			strTitle;
	BOOL			bExist	= FALSE;

	reg.Open(m_hRootKey, m_strSubKey);
	strTitle.LoadString(IDS_EDITOR_SZ_TITLE);

	strTitle	+= _T(": ");
	strTitle	+= strValue;

	dSZ.SetHeader(strTitle);

	bExist	= reg.IsValueExist(strValue);

	dSZ.m_strData	= reg.ReadString(strValue);

	if (dSZ.DoModal() == IDOK)
	{
		reg.WriteString(strValue, dSZ.m_strData);
		SearchValues();
	}
	else
	{
		if (!bExist)
		{
			reg.DeleteValue(strValue);
		}
	}

	reg.Close();
}

void CDualView::EditBinaryValue(CString strValue)
{
	CRegistryMFCX	reg;
	CEditorBinary	dBinary;
	CString			strTitle;

	reg.Open(m_hRootKey, m_strSubKey);
	strTitle.LoadString(IDS_EDITOR_BINARY_TITLE);

	strTitle	+= _T(": ");
	strTitle	+= strValue;

	dBinary.SetHeader(strTitle);

	dBinary.m_cbSize	= 4096;

	reg.ReadBinary(strValue, dBinary.m_cBuffer, (LPDWORD)&dBinary.m_cbSize);

	if (dBinary.DoModal() == IDOK)
	{
		reg.WriteBinary(strValue, dBinary.m_cBuffer, dBinary.m_cbSize);
		SearchValues();
	}

	reg.Close();
}

void CDualView::EditDWORDValue(CString strValue)
{
	CRegistryMFCX	reg;
	CEditorDWORD	dDWORD;
	CString			strTitle;

	reg.Open(m_hRootKey, m_strSubKey);
	strTitle.LoadString(IDS_EDITOR_DWORD_TITLE);

	strTitle	+= _T(": ");
	strTitle	+= strValue;

	dDWORD.SetHeader(strTitle);

	dDWORD.m_nBase	= 0;
	dDWORD.m_dwData	= reg.ReadDWORD(strValue);

	if (dDWORD.DoModal() == IDOK)
	{
		reg.WriteDWORD(strValue, dDWORD.m_dwData);
		SearchValues();
	}

	reg.Close();
}

void CDualView::EditMultiStringValue(CString strValue)
{
	CRegistryMFCX	reg;
	CEditorMultiSZ	dMSZ;
	CStringArray	arrValue;
	CString			strTitle;

	reg.Open(m_hRootKey, m_strSubKey);
	strTitle.LoadString(IDS_EDITOR_MULTI_SZ_TITLE);

	strTitle	+= _T(": ");
	strTitle	+= strValue;

	dMSZ.SetHeader(strTitle);
	reg.ReadMultiString(strValue, arrValue);
	dMSZ.m_strData.Empty();

	for (int i = 0; i < arrValue.GetSize(); i++)
	{
		dMSZ.m_strData	+= arrValue.GetAt(i);

		if (i < (arrValue.GetSize() - 1))
		{
			dMSZ.m_strData	+= _T("\r\n");
		}
	}

	if (dMSZ.DoModal() == IDOK)
	{
		int	nPos;

		arrValue.RemoveAll();

		do
		{
			nPos	= dMSZ.m_strData.Find(_T("\r\n"));

			if (nPos != -1)
			{
				arrValue.Add(dMSZ.m_strData.Left(nPos));

				dMSZ.m_strData	= dMSZ.m_strData.Mid(nPos + 2);
			}
			else
			{
				arrValue.Add(dMSZ.m_strData);
				dMSZ.m_strData.Empty();
			}
		} while (dMSZ.m_strData.GetLength() > 0);

		reg.WriteMultiString(strValue, arrValue);
		SearchValues();
	}

	reg.Close();
}

void CDualView::EditExpandStringValue(CString strValue)
{
	CRegistryMFCX	reg;
	CEditorSZ		dSZ;
	CString			strTitle;

	reg.Open(m_hRootKey, m_strSubKey);
	strTitle.LoadString(IDS_EDITOR_SZ_TITLE);

	strTitle	+= _T(": ");
	strTitle	+= strValue;

	dSZ.SetHeader(strTitle);

	dSZ.m_strData	= reg.ReadExpandString(strValue);

	if (dSZ.DoModal() == IDOK)
	{
		reg.WriteExpandString(strValue, dSZ.m_strData);
		SearchValues();
	}

	reg.Close();
}

BOOL CDualView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style	&= ~WS_BORDER;

	return CFormView::PreCreateWindow(cs);
}

void CDualView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	UINT	uiValue	= 0;
	DWORD	dwDPI	= GetDPI();
	HICON	hIcon;

	switch (dwDPI)
	{
	case DPI_192:
		{
			uiValue	= IDB_VALUEITEMS32;
		}
		break;
	case DPI_128:
		{
			uiValue	= IDB_VALUEITEMS21;
		}
		break;
	default:
		{
			uiValue	= IDB_VALUEITEMS16;
		}
		break;
	}

	m_iKeyImages.Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR8 | ILC_MASK, 2, 1);

	hIcon	= GetShellIcon(SHELLICON_DEVICE, ICON_SMALL);

	m_iKeyImages.Add(hIcon);
	DestroyIcon(hIcon);

	hIcon	= GetFileIcon(_T("\\"), ICON_SMALL);

	m_iKeyImages.Add(hIcon);
	DestroyIcon(hIcon);
	m_ctlKeyTree.SetImageList(&m_iKeyImages, TVSIL_NORMAL);
	m_iValueImages.Create(uiValue, GetSystemMetrics(SM_CXSMICON), 2, RGB(0xFA, 0xFA, 0xFA));
	m_ctlValueList.SetImageList(&m_iValueImages, LVSIL_SMALL);

	CRegistryMFCX	reg;

	LoadLayoutOptions();
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	SetDefaultKey(reg.ReadString(RV_RG_CURRENTKEY));
	reg.Close();
	RefreshLayout(TRUE);
	BuildRoot();
	m_ctlKeyTree.SetFocus();
}

void CDualView::OnPaint()
{
	CPaintDC	dc(this);

	if (m_nShowMode == VIEW_STANDARD)
	{
		RECT	rc;
		RECT	rcSplit;

		GetClientRect(&rc);

		if (m_bVertical == TRUE)
		{
			dc.MoveTo(rc.left, m_nSplit);
			dc.LineTo(rc.right, m_nSplit);
			dc.MoveTo(rc.left, m_nSplit + m_nSplitDistance);
			dc.LineTo(rc.right, m_nSplit + m_nSplitDistance);
			SetRect(&rcSplit, rc.left, m_nSplit + 1, rc.right, m_nSplit + m_nSplitDistance);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));
		}
		else
		{
			dc.MoveTo(m_nSplit, rc.top);
			dc.LineTo(m_nSplit, rc.bottom);
			dc.MoveTo(m_nSplit + m_nSplitDistance, rc.top);
			dc.LineTo(m_nSplit + m_nSplitDistance, rc.bottom);
			SetRect(&rcSplit, m_nSplit + 1, rc.top, m_nSplit + m_nSplitDistance, rc.bottom);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));
		}
	}

	CFormView::OnPaint();
}

void CDualView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nShowMode == VIEW_STANDARD)
	{
		SetCapture();

		m_bTracking			= TRUE;
		m_nTrackDistance	= ((m_bVertical == TRUE) ? point.y : point.x) - m_nSplit;
		m_nTracking			= m_nSplit;
	}

	Default();
}

void CDualView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_nShowMode == VIEW_STANDARD)
	{
		ReleaseCapture();

		m_bTracking	= FALSE;

		InvalidateRect(NULL);
		UpdateWindow();
	}

	CFormView::OnLButtonUp(nFlags, point);
}

void CDualView::OnMouseMove(UINT nFlags, CPoint point)
{
	RECT	rc;

	GetClientRect(&rc);

	if ((m_nShowMode == VIEW_STANDARD) && (m_bTracking == TRUE))
	{
		m_nTracking	= ((m_bVertical == TRUE) ? point.y : point.x) - m_nTracking;
		m_nSplit	= ((m_bVertical == TRUE) ? point.y : point.x) - m_nTrackDistance;

		if (m_bVertical == TRUE)
		{
			if (m_nSplit < 0)
			{
				m_nSplit	= 0;
			}
			if (m_nSplit >= rc.bottom - m_nSplitDistance)
			{
				m_nSplit	= rc.bottom - m_nSplitDistance - DRA::SCALEY(1);
			}

			m_rcKeyTree.bottom	= m_nSplit;
			m_rcValueList.top	= m_nSplit + m_nSplitDistance + DRA::SCALEY(1);
		}
		else
		{
			if (m_nSplit < 0)
			{
				m_nSplit	= 0;
			}
			if (m_nSplit >= rc.right - m_nSplitDistance)
			{
				m_nSplit	= rc.right - m_nSplitDistance - DRA::SCALEY(1);
			}

			m_rcKeyTree.right	= m_nSplit;
			m_rcValueList.left	= m_nSplit + m_nSplitDistance + DRA::SCALEY(1);
		}

		m_ctlKeyTree.MoveWindow(&m_rcKeyTree);
		m_ctlValueList.MoveWindow(&m_rcValueList);
		InvalidateRect(NULL);
		UpdateWindow();
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CDualView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	m_dwLastResizeTicks	= GetTickCount();

	if (m_nResizeTimerID == 0)
	{
		m_nResizeTimerID	= SetTimer(TID_RESIZE, MINIMUM_RESIZE, NULL);
	}
}

void CDualView::OnTvnSelchangedKeytree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView	= reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM		hItem		= pNMTreeView->itemNew.hItem;

	if (hItem)
	{
		CRegistryMFCX	reg;
		CString			strKey	= SearchFullKeyPath(hItem);

		reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
		reg.WriteString(RV_RG_CURRENTKEY, strKey);
		reg.Close();
		GetKeys(&m_hRootKey, m_strSubKey, strKey);

		if ((pNMTreeView->action) && (m_nShowMode != VIEW_KEYONLY))
		{
			SearchValues();
		}
	}

	*pResult	= 0;
}

void CDualView::OnTvnItemexpandedKeytree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView	= reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM		hItem		= pNMTreeView->itemNew.hItem;

	if ((pNMTreeView->action & TVE_COLLAPSE) && (hItem))
	{
		CRegistryMFCX	reg;
		CString			strKey	= SearchFullKeyPath(hItem);

		reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
		reg.WriteString(RV_RG_CURRENTKEY, strKey);
		reg.Close();
		GetKeys(&m_hRootKey, m_strSubKey, strKey);

		if (m_nShowMode != VIEW_KEYONLY)
		{
			SearchValues();
		}
	}

	*pResult	= 0;
}

void CDualView::OnTvnItemexpandingKeytree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView	= reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (pNMTreeView->action & TVE_EXPAND)
	{
		if (pNMTreeView->itemNew.lParam == ITEM_NEW)
		{
			TVITEM	item;

			item.mask	= TVIF_HANDLE | TVIF_PARAM;
			item.hItem	= pNMTreeView->itemNew.hItem;

			CString	strFullKeyPath	= SearchFullKeyPath(item.hItem);
			RECT	rc;

			if (SearchKeys(item.hItem, strFullKeyPath))
			{
				item.lParam	= ITEM_KEY;
			}
			else
			{
				item.lParam	= ITEM_KEY_NOSUB;
			}

			m_ctlKeyTree.SetItem(&item);
			m_ctlKeyTree.GetItemRect(item.hItem, &rc, FALSE);
			m_ctlKeyTree.InvalidateRect(&rc);
			m_ctlKeyTree.UpdateWindow();
		}
	}

	*pResult	= 0;
}

void CDualView::OnTvnGetdispinfoKeytree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if (pTVDispInfo->item.mask & TVIF_CHILDREN)
	{
		if ((pTVDispInfo->item.lParam == ITEM_NEW) || (pTVDispInfo->item.lParam == ITEM_KEY))
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

void CDualView::OnTvnBeginlabeleditKeytree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if ((m_ctlKeyTree.GetRootItem() == pTVDispInfo->item.hItem) || (m_ctlKeyTree.GetRootItem() == m_ctlKeyTree.GetParentItem(pTVDispInfo->item.hItem)))
	{
		*pResult	= 1;
	}
	else
	{
		m_strEdit	= m_ctlKeyTree.GetItemText(pTVDispInfo->item.hItem);

		::SHSipPreference(this->GetSafeHwnd(), SIP_UP);
		m_ctlKeyTree.EnsureVisible(pTVDispInfo->item.hItem);

		*pResult	= 0;
	}
}

void CDualView::OnTvnEndlabeleditKeytree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	CRegistryMFCX	reg;
	HKEY			hOldKey;
	HKEY			hNewKey;
	CString			strOldSubKey;
	CString			strNewSubKey;
	CString			strOldKey;
	CString			strNewKey;

	::SHSipPreference(this->GetSafeHwnd(), SIP_DOWN);

	if (!pTVDispInfo->item.pszText)
	{
		*pResult	= 0;

		return;
	}

	CString	strValidName	= pTVDispInfo->item.pszText;

	if (!strValidName.Compare(m_strEdit))
	{
		*pResult	= 0;

		return;
	}

	if (strValidName.Find(_T('\\')) != -1)
	{
		CString	strTitle;
		CString	strMessage;

		strTitle.LoadString(IDR_MAINFRAME);
		strMessage.LoadString(IDS_INVALID_KEY_NAME);

		strMessage	+= _T("\n\n");
		strMessage	+= _T("\\");

		MessageBox(strMessage, strTitle, MB_OK | MB_ICONWARNING);

		*pResult	= 0;

		return;
	}

	strOldKey.Format(_T("%s\\%s"), SearchFullKeyPath(pTVDispInfo->item.hItem, FALSE), m_strEdit);
	strNewKey.Format(_T("%s\\%s"), SearchFullKeyPath(pTVDispInfo->item.hItem, FALSE), strValidName);
	GetKeys(&hOldKey, strOldSubKey, strOldKey);
	GetKeys(&hNewKey, strNewSubKey, strNewKey);
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteString(RV_RG_CURRENTKEY, strNewKey);
	reg.Close();

	*pResult		= reg.Move(hOldKey, strOldSubKey, hNewKey, strNewSubKey);
	m_strEdit		= _T("");
	m_nKeyTimerID	= SetTimer(TID_KEY_REFRESH, 500, NULL);
}

LRESULT CDualView::OnTapAndHoldKeytree(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM	hItem	= (HTREEITEM)wParam;

	if (hItem != NULL)
	{
		CRegistryMFCX	reg;
		CString			strKey	= SearchFullKeyPath(hItem);
		UINT			uFlags;

		reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
		reg.WriteString(RV_RG_CURRENTKEY, strKey);
		reg.Close();
		GetKeys(&m_hRootKey, m_strSubKey, strKey);

		if (m_nShowMode != VIEW_KEYONLY)
		{
			SearchValues();
		}

		m_ctlKeyTree.Select(hItem, TVGN_CARET);

		CString	strItem;
		CMenu	ctlMenu;
		CMenu	ctlNewMenu;

		uFlags	= ((m_ctlKeyTree.GetRootItem() == hItem) ? MF_GRAYED : MF_ENABLED);

		ctlNewMenu.CreatePopupMenu();
		strItem.LoadString(IDS_DUAL_NEW_KEY);
		ctlNewMenu.AppendMenu(uFlags, ID_DUAL_NEW_KEY, strItem);
		ctlNewMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_DUAL_NEW_STRING);
		ctlNewMenu.AppendMenu(uFlags, ID_DUAL_NEW_STRING, strItem);
		strItem.LoadString(IDS_DUAL_NEW_BINARY);
		ctlNewMenu.AppendMenu(uFlags, ID_DUAL_NEW_BINARY, strItem);
		strItem.LoadString(IDS_DUAL_NEW_DWORD);
		ctlNewMenu.AppendMenu(uFlags, ID_DUAL_NEW_DWORD, strItem);
		strItem.LoadString(IDS_DUAL_NEW_MSTRING);
		ctlNewMenu.AppendMenu(uFlags, ID_DUAL_NEW_MSTRING, strItem);
		strItem.LoadString(IDS_DUAL_NEW_ESTRING);
		ctlNewMenu.AppendMenu(uFlags, ID_DUAL_NEW_ESTRING, strItem);

		uFlags	|= MF_POPUP;

		ctlMenu.CreatePopupMenu();

		if ((m_ctlKeyTree.GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) != TVIS_EXPANDED)
		{
			strItem.LoadString(IDS_KEY_EXPAND);
			ctlMenu.AppendMenu(((m_ctlKeyTree.GetItemData(hItem) != ITEM_KEY_NOSUB) ? MF_ENABLED : MF_GRAYED), ID_KEY_EXPAND, strItem);
		}
		else
		{
			strItem.LoadString(IDS_KEY_COLLAPSE);
			ctlMenu.AppendMenu(MF_ENABLED, ID_KEY_COLLAPSE, strItem);
		}

		strItem.LoadString(IDS_DUAL_NEW);
		ctlMenu.AppendMenu(uFlags, (UINT_PTR)ctlNewMenu.GetSafeHmenu(), strItem);
		strItem.LoadString(IDS_DUAL_FIND);
		ctlMenu.AppendMenu(MF_ENABLED, ID_DUAL_FIND, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);

		uFlags	= (((m_ctlKeyTree.GetRootItem() == hItem) || (m_ctlKeyTree.GetRootItem() == m_ctlKeyTree.GetParentItem(hItem))) ? MF_GRAYED : MF_ENABLED);

		strItem.LoadString(IDS_KEY_DELETE);
		ctlMenu.AppendMenu(uFlags, ID_KEY_DELETE, strItem);
		strItem.LoadString(IDS_KEY_RENAME);
		ctlMenu.AppendMenu(uFlags, ID_KEY_RENAME, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_DUAL_EXPORT);
		ctlMenu.AppendMenu(MF_ENABLED, ID_DUAL_EXPORT, strItem);
		strItem.LoadString(IDS_KEY_COPYNAME);
		ctlMenu.AppendMenu(MF_ENABLED, ID_KEY_COPYNAME, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_DUAL_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_DUAL_REFRESH, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}

	return 0;
}

void CDualView::OnLvnGetdispinfoValuelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO*	pDispInfo		= reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	CString			strValueName	= m_ctlValueList.GetItemText(pDispInfo->item.iItem, 0);
	DWORD			dwType			= m_ctlValueList.GetItemData(pDispInfo->item.iItem);
	CRegistryMFCX	reg;

	reg.Open(m_hRootKey, m_strSubKey);

	if (pDispInfo->item.mask & LVIF_IMAGE)
	{
		switch (dwType)
		{
		case REG_SZ:
			{
				pDispInfo->item.iImage	= 0;
			}
			break;
		case REG_EXPAND_SZ:
			{
				pDispInfo->item.iImage	= 0;
			}
			break;
		case REG_BINARY:
			{
				pDispInfo->item.iImage	= 1;
			}
			break;
		case REG_DWORD:
			{
				pDispInfo->item.iImage	= 1;
			}
			break;
		case REG_MULTI_SZ:
			{
				pDispInfo->item.iImage	= 0;
			}
			break;
		default:
			{
				pDispInfo->item.iImage	= 1;
			}
			break;
		}
	}

	// NOTE: pDispInfo->item.pszText should not be exceeded 256 characters

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CString	strData;

		switch (pDispInfo->item.iSubItem)
		{
		case 1:
			{
				if (m_bShowDataType)
				{
					switch (dwType)
					{
					case REG_SZ:
						{
							_tcscpy(pDispInfo->item.pszText, _T("REG_SZ"));
						}
						break;
					case REG_EXPAND_SZ:
						{
							_tcscpy(pDispInfo->item.pszText, _T("REG_EXPAND_SZ"));
						}
						break;
					case REG_BINARY:
						{
							_tcscpy(pDispInfo->item.pszText, _T("REG_BINARY"));
						}
						break;
					case REG_DWORD:
						{
							_tcscpy(pDispInfo->item.pszText, _T("REG_DWORD"));
						}
						break;
					case REG_MULTI_SZ:
						{
							_tcscpy(pDispInfo->item.pszText, _T("REG_MULTI_SZ"));
						}
						break;
					}
				}
				else
				{
					switch (dwType)
					{
					case REG_SZ:
						{
							CString	strDefault;

							strDefault.LoadString(IDS_DEFAULT_NAME);

							if (strValueName.CompareNoCase(strDefault) == 0)
							{
								strValueName.LoadString(IDS_DEFAULT);

								if (reg.IsValueExist(strValueName))
								{
									strData	= reg.ReadString(strValueName);

									if (strData.GetLength() >= 256)
									{
										strData	= strData.Left(252);
										strData	+= _T("...");
									}

									_tcscpy(pDispInfo->item.pszText, strData);
								}
								else
								{
									strData.LoadString(IDS_VALUE_NOT_SET);
									_tcscpy(pDispInfo->item.pszText, strData);
								}
							}
							else
							{
								strData	= reg.ReadString(strValueName);

								if (strData.GetLength() >= 256)
								{
									strData	= strData.Left(252);
									strData	+= _T("...");
								}

								_tcscpy(pDispInfo->item.pszText, strData);
							}
						}
						break;
					case REG_EXPAND_SZ:
						{
							strData	= reg.ReadExpandString(strValueName);

							if (strData.GetLength() >= 256)
							{
								strData	= strData.Left(252);
								strData	+= _T("...");
							}

							_tcscpy(pDispInfo->item.pszText, strData);
						}
						break;
					case REG_BINARY:
						{
							CString	strFormat;
							BYTE	cBuffer[4096];
							DWORD	dwSize	= 4096;

							reg.ReadBinary(strValueName, cBuffer, &dwSize);
							strData.Empty();

							for (DWORD i = 0; i < dwSize; i++)
							{
								strFormat.Format(_T("%02X"), cBuffer[i]);

								strData	+= strFormat;

								if (i < (dwSize - 1))
								{
									strData	+= _T(" ");
								}
							}

							if (strData.GetLength() >= 256)
							{
								strData	= strData.Left(252);
								strData	+= _T("...");
							}

							_tcscpy(pDispInfo->item.pszText, strData);
						}
						break;
					case REG_DWORD:
						{
							wsprintf(pDispInfo->item.pszText, _T("0x%08X (%d)"), reg.ReadDWORD(strValueName), reg.ReadDWORD(strValueName));
						}
						break;
					case REG_MULTI_SZ:
						{
							CStringArray	arrData;

							reg.ReadMultiString(strValueName, arrData);
							strData.Empty();

							for (int i = 0; i < arrData.GetSize(); i++)
							{
								strData	+= arrData.GetAt(i);

								if (i < (arrData.GetSize() - 1))
								{
									strData	+= _T(" ");
								}
							}

							if (strData.GetLength() >= 256)
							{
								strData	= strData.Left(252);
								strData	+= _T("...");
							}

							_tcscpy(pDispInfo->item.pszText, strData);
						}
						break;
					}
				}
			}
			break;
		case 2:
			{
				switch (dwType)
				{
				case REG_SZ:
					{
						CString	strDefault;

						strDefault.LoadString(IDS_DEFAULT_NAME);

						if (strValueName.CompareNoCase(strDefault) == 0)
						{
							strValueName.LoadString(IDS_DEFAULT);

							if (reg.IsValueExist(strValueName))
							{
								strData	= reg.ReadString(strValueName);

								if (strData.GetLength() >= 256)
								{
									strData	= strData.Left(252);
									strData	+= _T("...");
								}

								_tcscpy(pDispInfo->item.pszText, strData);
							}
							else
							{
								strData.LoadString(IDS_VALUE_NOT_SET);
								_tcscpy(pDispInfo->item.pszText, strData);
							}
						}
						else
						{
							strData	= reg.ReadString(strValueName);

							if (strData.GetLength() >= 256)
							{
								strData	= strData.Left(252);
								strData	+= _T("...");
							}

							_tcscpy(pDispInfo->item.pszText, strData);
						}
					}
					break;
				case REG_EXPAND_SZ:
					{
						strData	= reg.ReadExpandString(strValueName);

						if (strData.GetLength() >= 256)
						{
							strData	= strData.Left(252);
							strData	+= _T("...");
						}

						_tcscpy(pDispInfo->item.pszText, strData);
					}
					break;
				case REG_BINARY:
					{
						CString	strFormat;
						BYTE	cBuffer[4096];
						DWORD	dwSize	= 4096;

						reg.ReadBinary(strValueName, cBuffer, &dwSize);
						strData.Empty();

						for (DWORD i = 0; i < dwSize; i++)
						{
							strFormat.Format(_T("%02X"), cBuffer[i]);

							strData	+= strFormat;

							if (i < (dwSize - 1))
							{
								strData	+= _T(" ");
							}
						}

						if (strData.GetLength() >= 256)
						{
							strData	= strData.Left(252);
							strData	+= _T("...");
						}

						_tcscpy(pDispInfo->item.pszText, strData);
					}
					break;
				case REG_DWORD:
					{
						wsprintf(pDispInfo->item.pszText, _T("0x%08X (%d)"), reg.ReadDWORD(strValueName), reg.ReadDWORD(strValueName));
					}
					break;
				case REG_MULTI_SZ:
					{
						CStringArray	arrData;

						reg.ReadMultiString(strValueName, arrData);
						strData.Empty();

						for (int i = 0; i < arrData.GetSize(); i++)
						{
							strData	+= arrData.GetAt(i);

							if (i < (arrData.GetSize() - 1))
							{
								strData	+= _T(" ");
							}
						}

						if (strData.GetLength() >= 256)
						{
							strData	= strData.Left(252);
							strData	+= _T("...");
						}

						_tcscpy(pDispInfo->item.pszText, strData);
					}
					break;
				}
			}
			break;
		}
	}

	*pResult	= 0;
}

void CDualView::OnLvnBeginlabeleditValuelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO*	pDispInfo	= reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	CString			strDefault;

	strDefault.LoadString(IDS_DEFAULT_NAME);

	m_strEdit	= m_ctlValueList.GetItemText(pDispInfo->item.iItem, 0);

	if (!strDefault.CompareNoCase(m_strEdit))
	{
		*pResult	= 1;
	}
	else
	{
		::SHSipPreference(this->GetSafeHwnd(), SIP_UP);
		m_ctlValueList.EnsureVisible(pDispInfo->item.iItem, FALSE);

		*pResult	= 0;
	}
}

void CDualView::OnLvnEndlabeleditValuelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO*	pDispInfo	= reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	CRegistryMFCX	reg;
	DWORD			dwType	= (DWORD)pDispInfo->item.lParam;
	CString			strOldKeyPath;
	CString			strNewPath;

	::SHSipPreference(this->GetSafeHwnd(), SIP_DOWN);

	if (!pDispInfo->item.pszText)
	{
		*pResult	= 0;

		return;
	}

	CString	strValidName	= pDispInfo->item.pszText;

	if (!strValidName.Compare(m_strEdit))
	{
		*pResult	= 0;

		return;
	}

	if (strValidName.Find(_T('\\')) != -1)
	{
		CString	strTitle;
		CString	strMessage;

		strTitle.LoadString(IDR_MAINFRAME);
		strMessage.LoadString(IDS_INVALID_VALUE_NAME);

		strMessage	+= _T("\n\n");
		strMessage	+= _T("\\");

		MessageBox(strMessage, strTitle, MB_OK | MB_ICONWARNING);

		*pResult	= 0;

		return;
	}

	reg.Open(m_hRootKey, m_strSubKey);

	switch (dwType)
	{
	case REG_SZ:
		{
			CString	strValue;

			strValue	= reg.ReadString(m_strEdit);

			reg.WriteString(strValidName, strValue);
		}
		break;
	case REG_EXPAND_SZ:
		{
			CString	strValue;

			strValue	= reg.ReadExpandString(m_strEdit);

			reg.WriteExpandString(strValidName, strValue);
		}
		break;
	case REG_BINARY:
		{
			BYTE	cBuffer[4096];
			DWORD	dwSize	= 4096;

			reg.ReadBinary(m_strEdit, cBuffer, &dwSize);
			reg.WriteBinary(strValidName, cBuffer, dwSize);
		}
		break;
	case REG_DWORD:
		{
			DWORD	dwValue;

			dwValue	= reg.ReadDWORD(m_strEdit);

			reg.WriteDWORD(strValidName, dwValue);
		}
		break;
	case REG_MULTI_SZ:
		{
			CStringArray	arrValue;

			reg.ReadMultiString(m_strEdit, arrValue);
			reg.WriteMultiString(strValidName, arrValue);
		}
		break;
	}

	reg.DeleteValue(m_strEdit);
	reg.Close();

	*pResult		= 1;
	m_strEdit		= _T("");
	m_nValueTimerID	= SetTimer(TID_VAL_REFRESH, 500, NULL);
}

void CDualView::OnLvnKeydownValuelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVKEYDOWN	pLVKeyDown	= reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	if (pLVKeyDown->wVKey == VK_RETURN)
	{
		RECT	rc;
		int		nItem	= m_ctlValueList.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);

		if (nItem != -1)
		{
			m_ctlValueList.GetItemRect(nItem, &rc, LVIR_BOUNDS);
			m_ctlValueList.ClientToScreen(&rc);

			LPARAM	lParam	= MAKELPARAM((rc.left + ((rc.right - rc.left) / 4)), (rc.top + ((rc.bottom - rc.top) / 2)));
			WPARAM	wParam	= (WPARAM)nItem;

			SendMessage(WM_LCX_TAPANDHOLD, wParam, lParam);
		}
	}

	*pResult = 0;
}

void CDualView::OnNMDblclkValuelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	SendMessage(WM_COMMAND, MAKEWPARAM(ID_VALUE_MODIFY, 0), NULL);

	*pResult	= 0;
}

void CDualView::OnNMSetfocusValuelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	if ((m_ctlValueList.GetItemCount() > 0) && (m_ctlValueList.GetSelectedCount() == 0))
	{
		m_ctlValueList.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}

	*pResult	= 0;
}

LRESULT CDualView::OnTapAndHoldValuelist(WPARAM wParam, LPARAM lParam)
{
	int		nItem	= wParam;
	CString	strItem;
	CMenu	ctlMenu;

	if (nItem == -1)
	{
		CMenu	ctlNewMenu;

		ctlNewMenu.CreatePopupMenu();
		strItem.LoadString(IDS_DUAL_NEW_KEY);
		ctlNewMenu.AppendMenu(MF_ENABLED, ID_DUAL_NEW_KEY, strItem);
		ctlNewMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_DUAL_NEW_STRING);
		ctlNewMenu.AppendMenu(MF_ENABLED, ID_DUAL_NEW_STRING, strItem);
		strItem.LoadString(IDS_DUAL_NEW_BINARY);
		ctlNewMenu.AppendMenu(MF_ENABLED, ID_DUAL_NEW_BINARY, strItem);
		strItem.LoadString(IDS_DUAL_NEW_DWORD);
		ctlNewMenu.AppendMenu(MF_ENABLED, ID_DUAL_NEW_DWORD, strItem);
		strItem.LoadString(IDS_DUAL_NEW_MSTRING);
		ctlNewMenu.AppendMenu(MF_ENABLED, ID_DUAL_NEW_MSTRING, strItem);
		strItem.LoadString(IDS_DUAL_NEW_ESTRING);
		ctlNewMenu.AppendMenu(MF_ENABLED, ID_DUAL_NEW_ESTRING, strItem);
		ctlMenu.CreatePopupMenu();
		strItem.LoadString(IDS_DUAL_NEW);
		ctlMenu.AppendMenu(MF_ENABLED | MF_POPUP, (UINT_PTR)ctlNewMenu.GetSafeHmenu(), strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_DUAL_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_DUAL_REFRESH, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}
	else
	{
		UINT	uFlags;
		CString	strDefault;

		strDefault.LoadString(IDS_DEFAULT_NAME);

		uFlags	= ((m_ctlValueList.GetSelectedCount() > 1) ? MF_GRAYED : MF_ENABLED);

		ctlMenu.CreatePopupMenu();
		strItem.LoadString(IDS_VALUE_MODIFY);
		ctlMenu.AppendMenu(uFlags, ID_VALUE_MODIFY, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_VALUE_DELETE);
		ctlMenu.AppendMenu(MF_ENABLED, ID_VALUE_DELETE, strItem);

		uFlags	= (((m_ctlValueList.GetSelectedCount() > 1) || (strDefault.CompareNoCase(m_ctlValueList.GetItemText(nItem, 0)) == 0)) ? MF_GRAYED : MF_ENABLED);

		strItem.LoadString(IDS_VALUE_RENAME);
		ctlMenu.AppendMenu(uFlags, ID_VALUE_RENAME, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_DUAL_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_DUAL_REFRESH, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}

	return 0;
}

void CDualView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != 0)
	{
		if (nIDEvent == m_nKeyTimerID)
		{
			Refresh(FALSE);
			KillTimer(m_nKeyTimerID);

			m_nKeyTimerID	= 0;
		}

		if (nIDEvent == m_nValueTimerID)
		{
			Refresh(FALSE);
			KillTimer(m_nValueTimerID);

			m_nValueTimerID	= 0;
		}

		if (nIDEvent == m_nResizeTimerID)
		{
			DWORD	dwTicks	= GetTickCount() - m_dwLastResizeTicks;

			if (dwTicks > MINIMUM_RESIZE)
			{
				KillTimer(m_nResizeTimerID);
				RefreshLayout(FALSE);

				m_nResizeTimerID	= 0;
			}
		}
	}

	CFormView::OnTimer(nIDEvent);
}

void CDualView::OnDualImport()
{
	CFileDialogX	dImport;

	dImport.SetHeader(IDS_IMPORT_SELECT);
	dImport.SetFilters(_T("*.reg"));

	if (dImport.DoModal() == IDOK)
	{
		CRegistryX	reg;
		CString		strTitle;
		CString		strMessage;
		DWORD		dwType;

		strTitle.LoadString(IDR_MAINFRAME);

		if (reg.REG5Import(dImport.GetFullPath()))
		{
			strMessage.Format(IDS_IMPORT_SUCCESS, dImport.GetFullPath());

			dwType	= MB_OK | MB_ICONINFORMATION;
		}
		else
		{
			strMessage.Format(IDS_IMPORT_FAILED, dImport.GetFullPath());

			dwType	= MB_OK | MB_ICONERROR;
		}

		MessageBox(strMessage, strTitle, dwType);
	}
}

void CDualView::OnDualExport()
{
	HTREEITEM		hItem	= m_ctlKeyTree.GetSelectedItem();
	CExportRange	dExportRange;

	dExportRange.SetHeader(IDS_EXPORT_RANGE);

	dExportRange.m_strSelectedBrench	= SearchFullKeyPath(hItem);
	dExportRange.m_nExportRange			= ((hItem == m_ctlKeyTree.GetRootItem()) ? 0 : 1);

	if (dExportRange.DoModal() == IDOK)
	{
		CFileSaveDialogX	dSave;

		dSave.SetHeader(IDS_EXPORT_TITLE);
		dSave.SetTypePairs(IDS_EXPORT_FILTERS);

		if (dSave.DoModal() == IDOK)
		{
			BeginWaitCursor();

			if ((hItem == m_ctlKeyTree.GetRootItem()) || (dExportRange.m_nExportRange == 0))
			{
				CRegistryX	reg;
				CString		strTitle;
				CString		strMessage;
				UINT		uiType	= MB_OK | MB_ICONINFORMATION;

				strTitle.LoadString(IDR_MAINFRAME);
				strMessage.Format(IDS_EXPORT_SUCCESS, dSave.GetFullPath());
				reg.Open(HKEY_CLASSES_ROOT, _T(""));

				if (!reg.REG5Export(dSave.GetFullPath()))
				{
					strMessage.LoadString(IDS_EXPORT_FAILED);

					uiType	= MB_OK | MB_ICONERROR;
				}

				reg.Close();
				reg.Open(HKEY_CURRENT_USER, _T(""));

				if (!reg.REG5Export(dSave.GetFullPath(), TRUE, FALSE))
				{
					strMessage.LoadString(IDS_EXPORT_FAILED);

					uiType	= MB_OK | MB_ICONERROR;
				}

				reg.Close();
				reg.Open(HKEY_LOCAL_MACHINE, _T(""));

				if (!reg.REG5Export(dSave.GetFullPath(), TRUE, FALSE))
				{
					strMessage.LoadString(IDS_EXPORT_FAILED);

					uiType	= MB_OK | MB_ICONERROR;
				}

				reg.Close();
				MessageBox(strMessage, strTitle, uiType);
			}
			else
			{
				CRegistryX	reg;
				CString		strTitle;
				CString		strMessage;
				UINT		uiType	= MB_OK | MB_ICONINFORMATION;

				strTitle.LoadString(IDR_MAINFRAME);
				strMessage.Format(IDS_EXPORT_SUCCESS, dSave.GetFullPath());
				reg.Open(m_hRootKey, m_strSubKey);

				if (!reg.REG5Export(dSave.GetFullPath()))
				{
					strMessage.LoadString(IDS_EXPORT_FAILED);

					uiType	= MB_OK | MB_ICONERROR;
				}

				reg.Close();
				MessageBox(strMessage, strTitle, uiType);
			}

			EndWaitCursor();
		}
	}
}

void CDualView::OnDualFind()
{
	CRegistryMFCX	reg;
	CFindDialog		dFind;

	m_arrFindType.RemoveAll();
	m_arrFindSubKey.RemoveAll();
	m_arrFindValue.RemoveAll();
	m_arrFindData.RemoveAll();
	reg.Open(HKEY_CURRENT_USER, RK_RG_FIND);

	dFind.m_bKeys			= (BOOL)reg.ReadDWORD(RV_RG_KEYS, 1);
	dFind.m_bValues			= (BOOL)reg.ReadDWORD(RV_RG_VALUES, 1);
	dFind.m_bData			= (BOOL)reg.ReadDWORD(RV_RG_DATA, 1);
	dFind.m_bMatchOnly		= (BOOL)reg.ReadDWORD(RV_RG_MATCHONLY, 0);
	dFind.m_bCaseSensitive	= (BOOL)reg.ReadDWORD(RV_RG_CASESENSITIVE, 0);

	reg.Close();
	dFind.SetHeader(IDS_FIND_TITLE);

	if (dFind.DoModal() == IDOK)
	{
		if (!dFind.m_strFindWhat.IsEmpty())
		{
			int	nFound	= 0;

			BeginWaitCursor();
			reg.Open(HKEY_CURRENT_USER, RK_RG_FIND);
			reg.WriteDWORD(RV_RG_KEYS, dFind.m_bKeys);
			reg.WriteDWORD(RV_RG_VALUES, dFind.m_bValues);
			reg.WriteDWORD(RV_RG_DATA, dFind.m_bData);
			reg.WriteDWORD(RV_RG_MATCHONLY, dFind.m_bMatchOnly);
			reg.WriteDWORD(RV_RG_CASESENSITIVE, dFind.m_bCaseSensitive);
			reg.Close();
			reg.SetFindOptions(dFind.m_bKeys, dFind.m_bValues, dFind.m_bData, dFind.m_bMatchOnly, dFind.m_bCaseSensitive);

			nFound	= reg.BuildFindList(dFind.m_strFindWhat);

			if (nFound > 0)
			{
				for (int i = 0; i < nFound; i++)
				{
					m_arrFindType.Add(reg.m_arrFRType[i]);
					m_arrFindSubKey.Add(reg.m_arrFRSubKey[i]);
					m_arrFindValue.Add(reg.m_arrFRValue[i]);
					m_arrFindData.Add(reg.m_arrFRData[i]);
				}
			}

			EndWaitCursor();
			OnDualOpenFoundItems();
		}
	}
}

void CDualView::OnDualOpenFoundItems()
{
	CFoundItems	dFound;
	int			i;

	if (!m_arrFindType.GetSize())
	{
		CString	strMessage;
		CString	strTitle;

		strMessage.LoadString(IDS_FIND_NO_MATCH);
		strTitle.LoadString(IDR_MAINFRAME);
		MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
		return;
	}

	for (i = 0; i < m_arrFindType.GetSize(); i++)
	{
		dFound.m_arrFindType.Add(m_arrFindType[i]);
		dFound.m_arrFindSubKey.Add(m_arrFindSubKey[i]);
		dFound.m_arrFindValue.Add(m_arrFindValue[i]);
		dFound.m_arrFindData.Add(m_arrFindData[i]);
	}

	dFound.SetHeader(IDS_FOUND_ITEMS_TITLE);

	UINT	nResponse	= dFound.DoModal();

	m_arrFindType.RemoveAll();
	m_arrFindSubKey.RemoveAll();
	m_arrFindValue.RemoveAll();
	m_arrFindData.RemoveAll();

	for (i = 0; i < dFound.m_arrFindType.GetSize(); i++)
	{
		m_arrFindType.Add(dFound.m_arrFindType[i]);
		m_arrFindSubKey.Add(dFound.m_arrFindSubKey[i]);
		m_arrFindValue.Add(dFound.m_arrFindValue[i]);
		m_arrFindData.Add(dFound.m_arrFindData[i]);
	}

	if (nResponse == IDOK)
	{
		switch (m_arrFindType[dFound.m_nSelected])
		{
		case REGFIND_KEY:
			{
				Refresh(m_arrFindSubKey[dFound.m_nSelected]);
				m_ctlKeyTree.SetFocus();
			}
			break;
		case REGFIND_VALUE:
			{
				Refresh(m_arrFindSubKey[dFound.m_nSelected]);
				m_ctlValueList.SetFocus();

				for (i = 0; i < m_ctlValueList.GetItemCount(); i++)
				{
					if (m_arrFindValue[dFound.m_nSelected] == m_ctlValueList.GetItemText(i, 0))
					{
						m_ctlValueList.EnsureVisible(i, FALSE);
						m_ctlValueList.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
						break;
					}
				}
			}
			break;
		case REGFIND_DATA:
			{
				Refresh(m_arrFindSubKey[dFound.m_nSelected]);
				m_ctlValueList.SetFocus();

				for (i = 0; i < m_ctlValueList.GetItemCount(); i++)
				{
					if (m_arrFindValue[dFound.m_nSelected] == m_ctlValueList.GetItemText(i, 0))
					{
						m_ctlValueList.EnsureVisible(i, FALSE);
						m_ctlValueList.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
						break;
					}
				}
			}
			break;
		}
	}
}

void CDualView::OnDualModeStandard()
{
	ModeStandard();
}

void CDualView::OnDualModeKeyOnly()
{
	ModeKeyOnly();
}

void CDualView::OnDualModeValueOnly()
{
	ModeValueOnly();
}

void CDualView::OnDualLayout()
{
	CRegistryMFCX	reg;

	SetLayout(((m_bVertical == TRUE) ? FALSE : TRUE));
	RefreshLayout(FALSE);
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_LAYOUT, (m_bVertical ? 1 : 0));
	reg.Close();
}

void CDualView::OnDualNameEditor()
{
	CRegistryMFCX	reg;

	SetNameEditor(((m_bNameEditor == TRUE) ? FALSE : TRUE));
	RefreshLayout(FALSE);
	Refresh();
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_NAMEEDITOR, (m_bNameEditor ? 1 : 0));
	reg.Close();
}

void CDualView::OnDualNewKey()
{
	CRegistryMFCX	reg;
	CString			strFullKey;
	CString			strKeyName;
	int				nPos	= 0;

	reg.Open(m_hRootKey, m_strSubKey);

	do
	{
		nPos++;

		strKeyName.Format(IDS_NEW_KEY_TEMPLATE, nPos);

		if (!reg.IsSubKeyExist(strKeyName))
		{
			break;
		}
	} while (TRUE);

	reg.Close();
	strFullKey.Format(_T("%s\\%s"), m_strSubKey, strKeyName);
	reg.Open(m_hRootKey, strFullKey);
	reg.Close();

	HTREEITEM		hParentItem	= m_ctlKeyTree.GetSelectedItem();
	HTREEITEM		hItem;
	TVINSERTSTRUCT	tvi;

	tvi.hParent				= hParentItem;
	tvi.hInsertAfter		= TVI_SORT;
	tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText		= strKeyName.GetBuffer(strKeyName.GetLength());
	tvi.item.iImage			= 1;
	tvi.item.iSelectedImage	= 1;
	tvi.item.cChildren		= I_CHILDRENCALLBACK;
	tvi.item.lParam			= ITEM_NEW;

	hItem	= m_ctlKeyTree.InsertItem(&tvi);

	strKeyName.ReleaseBuffer();
	m_ctlKeyTree.SetFocus();

	if (m_bNameEditor)
	{
		EditKeyName(hItem);
	}
	else
	{
		m_ctlKeyTree.EditLabel(hItem);
	}
}

void CDualView::OnDualNewString()
{
	CRegistryMFCX	reg;
	CString			strValueName;
	int				nPos	= 0;

	reg.Open(m_hRootKey, m_strSubKey);

	do
	{
		nPos++;

		strValueName.Format(IDS_NEW_VALUE_TEMPLATE, nPos);

		if (!reg.IsValueExist(strValueName))
		{
			break;
		}
	} while (TRUE);

	reg.WriteString(strValueName, _T(""));
	reg.Close();
	SearchValues();

	for (int i = 0; i < m_ctlValueList.GetItemCount(); i++)
	{
		if (strValueName == m_ctlValueList.GetItemText(i, 0))
		{
			m_ctlValueList.SetFocus();

			if (m_bNameEditor)
			{
				EditValueName(i);
			}
			else
			{
				m_ctlValueList.EditLabel(i);
			}

			break;
		}
	}
}

void CDualView::OnDualNewBinary()
{
	CRegistryX	reg;
	CString		strValueName;
	int			nPos	= 0;

	reg.Open(m_hRootKey, m_strSubKey);

	do
	{
		nPos++;

		strValueName.Format(IDS_NEW_VALUE_TEMPLATE, nPos);

		if (!reg.IsValueExist(strValueName))
		{
			break;
		}
	} while (TRUE);

	DWORD	dwSize	= 0;

	reg.WriteBinary(strValueName, NULL, dwSize);
	reg.Close();
	SearchValues();

	for (int i = 0; i < m_ctlValueList.GetItemCount(); i++)
	{
		if (strValueName == m_ctlValueList.GetItemText(i, 0))
		{
			m_ctlValueList.SetFocus();

			if (m_bNameEditor)
			{
				EditValueName(i);
			}
			else
			{
				m_ctlValueList.EditLabel(i);
			}

			break;
		}
	}
}

void CDualView::OnDualNewDWORD()
{
	CRegistryX	reg;
	CString		strValueName;
	int			nPos	= 0;

	reg.Open(m_hRootKey, m_strSubKey);

	do
	{
		nPos++;

		strValueName.Format(IDS_NEW_VALUE_TEMPLATE, nPos);

		if (!reg.IsValueExist(strValueName))
		{
			break;
		}
	} while (TRUE);

	reg.WriteDWORD(strValueName, 0);
	reg.Close();
	SearchValues();

	for (int i = 0; i < m_ctlValueList.GetItemCount(); i++)
	{
		if (strValueName == m_ctlValueList.GetItemText(i, 0))
		{
			m_ctlValueList.SetFocus();

			if (m_bNameEditor)
			{
				EditValueName(i);
			}
			else
			{
				m_ctlValueList.EditLabel(i);
			}

			break;
		}
	}
}

void CDualView::OnDualNewMultiString()
{
	CRegistryX	reg;
	CString		strValueName;
	int			nPos	= 0;

	reg.Open(m_hRootKey, m_strSubKey);

	do
	{
		nPos++;

		strValueName.Format(IDS_NEW_VALUE_TEMPLATE, nPos);

		if (!reg.IsValueExist(strValueName))
		{
			break;
		}
	} while (TRUE);

	DWORD	dwSize	= 0;

	reg.WriteMultiString(strValueName, NULL, 0);
	reg.Close();
	SearchValues();

	for (int i = 0; i < m_ctlValueList.GetItemCount(); i++)
	{
		if (strValueName == m_ctlValueList.GetItemText(i, 0))
		{
			m_ctlValueList.SetFocus();

			if (m_bNameEditor)
			{
				EditValueName(i);
			}
			else
			{
				m_ctlValueList.EditLabel(i);
			}

			break;
		}
	}
}

void CDualView::OnDualNewExpandString()
{
	CRegistryMFCX	reg;
	CString			strValueName;
	int				nPos	= 0;

	reg.Open(m_hRootKey, m_strSubKey);

	do
	{
		nPos++;

		strValueName.Format(IDS_NEW_VALUE_TEMPLATE, nPos);

		if (!reg.IsValueExist(strValueName))
		{
			break;
		}
	} while (TRUE);

	reg.WriteExpandString(strValueName, _T(""));
	reg.Close();
	SearchValues();

	for (int i = 0; i < m_ctlValueList.GetItemCount(); i++)
	{
		if (strValueName == m_ctlValueList.GetItemText(i, 0))
		{
			m_ctlValueList.SetFocus();

			if (m_bNameEditor)
			{
				EditValueName(i);
			}
			else
			{
				m_ctlValueList.EditLabel(i);
			}

			break;
		}
	}
}

void CDualView::OnDualRefresh()
{
	Refresh();
}

void CDualView::OnDualOptions()
{
	COptionsSheet	Sheet(IDR_MAINFRAME);

	Sheet.SetTitle(IDS_DUAL_OPTIONS);
	Sheet.DoModal();
	LoadLayoutOptions();
	RefreshLayout(TRUE);
	Refresh();
}

void CDualView::OnDualSwitch()
{
	if (m_nShowMode != VIEW_STANDARD)
	{
		GoParentKey();
	}
	else
	{
		CWnd*	pWnd	= GetFocus();

		if (pWnd == ((CWnd*)&m_ctlKeyTree))
		{
			m_ctlValueList.SetFocus();
		}
		else
		{
			m_ctlKeyTree.SetFocus();
		}
	}
}

void CDualView::OnKeyExpand()
{
	HTREEITEM	hItem	= m_ctlKeyTree.GetSelectedItem();

	m_ctlKeyTree.Expand(hItem, TVE_EXPAND);
	m_ctlKeyTree.EnsureVisible(hItem);
}

void CDualView::OnKeyCollapse()
{
	HTREEITEM	hItem	= m_ctlKeyTree.GetSelectedItem();

	m_ctlKeyTree.Expand(hItem, TVE_COLLAPSE);
	m_ctlKeyTree.EnsureVisible(hItem);
}

void CDualView::OnKeyDelete()
{
	CString	strTitle;
	CString	strMessage;

	strTitle.LoadString(IDS_DELETEKEY_TITLE);
	strMessage.LoadString(IDS_DELETEKEY_CONFIRM);

	if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		HTREEITEM	hItem		= m_ctlKeyTree.GetSelectedItem();
		HTREEITEM	hNextItem;
		HKEY		hKey;
		CString		strSubKey;

		if ((hNextItem = m_ctlKeyTree.GetPrevSiblingItem(hItem)) == NULL)
		{
			if ((hNextItem = m_ctlKeyTree.GetNextSiblingItem(hItem)) == NULL)
			{
				hNextItem	= m_ctlKeyTree.GetParentItem(hItem);
			}
		}

		if (GetKeys(&hKey, strSubKey, SearchFullKeyPath(hItem)))
		{
			CRegistryMFCX	reg;
			CString			strKey	= SearchFullKeyPath(hNextItem);

			reg.DeleteKey(hKey, strSubKey);
			reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
			reg.WriteString(RV_RG_CURRENTKEY, strKey);
			reg.Close();
			m_ctlKeyTree.SelectItem(hNextItem);
			m_ctlKeyTree.DeleteItem(hItem);
		}
	}
}

void CDualView::OnKeyRename()
{
	m_ctlKeyTree.SetFocus();

	if (m_bNameEditor)
	{
		EditKeyName(m_ctlKeyTree.GetSelectedItem());
	}
	else
	{
		m_ctlKeyTree.EditLabel(m_ctlKeyTree.GetSelectedItem());
	}
}

void CDualView::OnKeyCopyname()
{
	CopyText(SearchFullKeyPath(m_ctlKeyTree.GetSelectedItem()));
}

void CDualView::OnValueModify()
{
	int		nItem		= m_ctlValueList.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);
	CString	strValue	= m_ctlValueList.GetItemText(nItem, 0);
	DWORD	dwType		= m_ctlValueList.GetItemData(nItem);

	switch (dwType)
	{
	case REG_SZ:
		{
			CString	strDefault;

			strDefault.LoadString(IDS_DEFAULT_NAME);

			if (strValue.CompareNoCase(strDefault) == 0)
			{
				strValue.LoadString(IDS_DEFAULT);
			}

			EditStringValue(strValue);
		}
		break;
	case REG_EXPAND_SZ:
		{
			EditExpandStringValue(strValue);
		}
		break;
	case REG_BINARY:
		{
			EditBinaryValue(strValue);
		}
		break;
	case REG_DWORD:
		{
			EditDWORDValue(strValue);
		}
		break;
	case REG_MULTI_SZ:
		{
			EditMultiStringValue(strValue);
		}
		break;
	}
}

void CDualView::OnValueDelete()
{
	CString	strTitle;
	CString	strMessage;
	CString	strDefault;

	strTitle.LoadString(IDS_DELETEVALUE_TITLE);
	strMessage.LoadString(IDS_DELETEVALUE_CONFIRM);
	strDefault.LoadString(IDS_DEFAULT_NAME);

	if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		CRegistryMFCX	reg;
		CStringArray	arrValueName;
		UINT			uCount	= m_ctlValueList.GetSelectedCount();
		int				nItem	= -1;
		UINT			i;

		arrValueName.RemoveAll();

		for (i = 0; i < uCount; i++)
		{
			nItem	= m_ctlValueList.GetNextItem(nItem, LVNI_SELECTED);

			arrValueName.Add(m_ctlValueList.GetItemText(nItem, 0));
		}

		reg.Open(m_hRootKey, m_strSubKey);

		for (i = 0; i < (UINT)arrValueName.GetSize(); i++)
		{
			if (!strDefault.CompareNoCase(arrValueName.GetAt(i)))
			{
				arrValueName[i].LoadString(IDS_DEFAULT);
			}

			reg.DeleteValue(arrValueName.GetAt(i));
		}

		reg.Close();
		SearchValues();
	}
}

void CDualView::OnValueRename()
{
	m_ctlValueList.SetFocus();

	if (m_bNameEditor)
	{
		EditValueName(m_ctlValueList.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED));
	}
	else
	{
		m_ctlValueList.EditLabel(m_ctlValueList.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED));
	}
}

void CDualView::OnValueShowDataType()
{
	CRegistryX	reg;

	SetShowDataType(((m_bShowDataType == TRUE) ? FALSE : TRUE));
	RefreshLayout(TRUE);
	Refresh();
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_SHOWDATATYPE, (m_bShowDataType ? 1 : 0));
	reg.Close();
}

void CDualView::OnValueAutoWidth()
{
	CRegistryX	reg;

	SetAutoWidth(((m_bAutoWidth == TRUE) ? FALSE : TRUE));
	RefreshLayout(FALSE);
	Refresh();
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_AUTOWIDTH, (m_bAutoWidth ? 1 : 0));
	reg.Close();
}

void CDualView::OnValueFullRowSelect()
{
	CRegistryX	reg;

	SetFullRowSelect(((m_bFullRowSelect == TRUE) ? FALSE : TRUE));
	RefreshLayout(FALSE);
	Refresh();
	reg.Open(HKEY_CURRENT_USER, RK_RG_ROOT);
	reg.WriteDWORD(RV_RG_FULLROWSELECT, (m_bFullRowSelect ? 1 : 0));
	reg.Close();
}

void CDualView::OnAbout()
{
	CAboutDialog	dAbout;

	dAbout.DoModal();
}

void CDualView::OnUpdateDualModeStandard(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_nShowMode == VIEW_STANDARD));
}

void CDualView::OnUpdateDualModeKeyOnly(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_nShowMode == VIEW_KEYONLY));
}

void CDualView::OnUpdateDualModeValueOnly(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_nShowMode == VIEW_VALUEONLY));
}

void CDualView::OnUpdateDualLayout(CCmdUI* pCmdUI)
{
	CString	strItem;

	strItem.LoadString(((m_bVertical == TRUE) ? IDS_DUAL_HORIZONTAL_LAYOUT : IDS_DUAL_VERTICAL_LAYOUT));
	pCmdUI->SetText(strItem);
	pCmdUI->Enable((BOOL)(m_nShowMode == VIEW_STANDARD));
}

void CDualView::OnUpdateDualNameEditor(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_bNameEditor == TRUE));
}

void CDualView::OnUpdateDualSwitch(CCmdUI* pCmdUI)
{
	CString	strItem;

	if (m_nShowMode != VIEW_STANDARD)
	{
		strItem.LoadString(IDS_UP);
		pCmdUI->SetText(strItem);
		pCmdUI->Enable((BOOL)(m_ctlKeyTree.GetSelectedItem() != m_ctlKeyTree.GetRootItem()));
	}
	else
	{
		strItem.LoadString(IDS_SWITCH);
		pCmdUI->SetText(strItem);
		pCmdUI->Enable(TRUE);
	}
}

void CDualView::OnUpdateValueShowDataType(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowDataType);
}

void CDualView::OnUpdateValueAutoWidth(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoWidth);
}

void CDualView::OnUpdateValueFullRowSelect(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bFullRowSelect);
}
