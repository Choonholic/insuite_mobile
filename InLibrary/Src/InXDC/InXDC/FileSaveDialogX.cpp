// FileSaveDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "FileSaveDialogX.h"
#include "FolderDialogX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CFileSaveDialogX, COptionsDialogX)

#define DEFAULT_MYDOCUMENTS	_T("\\My Documents")

CFileSaveDialogX::CFileSaveDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_SAVE_DIALOG, pParent), m_strName(_T("")), m_nType(0), m_bSetOverwrite(TRUE), m_bAskOverwrite(TRUE)
{
	BOOL	bFound	= GetShellFolders(SHFOLDER_MY_DOCUMENTS, m_strFolder.GetBuffer(MAX_PATH), MAX_PATH);

	m_strFolder.ReleaseBuffer();

	if (bFound == FALSE)
	{
		m_strFolder	= DEFAULT_MYDOCUMENTS;
	}

	SetHeader(IDS_SAVE_HEADER);
}

CFileSaveDialogX::~CFileSaveDialogX()
{
}

void CFileSaveDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);

#if (!defined(_DEBUG))
	DDX_Control(pDX, IDC_SAVE_NAME, m_ctlName);
	DDX_Control(pDX, IDC_SAVE_FOLDER, m_ctlFolder);
#endif

	DDX_Control(pDX, IDC_SAVE_NAME_H, m_ctlNameH);
	DDX_Control(pDX, IDC_SAVE_FOLDER_H, m_ctlFolderH);
	DDX_Control(pDX, IDC_SAVE_BROWSE, m_ctlBrowse);
	DDX_Control(pDX, IDC_SAVE_TYPE_H, m_ctlTypeH);
	DDX_Control(pDX, IDC_SAVE_TYPE, m_ctlType);
	DDX_CBIndex(pDX, IDC_SAVE_TYPE, m_nType);
	DDX_Text(pDX, IDC_SAVE_NAME, m_strName);
	DDX_Text(pDX, IDC_SAVE_FOLDER, m_strFolder);
}

BEGIN_MESSAGE_MAP(CFileSaveDialogX, COptionsDialogX)
	ON_BN_CLICKED(IDC_SAVE_BROWSE, &CFileSaveDialogX::OnBnClickedSaveBrowse)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CFileSaveDialogX::SetTypePairs(LPCTSTR lpszTypePairs)
{
	CString	strBase, strParsed;
	int		nPos;
	BOOL	bType	= FALSE;

	m_arrTypes.RemoveAll();
	m_arrDescriptions.RemoveAll();

	strBase	= lpszTypePairs;

	while (strBase.GetLength())
	{
		if ((nPos = strBase.Find(_T('|'))) != -1)
		{
			strParsed	= strBase.Left(nPos);
			strBase		= strBase.Mid(nPos + 1);
		}
		else
		{
			strParsed	= strBase;
			strBase		= _T("");
		}

		if (strParsed.GetLength())
		{
			if (bType == FALSE)
			{
				m_arrTypes.Add(strParsed);
			}
			else
			{
				m_arrDescriptions.Add(strParsed);
			}

			bType	= (BOOL)(!bType);
		}
	}
}

void CFileSaveDialogX::SetTypePairs(UINT nID)
{
	CString	strTypePairs;

	strTypePairs.LoadString(nID);
	SetTypePairs(strTypePairs);
}

void CFileSaveDialogX::SetDefaultFilename(LPCTSTR lpszFilename)
{
	m_strName	= lpszFilename;
}

void CFileSaveDialogX::SetDefaultFilename(UINT nID)
{
	m_strName.LoadString(nID);
}

void CFileSaveDialogX::SetOverwrite(BOOL bSet)
{
	m_bSetOverwrite	= bSet;
}

void CFileSaveDialogX::AskOverwrite(BOOL bSet)
{
	m_bAskOverwrite	= bSet;
}

CString CFileSaveDialogX::GetFullPath()
{
	CString	strFullPath;

	strFullPath.Format(_T("%s%s%s%s"), m_strFolder, ((m_strFolder != _T("\\")) ? _T("\\") : _T("")), m_strName, m_strExtension);
	return strFullPath;
}

CString	CFileSaveDialogX::GetPath()
{
	CString	strPath;

	if (m_strFolder != _T("\\"))
	{
		strPath	= m_strFolder.Left(m_strFolder.GetLength() - 1);
	}
	else
	{
		strPath	= m_strFolder;
	}

	return strPath;
}

CString CFileSaveDialogX::GetFile()
{
	CString	strFile;

	strFile	= m_strName + m_strExtension;

	return strFile;
}

CString	CFileSaveDialogX::GetTitle()
{
	return m_strName;
}

CString	CFileSaveDialogX::GetExtension()
{
	return m_strExtension;
}

