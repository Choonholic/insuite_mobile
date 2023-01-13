// MoveDialogX.h

#pragma once

#include "OptionsDialogX.h"
#include "ProgressCtrlX.h"

class AFX_EXT_CLASS CMoveDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CMoveDialogX)

public:
	CMoveDialogX(CWnd* pParent = NULL);
	virtual	~CMoveDialogX();

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
	static UINT	_MoveThread(LPVOID pParam);

	void	InitializeX();
	BOOL	IsEnableMove(LPCTSTR lpszFileName, DWORD& dwSize);
	BOOL	IsEnableDelete(LPCTSTR lpszFileName, DWORD& dwAttributes);
	void	ExecCopy(LPCTSTR lpszSource, LPCTSTR lpszTarget);
	BOOL	ExecDelete(LPCTSTR lpszFileName, DWORD& dwAttributes);

	virtual void	UpdateUI(int nType, LPCTSTR lpszFilename, DWORD dwCopied, DWORD dwSize);
	virtual void	MoveThread();
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();
	virtual void	OnCancel();

	CString			m_strSource;
	CString			m_strDestination;
	CStringArray	m_arrFiles;
	CStringArray	m_arrFolders;
	CStringArray	m_arrFilesDelete;
	CStringArray	m_arrFoldersDelete;
	ULONGLONG		m_ullMoved;
	ULONGLONG		m_ullTotal;
	int				m_nCurrent;
	int				m_nTotal;
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
