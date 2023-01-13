// PropertiesAttributesDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "PropertiesAttributesDialogX.h"
#include "DisplayX.h"

IMPLEMENT_DYNAMIC(CPropertiesAttributesDialogX, COptionsDialogX)

CPropertiesAttributesDialogX::CPropertiesAttributesDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_PROPATTRIBUTES_DIALOG, pParent), m_bReadOnly(FALSE), m_bHidden(FALSE), m_bArchive(FALSE), m_bSystem(FALSE), m_bROM(FALSE)
{
	SetHeader(IDS_PROP_ATTRIBUTES);
}

CPropertiesAttributesDialogX::~CPropertiesAttributesDialogX()
{
}

void CPropertiesAttributesDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROP_READONLY, m_ctlReadOnly);
	DDX_Control(pDX, IDC_PROP_HIDDEN, m_ctlHidden);
	DDX_Control(pDX, IDC_PROP_ARCHIVE, m_ctlArchive);
	DDX_Control(pDX, IDC_PROP_SYSTEM, m_ctlSystem);
	DDX_Control(pDX, IDC_PROP_ROM, m_ctlROM);
	DDX_Check(pDX, IDC_PROP_READONLY, m_bReadOnly);
	DDX_Check(pDX, IDC_PROP_HIDDEN, m_bHidden);
	DDX_Check(pDX, IDC_PROP_ARCHIVE, m_bArchive);
	DDX_Check(pDX, IDC_PROP_SYSTEM, m_bSystem);
	DDX_Check(pDX, IDC_PROP_ROM, m_bROM);
}

BEGIN_MESSAGE_MAP(CPropertiesAttributesDialogX, COptionsDialogX)
END_MESSAGE_MAP()

BOOL CPropertiesAttributesDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_OKAY);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_MENU_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	m_ctlROM.EnableWindow(FALSE);
	return TRUE;
}
