// ConfirmDeleteX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "DisplayX.h"
#include "ConfirmDeleteX.h"
#include "ShellX.h"

extern HINSTANCE	g_hModule;

IMPLEMENT_DYNAMIC(CConfirmDeleteX, CDialogX)

CConfirmDeleteX::CConfirmDeleteX(CWnd* pParent /*=NULL*/) : CDialogX(IDD_CONFIRM_DIALOG, pParent), m_bDoThisForAll(FALSE)
{
	InitializeX();
}

CConfirmDeleteX::~CConfirmDeleteX()
{
}

void CConfirmDeleteX::DoDataExchange(CDataExchange* pDX)
{
	CDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONF_MESSAGE, m_ctlMessage);
	DDX_Control(pDX, IDYES, m_ctlYes);
	DDX_Control(pDX, IDNO, m_ctlSkip);
	DDX_Control(pDX, IDC_CONF_DOALL, m_ctlDoThisForAll);
	DDX_Check(pDX, IDC_CONF_DOALL, m_bDoThisForAll);
}

BEGIN_MESSAGE_MAP(CConfirmDeleteX, CDialogX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDYES, &CConfirmDeleteX::OnBnClickedYes)
	ON_BN_CLICKED(IDNO, &CConfirmDeleteX::OnBnClickedSkip)
END_MESSAGE_MAP()

void CConfirmDeleteX::InitializeX()
{
	m_strFile		= _T("");
	m_dwAttributes	= FILE_ATTRIBUTE_NORMAL;
}

void CConfirmDeleteX::SetInfo(LPCTSTR lpszFileName, DWORD dwAttributes, DWORD dwFlags)
{
	m_strFile		= lpszFileName;
	m_dwAttributes	= dwAttributes;
	m_dwFlags		= dwFlags;
}

DWORD CConfirmDeleteX::GetInfo()
{
	return m_dwFlags;
}

BOOL CConfirmDeleteX::OnInitDialog()
{
	CDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	CString	strMessage;
	CString	strName;

	TakeName(m_strFile, strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	strName.ReleaseBuffer();

	switch (m_dwAttributes)
	{
	case FILE_ATTRIBUTE_READONLY:
		{
			AfxFormatString1(strMessage, IDS_DELC_READONLY, strName);
		}
		break;
	case FILE_ATTRIBUTE_HIDDEN:
		{
			AfxFormatString1(strMessage, IDS_DELC_HIDDEN, strName);
		}
		break;
	case FILE_ATTRIBUTE_SYSTEM:
		{
			AfxFormatString1(strMessage, IDS_DELC_SYSTEM, strName);
		}
		break;
	}

	m_ctlMessage.SetWindowText(strMessage);
	return TRUE;
}

void CConfirmDeleteX::OnSize(UINT nType, int cx, int cy)
{
	CDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;
	int		nWidth;
	int		nButton;

	m_ctlMessage.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.top;
	rc.right	= cx - nLeft;

	m_ctlMessage.MoveWindow(&rc);

	m_ctlSkip.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nWidth		= rc.right - rc.left;
	rc.right	= cx - nLeft;
	rc.left		= rc.right - nWidth;
	nButton		= rc.left;

	m_ctlSkip.MoveWindow(&rc);

	m_ctlYes.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= nButton - nLeft;
	rc.left		= rc.right - nWidth;
	nButton		= rc.left;

	m_ctlYes.MoveWindow(&rc);
}

void CConfirmDeleteX::OnPaint()
{
	CPaintDC	dc(this);
	HICON		hIcon	= GetShellIcon(SHELLICON_WARNING, ICON_BIG);

	if (hIcon != NULL)
	{
		dc.DrawIcon(DRA::SCALEX(4), DRA::SCALEY(4), hIcon);
		DestroyIcon(hIcon);
	}
}

void CConfirmDeleteX::OnBnClickedYes()
{
	UpdateData(TRUE);

	if (m_bDoThisForAll)
	{
		switch (m_dwAttributes)
		{
		case FILE_ATTRIBUTE_READONLY:
			{
				m_dwFlags	&= ~(0x0001);
				m_dwFlags	|= 0x0001;
			}
			break;
		case FILE_ATTRIBUTE_HIDDEN:
			{
				m_dwFlags	&= ~(0x0002);
				m_dwFlags	|= 0x0002;
			}
			break;
		case FILE_ATTRIBUTE_SYSTEM:
			{
				m_dwFlags	&= ~(0x0004);
				m_dwFlags	|= 0x0004;
			}
			break;
		}
	}

	EndDialog(IDYES);
}

void CConfirmDeleteX::OnBnClickedSkip()
{
	UpdateData(TRUE);

	if (m_bDoThisForAll)
	{
		switch (m_dwAttributes)
		{
		case FILE_ATTRIBUTE_READONLY:
			{
				m_dwFlags	&= ~(0x0100);
				m_dwFlags	|= 0x0100;
			}
			break;
		case FILE_ATTRIBUTE_HIDDEN:
			{
				m_dwFlags	&= ~(0x0200);
				m_dwFlags	|= 0x0200;
			}
			break;
		case FILE_ATTRIBUTE_SYSTEM:
			{
				m_dwFlags	&= ~(0x0400);
				m_dwFlags	|= 0x0400;
			}
			break;
		}
	}

	EndDialog(IDNO);
}

void CConfirmDeleteX::OnCancel()
{
	CDialogX::OnCancel();
}
