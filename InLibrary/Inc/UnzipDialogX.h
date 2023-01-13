// UnzipDialogX.h

#pragma once

#include "OptionsDialogX.h"
#include "ProgressCtrlX.h"
#include "UnzipX.h"

class AFX_EXT_CLASS CUnzipDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CUnzipDialogX)

public:
	CUnzipDialogX(CWnd* pParent = NULL);
	virtual	~CUnzipDialogX();

	void	SetSource(LPCTSTR lpszFileName, const char* szPassword);
	void	SetDestination(LPCTSTR lpszFolder);

	void	SetCodePage(UINT uiCodePage = CP_ACP);

	void	AddFile(LPCTSTR lpszFile);
	void	AddFiles(CStringArray &arrFiles);
	void	AddAllFiles();
	void	ClearFiles();

	void	ShowResult(BOOL bShow = FALSE);

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnClose();
	afx_msg void	OnDestroy();
	afx_msg LRESULT	OnThreadFinished(WPARAM wParam, LPARAM lParam);

protected:
	static UINT	_UnzipThread(LPVOID pParam);

	void	InitializeX();
	void	ExecUnzip();

	virtual void	UpdateUI(LPCTSTR lpszFilename, DWORD dwUnzipped, DWORD dwSize);
	virtual void	UnzipThread();
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();
	virtual void	OnCancel();

	CString		m_strFileName;
	char		m_szPassword[1024];
	CString		m_strDestination;
	UINT		m_uiCodePage;
	ULONGLONG	m_ullUnzipped;
	ULONGLONG	m_ullTotal;
	int			m_nCurrent;
	int			m_nTotal;
	CDWordArray	m_arrIndexes;
	HZIP		m_hZip;
	ZIPENTRY	m_tEntry;
	BOOL		m_bSafeToClose;
	BOOL		m_bAbort;
	BOOL		m_bShowResult;
	CWinThread*	m_pThread;

	CStatic			m_ctlFilename;
	CProgressCtrlX	m_ctlProgress;
	CProgressCtrlX	m_ctlTotal;
	CStatic			m_ctlStatus;

	DECLARE_MESSAGE_MAP()
};
