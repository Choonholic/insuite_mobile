// ConfirmDeleteX.h

#pragma once

#include "DialogX.h"

class CConfirmDeleteX : public CDialogX
{
	DECLARE_DYNAMIC(CConfirmDeleteX)

public:
	CConfirmDeleteX(CWnd* pParent = NULL);
	virtual	~CConfirmDeleteX();

	void	InitializeX();
	void	SetInfo(LPCTSTR lpszFileName, DWORD dwAttributes, DWORD dwFlags);
	DWORD	GetInfo();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg void	OnBnClickedYes();
	afx_msg void	OnBnClickedSkip();

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnCancel();

	CString	m_strFile;
	DWORD	m_dwAttributes;
	DWORD	m_dwFlags;
	BOOL	m_bDoThisForAll;

	CStatic	m_ctlMessage;
	CButton	m_ctlYes;
	CButton	m_ctlSkip;
	CButton	m_ctlDoThisForAll;

	DECLARE_MESSAGE_MAP()
};
