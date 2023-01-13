// ActionsPage.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "InClose_Settings.h"
#include "ActionsPage.h"
#include "RegistryMFCX.h"
#include "DisplayX.h"
#include "PhoneX.h"

IMPLEMENT_DYNAMIC(CActionsPage, CPropertyPageX)

CActionsPage::CActionsPage() : CPropertyPageX(CActionsPage::IDD), m_nSelectedAction(-1)
{
	m_nMaxActions	= (DetectPhoneEdition() ? MAX_ACTIONS : MAX_ACTIONS - 1);
}

CActionsPage::~CActionsPage()
{
}

void CActionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AC_SELECT_H, m_ctlSelectH);
	DDX_Control(pDX, IDC_AC_SELECT, m_ctlSelect);
	DDX_Control(pDX, IDC_AC_ASSIGN_H, m_ctlAssignH);
	DDX_Control(pDX, IDC_AC_ASSIGN, m_ctlAssign);
}

BEGIN_MESSAGE_MAP(CActionsPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AC_SELECT, &CActionsPage::OnLvnItemchangedAcSelect)
	ON_CBN_SELCHANGE(IDC_AC_ASSIGN, &CActionsPage::OnCbnSelchangeAcAssign)
END_MESSAGE_MAP()

#define AC_MAXITEMS		15
#define AC_FIRSTACTION	IDS_AC_TAP
#define AC_FIRSTITEM	IDS_NONE

void CActionsPage::LoadSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_CL_ACTIONS);

	m_nAssignments[0]	= reg.ReadDWORD(RV_CL_TAP, 2);
	m_nAssignments[1]	= reg.ReadDWORD(RV_CL_TAPANDHOLD, 1);
	m_nAssignments[2]	= reg.ReadDWORD(RV_CL_VERTDRAG, 1);
	m_nAssignments[3]	= reg.ReadDWORD(RV_CL_HORIDRAG, 8);
	m_nAssignments[4]	= reg.ReadDWORD(RV_CL_DIAGDRAG, 7);
	m_nAssignments[5]	= reg.ReadDWORD(RV_CL_TEND, 0);

	reg.Close();
}

void CActionsPage::SaveSettings()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_CL_ACTIONS);
	reg.WriteDWORD(RV_CL_TAP, m_nAssignments[0]);
	reg.WriteDWORD(RV_CL_TAPANDHOLD, m_nAssignments[1]);
	reg.WriteDWORD(RV_CL_VERTDRAG, m_nAssignments[2]);
	reg.WriteDWORD(RV_CL_HORIDRAG, m_nAssignments[3]);
	reg.WriteDWORD(RV_CL_DIAGDRAG, m_nAssignments[4]);
	reg.WriteDWORD(RV_CL_TEND, m_nAssignments[5]);
	reg.Close();

	CInCloseSetupApp*	pApp	= (CInCloseSetupApp*)AfxGetApp();
	HWND				hWnd	= pApp->FindInClose();

	if (hWnd)
	{
		::SendMessage(hWnd, pApp->WM_CL_SETTINGCHAGE, PP_ACTIONS, 0);
	}
}

void CActionsPage::SetHeader()
{
	CString	strItem;
	UINT	uiActions	= 0;
	DWORD	dwDPI		= GetDPI();

	switch (dwDPI)
	{
	case DPI_192:
		{
			uiActions	= IDB_ACTIONS32;
		}
		break;
	case DPI_128:
		{
			uiActions	= IDB_ACTIONS21;
		}
		break;
	default:
		{
			uiActions	= IDB_ACTIONS16;
		}
		break;
	}

	strItem.LoadString(IDS_AC_ACTION);
	m_ctlSelect.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(100), 0);
	strItem.LoadString(IDS_AC_ASSIGNMENT);
	m_ctlSelect.InsertColumn(1, strItem, LVCFMT_LEFT, DRA::SCALEX(120), 1);
	m_iActionImages.Create(uiActions, GetSystemMetrics(SM_CXSMICON), m_nMaxActions, RGB(0xFA, 0xFA, 0xFA));
	m_ctlSelect.SetImageList(&m_iActionImages, LVSIL_SMALL);
}