BOOL CFileSaveDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_SAVE);

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

	m_ctlType.ResetContent();
	ASSERT(m_arrDescriptions.GetSize() > 0);

	for (int i = 0; i < m_arrDescriptions.GetSize(); i++)
	{
		m_ctlType.AddString(m_arrDescriptions.GetAt(i));
	}

	m_nType	= 0;

	UpdateData(FALSE);
	return TRUE;
}

void CFileSaveDialogX::OnOK()
{
	CString		strFullPath;
	CFileStatus	fs;

	UpdateData(TRUE);

	if (m_strName.GetLength() < 1)
	{
		return;
	}

	m_strExtension	= m_arrTypes.GetAt(m_nType);

	strFullPath.Format(_T("%s%s%s%s"), m_strFolder, ((m_strFolder != _T("\\")) ? _T("\\") : _T("")), m_strName, m_strExtension);

	if (CFile::GetStatus(strFullPath, fs))
	{
		if (m_bSetOverwrite == FALSE)
		{
			return;
		}

		if (m_bAskOverwrite == TRUE)
		{
			CString	strMessage;
			CString	strTitle;

			strMessage.Format(IDS_SAVE_OVERWRITE, strFullPath);
			strTitle.LoadString(IDS_SAVE_TITLE);

			if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) != IDYES)
			{
				return;
			}
		}
	}

	COptionsDialogX::OnOK();
}

void CFileSaveDialogX::OnBnClickedSaveBrowse()
{
	CFolderDialogX	dFolder;

	UpdateData(TRUE);
	dFolder.SetDefaultPath(m_strFolder);
	dFolder.SetHeader(IDS_SAVE_FOLDER_HEADER);

	if (dFolder.DoModal() == IDOK)
	{
		m_strFolder	= dFolder.GetFullPath();
	}

	UpdateData(FALSE);
}

void CFileSaveDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rcEdit;
	RECT	rcText;
	RECT	rcButton;
	SIZE	size;

#if (defined(_DEBUG))
	CWnd*	pEdit	= NULL;
#endif

#if (!defined(_DEBUG))
	m_ctlName.GetWindowRect(&rcEdit);
	ScreenToClient(&rcEdit);

	size.cy			= rcEdit.bottom - rcEdit.top;
	rcEdit.right	= cx - DRA::SCALEX(3);

	m_ctlName.MoveWindow(&rcEdit);
#else
	pEdit	= GetDlgItem(IDC_SAVE_NAME);

	if (pEdit != NULL)
	{
		pEdit->GetWindowRect(&rcEdit);
		ScreenToClient(&rcEdit);

		size.cy			= rcEdit.bottom - rcEdit.top;
		rcEdit.right	= cx - DRA::SCALEX(3);

		pEdit->MoveWindow(&rcEdit);
	}
#endif

	m_ctlNameH.GetWindowRect(&rcText);
	ScreenToClient(&rcText);

	rcText.top		= rcEdit.top;
	rcText.bottom	= rcEdit.bottom;

	m_ctlNameH.MoveWindow(&rcText);

	m_ctlBrowse.GetWindowRect(&rcButton);
	ScreenToClient(&rcButton);

	size.cx				= rcButton.right - rcButton.left;
	size.cy				= rcButton.bottom - rcButton.top;
	rcButton.right		= cx - DRA::SCALEX(3);
	rcButton.left		= rcButton.right - size.cx;

	m_ctlBrowse.MoveWindow(&rcButton);

#if (!defined(_DEBUG))
	m_ctlFolder.GetWindowRect(&rcEdit);
	ScreenToClient(&rcEdit);

	rcEdit.right		= rcButton.left - DRA::SCALEX(3);
	rcEdit.top			= rcButton.top;
	rcEdit.bottom		= rcButton.bottom;

	m_ctlFolder.MoveWindow(&rcEdit);
#else
	pEdit	= GetDlgItem(IDC_SAVE_FOLDER);

	if (pEdit != NULL)
	{
		pEdit->GetWindowRect(&rcEdit);
		ScreenToClient(&rcEdit);

		rcEdit.right		= rcButton.left - DRA::SCALEX(3);
		rcEdit.top			= rcButton.top;
		rcEdit.bottom		= rcButton.bottom;

		pEdit->MoveWindow(&rcEdit);
	}
#endif

	m_ctlFolderH.GetWindowRect(&rcText);
	ScreenToClient(&rcText);

	rcText.top		= rcEdit.top;
	rcText.bottom	= rcEdit.bottom;

	m_ctlFolderH.MoveWindow(&rcText);

	m_ctlType.GetWindowRect(&rcEdit);
	ScreenToClient(&rcEdit);

	size.cy			= rcEdit.bottom - rcEdit.top;
	rcEdit.right	= cx - DRA::SCALEX(3);

	m_ctlType.MoveWindow(&rcEdit);

	m_ctlTypeH.GetWindowRect(&rcText);
	ScreenToClient(&rcText);

	rcText.top		= rcEdit.top;
	rcText.bottom	= rcEdit.bottom;

	m_ctlTypeH.MoveWindow(&rcText);
}
