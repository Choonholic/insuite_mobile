// FoundItems.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "FoundItems.h"
#include "RegistryMFCX.h"
#include "FileSaveDialogX.h"

IMPLEMENT_DYNAMIC(CFoundItems, COptionsDialogX)

CFoundItems::CFoundItems(CWnd* pParent /*=NULL*/) : COptionsDialogX(CFoundItems::IDD, pParent)
{
	m_nSelected	= (UINT)(-1);
}

CFoundItems::~CFoundItems()
{
}

void CFoundItems::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOUND_LIST, m_ctlFoundItems);
	DDX_Control(pDX, IDC_FOUND_CLEAR, m_ctlClear);
}

BEGIN_MESSAGE_MAP(CFoundItems, COptionsDialogX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FOUND_CLEAR, &CFoundItems::OnBnClickedFoundClear)
	ON_MESSAGE(WM_LCX_TAPANDHOLD, &CFoundItems::OnTapAndHoldFoundList)
	ON_COMMAND(ID_FIND_CLEAR_ALL, &CFoundItems::OnFindClearAll)
	ON_COMMAND(ID_FIND_LOCATE, &CFoundItems::OnFindLocate)
	ON_COMMAND(ID_FIND_DELETE, &CFoundItems::OnFindDelete)
	ON_COMMAND(ID_FIND_EXPORT, &CFoundItems::OnFindExport)
	ON_NOTIFY(NM_CLICK, IDC_FOUND_LIST, &CFoundItems::OnNMClickFoundList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FOUND_LIST, &CFoundItems::OnLvnItemchangedFoundList)
END_MESSAGE_MAP()

BOOL CFoundItems::GetKeys(PHKEY phKey, CString& strRootKey, LPCTSTR lpszKey)
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

void CFoundItems::SetHeaders()
{
	CString	strItem;

	strItem.LoadString(IDS_HEADER_TYPE);
	m_ctlFoundItems.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(40), 0);
	strItem.LoadString(IDS_HEADER_SUBKEY);
	m_ctlFoundItems.InsertColumn(1, strItem, LVCFMT_LEFT, DRA::SCALEX(160), 1);
	strItem.LoadString(IDS_HEADER_VALUE);
	m_ctlFoundItems.InsertColumn(2, strItem, LVCFMT_LEFT, DRA::SCALEX(80), 2);
	strItem.LoadString(IDS_HEADER_DATA);
	m_ctlFoundItems.InsertColumn(3, strItem, LVCFMT_LEFT, DRA::SCALEX(140), 3);
}

void CFoundItems::SetItems()
{
	LVITEM	lvi;
	DWORD	dwStyle;
	CString	strType;

	BeginWaitCursor();

	dwStyle	= m_ctlFoundItems.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT);
	dwStyle	|= LVS_EX_FULLROWSELECT;

	m_ctlFoundItems.SetExtendedStyle(dwStyle);
	m_ctlFoundItems.SetRedraw(FALSE);

	if (m_ctlFoundItems.GetItemCount() > 0)
	{
		m_ctlFoundItems.DeleteAllItems();
	}

	for (int i = 0; i < m_arrFindType.GetSize(); i++)
	{
		switch (m_arrFindType[i])
		{
		case REGFIND_KEY:
			{
				strType.LoadString(IDS_FOUND_KEY);
			}
			break;
		case REGFIND_VALUE:
			{
				strType.LoadString(IDS_FOUND_VALUE);
			}
			break;
		case REGFIND_DATA:
			{
				strType.LoadString(IDS_FOUND_DATA);
			}
			break;
		default:
			{
				strType.LoadString(IDS_FOUND_UNKNOWN);
			}
			break;
		}

		lvi.mask		= LVIF_TEXT | LVIF_PARAM;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.lParam		= m_arrFindType[i];
		lvi.pszText		= strType.GetBuffer(strType.GetLength());

		m_ctlFoundItems.InsertItem(&lvi);
		strType.ReleaseBuffer();
		m_ctlFoundItems.SetItemText(i, 1, m_arrFindSubKey[i]);
		m_ctlFoundItems.SetItemText(i, 2, m_arrFindValue[i]);
		m_ctlFoundItems.SetItemText(i, 3, m_arrFindData[i]);
	}

	m_ctlFoundItems.SetRedraw(TRUE);
	EndWaitCursor();
}

void CFoundItems::ClearItems()
{
	m_arrFindType.RemoveAll();
	m_arrFindSubKey.RemoveAll();
	m_arrFindValue.RemoveAll();
	m_arrFindData.RemoveAll();
}

void CFoundItems::SetButtons()
{
	CCommandBar*	pCommandBar	= GetCommandBarCtrl();

	m_ctlClear.EnableWindow((BOOL)(m_arrFindType.GetSize() > 0));
	pCommandBar->SendMessage(TB_SETSTATE, ID_FIND_LOCATE, ((BOOL)(m_ctlFoundItems.GetSelectedCount() == 1) ? MAKELONG(TBSTATE_ENABLED, 0) : 0));
}

BOOL CFoundItems::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_FIND_LOCATE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= ID_FIND_LOCATE;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_CLOSE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	SetHeaders();
	SetItems();
	SetButtons();
	return TRUE;
}

