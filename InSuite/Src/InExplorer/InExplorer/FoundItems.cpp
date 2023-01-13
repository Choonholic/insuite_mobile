// FoundItems.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "FoundItems.h"
#include "ShellX.h"
#include "RegistryMFCX.h"

#ifndef FOUND_MAX_AUTOWIDTH
#define FOUND_MAX_AUTOWIDTH	200
#endif

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
	ON_NOTIFY(NM_CLICK, IDC_FOUND_LIST, &CFoundItems::OnNMClickFoundList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FOUND_LIST, &CFoundItems::OnLvnItemchangedFoundList)
END_MESSAGE_MAP()

void CFoundItems::SetHeaders()
{
	CString	strItem;

	strItem.LoadString(IDS_HEADER_NAME);
	m_ctlFoundItems.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(80), 0);
	strItem.LoadString(IDS_HEADER_FOLDER);
	m_ctlFoundItems.InsertColumn(1, strItem, LVCFMT_LEFT, DRA::SCALEX(160), 1);
}

void CFoundItems::SetHeaderDefault()
{
	CHeaderCtrl*	pHeader	= m_ctlFoundItems.GetHeaderCtrl();

	if (pHeader)
	{
		HDITEM	hdi;

		hdi.mask	= HDI_WIDTH;

		pHeader->GetItem(0, &hdi);

		hdi.cxy	= DRA::SCALEX(80);

		pHeader->SetItem(0, &hdi);

		hdi.mask	= HDI_WIDTH;

		pHeader->GetItem(1, &hdi);

		hdi.cxy	= DRA::SCALEX(160);

		pHeader->SetItem(1, &hdi);
	}
}

void CFoundItems::SetItems()
{
	LVITEM	lvi;
	DWORD	dwStyle;
	CString	strName;
	CString	strFolder;

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

	for (int i = 0; i < m_arrFiles.GetCount(); i++)
	{
		TakeFileName(m_arrFiles[i], strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
		strName.ReleaseBuffer();
		TakePath(m_arrFiles[i], strFolder.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
		strFolder.ReleaseBuffer();

		lvi.mask		= LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.iImage		= GetSystemIconIndex(m_arrFiles[i]);
		lvi.pszText		= strName.GetBuffer(strName.GetLength());

		m_ctlFoundItems.InsertItem(&lvi);
		strName.ReleaseBuffer();
		m_ctlFoundItems.SetItemText(i, 1, strFolder);
	}

	if (m_bAutoWidth == TRUE)
	{
		if ((m_nMaxAutoWidth == 0) || (m_arrFiles.GetCount() < m_nMaxAutoWidth))
		{
			m_ctlFoundItems.SetAutoColumnWidth();
		}
		else
		{
			SetHeaderDefault();
		}
	}

	m_ctlFoundItems.SetRedraw(TRUE);
	EndWaitCursor();
}

void CFoundItems::ClearItems()
{
	m_arrFiles.RemoveAll();
}

void CFoundItems::SetButtons()
{
	CCommandBar*	pCommandBar	= GetCommandBarCtrl();

	m_ctlClear.EnableWindow((BOOL)(m_arrFiles.GetCount() > 0));
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

	HIMAGELIST	hSysIcons	= GetSystemIconList(ICON_SMALL);

	m_iSysSmall.Attach(hSysIcons);
	m_ctlFoundItems.SetImageList(&m_iSysSmall, LVSIL_SMALL);
	m_iSysSmall.Detach();

	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_FIND);

	m_bAutoWidth	= (BOOL)(reg.ReadDWORD(RV_EX_AUTOWIDTH, 0) == 1);
	m_nMaxAutoWidth	= reg.ReadDWORD(RV_EX_MAXAUTOWIDTH, FOUND_MAX_AUTOWIDTH);

	reg.Close();

	m_bAutoWidth	= FALSE;
	m_nMaxAutoWidth	= FOUND_MAX_AUTOWIDTH;

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

void CFoundItems::OnOK()
{
	COptionsDialogX::OnCancel();
}
