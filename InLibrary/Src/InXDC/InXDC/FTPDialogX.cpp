// FTPDialogX.cpp

// Based on CFTPTransfererDlg, Version 1.14 (18 May 2008)
// Defines the interface for an MFC class which provides a wrapping for FTP transfers in a synchronous manner via Wininet
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
#include "FTPDialogX.h"

IMPLEMENT_DYNAMIC(CFTPDialogX, COptionsDialogX)

CFTPDialogX::CFTPDialogX(CWnd* pParent) : COptionsDialogX(IDD_PROCESS_SINGLE_DIALOG, pParent), m_bSafeToClose(FALSE), m_pThread(NULL), m_dwLastPercentage(static_cast<DWORD>(-1)), m_ullLastTotalBytes(0), m_dwStartProgressTicks(0), m_dwLastProgressTicks(0)
{
}

void CFTPDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRCS_FILENAME, m_ctlFilename);
	DDX_Control(pDX, IDC_PRCS_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_PRCS_STATUS, m_ctlStatus);
}

BEGIN_MESSAGE_MAP(CFTPDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_FTP_THREAD_FINISHED, &CFTPDialogX::OnThreadFinished)
END_MESSAGE_MAP()

LRESULT CFTPDialogX::OnThreadFinished(WPARAM /* wParam */, LPARAM /* lParam */)
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

