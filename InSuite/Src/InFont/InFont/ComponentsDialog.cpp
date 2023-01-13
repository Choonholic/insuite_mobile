// ComponentsDialog.cpp

#include "stdafx.h"
#include "InFont.h"
#include "InFont_Settings.h"
#include "ComponentsDialog.h"
#include "RegistryMFCX.h"
#include "FontDialogX.h"
#include "FontX.h"
#include "SystemX.h"
#include "ComponentTags.h"

IMPLEMENT_DYNAMIC(CComponentsDialog, COptionsDialogX)

CComponentsDialog::CComponentsDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CComponentsDialog::IDD, pParent), m_bModified(FALSE)
{
}

CComponentsDialog::~CComponentsDialog()
{
}

void CComponentsDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CO_FONTS, m_ctlList);
}

BEGIN_MESSAGE_MAP(CComponentsDialog, COptionsDialogX)
	ON_WM_SIZE()
	ON_COMMAND(ID_MODIFY_COMPONENT, &CComponentsDialog::OnModifyComponent)
END_MESSAGE_MAP()

void CComponentsDialog::SetItems()
{
	m_arrItems.RemoveAll();
	m_arrItems.Add(TAG_DEFAULT_FONT);
	m_arrItems.Add(TAG_SYSTEM_FONT);
	m_arrItems.Add(TAG_POPUP_FONT);
	m_arrItems.Add(TAG_MENUBAR_FONT);
	m_arrItems.Add(TAG_OOM_FONT);
	m_arrItems.Add(TAG_BUTTON_FONT);

	m_ctlList.ResetContent();

	for (int i = 0; i < m_arrItems.GetCount(); i++)
	{
		m_ctlList.AddString(m_arrItems.GetAt(i));
	}
}

void CComponentsDialog::Refresh()
{
	SetItems();
	Invalidate();
}

BOOL CComponentsDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MODIFY);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= ID_MODIFY_COMPONENT;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_CLOSE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	m_bModified	= FALSE;

	SetItems();
	return TRUE;
}

void CComponentsDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;

	rc.left		= DRA::SCALEX(0);
	rc.top		= GetHeaderHeight() + DRA::SCALEY(1);
	rc.right	= cx - DRA::SCALEX(0);
	rc.bottom	= cy - DRA::SCALEY(1);

	m_ctlList.MoveWindow(&rc);
}

void CComponentsDialog::OnModifyComponent()
{
	ModifyComponent(m_ctlList.GetCurSel());
}

void CComponentsDialog::OnOK()
{
	CString		strTitle;
	CString		strMessage;
	CRegistryX	reg;
	DWORD		dwFlags;

	if (m_bModified == TRUE)
	{
		strTitle.LoadString(IDR_MAINFRAME);
		strMessage.LoadString(IDS_FINISH_RESET);

		if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONINFORMATION) == IDYES)
		{
			reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);
			
			if (reg.ReadDWORD(RV_FN_RESETMETHOD, 0))
			{
				dwFlags	= EWX_DEFER | EWX_PROMPT;
			}
			else
			{
				dwFlags	= 0;
			}

			reg.Close();
			Reset(dwFlags);
		}
	}

	COptionsDialogX::OnOK();
}

void CComponentsDialog::ModifyComponent(int nComponent)
{
	CFontDialogX	dFont;
	LOGFONT			lfFont;

	switch (nComponent)
	{
	case 0:
		{
			GetDefaultFont(&lfFont);
			dFont.SetHeader(IDS_DEFAULT_FONT);
			dFont.SetSrcFont(&lfFont);

			if (dFont.DoModal() == IDOK)
			{
				dFont.GetResFont(&lfFont);
				
				if (SetDefaultFont(&lfFont))
				{
					SetItems();

					m_bModified	= TRUE;
				}
			}
		}
		break;
	case 1:
		{
			GetSystemFont(&lfFont);
			dFont.SetHeader(IDS_SYSTEM_FONT);
			dFont.SetSrcFont(&lfFont);

			if (dFont.DoModal() == IDOK)
			{
				dFont.GetResFont(&lfFont);
				
				if (SetSystemFont(&lfFont))
				{
					SetItems();

					m_bModified	= TRUE;
				}
			}
		}
		break;
	case 2:
		{
			GetPopupMenuFont(&lfFont);
			dFont.SetHeader(IDS_POPUP_MENU_FONT);
			dFont.SetSrcFont(&lfFont);

			if (dFont.DoModal() == IDOK)
			{
				dFont.GetResFont(&lfFont);
				
				if (SetPopupMenuFont(&lfFont))
				{
					SetItems();

					m_bModified	= TRUE;
				}
			}
		}
		break;
	case 3:
		{
			GetMenuBarFont(&lfFont);
			dFont.SetHeader(IDS_MENU_BAR_FONT);
			dFont.SetSrcFont(&lfFont);

			if (dFont.DoModal() == IDOK)
			{
				dFont.GetResFont(&lfFont);
				
				if (SetMenuBarFont(&lfFont))
				{
					SetItems();

					m_bModified	= TRUE;
				}
			}
		}
		break;
	case 4:
		{
			GetOutOfMemoryFont(&lfFont);
			dFont.SetHeader(IDS_OOM_FONT);
			dFont.SetSrcFont(&lfFont);

			if (dFont.DoModal() == IDOK)
			{
				dFont.GetResFont(&lfFont);

				if (SetOutOfMemoryFont(&lfFont))
				{
					SetItems();

					m_bModified	= TRUE;
				}
			}
		}
		break;
	case 5:
		{
			GetButtonFont(&lfFont);
			dFont.SetHeader(IDS_BUTTON_FONT);
			dFont.SetSrcFont(&lfFont);

			if (dFont.DoModal() == IDOK)
			{
				dFont.GetResFont(&lfFont);
				
				if (SetButtonFont(&lfFont))
				{
					SetItems();

					m_bModified	= TRUE;
				}
			}
		}
		break;
	}
}
