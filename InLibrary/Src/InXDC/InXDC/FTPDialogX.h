// FTPDialogX.h

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

#pragma once

#include "wininet.h"
#include "OptionsDialogX.h"
#include "ProgressCtrlX.h"
#include "FTPTransfererX.h"

class AFX_EXT_CLASS CFTPDialogX : public COptionsDialogX, public CFTPTransfererX
{
	DECLARE_DYNAMIC(CFTPDialogX)

public:
	CFTPDialogX(CWnd* pParent = NULL);

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnClose();
	afx_msg void	OnDestroy();
	afx_msg LRESULT	OnThreadFinished(WPARAM wParam, LPARAM lParam);

protected:
	static UINT	_TransferThread(LPVOID pParam);

	virtual void	TransferThread();
	virtual void	SetPercentage(int nPercentage);
	virtual void	SetTimeLeft(DWORD dwSecondsLeft, ULONGLONG ullBytesRead, ULONGLONG ullFileSize);
	virtual void	SetStatus(LPCTSTR lpszStatus);
	virtual void	OnProgress(ULONGLONG ullBytesRead, BOOL bGotFileSize, ULONGLONG ullTotalBytesToRead);
	virtual void	HandleError(UINT nID, DWORD dwError);
	virtual void	OnWininetStatusCallBack(HINTERNET hInternet, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();
	virtual void	OnCancel();

	CString		m_strError;
	BOOL		m_bSafeToClose;
	ULONGLONG	m_ullLastTotalBytes;
	DWORD		m_dwLastPercentage;
	DWORD		m_dwLastProgressTicks;
	DWORD		m_dwStartProgressTicks;
	CWinThread*	m_pThread;

	CStatic			m_ctlFilename;
	CProgressCtrlX	m_ctlProgress;
	CStatic			m_ctlStatus;

	DECLARE_MESSAGE_MAP()
};
