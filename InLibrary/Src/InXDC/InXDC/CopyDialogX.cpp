// CopyDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "InLibrary_Messages.h"
#include "DisplayX.h"
#include "CopyDialogX.h"
#include "ConfirmCopyX.h"
#include "FileSearchX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CCopyDialogX, COptionsDialogX)

CCopyDialogX::CCopyDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_PROCESS_DOUBLE_DIALOG, pParent)
{
	m_ullCopied		= 0;
	m_ullTotal		= 0;
	m_nCurrent		= 0;
	m_nTotal		= 0;
	m_pThread		= NULL;
	m_bAbort		= FALSE;
	m_bSafeToClose	= FALSE;
	m_bShowResult	= FALSE;
}

CCopyDialogX::~CCopyDialogX()
{
}

void CCopyDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRCD_FILENAME, m_ctlFilename);
	DDX_Control(pDX, IDC_PRCD_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_PRCD_TOTAL, m_ctlTotal);
	DDX_Control(pDX, IDC_PRCD_STATUS, m_ctlStatus);
}

BEGIN_MESSAGE_MAP(CCopyDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_COPY_THREAD_FINISHED, &CCopyDialogX::OnThreadFinished)
END_MESSAGE_MAP()

void CCopyDialogX::SetSource(LPCTSTR lpszFolder)
{
	m_strSource	= lpszFolder;

	if ((m_strSource.GetLength() > 1) && (m_strSource.Right(1) != _T("\\")))
	{
		m_strSource	+= _T("\\");
	}
}

void CCopyDialogX::SetDestination(LPCTSTR lpszFolder)
{
	m_strDestination	= lpszFolder;

	if ((m_strDestination.GetLength() > 1) && (m_strDestination.Right(1) != _T("\\")))
	{
		m_strDestination	+= _T("\\");
	}
}

void CCopyDialogX::AddFile(LPCTSTR lpszFile)
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

		m_ullTotal	+= status.m_size;
	}
	else
	{
		CFileSearchX	Search;
		int				i;

		m_arrFolders.Add(strFile);

		m_ullTotal	+= 0;
		strFile		+= _T("\\*.*");

		Search.SetPartName(strFile);
		Search.DoSearch();

		for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
		{
			m_arrFiles.Add(Search.GetFullPath(&Search.m_arrFiles[i]));

			m_ullTotal	+= Search.GetFileSize(&Search.m_arrFiles[i]);
		}

		for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
		{
			m_arrFolders.Add(Search.GetFullPath(&Search.m_arrFolders[i]));

			m_ullTotal	+= 0;
		}
	}
}

void CCopyDialogX::AddFiles(CStringArray& arrFiles)
{
	for (int i = 0; i < arrFiles.GetSize(); i++)
	{
		AddFile(arrFiles.GetAt(i));
	}
}

void CCopyDialogX::ClearFiles()
{
	InitializeX();
}

void CCopyDialogX::ShowResult(BOOL bShow)
{
	m_bShowResult	= bShow;
}

UINT CCopyDialogX::_CopyThread(LPVOID pParam)
{
	CCopyDialogX*	pDialog	= (CCopyDialogX*)pParam;

	ASSERT(pDialog);
	ASSERT(pDialog->IsKindOf(RUNTIME_CLASS(CCopyDialogX)));
	pDialog->CopyThread();
	return 0;
}

void CCopyDialogX::InitializeX()
{
	m_ullCopied		= 0;
	m_ullTotal		= 0;
	m_nCurrent		= 0;
	m_nTotal		= 0;

	if (m_arrFiles.GetSize())
	{
		m_arrFiles.RemoveAll();
	}

	if (m_arrFolders.GetSize())
	{
		m_arrFolders.RemoveAll();
	}
}

BOOL CCopyDialogX::IsEnableCopy(LPCTSTR lpszFileName, DWORD& dwSize)
{
	CFileStatus	status;

	dwSize	= 0;

	if (!CFile::GetStatus(lpszFileName, status))
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

#define COPY_BLOCK	32768

void CCopyDialogX::ExecCopy(LPCTSTR lpszSource, LPCTSTR lpszTarget)
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

			UpdateUI(lpszSource, dwPos, (DWORD)(status.m_size));

			while (dwPos < (DWORD)(status.m_size))
			{
				dwSize		= ((((DWORD)(status.m_size) - dwPos) > COPY_BLOCK) ? COPY_BLOCK : ((DWORD)(status.m_size) - dwPos));
				dwPos		+= dwSize;
				m_ullCopied	+= dwSize;

				ReadFile(hSource, cBuffer, dwSize, &dwRead, NULL);
				WriteFile(hTarget, cBuffer, dwSize, &dwWritten, NULL);
				UpdateUI(lpszSource, dwPos, (DWORD)(status.m_size));
			}

			CloseHandle(hSource);
			CloseHandle(hTarget);
		}

		UpdateUI(lpszSource, 100, 100);
	}
}

