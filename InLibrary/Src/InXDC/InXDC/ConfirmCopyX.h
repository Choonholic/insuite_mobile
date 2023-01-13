// ConfirmCopyX.h

#pragma once

#include "DialogX.h"

#ifndef CONFIRMMODE
#define CONFIRMMODE
#define CONFIRMMODE_COPY	0
#define CONFIRMMODE_MOVE	1
#endif

class CConfirmCopyX : public CDialogX
{
	DECLARE_DYNAMIC(CConfirmCopyX)

public:
	CConfirmCopyX(CWnd* pParent = NULL);
	virtual	~CConfirmCopyX();

	void	InitializeX();

	void	SetMode(int nMode = CONFIRMMODE_COPY);
	void	SetInfo(LPCTSTR lpszFileName, LPCTSTR lpszSource, LPCTSTR lpszTarget, DWORD dwFlags);
	DWORD	GetInfo();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg void	OnBnClickedReplace();
	afx_msg void	OnBnClickedNotChange();

protected:
	CString	GetSizeString(ULONGLONG ulSize);
	CString	GetTimeString(CTime time);

	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnCancel();

	CString	m_strMessage;
	CString	m_strReplace;
	CString	m_strNotChange;

	CString	m_strSource;
	CString	m_strTarget;
	DWORD	m_dwFlags;
	int		m_nMode;

	CStatic	m_ctlMessage;
	CButton	m_ctlReplace;
	CButton	m_ctlNotChange;
	CButton	m_ctlDoConflict;
	BOOL	m_bDoConflict;

	DECLARE_MESSAGE_MAP()
};
