// HTTPDownloaderX.cpp

// Based on CHTTPDownloader, Version 1.30 (08 June 2008)
// Defines the implementation for an MFC class which provides a wrapping for downloading HTTP requests in a synchronous manner via Wininet
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
#include "InternetX.h"
#include "HTTPDownloaderX.h"

#pragma comment(lib, "Wininet.lib")

#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
#pragma comment(lib, "Crypt32.lib")
#endif

CHTTPDownloaderX::CHTTPDownloaderX(void) :	m_dwServiceType(0),
#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
											m_pCertContext(NULL),
#endif
											m_hInternetSession(NULL),
											m_hHTTPConnection(NULL),
											m_hHTTPFile(NULL),
											m_nPort(0),
											m_ullStartPos(0),
											m_ConnectionType(UsePreConfig),
											m_bPromptFileOverwrite(TRUE),
											m_bPromptForProxyDetails(TRUE),
											m_bPromptForHTTPDetails(TRUE),
											m_bPromptForCertificateDetails(TRUE),
											m_dbLimit(0),
											m_bNoURLRedirect(FALSE),
											m_dwReadBufferSize(1024),
											m_bProxyPreauthentication(TRUE),
											m_bHTTPPreauthentication(TRUE),
											m_strAcceptTypes(_T("*/*")),
											m_lAbort(0)
{
}

CHTTPDownloaderX::~CHTTPDownloaderX(void)
{
	ReleaseHandles();

#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
	ReleaseCertContext();
#endif
}

void CHTTPDownloaderX::AttachSession(HINTERNET hInternetSession)
{
	if (m_hInternetSession)
	{
		::InternetCloseHandle(m_hInternetSession);
	}

	m_hInternetSession	= hInternetSession;
}

HINTERNET CHTTPDownloaderX::DetachSession(void)
{
	HINTERNET	hTempSession	= m_hInternetSession;

	m_hInternetSession	= NULL;

	return hTempSession;
}

void CHTTPDownloaderX::ReleaseHandles(void)
{
	CSingleLock	sl(&m_csHandles, TRUE);

	if (m_hHTTPFile)
	{
		::InternetCloseHandle(m_hHTTPFile);

		m_hHTTPFile	= NULL;
	}

	if (m_hHTTPConnection)
	{
		::InternetCloseHandle(m_hHTTPConnection);

		m_hHTTPConnection	= NULL;
	}

	if (m_hInternetSession)
	{
		::InternetCloseHandle(m_hInternetSession);

		m_hInternetSession	= NULL;
	}
}

#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
void CHTTPDownloaderX::ReleaseCertContext(void)
{
	if (m_pCertContext)
	{
		CertFreeCertificateContext(m_pCertContext);

		m_pCertContext	= NULL;
	}
}
#endif

#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
BOOL CHTTPDownloaderX::SetClientCertificate(PCCERT_CONTEXT pCertContext)
{
	BOOL	bSuccess	= TRUE;

	ReleaseCertContext();

	if (pCertContext)
	{
		m_pCertContext	= CertDuplicateCertificateContext(pCertContext);
		bSuccess		= (m_pCertContext != NULL);
	}

	return bSuccess;
}
#endif

BOOL CHTTPDownloaderX::DeleteDownloadedFile(void)
{
	BOOL	bSuccess	= ::DeleteFile(m_strFileToDownloadInto);

	if (!bSuccess)
	{
		TRACE(_T("CHTTPDownloaderX::DeleteDownloadedFile, Failed to delete failed downloaded file, Error:%d\n"), ::GetLastError());
	}

	return bSuccess;
}

BOOL CHTTPDownloaderX::Download(void)
{
	BOOL	bDownloadFileAlreadyExists	= FALSE;
	BOOL	bSpecifiedDownloadFile		= (m_strFileToDownloadInto.GetLength() != 0);

	if (bSpecifiedDownloadFile)
	{
		bDownloadFileAlreadyExists	= (GetFileAttributes(m_strFileToDownloadInto) != 0xFFFFFFFF /* INVALID_FILE_ATTRIBUTES */);
	}

	BOOL	bSuccess	= _Download();

	ReleaseHandles();

	if (bSpecifiedDownloadFile)
	{
		try
		{
			m_FileToDownloadInto.Close();
		}
		catch (CFileException* pEx)
		{
			pEx->Delete();
		}

		if ((!bSuccess) && (!bDownloadFileAlreadyExists) && (GetFileAttributes(m_strFileToDownloadInto)))
		{
			DeleteDownloadedFile();
		}
	}

	return bSuccess;
}

