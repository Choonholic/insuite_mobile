// ZipDialogX.h

#pragma once

#include "OptionsDialogX.h"
#include "ProgressCtrlX.h"
#include "ZipX.h"

class AFX_EXT_CLASS CZipDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CZipDialogX)

public:
	CZipDialogX(CWnd* pParent = NULL);
	virtual	~CZipDialogX();

	void	SetSource(LPCTSTR lpszFolder);
	void	SetZIPFile(LPCTSTR lpszFileName, const char* szPassword);

	void	SetCodePage(UINT uiCodePage = CP_ACP);
	void	SetLevel(int nLevel = 8);

	void	AddFile(LPCTSTR lpszFile);
	void	AddFiles(CStringArray &arrFiles);
	void	ClearFiles();

	void	ShowResult(BOOL bShow = FALSE);

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnClose();
	afx_msg void	OnDestroy();
	afx_msg LRESULT	OnThreadFinished(WPARAM wParam, LPARAM lParam);

protected:
	static UINT	_ZipThread(LPVOID pParam);

	void	InitializeX();
	BOOL	IsEnableZip(LPCTSTR lpszFileName);
	void	ExecZip(LPCTSTR lpszSource, LPCTSTR lpszTarget);

	virtual void	UpdateUI(LPCTSTR lpszFilename);
	virtual void	ZipThread();
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();
	virtual void	OnCancel();

	CString			m_strSource;
	CString			m_strFileName;
	char			m_szPassword[1024];
	UINT			m_uiCodePage;
	int				m_nLevel;
	ULONGLONG		m_ullZipped;
	ULONGLONG		m_ullTotal;
	int				m_nCurrent;
	int				m_nTotal;
	HZIP			m_hZip;
	CStringArray	m_arrFiles;
	CStringArray	m_arrFolders;
	BOOL			m_bSafeToClose;
	BOOL			m_bAbort;
	BOOL			m_bShowResult;
	CWinThread*		m_pThread;

	CStatic			m_ctlFilename;
	CProgressCtrlX	m_ctlProgress;
	CStatic			m_ctlStatus;

	DECLARE_MESSAGE_MAP()
};
