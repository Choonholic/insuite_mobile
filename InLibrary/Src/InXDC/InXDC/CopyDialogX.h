// CopyDialogX.h

#pragma once

#include "OptionsDialogX.h"
#include "ProgressCtrlX.h"

class AFX_EXT_CLASS CCopyDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CCopyDialogX)

public:
	CCopyDialogX(CWnd* pParent = NULL);
	virtual	~CCopyDialogX();

	void	SetSource(LPCTSTR lpszFolder);
	void	SetDestination(LPCTSTR lpszFolder);

	void	AddFile(LPCTSTR lpszFile);
	void	AddFiles(CStringArray& arrFiles);
	void	ClearFiles();

	void	ShowResult(BOOL bShow = FALSE);

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnClose();
	afx_msg void	OnDestroy();
	afx_msg LRESULT	OnThreadFinished(WPARAM wParam, LPARAM lParam);

protected:
	static UINT	_CopyThread(LPVOID pParam);

	void	InitializeX();
	BOOL	IsEnableCopy(LPCTSTR lpszFileName, DWORD& dwSize);
	void	ExecCopy(LPCTSTR lpszSource, LPCTSTR lpszTarget);

	virtual void	UpdateUI(LPCTSTR lpszFilename, DWORD dwCopied, DWORD dwSize);
	virtual void	CopyThread();
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();
	virtual void	OnCancel();

	CString			m_strSource;
	CString			m_strDestination;
	ULONGLONG		m_ullCopied;
	ULONGLONG		m_ullTotal;
	int				m_nCurrent;
	int				m_nTotal;
	CStringArray	m_arrFiles;
	CStringArray	m_arrFolders;
	BOOL			m_bSafeToClose;
	BOOL			m_bAbort;
	BOOL			m_bShowResult;
	CWinThread*		m_pThread;

	CStatic			m_ctlFilename;
	CProgressCtrlX	m_ctlProgress;
	CProgressCtrlX	m_ctlTotal;
	CStatic			m_ctlStatus;

	DECLARE_MESSAGE_MAP()
};
