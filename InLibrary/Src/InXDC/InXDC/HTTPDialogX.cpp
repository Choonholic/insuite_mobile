// HTTPDialogX.cpp

// Based on CHTTPDownloaderDlg, Version 1.30 (08 June 2008)
// Defines the implementation for an MFC dialog which performs HTTP downloads similiar to the Internet Explorer download dialog using Wininet
//
// Copyright (c) 2006 - 2008 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)
// All rights reserved.
//
// Copyright / Usage Details:
//
// You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise)
// when your product is released in binary form. You are allowed to modify the source code in any way you want
// except you cannot modify the copyright details at the top of each module. If you want to distribute source
// code with your application, then you are only allowed to distribute versions released by the author. This is
// to maintain a single distribution point for the source code.

#include "stdafx.h"
#include "resourceppc.h"
#include "InLibrary_Messages.h"
#include "DisplayX.h"
#include "InternetX.h"
#include "HTTPDialogX.h"

IMPLEMENT_DYNAMIC(CHTTPDialogX, COptionsDialogX)

CHTTPDialogX::CHTTPDialogX(CWnd* pParent) : COptionsDialogX(IDD_PROCESS_SINGLE_DIALOG, pParent), m_bSafeToClose(FALSE), m_pThread(NULL), m_dwLastPercentage(static_cast<DWORD>(-1)), m_ullLastTotalBytes(0), m_dwStartProgressTicks(0), m_dwLastProgressTicks(0)
{
}

void CHTTPDialogX::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRCS_FILENAME, m_ctlFilename);
	DDX_Control(pDX, IDC_PRCS_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_PRCS_STATUS, m_ctlStatus);
}

BEGIN_MESSAGE_MAP(CHTTPDialogX, CDialog)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_HTTP_THREAD_FINISHED, &CHTTPDialogX::OnThreadFinished)
END_MESSAGE_MAP()

LRESULT CHTTPDialogX::OnThreadFinished(WPARAM wParam, LPARAM lParam)
{
	m_bSafeToClose	= TRUE;

	if (Abort())
	{
		EndDialog(IDCANCEL);
	}
	else if (m_strError.GetLength())
	{
		AfxMessageBox(m_strError);
		EndDialog(IDCANCEL);
	}
	else
	{
		EndDialog(IDOK);
	}

	return 0L;
}