void CActionsPage::SetMenu()
{
	CString	strItem;

	for (int i = 0; i < AC_MAXITEMS; i++)
	{
		strItem.LoadString(AC_FIRSTITEM + i);
		m_ctlAssign.AddString(strItem);
	}
}

void CActionsPage::SetItems()
{
	LVITEM		lvi;
	DWORD		dwStyle;
	CString		strAction;
	CString		strAssignment;

	dwStyle	= m_ctlSelect.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT);

	m_ctlSelect.SetExtendedStyle(dwStyle);
	m_ctlSelect.SetRedraw(FALSE);

	if (m_ctlSelect.GetItemCount() > 0)
	{
		m_ctlSelect.DeleteAllItems();
	}

	for (int i = 0; i < m_nMaxActions; i++)
	{
		strAction.LoadString(AC_FIRSTACTION + i);

		lvi.mask		= LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.iImage		= i;
		lvi.pszText		= strAction.GetBuffer();

		m_ctlSelect.InsertItem(&lvi);
		strAction.ReleaseBuffer();
		strAssignment.LoadString(AC_FIRSTITEM + m_nAssignments[i]);
		m_ctlSelect.SetItemText(i, 1, strAssignment);
	}

	m_ctlSelect.SetAutoColumnWidth();
	m_ctlSelect.SetRedraw(TRUE);
}

BOOL CActionsPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeader();
	SetMenu();
	LoadSettings();
	SetItems();
	m_ctlSelect.SetItemState(0, LVNI_SELECTED, LVNI_SELECTED);
	m_ctlAssign.SetCurSel(m_nAssignments[0]);
	UpdateData(FALSE);
	return TRUE;
}

void CActionsPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CActionsPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;
	RECT	rcList;
	RECT	rcCombo;
	SIZE	size;

	m_ctlAssign.GetWindowRect(&rcCombo);
	ScreenToClient(&rcCombo);

	size.cx			= rcCombo.right - rcCombo.left;
	size.cy			= rcCombo.bottom - rcCombo.top;
	rcCombo.bottom	= cy - rcCombo.left;
	rcCombo.top		= rcCombo.bottom - size.cy;
	rcCombo.right	= cx - rcCombo.left;

	m_ctlAssign.MoveWindow(&rcCombo);
	m_ctlAssignH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	size.cx		= rc.right - rc.left;
	size.cy		= rc.bottom - rc.top;
	rc.bottom	= rcCombo.top - DRA::SCALEY(3);
	rc.top		= rc.bottom - size.cy;
	rc.right	= cx - rc.left;

	m_ctlAssignH.MoveWindow(&rc);

	rcCombo.top	= rc.top;

	m_ctlSelectH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlSelectH.MoveWindow(&rc);

	m_ctlSelect.GetWindowRect(&rcList);
	ScreenToClient(&rcList);

	rcList.bottom	= rcCombo.top - DRA::SCALEY(6);
	rcList.top		= rc.bottom + DRA::SCALEY(3);
	rcList.right	= cx - rcList.left;

	m_ctlSelect.MoveWindow(&rcList);
	m_ctlSelect.SetAutoColumnWidth();
}

void CActionsPage::OnLvnItemchangedAcSelect(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW	pNMLV	= reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult	= 0;

	m_nSelectedAction	= m_ctlSelect.GetNextItem(-1, LVNI_SELECTED);

	if (m_nSelectedAction != -1)
	{
		m_ctlAssign.SetCurSel(m_nAssignments[m_nSelectedAction]);
		UpdateWindow();
	}
}

void CActionsPage::OnCbnSelchangeAcAssign()
{
	if (m_nSelectedAction != -1)
	{
		m_nAssignments[m_nSelectedAction]	= m_ctlAssign.GetCurSel();

		SetItems();
	}
}
