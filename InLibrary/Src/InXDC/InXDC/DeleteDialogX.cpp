// DeleteDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "InLibrary_Messages.h"
#include "DisplayX.h"
#include "DeleteDialogX.h"
#include "ConfirmDeleteX.h"
#include "FileSearchX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CDeleteDialogX, COptionsDialogX)

CDeleteDialogX::CDeleteDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_PROCESS_SINGLE_DIALOG, pParent)
{
	m_bConfirm		= TRUE;
	m_nCurrent		= 0;
	m_nTotal		= 0;
	m_pThread		= NULL;
	m_bAbort		= FALSE;
	m_bSafeToClose	= FALSE;
	m_bShowResult	= FALSE;
}

CDeleteDialogX::~CDeleteDialogX()
{
}

void CDeleteDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRCS_FILENAME, m_ctlFilename);
	DDX_Control(pDX, IDC_PRCS_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_PRCS_STATUS, m_ctlStatus);
}

BEGIN_MESSAGE_MAP(CDeleteDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DELETE_THREAD_FINISHED, &CDeleteDialogX::OnThreadFinished)
END_MESSAGE_MAP()

void CDeleteDialogX::SetConfirm(BOOL bConfirm)
{
	m_bConfirm	= bConfirm;
}

void CDeleteDialogX::AddFile(LPCTSTR lpszFile)
{
	CFileStatus	status;
	CString		strFile;

	strFile	= lpszFile;

	if ((strFile.GetLength() > 2) && (strFile.Right(1) == _T('\\')))
	{
		strFile	= strFile.Left(strFile.GetLength() - 1);
	}

	if (!CFile::GetStatus(strFile, status))
	{
		return;
	}

	if ((status.m_attribute & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
	{
		m_arrFiles.Add(strFile);
	}
	else
	{
		CFileSearchX	Search;
		int				i;

		m_arrFolders.Add(strFile);

		strFile	+= _T("\\*.*");

		Search.SetPartName(strFile);
		Search.DoSearch();

		for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
		{
			m_arrFiles.Add(Search.GetFullPath(&Search.m_arrFiles[i]));
		}

		for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
		{
			m_arrFolders.Add(Search.GetFullPath(&Search.m_arrFolders[i]));
		}
	}
}

void CDeleteDialogX::AddFiles(CStringArray& arrFiles)
{
	for (int i = 0; i < arrFiles.GetSize(); i++)
	{
		AddFile(arrFiles.GetAt(i));
	}
}

void CDeleteDialogX::ClearFiles()
{
	InitializeX();
}

void CDeleteDialogX::ShowResult(BOOL bShow)
{
	m_bShowResult	= bShow;
}

UINT CDeleteDialogX::_DeleteThread(LPVOID pParam)
{
	CDeleteDialogX*	pDialog	= (CDeleteDialogX*)pParam;

	ASSERT(pDialog);
	ASSERT(pDialog->IsKindOf(RUNTIME_CLASS(CDeleteDialogX)));
	pDialog->DeleteThread();
	return 0;
}

void CDeleteDialogX::InitializeX()
{
	if (m_arrFiles.GetSize())
	{
		m_arrFiles.RemoveAll();
	}

	if (m_arrFolders.GetSize())
	{
		m_arrFolders.RemoveAll();
	}
}

BOOL CDeleteDialogX::IsEnableDelete(LPCTSTR lpszFilename, DWORD& dwAttributes)
{
	dwAttributes	= GetFileAttributes(lpszFilename);

	if ((dwAttributes & FILE_ATTRIBUTE_INROM) == FILE_ATTRIBUTE_INROM)
	{
		return FALSE;
	}

	if ((dwAttributes & FILE_ATTRIBUTE_ROMMODULE) == FILE_ATTRIBUTE_ROMMODULE)
	{
		return FALSE;
	}

	if ((dwAttributes & FILE_ATTRIBUTE_TEMPORARY) == FILE_ATTRIBUTE_TEMPORARY)
	{
		return FALSE;
	}

	if (dwAttributes & FILE_ATTRIBUTE_ROMSTATICREF)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDeleteDialogX::ExecDelete(LPCTSTR lpszFileName, DWORD& dwAttributes)
{
	if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return RemoveDirectory(lpszFileName);
	}

	SetFileAttributes(lpszFileName, FILE_ATTRIBUTE_NORMAL);

	return DeleteFile(lpszFileName);
}

void CDeleteDialogX::UpdateUI(LPCTSTR lpszFilename)
{
	CString	strMessage;
	CString	strName;
	CString	strCurrent;
	CString	strTotal;

	TakeName(lpszFilename, strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	strName.ReleaseBuffer();
	AfxFormatString1(strMessage, IDS_DELE_DELETING, strName);
	m_ctlFilename.SetWindowText(strMessage);
	strCurrent.Format(_T("%d"), m_nTotal - m_nCurrent);
	strTotal.Format(_T("%d"), m_nTotal);
	AfxFormatString2(strMessage, IDS_DELE_REMAIN, strCurrent, strTotal);
	m_ctlStatus.SetWindowText(strMessage);
	m_ctlProgress.SetPos(m_nCurrent);
}

void CDeleteDialogX::DeleteThread()
{
	CString			strMessage;
	CString			strTitle;
	CString			strName;
	BOOL			bComplete	= TRUE;
	DWORD			dwFlags		= 0x0000;
	DWORD			dwAttributes;
	int				nReturn;
	CFileStatus		status;
	CConfirmDeleteX	dConfirm;

	m_nCurrent	= 0;
	m_nTotal	= m_arrFiles.GetSize() + m_arrFolders.GetSize();

	if (m_bConfirm)
	{
		CString	strTotal;

		if (m_arrFolders.GetCount() + m_arrFiles.GetCount() == 1)
		{
			strMessage.LoadString(IDS_DELE_CONFIRM_ONE);
		}
		else
		{
			strTotal.Format(_T("%d"), m_arrFolders.GetCount() + m_arrFiles.GetCount());
			AfxFormatString1(strMessage, IDS_DELE_CONFIRM_MULTI, strTotal);
		}

		strTitle.LoadString(IDS_DELE_CONFIRM_TITLE);

		if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) != IDYES)
		{
			m_bAbort	= TRUE;

			PostMessage(WM_DELETE_THREAD_FINISHED);
			return;
		}
	}

	if (m_arrFolders.GetSize())
	{
		TakeName(m_arrFolders.GetAt(0), strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
		strName.ReleaseBuffer();
		AfxFormatString1(strMessage, IDS_DELE_FOLDER, strName);
		strTitle.LoadString(IDS_DELE_CONFIRM_TITLE);

		if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) != IDYES)
		{
			m_bAbort	= TRUE;

			PostMessage(WM_DELETE_THREAD_FINISHED);
			return;
		}
	}

	while (m_arrFiles.GetSize())
	{
		m_nCurrent++;

		UpdateUI(m_arrFiles.GetAt(m_arrFiles.GetUpperBound()));

		if (IsEnableDelete(m_arrFiles.GetAt(m_arrFiles.GetUpperBound()), dwAttributes))
		{
			if ((dwAttributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
			{
				if ((dwFlags & 0x0100) == 0x0100)
				{
					nReturn	= IDNO;
				}
				else if ((dwFlags & 0x0001) == 0x0001)
				{
					nReturn	= IDYES;
				}
				else
				{
					dConfirm.InitializeX();
					dConfirm.SetInfo(m_arrFiles.GetAt(m_arrFiles.GetUpperBound()), FILE_ATTRIBUTE_READONLY, dwFlags);

					nReturn	= dConfirm.DoModal();
					dwFlags	= dConfirm.GetInfo();
				}
			}
			else if ((dwAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN)
			{
				if ((dwFlags & 0x0200) == 0x0200)
				{
					nReturn	= IDNO;
				}
				else if ((dwFlags & 0x0002) == 0x0002)
				{
					nReturn	= IDYES;
				}
				else
				{
					dConfirm.InitializeX();
					dConfirm.SetInfo(m_arrFiles.GetAt(m_arrFiles.GetUpperBound()), FILE_ATTRIBUTE_HIDDEN, dwFlags);

					nReturn	= dConfirm.DoModal();
					dwFlags	= dConfirm.GetInfo();
				}
			}
			else if ((dwAttributes & FILE_ATTRIBUTE_SYSTEM) == FILE_ATTRIBUTE_SYSTEM)
			{
				if ((dwFlags & 0x0400) == 0x0400)
				{
					nReturn	= IDNO;
				}
				else if ((dwFlags & 0x0004) == 0x0004)
				{
					nReturn	= IDYES;
				}
				else
				{
					dConfirm.InitializeX();
					dConfirm.SetInfo(m_arrFiles.GetAt(m_arrFiles.GetUpperBound()), FILE_ATTRIBUTE_SYSTEM, dwFlags);

					nReturn	= dConfirm.DoModal();
					dwFlags	= dConfirm.GetInfo();
				}
			}
			else
			{
				nReturn	= IDYES;
			}

			if (nReturn == IDYES)
			{
				ExecDelete(m_arrFiles.GetAt(m_arrFiles.GetUpperBound()), dwAttributes);
			}

			if (nReturn == IDNO)
			{
				bComplete	= FALSE;
			}

			if (nReturn == IDCANCEL)
			{
				m_bAbort	= TRUE;
			}
		}

		m_arrFiles.RemoveAt(m_arrFiles.GetUpperBound());

		if (m_bAbort)
		{
			PostMessage(WM_DELETE_THREAD_FINISHED);
			return;
		}
	}

	while (m_arrFolders.GetSize())
	{
		m_nCurrent++;

		UpdateUI(m_arrFolders.GetAt(m_arrFolders.GetUpperBound()));

		if (IsEnableDelete(m_arrFolders.GetAt(m_arrFolders.GetUpperBound()), dwAttributes))
		{
			ExecDelete(m_arrFolders.GetAt(m_arrFolders.GetUpperBound()), dwAttributes);
		}

		m_arrFolders.RemoveAt(m_arrFolders.GetUpperBound());

		if (m_bAbort)
		{
			PostMessage(WM_DELETE_THREAD_FINISHED);
			return;
		}
	}

	if (m_bShowResult)
	{
		strTitle.LoadString(IDS_DELE_HEADER);
		strMessage.LoadString(bComplete ? IDS_DELE_COMPLETE : IDS_DELE_INCOMPLETE);
		MessageBox(strMessage, strTitle, MB_OK | (bComplete ? MB_ICONINFORMATION : MB_ICONERROR));
	}

	PostMessage(WM_DELETE_THREAD_FINISHED);
}

BOOL CDeleteDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	SetHeader(IDS_DELE_HEADER);

	int		nTotal	= m_arrFiles.GetSize() + m_arrFolders.GetSize();
	CString	strMessage;

	if (nTotal < 1)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	m_ctlProgress.SetRange32(0, nTotal);
	strMessage.LoadString(IDS_DELE_PREPARING);
	m_ctlFilename.SetWindowText(strMessage);
	strMessage.LoadString(IDS_DELE_CALCULATING);
	m_ctlStatus.SetWindowText(strMessage);

	m_pThread	= AfxBeginThread(_DeleteThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	if (m_pThread == NULL)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	HideDoneButton();

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

	m_pThread->m_bAutoDelete	= FALSE;

	m_pThread->ResumeThread();
	return TRUE;
}

void CDeleteDialogX::OnDestroy()
{
	ShowDoneButton();

	if (m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		delete m_pThread;

		m_pThread	= NULL;
	}

	COptionsDialogX::OnDestroy();
}

void CDeleteDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlFilename.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlFilename.MoveWindow(&rc);

	m_ctlProgress.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlProgress.MoveWindow(&rc);

	m_ctlStatus.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlStatus.MoveWindow(&rc);
}

void CDeleteDialogX::OnOK()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CDeleteDialogX::OnCancel()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CDeleteDialogX::OnClose()
{
	if (m_bSafeToClose)
	{
		COptionsDialogX::OnClose();
	}
	else
	{
		m_bAbort	= TRUE;

		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	}
}

LRESULT CDeleteDialogX::OnThreadFinished(WPARAM wParam, LPARAM lParam)
{
	m_bSafeToClose	= TRUE;

	InitializeX();

	if (m_bAbort)
	{
		EndDialog(IDCANCEL);
	}
	else if (wParam)
	{
		EndDialog(IDCANCEL);
	}
	else
	{
		EndDialog(IDOK);
	}

	return 0L;
}
