// ExportRange.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "DisplayX.h"
#include "ExportRange.h"

IMPLEMENT_DYNAMIC(CExportRange, COptionsDialogX)

CExportRange::CExportRange(CWnd* pParent /*=NULL*/) : COptionsDialogX(CExportRange::IDD, pParent), m_nExportRange(0), m_strSelectedBrench(_T(""))
{
}

CExportRange::~CExportRange()
{
}

void CExportRange::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXP_ALL, m_ctlExportAll);
	DDX_Control(pDX, IDC_EXP_BRENCH, m_ctlSelectedBrench);
	DDX_Radio(pDX, IDC_EXP_ALL, m_nExportRange);
	DDX_Text(pDX, IDC_EXP_BRENCH, m_strSelectedBrench);
}

BEGIN_MESSAGE_MAP(CExportRange, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CExportRange::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_EXPORT);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);
	return TRUE;
}

void CExportRange::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlExportAll.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlSelectedBrench.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlSelectedBrench.MoveWindow(&rc);
}
