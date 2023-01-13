// BackupDialog.h

#pragma once

#include "OptionsDialogX.h"

class CBackupDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CBackupDialog)

public:
	CBackupDialog(CWnd* pParent = NULL);
	virtual	~CBackupDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedBrBackup();
	afx_msg void	OnBnClickedBrRestore();

	enum { IDD = IDD_BACKUP_DIALOG };

	CStatic	m_ctlBackupH;
	CButton	m_ctlBackup;
	CStatic	m_ctlRestoreH;
	CButton	m_ctlRestore;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