BOOL CHTTPDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();
	SetHeader(IDS_TRNF_HEADER);
	m_strError.Empty();

	m_dwLastPercentage	= static_cast<DWORD>(-1);
	m_ullLastTotalBytes	= 0;

	CFileStatus	fs;

	ASSERT(m_strFileToDownloadInto.GetLength());

	BOOL	bDownloadFileExists	= CFile::GetStatus(m_strFileToDownloadInto, fs);

	if ((bDownloadFileExists) && (m_ullStartPos == 0) && (m_bPromptFileOverwrite))
	{
		CString	strMessage;

		AfxFormatString1(strMessage, IDS_TRNF_ERRASKOVERWRITE, m_strFileToDownloadInto);

		if (AfxMessageBox(strMessage, MB_YESNO) != IDYES)
		{
			TRACE(_T("CHTTPDialogX::OnInitDialog, Failed to confirm file overwrite, download aborted\n"));
			EndDialog(IDCANCEL);
			return TRUE;
		}
	}

	INTERNET_PORT	nPort;
	CString			strObject;
	CString			strServer;
	BOOL			bSuccess	= FALSE;

	bSuccess	= ParseURLX(m_strURLToDownload, m_dwServiceType, strServer.GetBuffer(INTERNET_MAX_URL_LENGTH + 1), strObject.GetBuffer(INTERNET_MAX_URL_LENGTH + 1), nPort);

	strServer.ReleaseBuffer();
	strObject.ReleaseBuffer();

	if (!bSuccess)
	{
		m_strURLToDownload	= _T("http://") + m_strURLToDownload;
		bSuccess			= ParseURLX(m_strURLToDownload, m_dwServiceType, strServer.GetBuffer(INTERNET_MAX_URL_LENGTH + 1), strObject.GetBuffer(INTERNET_MAX_URL_LENGTH + 1), nPort);

		strServer.ReleaseBuffer();
		strObject.ReleaseBuffer();

		if (!bSuccess)
		{
			TRACE(_T("CHTTPDialogX::OnInitDialog, Failed to parse the URL: %s\n"), m_strURLToDownload);
			EndDialog(IDCANCEL);
			return TRUE;
		}
	}

	int	nSlash	= strObject.ReverseFind(_T('/'));

	if (nSlash == -1)
	{
		nSlash	= strObject.ReverseFind(_T('\\'));
	}

	if ((nSlash != -1) && (strObject.GetLength() > 1))
	{
		m_strFilename	= strObject.Right(strObject.GetLength() - nSlash - 1);
	}
	else
	{
		m_strFilename	= strObject;
	}

	CString	strFilename;

	AfxFormatString2(strFilename, IDS_TRNF_DOWNLOADNAME, m_strFilename, m_strServer);
	m_ctlFilename.SetWindowText(strFilename);

	m_pThread	= AfxBeginThread(_DownloadThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	if (m_pThread == NULL)
	{
		TRACE(_T("CHTTPDialogX::OnInitDialog, Failed to create download thread, dialog is aborting\n"));
		EndDialog(IDCANCEL);
		return TRUE;
	}

	HideDoneButton();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();

	TBBUTTON	tbButton;
	CString		strButton;

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

UINT CHTTPDialogX::_DownloadThread(LPVOID pParam)
{
	CHTTPDialogX*	pDialog	= static_cast<CHTTPDialogX*>(pParam);

	AFXASSUME(pDialog);
	ASSERT(pDialog->IsKindOf(RUNTIME_CLASS(CHTTPDialogX)));
	pDialog->DownloadThread();
	return 0;
}

void CHTTPDialogX::SetPercentage(int nPercentage)
{
	m_ctlProgress.SetPos(nPercentage);
}

void CHTTPDialogX::SetTimeLeft(DWORD dwSecondsLeft, ULONGLONG ullBytesRead, ULONGLONG ullFileSize)
{
	CString	strCopied;

	if (ullBytesRead < 1024)
	{
		CString	strBytes;

		strBytes.Format(_T("%I64u"), ullBytesRead);
		AfxFormatString1(strCopied, IDS_TRNF_BYTES, strBytes);
	}
	else if (ullBytesRead < 1048576)
	{
		CString	strKiloBytes;

		strKiloBytes.Format(_T("%0.1f"), static_cast<LONGLONG>(ullBytesRead) / 1024.0);
		AfxFormatString1(strCopied, IDS_TRNF_KILOBYTES, strKiloBytes);
	}
	else
	{
		CString	strMegaBytes;

		strMegaBytes.Format(_T("%0.2f"), static_cast<LONGLONG>(ullBytesRead) / 1048576.0);
		AfxFormatString1(strCopied, IDS_TRNF_MEGABYTES, strMegaBytes);
	}

	CString	strTotal;

	if (ullFileSize < 1024)
	{
		CString	strBytes;

		strBytes.Format(_T("%I64u"), ullFileSize);
		AfxFormatString1(strTotal, IDS_TRNF_BYTES, strBytes);
	}
	else if (ullFileSize < 1048576)
	{
		CString	strKiloBytes;

		strKiloBytes.Format(_T("%0.1f"), static_cast<LONGLONG>(ullFileSize) / 1024.0);
		AfxFormatString1(strTotal, IDS_TRNF_KILOBYTES, strKiloBytes);
	}
	else
	{
		CString	strMegaBytes;

		strMegaBytes.Format(_T("%0.2f"), static_cast<LONGLONG>(ullFileSize) / 1048576.0);
		AfxFormatString1(strTotal, IDS_TRNF_MEGABYTES, strMegaBytes);
	}

	CString	strOf;

	AfxFormatString2(strOf, IDS_TRNF_OF, strCopied, strTotal);

	CString	strTime;
	CString	strRemain;
	DWORD	dwMinutes	= dwSecondsLeft / 60;
	DWORD	dwSeconds	= dwSecondsLeft % 60;

	strTime.Format(_T("%u:%02u"), dwMinutes, dwSeconds);
	AfxFormatString1(strRemain, IDS_TRNF_TIMEREMAIN, strTime);

	CString	strTimeLeft;

	AfxFormatString2(strTimeLeft, IDS_TRNF_TIMELEFT, strOf, strRemain);
	m_ctlStatus.SetWindowText(strTimeLeft);
}

void CHTTPDialogX::DownloadThread()
{
	Download();
	PostMessage(WM_HTTP_THREAD_FINISHED);
}

void CHTTPDialogX::OnDestroy()
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

void CHTTPDialogX::OnOK()
{
	SetAbort();
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	CHTTPDownloaderX::SetStatus(IDS_TRNF_ABORTING);
}

void CHTTPDialogX::OnCancel()
{
	SetAbort();
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	CHTTPDownloaderX::SetStatus(IDS_TRNF_ABORTING);
}

void CHTTPDialogX::OnClose()
{
	if (m_bSafeToClose)
	{
		COptionsDialogX::OnClose();
	}
	else
	{
		SetAbort();
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		CHTTPDownloaderX::SetStatus(IDS_TRNF_ABORTING);
	}
}

void CHTTPDialogX::OnSize(UINT nType, int cx, int cy)
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

void CHTTPDialogX::SetStatus(LPCTSTR lpszStatus)
{
	CHTTPDownloaderX::SetStatus(lpszStatus);
	m_ctlStatus.SetWindowText(lpszStatus);
}

void CHTTPDialogX::HandleError(UINT nID, DWORD dwError)
{
	CHTTPDownloaderX::HandleError(nID, dwError);

	CString	strError;

	if (dwError)
	{
		strError.Format(_T("%s"), GetErrorMessage(dwError).operator LPCTSTR());
	}
	else
	{
		strError.Format(_T("%s"), GetErrorMessage(GetLastError()).operator LPCTSTR());
	}

	AfxFormatString1(m_strError, nID, strError);
}

void CHTTPDialogX::HandleStatusCode(UINT nID, DWORD dwStatusCode)
{
	CHTTPDownloaderX::HandleStatusCode(nID, dwStatusCode);

	CString	strError;

	strError.Format(_T("%u"), dwStatusCode);
	AfxFormatString1(m_strError, nID, strError);
}

void CHTTPDialogX::OnProgress(ULONGLONG ullTotalBytesRead, BOOL bGotFileSize, ULONGLONG ullFileSize)
{
	CHTTPDownloaderX::OnProgress(ullTotalBytesRead, bGotFileSize, ullFileSize);

	if (m_dwLastPercentage == -1)
	{
		m_dwStartProgressTicks	= GetTickCount();
		m_dwLastProgressTicks	= m_dwStartProgressTicks;
	}

	if (bGotFileSize)
	{
		DWORD	dwPercentage	= static_cast<DWORD>((ullTotalBytesRead + m_ullStartPos) * 100 / ullFileSize);

		if (dwPercentage != m_dwLastPercentage)
		{
			SetPercentage(dwPercentage);

			m_dwLastPercentage	= dwPercentage;
		}
	}

	DWORD	dwNowTicks	= GetTickCount();
	DWORD	dwTimeTaken	= dwNowTicks - m_dwLastProgressTicks;

	if (dwTimeTaken > 1000)
	{
#if 0
		double	dbKbPerSecond	= (static_cast<double>(static_cast<LONGLONG>(dwTotalBytesRead)) - static_cast<double>(static_cast<LONGLONG>(m_dwLastTotalBytes))) / (static_cast<double>(dwTimeTaken));
#endif

		m_dwLastProgressTicks	= dwNowTicks;
		m_ullLastTotalBytes		= ullTotalBytesRead;

		if (bGotFileSize)
		{
			if (ullTotalBytesRead)
			{
				DWORD	dwSecondsLeft	= static_cast<DWORD>((static_cast<double>(dwNowTicks) - m_dwStartProgressTicks) / static_cast<double>(static_cast<LONGLONG>(ullTotalBytesRead)) * (static_cast<LONGLONG>(ullFileSize) - static_cast<double>(static_cast<LONGLONG>(ullTotalBytesRead))) / 1000);

				SetTimeLeft(dwSecondsLeft, ullTotalBytesRead + m_ullStartPos, ullFileSize);
			}
		}
	}
}

void CHTTPDialogX::OnWininetStatusCallBack(HINTERNET /* hInternet */, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD /* dwStatusInformationLength */)
{
	// Note the MSDN documentation has very poor information on what the lpStatusInformation parameter actually
	// contains for various callbacks. Some users of the class has reported that it reports a LPTSTR for string
	// type information, but the author has seen it return a LPSTR even for Unicode builds. As a defense in depth
	// approach against this uncertainty, lets determine at runtime what type of string we have. Perhaps I will
	// check with MS support to get a definitive answer on this. For more info on this, check out
	// http://groups.google.com/group/microsoft.public.inetsdk.programming.wininet/browse_thread/thread/e75e20aa425cfb16/a569b0026815bc98

	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		{
			CHTTPDownloaderX::SetStatus(IDS_TRNF_RESOLVING, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	case INTERNET_STATUS_NAME_RESOLVED:
		{
			CHTTPDownloaderX::SetStatus(IDS_TRNF_RESOLVED, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		{
			CHTTPDownloaderX::SetStatus(IDS_TRNF_CONNECTING, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		{
			CHTTPDownloaderX::SetStatus(IDS_TRNF_CONNECTED, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	case INTERNET_STATUS_REDIRECT:
		{
			CHTTPDownloaderX::SetStatus(IDS_TRNF_REDIRECTING, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	default:
		{
		}
		break;
	}
}

BOOL CHTTPDialogX::DoCertificateSelection()
{
	DWORD	dwResult	= ::InternetErrorDlg(GetSafeHwnd(), m_hHTTPFile, ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED, FLAGS_ERROR_UI_FILTER_FOR_ERRORS | FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, NULL);

	return ((dwResult == ERROR_INTERNET_FORCE_RETRY) || (dwResult == ERROR_SUCCESS));
}

BOOL CHTTPDialogX::HandlePrompt(DWORD& dwError)
{
	dwError	= ::InternetErrorDlg(GetSafeHwnd(), m_hHTTPFile, ERROR_INTERNET_INCORRECT_PASSWORD, FLAGS_ERROR_UI_FILTER_FOR_ERRORS | FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, NULL);

	if (dwError == ERROR_SUCCESS)
	{
		SetAbort();
	}

	return (dwError == ERROR_INTERNET_FORCE_RETRY);
}

void CHTTPDialogX::OnGotContentLength(ULONGLONG ullContentLength)
{
	CHTTPDownloaderX::OnGotContentLength(ullContentLength);
	m_ctlProgress.SetRange(0, 100);
}