BOOL CFTPDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();
	SetHeader(IDS_TRNF_HEADER);

	m_strError.Empty();

	m_dwLastPercentage	= static_cast<DWORD>(-1);
	m_ullLastTotalBytes	= 0;

	m_ctlProgress.SetRange(0, 100);

	CString	strFileStatus;
	CString	strParsedRemote;
	int		nParsedPos	= (m_strRemoteFile.ReverseFind(_T('/')) + 1);

	strParsedRemote	= m_strRemoteFile.Mid(nParsedPos);

	ASSERT(m_strRemoteFile.GetLength());
	ASSERT(m_strServer.GetLength());

	if (m_bDownload)
	{
		AfxFormatString2(strFileStatus, IDS_TRNF_DOWNLOADNAME, m_strRemoteFile, m_strServer);
	}
	else
	{
		AfxFormatString2(strFileStatus, IDS_TRNF_UPLOADNAME, m_strRemoteFile, m_strServer);
	}

	m_ctlFilename.SetWindowText(strFileStatus);

	m_pThread	= AfxBeginThread(_TransferThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	if (m_pThread == NULL)
	{
		TRACE(_T("CFTPDialogX::OnInitDialog, Failed to create transfer thread, dialog is aborting\n"));
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

UINT CFTPDialogX::_TransferThread(LPVOID pParam)
{
	CFTPDialogX*	pDialog	= reinterpret_cast<CFTPDialogX*>(pParam);

	ASSERT(pDialog);
	ASSERT(pDialog->IsKindOf(RUNTIME_CLASS(CFTPDialogX)));
	pDialog->TransferThread();
	return 0;
}

void CFTPDialogX::SetPercentage(int nPercentage)
{
	m_ctlProgress.SetPos(nPercentage);
}

void CFTPDialogX::SetTimeLeft(DWORD dwSecondsLeft, ULONGLONG ullBytesRead, ULONGLONG ullFileSize)
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


void CFTPDialogX::TransferThread()
{
	Transfer();
	PostMessage(WM_FTP_THREAD_FINISHED);
}

void CFTPDialogX::OnDestroy()
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

void CFTPDialogX::OnSize(UINT nType, int cx, int cy)
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

void CFTPDialogX::OnOK()
{
	SetAbort();
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	CFTPTransfererX::SetStatus(IDS_TRNF_ABORTING);
}


void CFTPDialogX::OnCancel()
{
	SetAbort();
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	CFTPTransfererX::SetStatus(IDS_TRNF_ABORTING);
}

void CFTPDialogX::OnClose()
{
	if (m_bSafeToClose)
	{
		COptionsDialogX::OnClose();
	}
	else
	{
		SetAbort();
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
		CFTPTransfererX::SetStatus(IDS_TRNF_ABORTING);
	}
}

void CFTPDialogX::SetStatus(LPCTSTR lpszStatus)
{
	CFTPTransfererX::SetStatus(lpszStatus);
	m_ctlStatus.SetWindowText(lpszStatus);
}

void CFTPDialogX::HandleError(UINT nID, DWORD dwError)
{
	CFTPTransfererX::HandleError(nID, dwError);

	CString	strError;

	if (dwError == ERROR_INTERNET_EXTENDED_ERROR)
	{
		DWORD	dwInetError	= 0;
		DWORD	dwSize		= 0;

		::InternetGetLastResponseInfo(&dwInetError, NULL, &dwSize);

		TCHAR*	pszResponse	= new TCHAR[dwSize + 1];

		::InternetGetLastResponseInfo(&dwInetError, pszResponse, &dwSize);

		pszResponse[dwSize]	= _T('\0');
		strError			= pszResponse;

		delete [] pszResponse;
	}
	else
	{
		strError	= GetErrorMessage(dwError);
	}

	AfxFormatString1(m_strError, nID, strError);
}

void CFTPDialogX::OnProgress(ULONGLONG ullTotalBytesRead, BOOL bGotFileSize, ULONGLONG ullFileSize)
{
	CFTPTransfererX::OnProgress(ullTotalBytesRead, bGotFileSize, ullFileSize);

	if (m_dwLastPercentage == -1)
	{
		m_dwStartProgressTicks	= GetTickCount();
		m_dwLastProgressTicks	= m_dwStartProgressTicks;
	}

	if (bGotFileSize)
	{
		DWORD	dwPercentage	= static_cast<DWORD>(ullTotalBytesRead * 100 / ullFileSize);

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
		double	dbKbPerSecond	= (static_cast<double>(static_cast<LONGLONG>(ullTotalBytesRead)) - static_cast<double>(static_cast<LONGLONG>(m_ullLastTotalBytes))) / (static_cast<double>(dwTimeTaken));
#endif

		m_dwLastProgressTicks	= dwNowTicks;
		m_ullLastTotalBytes		= ullTotalBytesRead;

		if (bGotFileSize)
		{
			if (ullTotalBytesRead)
			{
				DWORD	dwSecondsLeft	= static_cast<DWORD>((static_cast<double>(dwNowTicks) - m_dwStartProgressTicks) / static_cast<double>(static_cast<LONGLONG>(ullTotalBytesRead)) * (static_cast<double>(static_cast<LONGLONG>(ullFileSize)) - static_cast<double>(static_cast<LONGLONG>(ullTotalBytesRead))) / 1000);

				SetTimeLeft(dwSecondsLeft, ullTotalBytesRead, ullFileSize);
			}
		}
	}
}

void CFTPDialogX::OnWininetStatusCallBack(HINTERNET /*hInternet*/, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD /*dwStatusInformationLength*/)
{
	// Note the MSDN documentation has very poor information on what the lpStatusInformation parameter actually
	// contains for various callbacks. Some users of the class has reported that it reports a LPTSTR for string
	// type information, but the author has seen it return a LPSTR even for Unicode builds. As a defense in depth
	// approach against this uncertainty, lets determine at runtime what type of string we have. Perhaps I will
	// check with MS support to get a definitive answer on this. For more info on this, check out
	// http://groups.google.com/group/microsoft.public.inetsdk.programming.wininet/browse_thread/thread/e75e20aa425cfb16/a569b0026815bc98

	USES_CONVERSION;

	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		{
			CFTPTransfererX::SetStatus(IDS_TRNF_RESOLVING, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	case INTERNET_STATUS_NAME_RESOLVED:
		{
			CFTPTransfererX::SetStatus(IDS_TRNF_RESOLVED, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		{
			CFTPTransfererX::SetStatus(IDS_TRNF_CONNECTING, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		{
			CFTPTransfererX::SetStatus(IDS_TRNF_CONNECTED, static_cast<LPTSTR>(lpvStatusInformation));
		}
		break;
	default:
		{
		}
		break;
	}
}
