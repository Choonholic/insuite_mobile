// FTPTransfererX.cpp

// Based on CFTPTransferer, Version 1.14 (18 May 2008)
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

#include "StdAfx.h"
#include "resourceppc.h"
#include "FTPTransfererX.h"

#pragma comment(lib, "Wininet.lib")

CFTPTransfererX::CFTPTransfererX(void) :	m_bDownload(TRUE),
											m_nPort(INTERNET_DEFAULT_FTP_PORT),
											m_bBinary(TRUE),
											m_bPasv(FALSE),
											m_dbLimit(0),
											m_dwConnectionTimeout(0),
											m_ConnectionType(UsePreConfig),
											m_dwReadBufferSize(1024),
											m_hInternetSession(NULL),
											m_hFTPConnection(NULL),
											m_hFTPFile(NULL),
											m_lAbort(0),
											m_dwTempError(0)
{
}

CFTPTransfererX::~CFTPTransfererX(void)
{
	ReleaseHandles();
}

void CFTPTransfererX::AttachSession(HINTERNET hInternetSession)
{
	if (m_hInternetSession)
	{
		::InternetCloseHandle(m_hInternetSession);
	}

	m_hInternetSession	= hInternetSession;
}

HINTERNET CFTPTransfererX::DetachSession(void)
{
	HINTERNET	hTempSession	= m_hInternetSession;

	m_hInternetSession	= NULL;

	return hTempSession;
}

void CFTPTransfererX::ReleaseHandles(void)
{
	CSingleLock	sl(&m_csHandles, TRUE);

	if (m_hFTPFile)
	{
		::InternetCloseHandle(m_hFTPFile);

		m_hFTPFile			= NULL;
	}

	if (m_hFTPConnection)
	{
		::InternetCloseHandle(m_hFTPConnection);

		m_hFTPConnection	= NULL;
	}

	if (m_hInternetSession)
	{
		::InternetCloseHandle(m_hInternetSession);

		m_hInternetSession	= NULL;
	}
}

BOOL CFTPTransfererX::DeleteDownloadedFile(void)
{
	BOOL	bSuccess	= ::DeleteFile(m_strLocalFile);

	if (!bSuccess)
	{
		TRACE(_T("CFTPTransfererX::DeleteDownloadedFile, Failed to delete failed downloaded file, Error:%d\n"), ::GetLastError());
	}

	return bSuccess;
}

BOOL CFTPTransfererX::Transfer(void)
{
	BOOL	bDownloadFileAlreadyExists	= (GetFileAttributes(m_strLocalFile) != 0xFFFFFFFF /* INVALID_FILE_ATTRIBUTES */);
	BOOL	bSuccess					= _Transfer();

	ReleaseHandles();

	try
	{
		m_LocalFile.Close();
	}
	catch (CFileException* pEx)
	{
		pEx->Delete();
	}

	if ((m_bDownload) && (!bSuccess) && (!bDownloadFileAlreadyExists) && (GetFileAttributes(m_strLocalFile)))
	{
		DeleteDownloadedFile();
	}

	return bSuccess;
}

