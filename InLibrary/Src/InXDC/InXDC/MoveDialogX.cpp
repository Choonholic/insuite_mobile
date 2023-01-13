// MoveDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "InLibrary_Messages.h"
#include "DisplayX.h"
#include "MoveDialogX.h"
#include "ConfirmCopyX.h"
#include "ConfirmDeleteX.h"
#include "FileSearchX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CMoveDialogX, COptionsDialogX)

#ifndef MESSAGE_MOVING
#define MESSAGE_MOVING		0
#define MESSAGE_DELETING	1
#endif

CMoveDialogX::CMoveDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_PROCESS_DOUBLE_DIALOG, pParent)
{
	m_ullMoved		= 0;
	m_ullTotal		= 0;
	m_nCurrent		= 0;
	m_nTotal		= 0;
	m_pThread		= NULL;
	m_bAbort		= FALSE;
	m_bSafeToClose	= FALSE;
	m_bShowResult	= FALSE;
}

CMoveDialogX::~CMoveDialogX()
{
}

void CMoveDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRCD_FILENAME, m_ctlFilename);
	DDX_Control(pDX, IDC_PRCD_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_PRCD_TOTAL, m_ctlTotal);
	DDX_Control(pDX, IDC_PRCD_STATUS, m_ctlStatus);
}

BEGIN_MESSAGE_MAP(CMoveDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MOVE_THREAD_FINISHED, &CMoveDialogX::OnThreadFinished)
END_MESSAGE_MAP()

void CMoveDialogX::SetSource(LPCTSTR lpszFolder)
{
	m_strSource	= lpszFolder;

	if ((m_strSource.GetLength() > 1) && (m_strSource.Right(1) != _T("\\")))
	{
		m_strSource	+= _T("\\");
	}
}

void CMoveDialogX::SetDestination(LPCTSTR lpszFolder)
{
	m_strDestination	= lpszFolder;

	if ((m_strDestination.GetLength() > 1) && (m_strDestination.Right(1) != _T("\\")))
	{
		m_strDestination	+= _T("\\");
	}
}

void CMoveDialogX::AddFile(LPCTSTR lpszFile)
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
		m_arrFilesDelete.Add(strFile);

		m_ullTotal	+= status.m_size;
	}
	else
	{
		CFileSearchX	Search;
		int				i;

		m_arrFolders.Add(strFile);
		m_arrFoldersDelete.Add(strFile);

		m_ullTotal	+= 0;

		strFile += _T("\\*.*");

		Search.SetPartName(strFile);
		Search.DoSearch();

		for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
		{
			m_arrFiles.Add(Search.GetFullPath(&Search.m_arrFiles[i]));
			m_arrFilesDelete.Add(Search.GetFullPath(&Search.m_arrFiles[i]));

			m_ullTotal	+= Search.GetFileSize(&Search.m_arrFiles[i]);
		}

		for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
		{
			m_arrFolders.Add(Search.GetFullPath(&Search.m_arrFolders[i]));
			m_arrFoldersDelete.Add(Search.GetFullPath(&Search.m_arrFolders[i]));

			m_ullTotal	+= 0;
		}
	}
}

void CMoveDialogX::AddFiles(CStringArray& arrFiles)
{
	for (int i = 0; i < arrFiles.GetSize(); i++)
	{
		AddFile(arrFiles.GetAt(i));
	}
}

void CMoveDialogX::ClearFiles()
{
	InitializeX();
}

void CMoveDialogX::ShowResult(BOOL bShow)
{
	m_bShowResult	= bShow;
}

UINT CMoveDialogX::_MoveThread(LPVOID pParam)
{
	CMoveDialogX*	pDialog	= (CMoveDialogX*)pParam;

	ASSERT(pDialog);
	ASSERT(pDialog->IsKindOf(RUNTIME_CLASS(CMoveDialogX)));
	pDialog->MoveThread();
	return 0;
}

