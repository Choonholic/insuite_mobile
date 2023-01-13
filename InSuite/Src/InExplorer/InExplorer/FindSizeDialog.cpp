// FindSizeDialog.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "FindSizeDialog.h"
#include "DisplayX.h"

IMPLEMENT_DYNAMIC(CFindSizeDialog, COptionsDialogX)

CFindSizeDialog::CFindSizeDialog(CWnd* pParent) : COptionsDialogX(CFindSizeDialog::IDD, pParent), m_bLarger(FALSE), m_dwLarger(0), m_bSmaller(FALSE), m_dwSmaller(0x40000000)
{
}

CFindSizeDialog::~CFindSizeDialog()
{
}

void CFindSizeDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIND_SIZELARGER, m_ctlLarger);
	DDX_Control(pDX, IDC_FIND_SIZELARGEREDIT, m_ctlLargerEdit);
	DDX_Control(pDX, IDC_FIND_SIZELARGERSPIN, m_ctlLargerSpin);
	DDX_Control(pDX, IDC_FIND_SIZESMALLER, m_ctlSmaller);
	DDX_Control(pDX, IDC_FIND_SIZESMALLEREDIT, m_ctlSmallerEdit);
	DDX_Control(pDX, IDC_FIND_SIZESMALLERSPIN, m_ctlSmallerSpin);
	DDX_Check(pDX, IDC_FIND_SIZELARGER, m_bLarger);
	DDX_Text(pDX, IDC_FIND_SIZELARGEREDIT, m_dwLarger);
	DDX_Check(pDX, IDC_FIND_SIZESMALLER, m_bSmaller);
	DDX_Text(pDX, IDC_FIND_SIZESMALLEREDIT, m_dwSmaller);
}

BEGIN_MESSAGE_MAP(CFindSizeDialog, COptionsDialogX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FIND_SIZELARGER, &CFindSizeDialog::OnBnClickedFindSizelarger)
	ON_BN_CLICKED(IDC_FIND_SIZESMALLER, &CFindSizeDialog::OnBnClickedFindSizesmaller)
END_MESSAGE_MAP()

void CFindSizeDialog::UpdateControls()
{
	UpdateData(TRUE);
	m_ctlLargerEdit.EnableWindow(m_bLarger);
	m_ctlLargerSpin.EnableWindow(m_bLarger);
	m_ctlSmallerEdit.EnableWindow(m_bSmaller);
	m_ctlSmallerSpin.EnableWindow(m_bSmaller);
}

BOOL CFindSizeDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	m_ctlLargerSpin.SetRange32(0, 0x7FFFFFFF);
	m_ctlSmallerSpin.SetRange32(0, 0x7FFFFFFF);
	UpdateControls();
	return TRUE;
}

void CFindSizeDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nWidth;
	int		nLeft;

	m_ctlLarger.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlLargerSpin.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nWidth		= rc.right - rc.left;
	rc.right	= cx - nLeft;
	rc.left		= rc.right - nWidth;

	m_ctlLargerSpin.MoveWindow(&rc);

	m_ctlLargerEdit.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft - nWidth + DRA::SCALEX(1);

	m_ctlLargerEdit.MoveWindow(&rc);

	m_ctlSmallerSpin.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;
	rc.left		= rc.right - nWidth;

	m_ctlSmallerSpin.MoveWindow(&rc);

	m_ctlSmallerEdit.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft - nWidth + DRA::SCALEX(1);

	m_ctlSmallerEdit.MoveWindow(&rc);
}

void CFindSizeDialog::OnBnClickedFindSizelarger()
{
	UpdateControls();
}

void CFindSizeDialog::OnBnClickedFindSizesmaller()
{
	UpdateControls();
}
