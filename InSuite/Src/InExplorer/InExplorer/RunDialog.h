// RunDialog.h

#pragma once

#include "OptionsDialogX.h"

class CRunDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CRunDialog)

public:
	CRunDialog(CWnd* pParent = NULL);
	virtual	~CRunDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedRunBrowse();

	enum { IDD = IDD_RUN };

	CStatic	m_ctlFileH;
	CEdit	m_ctlFile;
	CEdit	m_ctlParameters;
	CButton	m_ctlBrowse;
	CString	m_strFile;
	CString	m_strParameters;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();

	DECLARE_MESSAGE_MAP()
};