void CMoveDialogX::InitializeX()
{
	m_ullMoved	= 0;
	m_ullTotal	= 0;
	m_nCurrent	= 0;
	m_nTotal	= 0;

	if (m_arrFiles.GetSize())
	{
		m_arrFiles.RemoveAll();
	}

	if (m_arrFolders.GetSize())
	{
		m_arrFolders.RemoveAll();
	}

	if (m_arrFilesDelete.GetSize())
	{
		m_arrFilesDelete.RemoveAll();
	}

	if (m_arrFoldersDelete.GetSize())
	{
		m_arrFoldersDelete.RemoveAll();
	}
}

BOOL CMoveDialogX::IsEnableMove(LPCTSTR lpszFileName, DWORD& dwSize)
{
	CFileStatus	status;

	dwSize	= 0;

	if (!CFile::GetStatus(lpszFileName, status))
	{
		return FALSE;
	}

	if ((status.m_attribute & FILE_ATTRIBUTE_INROM) == FILE_ATTRIBUTE_INROM)
	{
		return FALSE;
	}

	if ((status.m_attribute & FILE_ATTRIBUTE_ROMMODULE) == FILE_ATTRIBUTE_ROMMODULE)
	{
		return FALSE;
	}

	if ((status.m_attribute & FILE_ATTRIBUTE_TEMPORARY) == FILE_ATTRIBUTE_TEMPORARY)
	{
		return FALSE;
	}

	if ((status.m_attribute & FILE_ATTRIBUTE_ROMSTATICREF) == FILE_ATTRIBUTE_ROMSTATICREF)
	{
		return FALSE;
	}

	dwSize	= (DWORD)(status.m_size);

	return TRUE;
}

BOOL CMoveDialogX::IsEnableDelete(LPCTSTR lpszFilename, DWORD& dwAttributes)
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

#define COPY_BLOCK	32768