void CCopyDialogX::UpdateUI(LPCTSTR lpszFilename, DWORD dwCopied, DWORD dwSize)
{
	CString	strMessage;
	CString	strName;
	CString	strCurrent;
	CString	strTotal;
	DWORD	dwProgress;

	TakeName(lpszFilename, strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	strName.ReleaseBuffer();
	AfxFormatString1(strMessage, IDS_COPY_COPYING, strName);
	m_ctlFilename.SetWindowText(strMessage);
	strCurrent.Format(_T("%d"), m_nTotal - m_nCurrent);
	strTotal.Format(_T("%d"), m_nTotal);
	AfxFormatString2(strMessage, IDS_COPY_REMAIN, strCurrent, strTotal);
	m_ctlStatus.SetWindowText(strMessage);
	m_ctlTotal.SetPos(static_cast<int>(m_ullCopied));

	dwProgress	= (DWORD)((ULONGLONG)((ULONGLONG)dwCopied * 100) / (ULONGLONG)dwSize);

	m_ctlProgress.SetPos(dwProgress);
}

void CCopyDialogX::CopyThread()
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
	int				nReturn;
	CFileStatus		status;
	CConfirmCopyX	dConfirm;

	m_nCurrent	= 0;
	m_nTotal	= m_arrFiles.GetSize() + m_arrFolders.GetSize();

	while (m_arrFolders.GetSize())
	{
		UpdateUI(m_arrFolders.GetAt(0), 100, 100);

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
				dConfirm.SetMode(CONFIRMMODE_COPY);
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
			PostMessage(WM_COPY_THREAD_FINISHED);
			return;
		}
	}

	while (m_arrFiles.GetSize())
	{
		if (IsEnableCopy(m_arrFiles.GetAt(0), dwSize))
		{
			strSource	= m_arrFiles.GetAt(0).Mid(m_strSource.GetLength());

			CombineFileName(strTarget.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strDestination, strSource);
			strTarget.ReleaseBuffer();

			bExist		= CFile::GetStatus(strTarget, status);

			if (bExist)
			{
				if ((dwFlags & 0x0001) == 0x0001)
				{
					nReturn	= IDYES;
				}
				else
				{
					dConfirm.InitializeX();
					dConfirm.SetMode(CONFIRMMODE_COPY);
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
			UpdateUI(m_arrFiles.GetAt(0), 100, 100);

			bComplete	= FALSE;
		}

		m_nCurrent++;

		m_arrFiles.RemoveAt(0);

		if (m_bAbort)
		{
			PostMessage(WM_COPY_THREAD_FINISHED);
			return;
		}
	}

	if (m_bShowResult)
	{
		strTitle.LoadString(IDS_COPY_HEADER);
		strMessage.LoadString(bComplete ? IDS_COPY_COMPLETE : IDS_COPY_INCOMPLETE);
		MessageBox(strMessage, strTitle, MB_OK | (bComplete ? MB_ICONINFORMATION : MB_ICONERROR));
	}

	PostMessage(WM_COPY_THREAD_FINISHED);
}

BOOL CCopyDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	SetHeader(IDS_COPY_HEADER);

	int		nTotal	= m_arrFiles.GetSize() + m_arrFolders.GetSize();
	CString	strMessage;

	if (nTotal < 1)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	m_ctlProgress.SetRange32(0, 100);
	m_ctlTotal.SetRange32(0, static_cast<int>(m_ullTotal));
	strMessage.LoadString(IDS_COPY_PREPARING);
	m_ctlFilename.SetWindowText(strMessage);
	strMessage.LoadString(IDS_COPY_CALCULATING);
	m_ctlStatus.SetWindowText(strMessage);

	m_pThread	= AfxBeginThread(_CopyThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

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

void CCopyDialogX::OnDestroy()
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

void CCopyDialogX::OnSize(UINT nType, int cx, int cy)
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

void CCopyDialogX::OnOK()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CCopyDialogX::OnCancel()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CCopyDialogX::OnClose()
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

LRESULT CCopyDialogX::OnThreadFinished(WPARAM wParam, LPARAM lParam)
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