BOOL CHTTPDownloaderX::CreateSession(void)
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

		TRACE(_T("CHTTPDownloaderX::CreateSession, Failed in call to InternetOpen, Error:%d\n"), dwError);
		HandleError(IDS_TRNF_ERRHTTP, dwError);
		return FALSE;
	}

	if (::InternetSetStatusCallback(m_hInternetSession, _OnWininetStatusCallBack) == INTERNET_INVALID_STATUS_CALLBACK)
	{
		DWORD	dwError	= GetLastError();

		TRACE(_T("CHTTPDownloaderX::Download, Failed in call to InternetSetStatusCallback, Error:%d\n"), dwError);
		HandleError(IDS_TRNF_ERRHTTP, dwError);
		return FALSE;
	}

	return TRUE;
}

BOOL CHTTPDownloaderX::CreateConnection(void)
{
	CSingleLock	sl(&m_csHandles, TRUE);

	ASSERT(m_hHTTPConnection == NULL);

	if (m_strHTTPUserName.GetLength())
	{
		m_hHTTPConnection	= ::InternetConnect(m_hInternetSession, m_strServer, m_nPort, m_strHTTPUserName, m_strHTTPPassword, INTERNET_SERVICE_HTTP, 0, reinterpret_cast<DWORD_PTR>(this));
	}
	else
	{
		m_hHTTPConnection	= ::InternetConnect(m_hInternetSession, m_strServer, m_nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, reinterpret_cast<DWORD_PTR>(this));
	}

	if (m_hHTTPConnection == NULL)
	{
		DWORD	dwError	= GetLastError();

		TRACE(_T("CHTTPDownloaderX::CreateConnection, Failed in call to InternetConnect, Error:%d\n"), dwError);
		HandleError(IDS_TRNF_ERRCONNECT, dwError);
		return FALSE;
	}

	return TRUE;
}

BOOL CHTTPDownloaderX::CreateRequest(void)
{
	CStringArray			AcceptTypes;
	int						nAcceptTypesLength	= m_strAcceptTypes.GetLength();
	ATL::CHeapPtr<TCHAR>	szAcceptTypes;

	if (!szAcceptTypes.Allocate(nAcceptTypesLength + 1))
	{
		TRACE(_T("CHTTPDownloaderX::CreateRequest, Failed in allocate AcceptTypes buffer\n"));
		HandleError(IDS_TRNF_ERRHTTP, ERROR_OUTOFMEMORY);
		return FALSE;
	}

	_tcscpy_s(szAcceptTypes.m_pData, nAcceptTypesLength + 1, m_strAcceptTypes);

	for (int pos = 0, start = 0; pos <= nAcceptTypesLength; pos++)
	{
		if ((szAcceptTypes[pos] == _T(',')) || (szAcceptTypes[pos] == _T(';')) || (szAcceptTypes[pos] == 0))
		{
			szAcceptTypes[pos]	= 0;

			CString	strAcceptType(&szAcceptTypes[start]);

			if (strAcceptType.GetLength())
			{
				AcceptTypes.Add(strAcceptType);
			}

			start	= pos + 1;
		}
	}

	size_t					nAcceptTypes		= AcceptTypes.GetSize();
	ATL::CHeapPtr<LPCTSTR>	ppszAcceptTypes;

	if (!ppszAcceptTypes.Allocate(nAcceptTypes + 1))
	{
		TRACE(_T("CHTTPDownloaderX::CreateRequest, Failed in allocate AcceptTypes buffer\n"));
		HandleError(IDS_TRNF_ERRHTTP, ERROR_OUTOFMEMORY);
		return FALSE;
	}

	for (size_t i = 0; i < nAcceptTypes; i++)
	{
		ppszAcceptTypes.m_pData[i]	= AcceptTypes.ElementAt(i).operator LPCTSTR();
	}

	ppszAcceptTypes.m_pData[nAcceptTypes]	= NULL;

	CSingleLock	sl(&m_csHandles, TRUE);

	ASSERT(m_hHTTPFile == NULL);

	m_hHTTPFile	= HttpOpenRequest(m_hHTTPConnection, (m_strHTTPVerb.GetLength() ? m_strHTTPVerb.operator LPCTSTR() : NULL), m_strObject, (m_strHTTPVersion.GetLength() ? m_strHTTPVersion.operator LPCTSTR() : NULL), (m_strReferrer.GetLength() ? m_strReferrer.operator LPCTSTR() : NULL), ppszAcceptTypes, GetOpenRequestFlags(), reinterpret_cast<DWORD_PTR>(this));

	if (m_hHTTPFile == NULL)
	{
		DWORD	dwError	= GetLastError();

		TRACE(_T("CHTTPDownloaderX::CreateRequest, Failed in call to HttpOpenRequest, Error:%d\n"), dwError);
		HandleError(IDS_TRNF_ERRCONNECT, dwError);
		return FALSE;
	}

	return TRUE;
}