BOOL CFTPTransfererX::CreateSession(void)
{
	CSingleLock	sl(&m_csHandles, TRUE);

	if (m_hInternetSession == NULL)
	{
		switch (m_ConnectionType)
		{
		case UsePreConfig:
			{
				m_hInternetSession	= ::InternetOpen((m_strUserAgent.GetLength() ? m_strUserAgent : AfxGetAppName()), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
				break;
			}
		case DirectToInternet:
			{
				m_hInternetSession	= ::InternetOpen((m_strUserAgent.GetLength() ? m_strUserAgent : AfxGetAppName()), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
				break;
			}
		case UseProxy:
			{
				ASSERT(m_strProxyServer.GetLength());

				m_hInternetSession	= ::InternetOpen((m_strUserAgent.GetLength() ? m_strUserAgent : AfxGetAppName()), INTERNET_OPEN_TYPE_PROXY, m_strProxyServer, NULL, 0);
				break;
			}
		default:
			{
				ASSERT(FALSE);
				break;
			}
		}
	}

	if (m_hInternetSession == NULL)
	{
		DWORD	dwError	= GetLastError();

		TRACE(_T("CFTPTransfererX::CreateSession, Failed in call to InternetOpen, Error:%d\n"), dwError);
		HandleError(IDS_TRNF_ERRFTP, dwError);
		return FALSE;
	}

	if (::InternetSetStatusCallback(m_hInternetSession, _OnWininetStatusCallBack) == INTERNET_INVALID_STATUS_CALLBACK)
	{
		DWORD	dwError	= GetLastError();

		TRACE(_T("CFTPTransfererX::Download, Failed in call to InternetSetStatusCallback, Error:%d\n"), dwError);
		HandleError(IDS_TRNF_ERRFTP, dwError);
		return FALSE;
	}

	return TRUE;
}

BOOL CFTPTransfererX::CreateConnection(void)
{
	CSingleLock	sl(&m_csHandles, TRUE);

	ASSERT(m_hFTPConnection == NULL);

	if (m_dwConnectionTimeout == 0)
	{
		MakeConnection();
	}
	else
	{
		CWinThread*	pConnectionThread	= AfxBeginThread(_MakeConnectionThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

		if (pConnectionThread == NULL)
		{
			DWORD	dwError	= GetLastError();

			TRACE(_T("CFTPTransfererX::CreateConnection, Failed in create the connection thread, Error:%d\n"), dwError);
			HandleError(IDS_TRNF_ERRCONNECT, dwError);
			return FALSE;
		}

		pConnectionThread->m_bAutoDelete	= FALSE;

		pConnectionThread->ResumeThread();

		if (WaitForSingleObject(pConnectionThread->m_hThread, m_dwConnectionTimeout) == WAIT_TIMEOUT)
		{
			::InternetCloseHandle(m_hInternetSession);

			m_hInternetSession	= NULL;

			WaitForSingleObject(pConnectionThread->m_hThread, INFINITE);

			delete pConnectionThread;

			TRACE(_T("CFTPTransfererX::CreateConnection, Failed in connect to the FTP server in a timely manner\n"));
			HandleError(IDS_TRNF_ERRCONNECT, ERROR_TIMEOUT);
			return FALSE;
		}

		delete pConnectionThread;
	}

	if (m_hFTPConnection == NULL)
	{
		TRACE(_T("CFTPTransfererX::CreateConnection, Failed in call to InternetConnect, Error:%d\n"), m_dwTempError);
		HandleError(IDS_TRNF_ERRCONNECT, m_dwTempError);
		return FALSE;
	}

	return TRUE;
}

BOOL CFTPTransfererX::CreateRequest(void)
{
	CSingleLock	sl(&m_csHandles, TRUE);

	ASSERT(m_hFTPFile == NULL);

	m_hFTPFile	= FtpOpenFile(m_hFTPConnection, m_strRemoteFile, m_bDownload ? GENERIC_READ : GENERIC_WRITE, m_bBinary ? FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE : FTP_TRANSFER_TYPE_ASCII | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, reinterpret_cast<DWORD_PTR>(this));

	if (m_hFTPFile == NULL)
	{
		DWORD	dwLastError	= ::GetLastError();

		TRACE(_T("CFTPTransfererX::_Transfer, Failed in call to FtpOpenFile, Error:%d\n"), dwLastError);
		HandleError(IDS_TRNF_ERROPENFILE, dwLastError);
		return FALSE;
	}

	if (m_bDownload)
	{
		SetStatus(IDS_TRNF_RETRIEVING);
	}
	else
	{
		SetStatus(IDS_TRNF_UPLOADING);
	}

	return TRUE;
}

BOOL CFTPTransfererX::OnTransferData(const BYTE* pbyData, DWORD dwSize)
{
	if (m_bDownload)
	{
		try
		{
			m_LocalFile.Write(pbyData, dwSize);
		}
		catch (CFileException* pEx)
		{
			TRACE(_T("CFTPTransfererX::OnTransferData, An exception occured while writing to the download file\n"));
			HandleError(IDS_TRNF_ERRUPLOAD, pEx->m_lOsError);
			pEx->Delete();
			return FALSE;
		}
	}
	else
	{
		ASSERT(m_hFTPFile);

		DWORD	dwBytesWritten	= 0;

		if (!::InternetWriteFile(m_hFTPFile, pbyData, dwSize, &dwBytesWritten))
		{
			DWORD	dwLastError	= ::GetLastError();

			TRACE(_T("CFTPTransferDlg::TransferThread, Failed in call to InternetWriteFile, Error:%d\n"), dwLastError);
			HandleError(IDS_TRNF_ERRUPLOAD, dwLastError);
			return FALSE;
		}
	}

	return (!Abort());
}

BOOL CFTPTransfererX::GetRemoteFileLength(ULONGLONG& ullFileSize)
{
	BOOL			bSuccess	= FALSE;
	WIN32_FIND_DATA	wfd;
	HINTERNET		hFind		= ::FtpFindFirstFile(m_hFTPConnection, m_strRemoteFile, &wfd, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, reinterpret_cast<DWORD_PTR>(this));

	if (hFind)
	{
		bSuccess	= TRUE;
		ullFileSize	= wfd.nFileSizeLow + (static_cast<ULONGLONG>(wfd.nFileSizeHigh) << 32);

		InternetCloseHandle(hFind);
	}

	return bSuccess;
}

BOOL CFTPTransfererX::_Transfer(void)
{
	ReleaseHandles();

	if (m_bDownload)
	{
		BOOL			bDownloadFileExists	= (GetFileAttributes(m_strLocalFile) != 0xFFFFFFFF /* INVALID_FILE_ATTRIBUTES */);
		CFileException	ex;

		if (!m_LocalFile.Open(m_strLocalFile, bDownloadFileExists ? CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite : CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite, &ex))
		{
			TRACE(_T("CFTPTransfererX::_Transfer, Failed to open the file to download into, Error:%d\n"), ex.m_lOsError);
			HandleError(IDS_TRNF_ERRLOCALOPEND, ex.m_lOsError);
			return FALSE;
		}

		try
		{
			m_LocalFile.Seek(0, CFile::begin);
			m_LocalFile.SetLength(0);
		}
		catch (CFileException* pEx)
		{
			TRACE(_T("CFTPTransfererX::_Transfer, An exception occured while setting file pointer position, Error:%d\n"), pEx->m_lOsError);
			HandleError(IDS_TRNF_ERRFILESEEK, pEx->m_lOsError);
			pEx->Delete();
			return FALSE;
		}
	}
	else
	{
		CFileException	ex;

		if (!m_LocalFile.Open(m_strLocalFile, CFile::modeRead | CFile::shareDenyWrite, &ex))
		{
			TRACE(_T("CFTPTransfererX::_Transfer, Failed to open the file to upload, Error:%d\n"), ex.m_lOsError);
			HandleError(IDS_TRNF_ERRLOCALOPENU, ex.m_lOsError);
			return FALSE;
		}
	}

	if (!CreateSession())
	{
		return FALSE;
	}

	if (!CreateConnection())
	{
		return FALSE;
	}

	ULONGLONG	ullFileSize		= 0;
	BOOL		bGotFileSize	= FALSE;

	if (m_bDownload)
	{
		SetStatus(IDS_TRNF_GETTINGINFO);

		bGotFileSize	= GetRemoteFileLength(ullFileSize);
	}
	else
	{
		bGotFileSize	= TRUE;
		ullFileSize		= m_LocalFile.GetLength();
	}

	if (!CreateRequest())
	{
		return FALSE;
	}

	DWORD		dwStartTicks		= ::GetTickCount();
	DWORD		dwBytesRead			= 0;
	BYTE*		byReadBuf			= static_cast<BYTE*>(_alloca(m_dwReadBufferSize));
	ULONGLONG	ullTotalBytesRead	= 0;

	do
	{
		if (m_bDownload)
		{
			if (!::InternetReadFile(m_hFTPFile, byReadBuf, m_dwReadBufferSize, &dwBytesRead))
			{
				DWORD	dwLastError	= ::GetLastError();

				TRACE(_T("CFTPTransfererX::_Transfer, Failed in call to InternetReadFile, Error:%d\n"), dwLastError);
				HandleError(IDS_TRNF_ERRDOWNLOAD, dwLastError);
				return FALSE;
			}
		}
		else
		{
			try
			{
				dwBytesRead	= m_LocalFile.Read(byReadBuf, m_dwReadBufferSize);
			}
			catch (CFileException* pEx)
			{
				TRACE(_T("CFTPTransfererX::_Transfer, An exception occured while reading the local file, Error:%d\n"), pEx->m_lOsError);
				HandleError(IDS_TRNF_ERRDOWNLOAD, pEx->m_lOsError);
				pEx->Delete();
				return FALSE;
			}
		}

		if (dwBytesRead)
		{
			if (!OnTransferData(byReadBuf, dwBytesRead))
			{
				return FALSE;
			}

			ullTotalBytesRead	+= dwBytesRead;

			if (m_dbLimit > 0)
			{
				double	t	= static_cast<double>(GetTickCount() - dwStartTicks);
				double	q	= static_cast<double>(static_cast<LONGLONG>(ullTotalBytesRead)) / t;

				if (q > m_dbLimit)
				{
					Sleep(static_cast<DWORD>(((q * t) / m_dbLimit) - t));
				}
			}

			OnProgress(ullTotalBytesRead, bGotFileSize, ullFileSize);
		}
	} while (dwBytesRead && !Abort());

	return TRUE;
}

void CFTPTransfererX::OnProgress(ULONGLONG ullTotalBytesRead, BOOL bGotMaxLength, ULONGLONG ullTotalBytesToRead)
{
#ifndef _DEBUG
	UNREFERENCED_PARAMETER(ullTotalBytesRead);
	UNREFERENCED_PARAMETER(ullTotalBytesToRead);
#endif

	if (bGotMaxLength)
	{
		TRACE(_T("Transferred %I64u of %I64u bytes\n"), ullTotalBytesRead, ullTotalBytesToRead);
	}
	else
	{
		TRACE(_T("Transferred %I64u bytes\n"), ullTotalBytesRead);
	}
}

BOOL CFTPTransfererX::Abort(void)
{
	LONG	lAbort	= 0;

	InterlockedExchange(&lAbort, m_lAbort);

	return (lAbort != 0);
}

void CFTPTransfererX::SetAbort(void)
{
	InterlockedExchange(&m_lAbort, TRUE);
	ReleaseHandles();
}

void CFTPTransfererX::HandleError(UINT nID, DWORD dwError)
{
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

	CString	strErrorMsg;

	AfxFormatString1(strErrorMsg, nID, strError);
	TRACE(_T("%s\n"), strErrorMsg.operator LPCTSTR());
}

void CFTPTransfererX::SetStatus(UINT nID)
{
	CString	strStatus;

	strStatus.LoadString(nID);
	SetStatus(strStatus);
}

void CFTPTransfererX::SetStatus(UINT nID, LPCTSTR lpsz1)
{
	CString	strStatus;

	AfxFormatString1(strStatus, nID, lpsz1);
	SetStatus(strStatus);
}

void CFTPTransfererX::SetStatus(LPCTSTR lpszStatus)
{
#ifndef _DEBUG
	UNREFERENCED_PARAMETER(lpszStatus);
#endif

	TRACE(_T("CFTPTransfererX::SetStatus, %s\n"), lpszStatus);
}

CString CFTPTransfererX::GetErrorMessage(DWORD dwError)
{
	HMODULE	hWininet	= GetModuleHandle(_T("wininet.dll"));
	CString	rVal;
	LPTSTR	pstrError	= rVal.GetBuffer(4096);
	LPTSTR	lpBuffer;
	DWORD	dwReturn	= FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE, hWininet, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT), reinterpret_cast<LPTSTR>(&lpBuffer), 0, NULL);

	if (dwReturn == 0)
	{
		*pstrError	= _T('\0');
	}
	else
	{
		_tcsncpy_s(pstrError, 4096, lpBuffer, _TRUNCATE);
		LocalFree(lpBuffer);
	}

	rVal.ReleaseBuffer();
	return rVal;
}

void CALLBACK CFTPTransfererX::_OnWininetStatusCallBack(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	CFTPTransfererX*	pThis	= reinterpret_cast<CFTPTransfererX*>(dwContext);

	ASSERT(pThis);
	pThis->OnWininetStatusCallBack(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
}

void CFTPTransfererX::OnWininetStatusCallBack(HINTERNET /* hInternet */, DWORD /* dwInternetStatus */, LPVOID /* lpvStatusInformation */, DWORD /* dwStatusInformationLength */)
{
}

UINT CFTPTransfererX::_MakeConnectionThread(LPVOID pParam)
{
	CFTPTransfererX*	pThis	= reinterpret_cast<CFTPTransfererX*>(pParam);

	ASSERT(pThis);
	return pThis->MakeConnection();
}

UINT CFTPTransfererX::MakeConnection(void)
{
	ASSERT(m_hFTPConnection == NULL);
	ASSERT(m_strServer.GetLength());

	if (m_strUserName.GetLength())
	{
		m_hFTPConnection	= ::InternetConnect(m_hInternetSession, m_strServer, m_nPort, m_strUserName, m_strPassword, INTERNET_SERVICE_FTP, (m_bPasv ? INTERNET_FLAG_PASSIVE : 0), reinterpret_cast<DWORD_PTR>(this));
	}
	else
	{
		m_hFTPConnection	= ::InternetConnect(m_hInternetSession, m_strServer, m_nPort, NULL, NULL, INTERNET_SERVICE_FTP, (m_bPasv ? INTERNET_FLAG_PASSIVE : 0), reinterpret_cast<DWORD_PTR>(this));
	}

	if (m_hFTPConnection == NULL)
	{
		m_dwTempError		= ::GetLastError();
	}

	return (m_hFTPConnection ? 0 : 1);
}
