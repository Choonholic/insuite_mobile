// MenuPage.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "InSuite_Messages.h"
#include "InClose_Settings.h"
#include "MenuPage.h"
#include "RegistryMFCX.h"
#include "DisplayX.h"

#define MNU_INTELLICLOSE		0x0001
#define MNU_MINIMIZE			0x0002
#define MNU_CLOSE				0x0004
#define MNU_CLOSEALL			0x0008
#define MNU_CLOSEALLBUTACTIVE	0x0010
#define MNU_RESET				0x0020
#define MNU_ROTATE				0x0040
#define MNU_TODAY				0x0080
#define MNU_SCREENOFF			0x0100
#define MNU_SUSPEND				0x0200
#define MNU_SETEXCLUDED			0x0400
#define MNU_SETTINGS			0x0800
#define MNU_EXIT				0x1000
#define MNU_TASKS				0x2000
#define MNU_FIRST				(IDS_INTELLICLOSE)

IMPLEMENT_DYNAMIC(CMenuPage, CPropertyPageX)

CMenuPage::CMenuPage() : CPropertyPageX(CMenuPage::IDD)
{
}

CMenuPage::~CMenuPage()
{
}

void CMenuPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ME_MENU, m_ctlMenu);
	DDX_Control(pDX, IDC_ME_UP, m_ctlUp);
	DDX_Control(pDX, IDC_ME_DOWN, m_ctlDown);
}

BEGIN_MESSAGE_MAP(CMenuPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ME_UP, &CMenuPage::OnBnClickedUp)
	ON_BN_CLICKED(IDC_ME_DOWN, &CMenuPage::OnBnClickedDown)
	ON_NOTIFY(NM_CLICK, IDC_ME_MENU, &CMenuPage::OnNMClickMenu)
	ON_MESSAGE(WM_TCX_CHECKTOGGLE, &CMenuPage::OnTcxCheckToggle)
	ON_MESSAGE(WM_CL_MENUCHECK, &CMenuPage::OnMenuCheck)
END_MESSAGE_MAP()

void CMenuPage::LoadSettings()
{
	MENUINFO	DefaultMenu[MNU_ITEMS]	=
	{
		{ TRUE,		MNU_INTELLICLOSE		},
		{ TRUE,		MNU_MINIMIZE			},
		{ TRUE,		MNU_CLOSE				},
		{ TRUE,		MNU_CLOSEALL			},
		{ FALSE,	MNU_CLOSEALLBUTACTIVE	},
		{ TRUE,		MNU_TASKS				},
		{ TRUE,		MNU_RESET				},
		{ TRUE,		MNU_ROTATE				},
		{ FALSE,	MNU_TODAY				},
		{ FALSE,	MNU_SCREENOFF			},
		{ FALSE,	MNU_SUSPEND				},
		{ TRUE,		MNU_SETEXCLUDED			},
		{ TRUE,		MNU_SETTINGS			},
		{ FALSE,	MNU_EXIT				}
	};

	CRegistryMFCX	reg;
	CString			strMenu;

	for (int i = 0; i < MNU_ITEMS; i++)
	{
		strMenu.Format(_T("%s\\%04X"), RK_CL_MENUITEMS, i);
		reg.Open(HKEY_CURRENT_USER, strMenu);

		m_MenuInfo[i].bEnable	= reg.ReadDWORD(RV_CL_ENABLE, DefaultMenu[i].bEnable);
		m_MenuInfo[i].dwItem	= reg.ReadDWORD(RV_CL_ITEM, DefaultMenu[i].dwItem);

		reg.Close();
	}
}

void CMenuPage::SaveSettings()
{
	CRegistryMFCX	reg;
	CString			strMenu;

	for (int i = 0; i < MNU_ITEMS; i++)
	{
		strMenu.Format(_T("%s\\%04X"), RK_CL_MENUITEMS, i);
		reg.Open(HKEY_CURRENT_USER, strMenu);
		reg.WriteDWORD(RV_CL_ENABLE, m_MenuInfo[i].bEnable);
		reg.WriteDWORD(RV_CL_ITEM, m_MenuInfo[i].dwItem);
		reg.Close();
	}

	CInCloseSetupApp*	pApp	= (CInCloseSetupApp*)AfxGetApp();
	HWND				hWnd	= pApp->FindInClose();

	if (hWnd)
	{
		::SendMessage(hWnd, pApp->WM_CL_SETTINGCHAGE, PP_MENU, 0);
	}
}

