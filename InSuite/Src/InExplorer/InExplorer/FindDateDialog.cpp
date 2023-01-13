// FindDateDialog.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "FindDateDialog.h"
#include "DisplayX.h"

IMPLEMENT_DYNAMIC(CFindDateDialog, COptionsDialogX)

CFindDateDialog::CFindDateDialog(CWnd* pParent) : COptionsDialogX(CFindDateDialog::IDD, pParent), m_bFrom(FALSE), m_bTo(FALSE)
{
}

CFindDateDialog::~CFindDateDialog()
{
}

void CFindDateDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIND_DATEFROM, m_ctlFrom);
	DDX_Control(pDX, IDC_FIND_DATEFROMPICK, m_ctlFromPicker);
	DDX_Control(pDX, IDC_FIND_DATETO, m_ctlTo);
	DDX_Control(pDX, IDC_FIND_DATETOPICK, m_ctlToPicker);
	DDX_Check(pDX, IDC_FIND_DATEFROM, m_bFrom);
	DDX_Check(pDX, IDC_FIND_DATETO, m_bTo);
}

BEGIN_MESSAGE_MAP(CFindDateDialog, COptionsDialogX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FIND_DATEFROM, &CFindDateDialog::OnBnClickedFindDatefrom)
	ON_BN_CLICKED(IDC_FIND_DATETO, &CFindDateDialog::OnBnClickedFindDateto)
END_MESSAGE_MAP()

void CFindDateDialog::UpdateControls()
{
	UpdateData(TRUE);
	m_ctlFromPicker.EnableWindow(m_bFrom);
	m_ctlToPicker.EnableWindow(m_bTo);
}

BOOL CFindDateDialog::OnInitDialog()
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

	UpdateControls();
	return TRUE;
}

void CFindDateDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlFrom.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlFromPicker.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlFromPicker.MoveWindow(&rc);

	m_ctlToPicker.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlToPicker.MoveWindow(&rc);
}

void CFindDateDialog::OnBnClickedFindDatefrom()
{
	UpdateControls();
}

void CFindDateDialog::OnBnClickedFindDateto()
{
	UpdateControls();
}

void CFindDateDialog::OnOK()
{
	m_ctlFromPicker.GetTime(&m_stFrom);
	m_ctlToPicker.GetTime(&m_stTo);
}
