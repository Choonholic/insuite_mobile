// DeleteDialogX.h

#pragma once

#include "OptionsDialogX.h"
#include "ProgressCtrlX.h"

class AFX_EXT_CLASS CDeleteDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CDeleteDialogX)

public:
	CDeleteDialogX(CWnd* pParent = NULL);
	virtual	~CDeleteDialogX();

	void	SetConfirm(BOOL bConfirm = TRUE);

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
	static UINT	_DeleteThread(LPVOID pParam);

	void	InitializeX();
	BOOL	IsEnableDelete(LPCTSTR lpszFilename, DWORD& dwAttributes);
	BOOL	ExecDelete(LPCTSTR lpszFileName, DWORD& dwAttributes);

	virtual void	UpdateUI(LPCTSTR lpszFilename);
	virtual void	DeleteThread();
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();
	virtual void	OnCancel();

	BOOL			m_bConfirm;
	CStringArray	m_arrFiles;
	CStringArray	m_arrFolders;
	int				m_nCurrent;
	int				m_nTotal;
	BOOL			m_bSafeToClose;
	BOOL			m_bAbort;
	BOOL			m_bShowResult;
	CWinThread*		m_pThread;

	CStatic			m_ctlFilename;
	CProgressCtrlX	m_ctlProgress;
	CStatic			m_ctlStatus;

	DECLARE_MESSAGE_MAP()
};
