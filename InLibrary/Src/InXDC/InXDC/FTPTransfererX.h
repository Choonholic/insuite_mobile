// FTPTransfererX.h

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

#pragma once

#include "afxmt.h"
#include "wininet.h"

class AFX_EXT_CLASS CFTPTransfererX
{
public:
	CFTPTransfererX(void);
	~CFTPTransfererX(void);

	BOOL		Transfer(void);
	BOOL		DeleteDownloadedFile(void);
	void		AttachSession(HINTERNET hInternetSession);
	HINTERNET	DetachSession(void);

	enum ConnectionType
	{
		UsePreConfig,
		DirectToInternet,
		UseProxy,
	};

	BOOL			m_bDownload;			// TRUE if it's a download, FALSE if an upload
	CString			m_strServer;			// e.g. "ftp.microsoft.com"
	CString			m_strRemoteFile;		// e.g. "/pub/somefile.ext"
	CString			m_strLocalFile;			// e.g. "c:\temp\somfile.txt"
	CString			m_strUserName;			// Username to login to the server with
	CString			m_strPassword;			// password to login to the server with
	INTERNET_PORT	m_nPort;				// If you want to change the port to make access on, by default it will be 21
	BOOL			m_bBinary;				// TRUE if binary transfer, FALSE for ascii transfer
	BOOL			m_bPasv;				// should a PASV connection be used
	double			m_dbLimit;				// For BANDWIDTH Throptling, The value in Bytes / Second to limit the connection to
	DWORD			m_dwConnectionTimeout;	// The connection timeout to use (in milliseconds), (Specify a value of 0 not to use a connection timeout)
	CString			m_strUserAgent;			// The User agent to use
	CString			m_strProxyServer;		// The proxy server to use
	ConnectionType	m_ConnectionType;		// The connection type to use
	DWORD			m_dwReadBufferSize;		// The size of the receive buffer when we read the HTTP response

protected:
	HINTERNET			m_hInternetSession;
	HINTERNET			m_hFTPConnection;
	HINTERNET			m_hFTPFile;
	LONG				m_lAbort;
	CFile				m_LocalFile;
	CCriticalSection	m_csHandles;		// Critical section we use to serialize access to the Wininet handles
	DWORD				m_dwTempError;

	static void CALLBACK	_OnWininetStatusCallBack(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	static CString			GetErrorMessage(DWORD dwError);
	static UINT				_MakeConnectionThread(LPVOID pParam);

	void	ReleaseHandles(void);
	void	SetAbort(void);
	BOOL	_Transfer(void);

	virtual BOOL	CreateSession(void);
	virtual BOOL	CreateConnection(void);
	virtual BOOL	CreateRequest(void);
	virtual UINT	MakeConnection(void);
	virtual void	OnWininetStatusCallBack(HINTERNET hInternet, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	virtual void	HandleError(UINT nID, DWORD dwError);
	virtual void	SetStatus(UINT nID);
	virtual void	SetStatus(UINT nID, LPCTSTR lpsz1);
	virtual void	SetStatus(LPCTSTR lpszStatus);
	virtual BOOL	Abort(void);
	virtual void	OnProgress(ULONGLONG ullBytesRead, BOOL bGotFileSize, ULONGLONG ullTotalBytesToRead);
	virtual BOOL	OnTransferData(const BYTE* pbyData, DWORD dwSize);
	virtual BOOL	GetRemoteFileLength(ULONGLONG& ullFileSize);
};