void CMenuPage::SetTree()
{
	m_ctlMenu.DeleteAllItems();

	CString			strItem;
	TVINSERTSTRUCT	tvi;
	HTREEITEM		hItem;
	int				nPos;

	for (int i = 0; i < MNU_ITEMS; i++)
	{
		for (nPos = 0; nPos < MNU_ITEMS; nPos++)
		{
			if (m_MenuInfo[i].dwItem == (DWORD)(0x01 << nPos))
			{
				break;
			}
		}

		strItem.LoadString(MNU_FIRST + nPos);

		tvi.hParent				= TVI_ROOT;
		tvi.hInsertAfter		= TVI_LAST;
		tvi.item.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
		tvi.item.pszText		= strItem.GetBuffer();
		tvi.item.iImage			= nPos;
		tvi.item.iSelectedImage	= nPos;
		tvi.item.lParam			= MAKELONG(m_MenuInfo[i].dwItem, i);

		hItem	= m_ctlMenu.InsertItem(&tvi);

		m_ctlMenu.SetCheck(hItem, m_MenuInfo[i].bEnable);
		strItem.ReleaseBuffer();
	}
}

void CMenuPage::GetTree()
{
	HTREEITEM	hItem;
	int			i;

	hItem	= m_ctlMenu.GetNextItem(TVI_ROOT, TVGN_ROOT);

	for (i = 0; ((i < MNU_ITEMS) && (hItem != NULL)); i++)
	{
		m_MenuInfo[i].bEnable	= m_ctlMenu.GetCheck(hItem);
		m_MenuInfo[i].dwItem	= LOWORD(m_ctlMenu.GetItemData(hItem));
		hItem					= m_ctlMenu.GetNextItem(hItem, TVGN_NEXT);
	}
}

BOOL CMenuPage::OnInitDialog()
{
	UINT	uiMenu	= 0;
	DWORD	dwDPI	= GetDPI();

	switch (dwDPI)
	{
	case DPI_192:
		{
			uiMenu	= IDB_MENU32;
		}
		break;
	case DPI_128:
		{
			uiMenu	= IDB_MENU21;
		}
		break;
	default:
		{
			uiMenu	= IDB_MENU16;
		}
		break;
	}

	CPropertyPageX::OnInitDialog();
	m_ctlMenu.SetCheckListMode(TRUE);
	m_iMenuImages.Create(uiMenu, GetSystemMetrics(SM_CXSMICON), MNU_ITEMS, RGB(0xFA, 0xFA, 0xFA));
	m_ctlMenu.SetImageList(&m_iMenuImages, TVSIL_NORMAL);
	LoadSettings();
	SetTree();
	return TRUE;
}

void CMenuPage::OnOK()
{
	GetTree();
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CMenuPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rcList;
	RECT	rcUp, rcDown;
	SIZE	size;

	m_ctlDown.GetWindowRect(&rcDown);
	ScreenToClient(&rcDown);

	size.cx			= rcDown.right - rcDown.left;
	size.cy			= rcDown.bottom - rcDown.top;
	rcDown.bottom	= cy - DRA::SCALEY(3);
	rcDown.top		= rcDown.bottom - size.cy;
	rcDown.right	= cx - DRA::SCALEX(3);
	rcDown.left		= rcDown.right - size.cx;

	m_ctlDown.MoveWindow(&rcDown);

	m_ctlUp.GetWindowRect(&rcUp);
	ScreenToClient(&rcUp);

	rcUp.right	= rcDown.left - DRA::SCALEX(3);
	rcUp.left	= rcUp.right - size.cx;
	rcUp.bottom	= rcDown.bottom;
	rcUp.top	= rcDown.top;

	m_ctlUp.MoveWindow(&rcUp);

	m_ctlMenu.GetWindowRect(&rcList);
	ScreenToClient(&rcList);

	rcList.bottom	= rcDown.top - DRA::SCALEY(3);
	rcList.right	= cx - DRA::SCALEX(3);

	m_ctlMenu.MoveWindow(&rcList);
}

