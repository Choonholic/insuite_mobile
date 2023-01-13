// InstallDialog.h

#pragma once

#include "OptionsDialogX.h"

class CInstallDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CInstallDialog)

public:
	CInstallDialog(CWnd* pParent = NULL);
	virtual	~CInstallDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_INSTALL_DIALOG };

	CString	m_strFileName;
	CString	m_strFontFace;
	CString	m_strVersion;
	CString	m_strCopyright;
	CStatic	m_ctlFileName;
	CStatic	m_ctlFontFace;
	CStatic	m_ctlVersion;
	CStatic	m_ctlCopyright;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