void CFoundItems::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	SIZE	size;
	int		nLeft;

	m_ctlFoundItems.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlClear.GetWindowRect(&rc);
	ScreenToClient(&rc);

	size.cx	= rc.right - rc.left;
	size.cy	= rc.bottom - rc.top;

	rc.right	= cx - nLeft;
	rc.left		= rc.right - size.cx;
	rc.bottom	= cy - nLeft;
	rc.top		= rc.bottom - size.cy;
	size.cy		= rc.top;

	m_ctlClear.MoveWindow(&rc);

	m_ctlFoundItems.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;
	rc.bottom	= size.cy - nLeft;

	m_ctlFoundItems.MoveWindow(&rc);
	m_ctlFoundItems.SetAutoColumnWidth();
}

void CFoundItems::OnBnClickedFoundClear()
{
	ClearItems();
	SetItems();
	SetButtons();
}

void CFoundItems::OnNMClickFoundList(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetButtons();

	*pResult	= 0;
}

LRESULT CFoundItems::OnTapAndHoldFoundList(WPARAM wParam, LPARAM lParam)
{
	int		nItem	= wParam;
	CString	strItem;
	CMenu	ctlMenu;

	SetButtons();

	if (nItem == -1)
	{
		ctlMenu.CreatePopupMenu();
		strItem.LoadString(IDS_FIND_CLEAR_ALL);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FIND_CLEAR_ALL, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}
	else
	{
		UINT	uFlags	= ((m_ctlFoundItems.GetSelectedCount() > 1) ? MF_GRAYED : MF_ENABLED);

		ctlMenu.CreatePopupMenu();
		strItem.LoadString(IDS_FIND_LOCATE);
		ctlMenu.AppendMenu(uFlags, ID_FIND_LOCATE, strItem);
		strItem.LoadString(IDS_FIND_DELETE);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FIND_DELETE, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FIND_EXPORT);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FIND_EXPORT, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}

	return 0;
}

void CFoundItems::OnLvnItemchangedFoundList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW	pNMLV	= reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	SetButtons();

	*pResult	= 0;
}

void CFoundItems::OnFindClearAll()
{
	ClearItems();
	SetItems();
	SetButtons();
}

void CFoundItems::OnFindLocate()
{
	m_nSelected	= m_ctlFoundItems.GetNextItem(-1, LVNI_SELECTED);

	if (m_nSelected != -1)
	{
		COptionsDialogX::OnOK();
	}
}

void CFoundItems::OnFindDelete()
{
	CString	strTitle;
	CString	strMessage;

	strTitle.LoadString(IDS_FOUND_DELETE_TITLE);
	strMessage.LoadString(IDS_FOUND_DELETE_CONFIRM);

	if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		CRegistryMFCX	reg;
		UINT			uCount	= m_ctlFoundItems.GetSelectedCount();
		int				nItem	= -1;
		DWORD			dwType;
		UINT			i;
		HKEY			hKey;
		CString			strSubKey;
		CString			strValue;
		CDWordArray		arrDeleted;

		for (i = 0; i < uCount; i++)
		{
			nItem	= m_ctlFoundItems.GetNextItem(nItem, LVNI_SELECTED);
			dwType	= m_ctlFoundItems.GetItemData(nItem);

			arrDeleted.Add(nItem);
			GetKeys(&hKey, strSubKey, m_arrFindSubKey[nItem]);

			switch (dwType)
			{
			case REGFIND_KEY:
				{
					reg.DeleteKey(hKey, strSubKey);
				}
				break;
			case REGFIND_VALUE:
				{
					reg.Open(hKey, strSubKey);
					reg.DeleteValue(m_arrFindValue[nItem]);
					reg.Close();
				}
				break;
			case REGFIND_DATA:
				{
					reg.Open(hKey, strSubKey);
					reg.DeleteValue(m_arrFindValue[nItem]);
					reg.Close();
				}
				break;
			}
		}

		for (i = uCount; i > 0; i--)
		{
			m_arrFindType.RemoveAt(i - 1);
			m_arrFindSubKey.RemoveAt(i - 1);
			m_arrFindValue.RemoveAt(i - 1);
			m_arrFindData.RemoveAt(i - 1);
		}

		SetItems();
	}
}

void CFoundItems::OnFindExport()
{
	CRegistryMFCX		reg;
	UINT				uCount	= m_ctlFoundItems.GetSelectedCount();
	int					nItem	= -1;
	UINT				i;
	HKEY				hKey;
	CString				strSubKey;
	CFileSaveDialogX	dSave;
	CString				strTitle;
	CString				strMessage;
	UINT				uiType	= MB_OK | MB_ICONINFORMATION;

	dSave.SetHeader(IDS_EXPORT_TITLE);
	dSave.SetTypePairs(IDS_EXPORT_FILTERS);

	if (dSave.DoModal() == IDOK)
	{
		BeginWaitCursor();
		strTitle.LoadString(IDR_MAINFRAME);
		strMessage.Format(IDS_EXPORT_SUCCESS, dSave.GetFullPath());

		for (i = 0; i < uCount; i++)
		{
			nItem	= m_ctlFoundItems.GetNextItem(nItem, LVNI_SELECTED);

			GetKeys(&hKey, strSubKey, m_arrFindSubKey[nItem]);
			reg.Open(hKey, strSubKey);

			if (!reg.REG5Export(dSave.GetFullPath(), TRUE, (BOOL)(i == 0)))
			{
				strMessage.LoadString(IDS_EXPORT_FAILED);

				uiType	= MB_OK | MB_ICONERROR;
			}

			reg.Close();
		}
	
		MessageBox(strMessage, strTitle, uiType);
		EndWaitCursor();
	}
}

void CFoundItems::OnOK()
{
	COptionsDialogX::OnCancel();
}