void CMenuPage::OnBnClickedUp()
{
	HTREEITEM	hItem	= m_ctlMenu.GetNextItem(TVI_ROOT, TVGN_CARET);
	HTREEITEM	hPrev	= m_ctlMenu.GetNextItem(hItem, TVGN_PREVIOUS);

	if ((hItem != NULL) && (hPrev != NULL))
	{
		DWORD		dwCurrent	= HIWORD(m_ctlMenu.GetItemData(hItem));
		DWORD		dwPrevious	= HIWORD(m_ctlMenu.GetItemData(hPrev));
		MENUINFO	MenuInfo;

		MenuInfo.bEnable				= m_MenuInfo[dwCurrent].bEnable;
		MenuInfo.dwItem					= m_MenuInfo[dwCurrent].dwItem;
		m_MenuInfo[dwCurrent].bEnable	= m_MenuInfo[dwPrevious].bEnable;
		m_MenuInfo[dwCurrent].dwItem	= m_MenuInfo[dwPrevious].dwItem;
		m_MenuInfo[dwPrevious].bEnable	= MenuInfo.bEnable;
		m_MenuInfo[dwPrevious].dwItem	= MenuInfo.dwItem;

		SetTree();

		HTREEITEM	hFind	= m_ctlMenu.GetNextItem(TVI_ROOT, TVGN_ROOT);

		for (int i = 0; i < MNU_ITEMS; i++)
		{
			if (LOWORD(m_ctlMenu.GetItemData(hFind)) == MenuInfo.dwItem)
			{
				m_ctlMenu.SelectItem(hFind);
				m_ctlMenu.EnsureVisible(hFind);
				break;
			}

			hFind	= m_ctlMenu.GetNextItem(hFind, TVGN_NEXT);
		}
	}
}

void CMenuPage::OnBnClickedDown()
{
	HTREEITEM	hItem	= m_ctlMenu.GetNextItem(TVI_ROOT, TVGN_CARET);
	HTREEITEM	hNext	= m_ctlMenu.GetNextItem(hItem, TVGN_NEXT);

	if ((hItem != NULL) && (hNext != NULL))
	{
		DWORD		dwCurrent	= HIWORD(m_ctlMenu.GetItemData(hItem));
		DWORD		dwNext		= HIWORD(m_ctlMenu.GetItemData(hNext));
		MENUINFO	MenuInfo;

		MenuInfo.bEnable				= m_MenuInfo[dwCurrent].bEnable;
		MenuInfo.dwItem					= m_MenuInfo[dwCurrent].dwItem;
		m_MenuInfo[dwCurrent].bEnable	= m_MenuInfo[dwNext].bEnable;
		m_MenuInfo[dwCurrent].dwItem	= m_MenuInfo[dwNext].dwItem;
		m_MenuInfo[dwNext].bEnable		= MenuInfo.bEnable;
		m_MenuInfo[dwNext].dwItem		= MenuInfo.dwItem;

		SetTree();

		HTREEITEM	hFind	= m_ctlMenu.GetNextItem(TVI_ROOT, TVGN_ROOT);

		for (int i = 0; i < MNU_ITEMS; i++)
		{
			if (LOWORD(m_ctlMenu.GetItemData(hFind)) == MenuInfo.dwItem)
			{
				m_ctlMenu.SelectItem(hFind);
				m_ctlMenu.EnsureVisible(hFind);
				break;
			}

			hFind	= m_ctlMenu.GetNextItem(hFind, TVGN_NEXT);
		}
	}
}

void CMenuPage::OnNMClickMenu(NMHDR* pNMHDR, LRESULT* pResult)
{
	POINT		pt;
	HTREEITEM	hItem;
	UINT		uFlags;

	m_ctlMenu.GetClickedPoint(&pt);

	hItem	= m_ctlMenu.HitTest(pt, &uFlags);

	if (hItem != NULL)
	{
		PostMessage(WM_CL_MENUCHECK, (WPARAM)hItem, 0);
	}

	*pResult	= 0;
}

LRESULT CMenuPage::OnTcxCheckToggle(WPARAM wParam, LPARAM lParam)
{
	HWND		hWnd	= (HWND)lParam;
	HTREEITEM	hItem	= (HTREEITEM)wParam;

	if (m_ctlMenu.GetSafeHwnd() == hWnd)
	{
		PostMessage(WM_CL_MENUCHECK, (WPARAM)hItem, 0);
	}

	return 0;
}

LRESULT CMenuPage::OnMenuCheck(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM	hItem	= m_ctlMenu.GetNextItem(TVI_ROOT, TVGN_CARET);
	HTREEITEM	hPrev	= m_ctlMenu.GetNextItem(hItem, TVGN_PREVIOUS);
	HTREEITEM	hNext	= m_ctlMenu.GetNextItem(hItem, TVGN_NEXT);

	if (hItem != NULL)
	{
		m_MenuInfo[HIWORD(m_ctlMenu.GetItemData(hItem))].bEnable	= m_ctlMenu.GetCheck(hItem);
	}

	m_ctlUp.EnableWindow((BOOL)(hPrev != NULL));
	m_ctlDown.EnableWindow((BOOL)(hNext != NULL));
	return 0;
}
