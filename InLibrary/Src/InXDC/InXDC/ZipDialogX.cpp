// ZipDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "InLibrary_Messages.h"
#include "DisplayX.h"
#include "ZipDialogX.h"
#include "FileSearchX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CZipDialogX, COptionsDialogX)

CZipDialogX::CZipDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_PROCESS_SINGLE_DIALOG, pParent)
{
	m_hZip			= 0;
	m_uiCodePage	= CP_ACP;
	m_nLevel		= 8;
	m_ullZipped		= 0;
	m_ullTotal		= 0;
	m_nCurrent		= 0;
	m_nTotal		= 0;
	m_pThread		= NULL;
	m_bAbort		= FALSE;
	m_bSafeToClose	= FALSE;
	m_bShowResult	= FALSE;
}

CZipDialogX::~CZipDialogX()
{
}

void CZipDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRCS_FILENAME, m_ctlFilename);
	DDX_Control(pDX, IDC_PRCS_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_PRCS_STATUS, m_ctlStatus);
}

BEGIN_MESSAGE_MAP(CZipDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_ZIP_THREAD_FINISHED, &CZipDialogX::OnThreadFinished)
END_MESSAGE_MAP()

void CZipDialogX::SetSource(LPCTSTR lpszFolder)
{
	m_strSource	= lpszFolder;

	if ((m_strSource.GetLength() > 1) && (m_strSource.Right(1) != _T("\\")))
	{
		m_strSource += _T("\\");
	}
}

void CZipDialogX::SetZIPFile(LPCTSTR lpszFileName, const char* szPassword)
{
	m_strFileName	= lpszFileName;

	if (szPassword != NULL)
	{
		strcpy_s(m_szPassword, 1024, szPassword);
	}
	else
	{
		m_szPassword[0]	= NULL;
	}

	ZipCodePage(m_uiCodePage);
	ZipLevel(m_nLevel);
}

void CZipDialogX::SetCodePage(UINT uiCodePage)
{
	m_uiCodePage	= uiCodePage;

	ZipCodePage(m_uiCodePage);
}

void CZipDialogX::SetLevel(int nLevel)
{
	m_nLevel	= nLevel;

	ZipLevel(m_nLevel);
}

void CZipDialogX::AddFile(LPCTSTR lpszFile)
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

void CZipDialogX::AddFiles(CStringArray &arrFiles)
{
	for (int i = 0; i < arrFiles.GetSize(); i++)
	{
		AddFile(arrFiles.GetAt(i));
	}
}

void CZipDialogX::ClearFiles()
{
	InitializeX();
}

void CZipDialogX::ShowResult(BOOL bShow)
{
	m_bShowResult	= bShow;
}

UINT CZipDialogX::_ZipThread(LPVOID pParam)
{
	CZipDialogX*	pDialog	= (CZipDialogX*)pParam;

	ASSERT(pDialog);
	ASSERT(pDialog->IsKindOf(RUNTIME_CLASS(CZipDialogX)));
	pDialog->ZipThread();
	return 0;
}

void CZipDialogX::InitializeX()
{
	m_ullZipped		= 0;
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

BOOL CZipDialogX::IsEnableZip(LPCTSTR lpszFileName)
{
	CFileStatus	status;

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

	return TRUE;
}

void CZipDialogX::ExecZip(LPCTSTR lpszSource, LPCTSTR lpszTarget)
{
	CFileStatus	status;

	if (CFile::GetStatus(lpszSource, status))
	{
		UpdateUI(lpszSource);

		if ((status.m_attribute & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			ZipAddFolder(m_hZip, lpszTarget);

			m_ullZipped	+= 0;
		}
		else
		{
			ZipAdd(m_hZip, lpszTarget, lpszSource);

			m_ullZipped	+= status.m_size;
		}
	}

	UpdateUI(lpszSource);
}

void CZipDialogX::UpdateUI(LPCTSTR lpszFilename)
{
	CString	strMessage;
	CString	strName;
	CString	strCurrent;
	CString	strTotal;

	TakeName(lpszFilename, strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	strName.ReleaseBuffer();
	AfxFormatString1(strMessage, IDS_ZIPX_COMPRESSING, strName);
	m_ctlFilename.SetWindowText(strMessage);
	strCurrent.Format(_T("%d"), m_nTotal - m_nCurrent);
	strTotal.Format(_T("%d"), m_nTotal);
	AfxFormatString2(strMessage, IDS_ZIPX_REMAINZ, strCurrent, strTotal);
	m_ctlStatus.SetWindowText(strMessage);
	m_ctlProgress.SetPos(static_cast<int>(m_ullZipped));
}

void CZipDialogX::ZipThread()
{
	CString		strMessage;
	CString		strTitle;
	CString		strName;
	CString		strTarget;
	CFileStatus	status;

	m_hZip	= CreateZip(m_strFileName, (strlen(m_szPassword) ? m_szPassword : NULL));

	if (m_hZip == 0)
	{
		PostMessage(WM_ZIP_THREAD_FINISHED);
		return;
	}

	m_nCurrent	= 0;
	m_nTotal	= m_arrFiles.GetSize() + m_arrFolders.GetSize();

	while (m_arrFolders.GetSize())
	{
		UpdateUI(m_arrFolders.GetAt(0));

		strTarget	= m_arrFolders.GetAt(0).Mid(m_strSource.GetLength());

		ExecZip(m_arrFolders.GetAt(0), strTarget);

		m_nCurrent++;

		m_arrFolders.RemoveAt(0);

		if (m_bAbort)
		{
			CloseZip(m_hZip);
			PostMessage(WM_ZIP_THREAD_FINISHED);
			return;
		}
	}

	while (m_arrFiles.GetSize())
	{
		if (IsEnableZip(m_arrFiles.GetAt(0)))
		{
			strTarget	= m_arrFiles.GetAt(0).Mid(m_strSource.GetLength());

			ExecZip(m_arrFiles.GetAt(0), strTarget);
		}

		m_nCurrent++;

		m_arrFiles.RemoveAt(0);

		if (m_bAbort)
		{
			CloseZip(m_hZip);
			PostMessage(WM_ZIP_THREAD_FINISHED);
			return;
		}
	}

	CloseZip(m_hZip);

	if (m_bShowResult)
	{
		strTitle.LoadString(IDS_ZIPX_HEADERZ);
		strMessage.LoadString(IDS_ZIPX_COMPLETE);
		MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
	}

	PostMessage(WM_ZIP_THREAD_FINISHED);
}

BOOL CZipDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	SetHeader(IDS_ZIPX_HEADERZ);

	int		nTotal	= m_arrFiles.GetSize() + m_arrFolders.GetSize();
	CString	strMessage;

	if (nTotal < 1)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	m_ctlProgress.SetRange32(0, static_cast<int>(m_ullTotal));
	strMessage.LoadString(IDS_ZIPX_PREPARINGZ);
	m_ctlFilename.SetWindowText(strMessage);
	strMessage.LoadString(IDS_ZIPX_CALCULATING);
	m_ctlStatus.SetWindowText(strMessage);

	m_pThread	= AfxBeginThread(_ZipThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

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

void CZipDialogX::OnDestroy()
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

void CZipDialogX::OnSize(UINT nType, int cx, int cy)
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

void CZipDialogX::OnOK()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CZipDialogX::OnCancel()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CZipDialogX::OnClose()
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

LRESULT CZipDialogX::OnThreadFinished(WPARAM wParam, LPARAM lParam)
{
	m_bSafeToClose	= TRUE;

	if (m_hZip != 0)
	{
		CloseZip(m_hZip);
	}

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
