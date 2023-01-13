// PropertiesDialog.cpp

#include "stdafx.h"
#include "InFont.h"
#include "DisplayX.h"
#include "PropertiesDialog.h"

IMPLEMENT_DYNAMIC(CPropertiesDialog, COptionsDialogX)

CPropertiesDialog::CPropertiesDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CPropertiesDialog::IDD, pParent)
{
}

CPropertiesDialog::~CPropertiesDialog()
{
	if (m_PreviewFont.m_hObject)
	{
		m_PreviewFont.DeleteObject();
	}
}

void CPropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PR_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_PR_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_PR_COPYRIGHT, m_strCopyright);
	DDX_Control(pDX, IDC_PR_FILENAME, m_ctlFileName);
	DDX_Control(pDX, IDC_PR_VERSION, m_ctlVersion);
	DDX_Control(pDX, IDC_PR_COPYRIGHT, m_ctlCopyright);
	DDX_Control(pDX, IDC_PR_PREVIEW, m_ctlPreview);
}

BEGIN_MESSAGE_MAP(CPropertiesDialog, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CPropertiesDialog::SetPreviewFont()
{
	if (m_PreviewFont.m_hObject)
	{
		m_PreviewFont.DeleteObject();
	}

	if (m_PreviewFont.CreatePointFont(140, m_strFontFace))
	{
		m_ctlPreview.SetFont(&m_PreviewFont);
	}
}

BOOL CPropertiesDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_CLOSE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	m_strCopyright.Replace(_T("&"), _T("&&"));

	CString	strPreview;

	strPreview.LoadString(m_nEncodingID ? IDS_PREVIEW : IDS_PREVIEW_SYMBOLS);
	m_ctlPreview.SetWindowText(strPreview);
	SetPreviewFont();
	UpdateData(FALSE);
	return TRUE;
}

void CPropertiesDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlPreview.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.left;
	rc.right	= cx - nLeft;
	rc.bottom	= cy - nLeft;

	m_ctlPreview.MoveWindow(&rc);

	m_ctlFileName.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlFileName.MoveWindow(&rc);

	m_ctlVersion.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlVersion.MoveWindow(&rc);

	m_ctlCopyright.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlCopyright.MoveWindow(&rc);
}
