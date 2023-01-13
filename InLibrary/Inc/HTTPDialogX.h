// HTTPDialogX.h

// Based on CHTTPDownloaderDlg, Version 1.30 (08 June 2008)
// Defines the interface for an MFC dialog which performs HTTP downloads similiar to the Internet Explorer download dialog
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

#include "wininet.h"
#include "OptionsDialogX.h"
#include "ProgressCtrlX.h"
#include "HTTPDownloaderX.h"

class AFX_EXT_CLASS CHTTPDialogX : public COptionsDialogX, public CHTTPDownloaderX
{
	DECLARE_DYNAMIC(CHTTPDialogX)

public:
	CHTTPDialogX(CWnd* pParent = NULL);

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnClose();
	afx_msg void	OnDestroy();
	afx_msg LRESULT	OnThreadFinished(WPARAM wParam, LPARAM lParam);

protected:
	static UINT	_DownloadThread(LPVOID pParam);

	virtual void	OnWininetStatusCallBack(HINTERNET hInternet, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	virtual void	DownloadThread();
	virtual void	HandleError(UINT nID, DWORD dwError);
	virtual void	HandleStatusCode(UINT nID, DWORD dwStatusCode);
	virtual void	OnProgress(ULONGLONG ullBytesRead, BOOL bGotFileSize, ULONGLONG ullTotalBytesToRead);
	virtual void	SetPercentage(int nPercentage);
	virtual void	SetTimeLeft(DWORD dwSecondsLeft, ULONGLONG ullBytesRead, ULONGLONG ullFileSize);
	virtual void	SetStatus(LPCTSTR lpszStatus);
	virtual BOOL	DoCertificateSelection();
	virtual BOOL	HandlePrompt(DWORD& dwError);
	virtual void	OnGotContentLength(ULONGLONG ullContentLength);
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();
	virtual void	OnCancel();

	CStatic			m_ctlFilename;
	CProgressCtrlX	m_ctlProgress;
	CStatic			m_ctlStatus;

	CString			m_strFilename;
	CString			m_strError;
	BOOL			m_bSafeToClose;
	CWinThread*		m_pThread;
	ULONGLONG		m_ullLastTotalBytes;
	DWORD			m_dwLastPercentage;
	DWORD			m_dwLastProgressTicks;
	DWORD			m_dwStartProgressTicks;

	DECLARE_MESSAGE_MAP()
};