void CMoveDialogX::ExecCopy(LPCTSTR lpszSource, LPCTSTR lpszTarget)
{
	CFileStatus	status;
	HANDLE		hSource;
	HANDLE		hTarget;
	DWORD		dwPos;
	DWORD		dwSize;
	DWORD		dwRead;
	DWORD		dwWritten;
	BYTE		cBuffer[COPY_BLOCK];

	if (CFile::GetStatus(lpszTarget, status))
	{
		if ((status.m_attribute & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
		{
			SetFileAttributes(lpszTarget, FILE_ATTRIBUTE_NORMAL);
			DeleteFile(lpszTarget);
		}
	}

	if (CFile::GetStatus(lpszSource, status))
	{
		if ((status.m_attribute & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			CreateDirectory(lpszTarget, NULL);
		}
		else
		{
			hSource	= CreateFile(lpszSource, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, status.m_attribute, NULL);
			hTarget	= CreateFile(lpszTarget, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			dwPos	= 0;

			UpdateUI(MESSAGE_MOVING, lpszSource, dwPos, (DWORD)(status.m_size));

			while (dwPos < (DWORD)(status.m_size))
			{
				dwSize		= ((((DWORD)(status.m_size) - dwPos) > COPY_BLOCK) ? COPY_BLOCK : ((DWORD)(status.m_size) - dwPos));
				dwPos		+= dwSize;
				m_ullMoved	+= dwSize;

				ReadFile(hSource, cBuffer, dwSize, &dwRead, NULL);
				WriteFile(hTarget, cBuffer, dwSize, &dwWritten, NULL);
				UpdateUI(MESSAGE_MOVING, lpszSource, dwPos, (DWORD)(status.m_size));
			}

			CloseHandle(hSource);
			CloseHandle(hTarget);
		}

		UpdateUI(MESSAGE_MOVING, lpszSource, 100, 100);
	}
}

BOOL CMoveDialogX::ExecDelete(LPCTSTR lpszFileName, DWORD& dwAttributes)
{
	if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return RemoveDirectory(lpszFileName);
	}

	SetFileAttributes(lpszFileName, FILE_ATTRIBUTE_NORMAL);
	return DeleteFile(lpszFileName);
}

void CMoveDialogX::UpdateUI(int nType, LPCTSTR lpszFilename, DWORD dwCopied, DWORD dwSize)
{
	CString	strMessage;
	CString	strName;
	CString	strCurrent;
	CString	strTotal;
	DWORD	dwProgress;

	TakeName(lpszFilename, strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	strName.ReleaseBuffer();

	switch (nType)
	{
	case MESSAGE_MOVING:
		{
			AfxFormatString1(strMessage, IDS_MOVE_MOVING, strName);
		}
		break;
	case MESSAGE_DELETING:
		{
			AfxFormatString1(strMessage, IDS_DELE_DELETING, strName);
		}
		break;
	}

	m_ctlFilename.SetWindowText(strMessage);
	strCurrent.Format(_T("%d"), m_nTotal - m_nCurrent);
	strTotal.Format(_T("%d"), m_nTotal);

	switch (nType)
	{
	case MESSAGE_MOVING:
		{
			AfxFormatString2(strMessage, IDS_MOVE_REMAIN, strCurrent, strTotal);
		}
		break;
	case MESSAGE_DELETING:
		{
			AfxFormatString2(strMessage, IDS_DELE_REMAIN, strCurrent, strTotal);
		}
		break;
	}

	m_ctlStatus.SetWindowText(strMessage);
	m_ctlTotal.SetPos(static_cast<int>(m_ullMoved));

	dwProgress	= (DWORD)((ULONGLONG)((ULONGLONG)dwCopied * 100) / (ULONGLONG)dwSize);

	m_ctlProgress.SetPos(dwProgress);
}

void CMoveDialogX::MoveThread()
{
	CString			strMessage;
	CString			strTitle;
	CString			strName;
	BOOL			bComplete	= TRUE;
	DWORD			dwFlags		= 0x0000;
	CString			strSource;
	CString			strTarget;
	BOOL			bExist;
	DWORD			dwSize;
	DWORD			dwAttributes;
	int				nReturn;
	CFileStatus		status;
	CConfirmCopyX	dConfirm;

	m_nCurrent	= 0;
	m_nTotal	= m_arrFiles.GetSize() + m_arrFolders.GetSize();

	while (m_arrFolders.GetSize())
	{
		UpdateUI(MESSAGE_MOVING, m_arrFolders.GetAt(0), 100, 100);

		strSource	= m_arrFolders.GetAt(0).Mid(m_strSource.GetLength());

		CombineFileName(strTarget.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strDestination, strSource);
		strTarget.ReleaseBuffer();

		bExist	= CFile::GetStatus(strTarget, status);

		if (bExist)
		{
			if ((dwFlags & 0x0001) == 0x0001)
			{
				nReturn	= IDYES;
			}
			else
			{
				dConfirm.InitializeX();
				dConfirm.SetMode(CONFIRMMODE_MOVE);
				dConfirm.SetInfo(strSource, m_strDestination, m_strSource, dwFlags);

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
			ExecCopy(m_arrFolders.GetAt(0), strTarget);
		}

		if (nReturn == IDNO)
		{
			bComplete	= FALSE;
		}

		if (nReturn == IDCANCEL)
		{
			m_bAbort	= TRUE;
		}

		m_nCurrent++;

		m_arrFolders.RemoveAt(0);

		if (m_bAbort)
		{
			PostMessage(WM_MOVE_THREAD_FINISHED);
			return;
		}
	}

	while (m_arrFiles.GetSize())
	{
		if (IsEnableMove(m_arrFiles.GetAt(0), dwSize))
		{
			strSource	= m_arrFiles.GetAt(0).Mid(m_strSource.GetLength());

			CombineFileName(strTarget.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strDestination, strSource);
			strTarget.ReleaseBuffer();

			bExist	= CFile::GetStatus(strTarget, status);

			if (bExist)
			{
				if ((dwFlags & 0x0001) == 0x0001)
				{
					nReturn	= IDYES;
				}
				else
				{
					dConfirm.InitializeX();
					dConfirm.SetMode(CONFIRMMODE_MOVE);
					dConfirm.SetInfo(strSource, m_strDestination, m_strSource, dwFlags);

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
				ExecCopy(m_arrFiles.GetAt(0), strTarget);
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
		else
		{
			UpdateUI(MESSAGE_MOVING, m_arrFiles.GetAt(0), 100, 100);

			bComplete	= FALSE;
		}

		m_nCurrent++;

		m_arrFiles.RemoveAt(0);

		if (m_bAbort)
		{
			PostMessage(WM_MOVE_THREAD_FINISHED);
			return;
		}
	}

	m_nCurrent	= 0;

	while (m_arrFilesDelete.GetSize())
	{
		m_nCurrent++;

		UpdateUI(MESSAGE_DELETING, m_arrFilesDelete.GetAt(m_arrFilesDelete.GetUpperBound()), 100, 100);

		if (IsEnableDelete(m_arrFilesDelete.GetAt(m_arrFilesDelete.GetUpperBound()), dwAttributes))
		{
			ExecDelete(m_arrFilesDelete.GetAt(m_arrFilesDelete.GetUpperBound()), dwAttributes);
		}

		m_arrFilesDelete.RemoveAt(m_arrFilesDelete.GetUpperBound());

		if (m_bAbort)
		{
			PostMessage(WM_MOVE_THREAD_FINISHED);
			return;
		}
	}

	while (m_arrFoldersDelete.GetSize())
	{
		m_nCurrent++;

		UpdateUI(MESSAGE_DELETING, m_arrFoldersDelete.GetAt(m_arrFoldersDelete.GetUpperBound()), 100, 100);

		if (IsEnableDelete(m_arrFoldersDelete.GetAt(m_arrFoldersDelete.GetUpperBound()), dwAttributes))
		{
			ExecDelete(m_arrFoldersDelete.GetAt(m_arrFoldersDelete.GetUpperBound()), dwAttributes);
		}

		m_arrFoldersDelete.RemoveAt(m_arrFoldersDelete.GetUpperBound());

		if (m_bAbort)
		{
			PostMessage(WM_MOVE_THREAD_FINISHED);
			return;
		}
	}

	if (m_bShowResult)
	{
		strTitle.LoadString(IDS_MOVE_HEADER);
		strMessage.LoadString(bComplete ? IDS_MOVE_COMPLETE : IDS_MOVE_INCOMPLETE);
		MessageBox(strMessage, strTitle, MB_OK | (bComplete ? MB_ICONINFORMATION : MB_ICONERROR));
	}

	PostMessage(WM_MOVE_THREAD_FINISHED);
}

BOOL CMoveDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	SetHeader(IDS_MOVE_HEADER);

	int		nTotal	= m_arrFiles.GetSize() + m_arrFolders.GetSize();
	CString	strMessage;

	if (nTotal < 1)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	m_ctlProgress.SetRange32(0, 100);
	m_ctlTotal.SetRange32(0, static_cast<int>(m_ullTotal));
	strMessage.LoadString(IDS_MOVE_PREPARING);
	m_ctlFilename.SetWindowText(strMessage);
	strMessage.LoadString(IDS_MOVE_CALCULATING);
	m_ctlStatus.SetWindowText(strMessage);

	m_pThread	= AfxBeginThread(_MoveThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

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

void CMoveDialogX::OnDestroy()
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

void CMoveDialogX::OnSize(UINT nType, int cx, int cy)
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

	m_ctlTotal.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlTotal.MoveWindow(&rc);

	m_ctlStatus.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlStatus.MoveWindow(&rc);
}

void CMoveDialogX::OnOK()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CMoveDialogX::OnCancel()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CMoveDialogX::OnClose()
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

LRESULT CMoveDialogX::OnThreadFinished(WPARAM wParam, LPARAM lParam)
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
