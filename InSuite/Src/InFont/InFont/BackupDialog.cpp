// BackupDialog.cpp

#include "stdafx.h"
#include "InFont.h"
#include "InFont_Settings.h"
#include "DisplayX.h"
#include "FileSaveDialogX.h"
#include "FileDialogX.h"
#include "RegistryX.h"
#include "SystemX.h"
#include "BackupDialog.h"
#include "FontInfo.h"

IMPLEMENT_DYNAMIC(CBackupDialog, COptionsDialogX)

CBackupDialog::CBackupDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CBackupDialog::IDD, pParent)
{
}

CBackupDialog::~CBackupDialog()
{
}

void CBackupDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BR_BACKUP_H, m_ctlBackupH);
	DDX_Control(pDX, IDC_BR_BACKUP, m_ctlBackup);
	DDX_Control(pDX, IDC_BR_RESTORE_H, m_ctlRestoreH);
	DDX_Control(pDX, IDC_BR_RESTORE, m_ctlRestore);
}

BEGIN_MESSAGE_MAP(CBackupDialog, COptionsDialogX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BR_BACKUP, &CBackupDialog::OnBnClickedBrBackup)
	ON_BN_CLICKED(IDC_BR_RESTORE, &CBackupDialog::OnBnClickedBrRestore)
END_MESSAGE_MAP()

BOOL CBackupDialog::OnInitDialog()
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
	return TRUE;
}

void CBackupDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	RECT	rcButton;
	int		nLeft;
	int		nWidth;

	m_ctlBackupH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft	= rc.left;

	m_ctlBackup.GetWindowRect(&rcButton);
	ScreenToClient(&rcButton);

	nWidth			= rcButton.right - rcButton.left;
	rcButton.right	= cx - nLeft;
	rcButton.left	= rcButton.right - nWidth;

	m_ctlBackup.MoveWindow(&rcButton);

	rc.right	= rcButton.left - nLeft;

	m_ctlBackupH.MoveWindow(&rc);

	m_ctlRestore.GetWindowRect(&rcButton);
	ScreenToClient(&rcButton);

	rcButton.right	= cx - nLeft;
	rcButton.left	= rcButton.right - nWidth;

	m_ctlRestore.MoveWindow(&rcButton);

	m_ctlRestoreH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= rcButton.left - nLeft;

	m_ctlRestoreH.MoveWindow(&rc);
}

void CBackupDialog::OnBnClickedBrBackup()
{
	CFileSaveDialogX	dBackup;
	CString				strDefault;
	SYSTEMTIME			stCurrentTime;

	GetLocalTime(&stCurrentTime);
	strDefault.Format(_T("IF_%04d%02d%02d_%02d%02d%02d"), stCurrentTime.wYear, stCurrentTime.wMonth, stCurrentTime.wDay, stCurrentTime.wHour, stCurrentTime.wMinute, stCurrentTime.wSecond);
	dBackup.SetHeader(IDS_BACKUP_SELECT);
	dBackup.SetTypePairs(IDS_BACKUP_FILTERS);
	dBackup.SetDefaultFilename(strDefault);

	if (dBackup.DoModal() == IDOK)
	{
		CRegistryX	reg;
		CString		strTitle;
		CString		strMessage;
		UINT		uiType	= MB_OK | MB_ICONINFORMATION;

		strTitle.LoadString(IDR_MAINFRAME);
		strMessage.LoadString(IDS_BACKUP_SUCCESS);
		reg.Open(HKEY_LOCAL_MACHINE, FONTLINK_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath()))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		reg.Open(HKEY_LOCAL_MACHINE, FONTPATH_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath(), TRUE, FALSE))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		reg.Open(HKEY_CURRENT_USER, EUDC_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath(), TRUE, FALSE))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		reg.Open(HKEY_LOCAL_MACHINE, GDI_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath(), FALSE, FALSE))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		reg.Open(HKEY_LOCAL_MACHINE, SYSFNT_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath(), TRUE, FALSE))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		reg.Open(HKEY_LOCAL_MACHINE, MENUFNT_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath(), TRUE, FALSE))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		reg.Open(HKEY_LOCAL_MACHINE, OOMFNT_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath(), TRUE, FALSE))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		reg.Open(HKEY_LOCAL_MACHINE, BUTTONFNT_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath(), TRUE, FALSE))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		reg.Open(HKEY_LOCAL_MACHINE, FONTALIAS_KEY);

		if (!reg.REG5Export(dBackup.GetFullPath(), TRUE, FALSE))
		{
			strMessage.LoadString(IDS_BACKUP_FAILED);

			uiType	= MB_OK | MB_ICONERROR;
		}

		reg.Close();
		MessageBox(strMessage, strTitle, uiType);
	}
}

void CBackupDialog::OnBnClickedBrRestore()
{
	CFileDialogX	dRestore;

	dRestore.SetHeader(IDS_RESTORE_SELECT);
	dRestore.SetFilters(_T("*.reg"));

	if (dRestore.DoModal() == IDOK)
	{
		CRegistryX	reg;
		CString		strTitle;
		CString		strMessage;
		DWORD		dwFlags;

		strTitle.LoadString(IDR_MAINFRAME);
		reg.DeleteKey(HKEY_LOCAL_MACHINE, SYSLINK_KEY);
		reg.DeleteKey(HKEY_LOCAL_MACHINE, SKIP_KEY);
		reg.Open(HKEY_LOCAL_MACHINE, FONTPATH_KEY);
		reg.DeleteValue(FONTPATH_VALUE);
		reg.Close();

		if (reg.REG5Import(dRestore.GetFullPath()))
		{
			strMessage.LoadString(IDS_RESTORE_SUCCESS);

			if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONQUESTION) == IDYES)
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
		else
		{
			strMessage.LoadString(IDS_RESTORE_FAILED);
			MessageBox(strMessage, strTitle, MB_OK | MB_ICONERROR);
		}
	}
}
