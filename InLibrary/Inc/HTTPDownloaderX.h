// HTTPDownloaderX.h

// Based on CHTTPDownloader, Version 1.30 (08 June 2008)
// Defines the interface for an MFC class which provides a wrapping for downloading HTTP requests in a synchronous manner via Wininet
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

#pragma once

#include "afxmt.h"
#include "wininet.h"
#include "wincrypt.h"

class AFX_EXT_CLASS CHTTPDownloaderX
{
public:
	CHTTPDownloaderX(void);
	~CHTTPDownloaderX(void);

	BOOL		Download(void);
	BOOL		DeleteDownloadedFile(void);
	void		AttachSession(HINTERNET hInternetSession);
	HINTERNET	DetachSession(void);

	enum ConnectionType
	{
		UsePreConfig,
		DirectToInternet,
		UseProxy,
	};

	CString			m_strURLToDownload;					// The URL to download
	CString			m_strFileToDownloadInto;			// Where to download it to
	CString			m_strProxyServer;					// The proxy server to use
	CString			m_strProxyUserName;					// The proxy server username to use
	CString			m_strProxyPassword;					// The proxy server password to use
	CString			m_strHTTPUserName;					// The HTTP server username to use
	CString			m_strHTTPPassword;					// The HTTP server password to use
	CString			m_strUserAgent;						// The HTTP_USER_AGENT header to use
	CString			m_strReferrer;						// Allows you to customize the HTTP_REFERRER header
	CString			m_strHTTPVerb;						// The HTTP verb to use (normally would be "GET")
	CString			m_strHTTPVersion;					// The HTTP version to use (e.g. "HTTP/1.0", "HTTP/1.1" etc)
	ConnectionType	m_ConnectionType;					// The connection type to use
	BOOL			m_bPromptFileOverwrite;				// Should overwrites result in a prompt, the base CHttpDownloader does not support this
	BOOL			m_bPromptForProxyDetails;			// Should we prompt for proxy credentials if required
	BOOL			m_bPromptForHTTPDetails;			// Should we prompt for HTTP server credentials if required
	BOOL			m_bPromptForCertificateDetails;		// Should we prompt for a client certificate if required
	double			m_dbLimit;							// For BANDWIDTH Throptling, The value in Bytes / Second to limit the connection to
	ULONGLONG		m_ullStartPos;						// Offset to resume the download at
	BOOL			m_bNoURLRedirect;					// Set to TRUE, if you do not want 302 redirects to be followed
	DWORD			m_dwReadBufferSize;					// The size of the receive buffer when we read the HTTP response
	BOOL			m_bProxyPreauthentication;			// Should we supply credentials on the first request for the Proxy rather than starting out with anonymous credentials and only authenticating when challenged
	BOOL			m_bHTTPPreauthentication;			// Should we supply credentials on the first request for the HTTP Server rather than starting out with anonymous credentials and only authenticating when challenged
	CString			m_strAcceptTypes;					// The AcceptTypes header to send

#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
	BOOL	SetClientCertificate(PCCERT_CONTEXT pClientCertContext);	//The client certificate we will use. Note the code makes a copy of the cert context through CertDuplicateCertificateContext, so client code still needs to destroy the passed in cert context parameter using CertFreeCertificateContext
#endif

protected:
	DWORD				m_dwServiceType;
	HINTERNET			m_hInternetSession;
	HINTERNET			m_hHTTPConnection;
	HINTERNET			m_hHTTPFile;
	CString				m_strServer;
	CString				m_strObject;
	INTERNET_PORT		m_nPort;
	LONG				m_lAbort;
	CFile				m_FileToDownloadInto;
	CCriticalSection	m_csHandles;			// Critical section we use to serialize access to the Wininet handles

#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
	PCCERT_CONTEXT		m_pCertContext;
#endif

	static void	CALLBACK	_OnWininetStatusCallBack(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	static BOOL				QueryStatusNumber(HINTERNET hInternet, DWORD dwFlag, DWORD& dwCode);
	static BOOL				QueryStatusCode(HINTERNET hInternet, DWORD& dwCode);
	static BOOL				QueryContentLength(HINTERNET hInternet, ULONGLONG& ullCode);
	static CString			GetErrorMessage(DWORD dwError);

	void	ReleaseHandles(void);

#ifndef HTTPDOWNLOADER_NO_CERTIFICATE_SUPPORT
	void	ReleaseCertContext(void);
#endif

	void	SetAbort(void);
	BOOL	_Download(void);

	virtual BOOL	CreateSession(void);
	virtual BOOL	CreateConnection(void);
	virtual BOOL	CreateRequest(void);
	virtual BOOL	DoHTTPAuthentication(void);
	virtual BOOL	DoProxyAuthentication(void);
	virtual BOOL	DoCertificateSelection(void);
	virtual void	OnWininetStatusCallBack(HINTERNET hInternet, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	virtual void	HandleError(UINT nID, DWORD dwError);
	virtual void	HandleStatusCode(UINT nID, DWORD dwStatusCode);
	virtual DWORD	GetOpenRequestFlags(void);
	virtual CString	GetOptionalHeaders(void);
	virtual BOOL	HandlePrompt(DWORD &dwError);
	virtual void	SetStatus(UINT nID);
	virtual void	SetStatus(UINT nID, LPCTSTR lpsz1);
	virtual void	SetStatus(LPCTSTR lpszStatus);
	virtual BOOL	Abort(void);
	virtual void	OnGotContentLength(ULONGLONG ullCotentLength);
	virtual void	OnProgress(ULONGLONG ullBytesRead, BOOL bGotFileSize, ULONGLONG ullTotalBytesToRead);
	virtual BOOL	OnDownloadData(const BYTE* pbyData, DWORD dwSize);
};