BOOL CHTTPDownloaderX::DoHTTPAuthentication(void)
{
	int	nHTTPUserLength	= m_strHTTPUserName.GetLength();

	if (nHTTPUserLength)
	{
		if (!InternetSetOption(m_hHTTPFile, INTERNET_OPTION_USERNAME, reinterpret_cast<LPVOID>(const_cast<LPTSTR>(m_strHTTPUserName.operator LPCTSTR())), (nHTTPUserLength + 1) * sizeof(TCHAR)))
		{
			DWORD	dwError	= GetLastError();

			TRACE(_T("CHTTPDownloaderX::DoHTTPAuthentication, Failed to set HTTP server username details, Error:%d\n"), dwError);
			HandleStatusCode(IDS_TRNF_ERRHTTP, dwError);
			return FALSE;
		}

		if (!InternetSetOption(m_hHTTPFile, INTERNET_OPTION_PASSWORD, reinterpret_cast<LPVOID>(const_cast<LPTSTR>(m_strHTTPPassword.operator LPCTSTR())), (m_strHTTPPassword.GetLength() + 1) * sizeof(TCHAR)))
		{
			DWORD	dwError	= GetLastError();

			TRACE(_T("CHTTPDownloaderX::DoHTTPAuthentication, Failed to set HTTP server password details, Error:%d\n"), dwError);
			HandleStatusCode(IDS_TRNF_ERRHTTP, dwError);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CHTTPDownloaderX::DoProxyAuthentication(void)
{
	int	nProxyUserLength	= m_strProxyUserName.GetLength();

	if (nProxyUserLength)
	{
		if (!InternetSetOption(m_hHTTPFile, INTERNET_OPTION_PROXY_USERNAME, reinterpret_cast<LPVOID>(const_cast<LPTSTR>(m_strProxyUserName.operator LPCTSTR())), (nProxyUserLength + 1) * sizeof(TCHAR)))
		{
			DWORD	dwError	= GetLastError();

			TRACE(_T("CHTTPDownloaderX::DoProxyAuthentication, Failed to set Proxy username details, Error:%d\n"), dwError);
			HandleStatusCode(IDS_TRNF_ERRHTTP, dwError);
			return FALSE;
		}

		if (!InternetSetOption(m_hHTTPFile, INTERNET_OPTION_PROXY_PASSWORD, reinterpret_cast<LPVOID>(const_cast<LPTSTR>(m_strProxyPassword.operator LPCTSTR())), (m_strProxyPassword.GetLength() + 1) * sizeof(TCHAR)))
		{
			DWORD	dwError	= GetLastError();

			TRACE(_T("CHTTPDownloaderX::DoProxyAuthentication, Failed to set Proxy password details, Error:%d\n"), dwError);
			HandleStatusCode(IDS_TRNF_ERRHTTP, dwError);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CHTTPDownloaderX::OnDownloadData(const BYTE* pbyData, DWORD dwSize)
{
	if (m_strFileToDownloadInto.GetLength())
	{
		try
		{
			m_FileToDownloadInto.Write(pbyData, dwSize);
		}
		catch (CFileException* pEx)
		{
			TRACE(_T("CHTTPDownloaderX::OnDownloadData, An exception occured while writing to the download file\n"));
			HandleError(IDS_TRNF_ERRDOWNLOAD, pEx->m_lOsError);
			pEx->Delete();
			return FALSE;
		}
	}

	return (!Abort());
}

BOOL CHTTPDownloaderX::DoCertificateSelection(void)
{
	BOOL	bSuccess	= FALSE;

#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
	if (m_pCertContext)
	{
		bSuccess	= InternetSetOption(m_hHTTPFile, INTERNET_OPTION_CLIENT_CERT_CONTEXT, reinterpret_cast<LPVOID>(const_cast<PCERT_CONTEXT>(m_pCertContext)), sizeof(CERT_CONTEXT));
	}
#endif

	return bSuccess;
}

BOOL CHTTPDownloaderX::_Download(void)
{
	ReleaseHandles();
	ASSERT(m_strURLToDownload.GetLength());

	BOOL	bSuccess	= FALSE;

	bSuccess	= ParseURLX(m_strURLToDownload, m_dwServiceType, m_strServer.GetBuffer(INTERNET_MAX_URL_LENGTH + 1), m_strObject.GetBuffer(INTERNET_MAX_URL_LENGTH + 1), m_nPort);

	m_strServer.ReleaseBuffer();
	m_strObject.ReleaseBuffer();

	if (!bSuccess)
	{
		m_strURLToDownload	= _T("http://") + m_strURLToDownload;
		bSuccess			= ParseURLX(m_strURLToDownload, m_dwServiceType, m_strServer.GetBuffer(INTERNET_MAX_URL_LENGTH + 1), m_strObject.GetBuffer(INTERNET_MAX_URL_LENGTH + 1), m_nPort);

		m_strServer.ReleaseBuffer();
		m_strObject.ReleaseBuffer();

		if (!bSuccess)
		{
			TRACE(_T("CHTTPDownloaderX::_Download, Failed to parse the URL: %s\n"), m_strURLToDownload);
			return FALSE;
		}
	}

	if (m_strFileToDownloadInto.GetLength())
	{
		BOOL	bDownloadFileExists	= (GetFileAttributes(m_strFileToDownloadInto) != 0xFFFFFFFF /* INVALID_FILE_ATTRIBUTES */);
		DWORD	dwFileFlags			= 0;

		if ((bDownloadFileExists) && (m_ullStartPos > 0))
		{
			dwFileFlags	= (CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyWrite);
		}
		else
		{
			dwFileFlags	= (CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
		}

		if (!m_FileToDownloadInto.Open(m_strFileToDownloadInto, dwFileFlags))
		{
			DWORD	dwError	= GetLastError();

			TRACE(_T("CHTTPDownloaderX::_Download, Failed to open the file to download into, Error:%d\n"), dwError);
			HandleError(IDS_TRNF_ERRLOCALOPEND, dwError);
			return FALSE;
		}
		else
		{
			try
			{
				m_FileToDownloadInto.Seek(m_ullStartPos, CFile::begin);
				m_FileToDownloadInto.SetLength(m_ullStartPos);
			}
			catch (CFileException* pEx)
			{
				TRACE(_T("CHTTPDownloaderX::_Download, An exception occured while set file pointer position, Error:%d\n"), pEx->m_lOsError);
				HandleError(IDS_TRNF_ERRFILESEEK, pEx->m_lOsError);
				pEx->Delete();
				return FALSE;
			}
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

	if (!CreateRequest())
	{
		return FALSE;
	}

	BOOL	bRetryingProxy			= FALSE;
	BOOL	bRetryingHTTP			= FALSE;
	BOOL	bRetryingCertificate	= FALSE;

	if (m_bProxyPreauthentication)
	{
		if (!DoProxyAuthentication())
		{
			return FALSE;
		}

		bRetryingProxy	= TRUE;
	}

	if (m_bHTTPPreauthentication)
	{
		if (!DoHTTPAuthentication())
		{
			return FALSE;
		}

		bRetryingHTTP	= TRUE;
	}

resend:
	CString	strHeaders(GetOptionalHeaders());
	BOOL	bSend	= ::HttpSendRequest(m_hHTTPFile, (strHeaders.GetLength() ? strHeaders.operator LPCTSTR() : NULL), strHeaders.GetLength(), NULL, 0);

	if (!bSend)
	{
		DWORD	dwError	= GetLastError();

		if ((dwError == ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED) && (!bRetryingCertificate))
		{
			if (m_bPromptForCertificateDetails)
			{
				if (DoCertificateSelection())
				{
					bRetryingCertificate	= TRUE;

					goto resend;
				}
			}
			else if (CHTTPDownloaderX::DoCertificateSelection())
			{
				bRetryingCertificate	= TRUE;

				goto resend;
			}
		}

		TRACE(_T("CHTTPDownloaderX::_Download, Failed in call to HttpSendRequest, Error:%d\n"), dwError);
		HandleError(IDS_TRNF_ERRCONNECT, dwError);
		return FALSE;
	}

	BOOL	bPrompt			= FALSE;
	DWORD	dwStatusCode	= 0;

	if (!QueryStatusCode(m_hHTTPFile, dwStatusCode))
	{
		DWORD	dwError	= GetLastError();

		TRACE(_T("CHTTPDownloaderX::_Download, Failed in call to HttpQueryInfo for HTTP query status code, Error:%d\n"), dwError);
		HandleError(IDS_TRNF_ERRSERVERRSP, dwError);
		return FALSE;
	}

	if ((dwStatusCode == HTTP_STATUS_PROXY_AUTH_REQ) || (dwStatusCode == HTTP_STATUS_DENIED))
	{
		ATL::CHeapPtr<BYTE>	readBuffer;

		if (!readBuffer.Allocate(m_dwReadBufferSize))
		{
			TRACE(_T("CHTTPDownloaderX::_Download, Failed in allocate read buffer\n"));
			HandleError(IDS_TRNF_ERRHTTP, ERROR_OUTOFMEMORY);
			return FALSE;
		}

		DWORD	dwSize		= 0;

		do
		{
			::InternetReadFile(m_hHTTPFile, readBuffer.m_pData, m_dwReadBufferSize, &dwSize);
		} while (dwSize != 0);
	}

	if (dwStatusCode == HTTP_STATUS_PROXY_AUTH_REQ)
	{
		int	nProxyUserLength	= m_strProxyUserName.GetLength();

		if ((nProxyUserLength) && (!bRetryingProxy))
		{
			if (!DoProxyAuthentication())
			{
				return FALSE;
			}

			bRetryingProxy	= TRUE;

			goto resend;
		}
		else if (m_bPromptForProxyDetails)
		{
			bPrompt	= TRUE;
		}
		else
		{
			TRACE(_T("CHTTPDownloaderX::_Download, Failed to retrieve a HTTP OK or partial content status, Status Code:%d\n"), dwStatusCode);
			HandleStatusCode(IDS_TRNF_ERRHTTPRSP, dwStatusCode);
			return FALSE;
		}
	}
	else if (dwStatusCode == HTTP_STATUS_DENIED)
	{
		int	nHTTPUserLength	= m_strHTTPUserName.GetLength();

		if ((nHTTPUserLength) && (!bRetryingHTTP))
		{
			if (!DoHTTPAuthentication())
			{
				return FALSE;
			}

			bRetryingHTTP	= TRUE;

			goto resend;
		}
		else if (m_bPromptForHTTPDetails)
		{
			bPrompt	= TRUE;
		}
		else
		{
			TRACE(_T("CHTTPDownloaderX::_Download, Failed to retrieve a HTTP OK or partial content status, Status Code:%d\n"), dwStatusCode);
			HandleStatusCode(IDS_TRNF_ERRHTTPRSP, dwStatusCode);
			return FALSE;
		}
	}
	else if ((dwStatusCode != HTTP_STATUS_OK) && (dwStatusCode != HTTP_STATUS_PARTIAL_CONTENT))
	{
		TRACE(_T("CHTTPDownloaderX::_Download, Failed to retrieve a HTTP OK or partial content status, Status Code:%d\n"), dwStatusCode);
		HandleStatusCode(IDS_TRNF_ERRHTTPRSP, dwStatusCode);
		return FALSE;
	}

	if (bPrompt)
	{
		DWORD	dwError	= 0;

		if (HandlePrompt(dwError))
		{
			goto resend;
		}
		else
		{
			if (dwError != 0)
			{
				HandleError(IDS_TRNF_ABORTING, dwError);
			}

			return FALSE;
		}
	}

	SetStatus(IDS_TRNF_GETTINGINFO);

	ULONGLONG	ullTotalBytesToRead	= 0;
	BOOL		bGotFileSize		= FALSE;

	if (QueryContentLength(m_hHTTPFile, ullTotalBytesToRead))
	{
		bGotFileSize	= TRUE;

		OnGotContentLength(ullTotalBytesToRead);
	}
	else
	{
		TRACE(_T("CHTTPDownloaderX::_Download, failed to get content length for request, continuing\n"));
	}

	SetStatus(IDS_TRNF_RETRIEVING);

	DWORD				dwStartTicks		= ::GetTickCount();
	DWORD				dwBytesRead			= 0;
	ULONGLONG			ullTotalBytesRead	= 0;
	ATL::CHeapPtr<BYTE>	readBuffer;

	if (!readBuffer.Allocate(m_dwReadBufferSize))
	{
		TRACE(_T("CHTTPDownloaderX::_Download, Failed in allocate read buffer\n"));
		HandleError(IDS_TRNF_ERRHTTP, ERROR_OUTOFMEMORY);
		return FALSE;
	}

	do
	{
		if (!::InternetReadFile(m_hHTTPFile, readBuffer.m_pData, m_dwReadBufferSize, &dwBytesRead))
		{
			DWORD	dwError	= GetLastError();

			TRACE(_T("CHTTPDownloaderX::_Download, Failed in call to InternetReadFile, Error:%d\n"), dwError);
			HandleError(IDS_TRNF_ERRDOWNLOAD, dwError);
			return FALSE;
		}
		else
		{
			if (dwBytesRead)
			{
				if (!OnDownloadData(readBuffer.m_pData, dwBytesRead))
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
			}

			OnProgress(ullTotalBytesRead, bGotFileSize, ullTotalBytesToRead + m_ullStartPos);
		}
	} while ((dwBytesRead) && (!Abort()));

	return TRUE;
}

CString CHTTPDownloaderX::GetOptionalHeaders(void)
{
	CString	strHeaders;

	if (m_ullStartPos != 0)
	{
		strHeaders.Format(_T("Range: bytes=%I64u-\r\n"), m_ullStartPos);
	}

	return strHeaders;
}

void CHTTPDownloaderX::OnGotContentLength(ULONGLONG ullContentLength)
{
#ifndef _DEBUG
	UNREFERENCED_PARAMETER(ullContentLength);
#endif

	TRACE(_T("Got a Content-Length header value of %I64u\n"), ullContentLength);
}

void CHTTPDownloaderX::OnProgress(ULONGLONG ullTotalBytesRead, BOOL bGotContentLength, ULONGLONG ullTotalBytesToRead)
{
#ifndef _DEBUG
	UNREFERENCED_PARAMETER(ullTotalBytesRead);
	UNREFERENCED_PARAMETER(bGotContentLength);
	UNREFERENCED_PARAMETER(ullTotalBytesToRead);
#endif

	if (bGotContentLength)
	{
		TRACE(_T("Received %I64u of %I64u bytes\n"), ullTotalBytesRead, ullTotalBytesToRead);
	}
	else
	{
		TRACE(_T("Received %I64u bytes\n"), ullTotalBytesRead);
	}
}

BOOL CHTTPDownloaderX::Abort(void)
{
	LONG	lAbort	= 0;

	InterlockedExchange(&lAbort, m_lAbort);

	return (lAbort != 0);
}

void CHTTPDownloaderX::SetAbort(void)
{
	InterlockedExchange(&m_lAbort, TRUE);
	ReleaseHandles();
}

void CHTTPDownloaderX::HandleError(UINT nID, DWORD dwError)
{
	CString	strError;

	if (dwError)
	{
		strError.Format(_T("%s"), GetErrorMessage(dwError).operator LPCTSTR());
	}
	else
	{
		strError.Format(_T("%s"), GetErrorMessage(GetLastError()).operator LPCTSTR());
	}

	CString	strErrorMsg;

	AfxFormatString1(strErrorMsg, nID, strError);
	TRACE(_T("%s\n"), strErrorMsg.operator LPCTSTR());
}

void CHTTPDownloaderX::SetStatus(UINT nID)
{
	CString	strStatus;

	if (strStatus.LoadString(nID))
	{
		SetStatus(strStatus);
	}
}

void CHTTPDownloaderX::SetStatus(UINT nID, LPCTSTR lpsz1)
{
	CString	strStatus;

	AfxFormatString1(strStatus, nID, lpsz1);
	SetStatus(strStatus);
}

void CHTTPDownloaderX::SetStatus(LPCTSTR lpszStatus)
{
#ifndef _DEBUG
	UNREFERENCED_PARAMETER(lpszStatus);
#endif

	TRACE(_T("CHTTPDownloaderX::SetStatus, %s\n"), lpszStatus);
}

void CHTTPDownloaderX::HandleStatusCode(UINT nID, DWORD dwStatusCode)
{
	CString	strStatusCode;
	CString	strMsg;

	strStatusCode.Format(_T("%u"), dwStatusCode);
	AfxFormatString1(strMsg, nID, strStatusCode);
	TRACE(strMsg);
}

BOOL CHTTPDownloaderX::HandlePrompt(DWORD&/* dwError */)
{
	return FALSE;
}

CString CHTTPDownloaderX::GetErrorMessage(DWORD dwError)
{
	HMODULE	hWininet	= GetModuleHandle(_T("Wininet.dll"));
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

BOOL CHTTPDownloaderX::QueryStatusNumber(HINTERNET hInternet, DWORD dwFlag, DWORD& dwCode)
{
	dwCode	= 0;

	DWORD	dwSize	= sizeof(DWORD);

	return HttpQueryInfo(hInternet, dwFlag | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL);
}

BOOL CHTTPDownloaderX::QueryStatusCode(HINTERNET hInternet, DWORD& dwCode)
{
	return QueryStatusNumber(hInternet, HTTP_QUERY_STATUS_CODE, dwCode);
}

BOOL CHTTPDownloaderX::QueryContentLength(HINTERNET hInternet, ULONGLONG& ullCode)
{
	TCHAR	szContentLength[32];
	DWORD	dwSize		= sizeof(szContentLength);
	BOOL	bSuccess	= HttpQueryInfo(hInternet, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwSize, NULL);

	if (bSuccess)
	{
		ullCode	= _ttoi64(szContentLength);
	}

	return bSuccess;
}

DWORD CHTTPDownloaderX::GetOpenRequestFlags(void)
{
	DWORD	dwFlags	= INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION;

	if (m_dwServiceType == INLIB_INET_SERVICE_HTTPS)
	{
		dwFlags	|= (INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID);
	}

	if (m_bNoURLRedirect)
	{
		dwFlags	|= INTERNET_FLAG_NO_AUTO_REDIRECT;
	}

	return dwFlags;
}

void CALLBACK CHTTPDownloaderX::_OnWininetStatusCallBack(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
	CHTTPDownloaderX*	pThis	= reinterpret_cast<CHTTPDownloaderX*>(dwContext);

	AFXASSUME(pThis);
	pThis->OnWininetStatusCallBack(hInternet, dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
}

void CHTTPDownloaderX::OnWininetStatusCallBack(HINTERNET /* hInternet */, DWORD /* dwInternetStatus */, LPVOID /* lpvStatusInformation */, DWORD /* dwStatusInformationLength */)
{
}
