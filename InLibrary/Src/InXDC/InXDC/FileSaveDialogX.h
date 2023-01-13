// FileSaveDialogX.h

#pragma once

#include "OptionsDialogX.h"

class AFX_EXT_CLASS CFileSaveDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFileSaveDialogX)

public:
	CFileSaveDialogX(CWnd* pParent = NULL);
	virtual	~CFileSaveDialogX();

	void	SetTypePairs(LPCTSTR lpszTypePairs);
	void	SetTypePairs(UINT nID);
	void	SetDefaultFilename(LPCTSTR lpszFilename);
	void	SetDefaultFilename(UINT nID);
	void	SetOverwrite(BOOL bSet = TRUE);
	void	AskOverwrite(BOOL bSet = TRUE);

	CString	GetFullPath();
	CString	GetPath();
	CString	GetFile();
	CString	GetTitle();
	CString	GetExtension();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnBnClickedSaveBrowse();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();

	CStringArray	m_arrTypes;
	CStringArray	m_arrDescriptions;
	CString			m_strExtension;
	BOOL			m_bSetOverwrite;
	BOOL			m_bAskOverwrite;

#if (!defined(_DEBUG))
	CEdit	m_ctlName;
	CEdit	m_ctlFolder;
#endif

	CStatic		m_ctlNameH;
	CString		m_strName;
	CStatic		m_ctlFolderH;
	CString		m_strFolder;
	CButton		m_ctlBrowse;
	CStatic		m_ctlTypeH;
	CComboBox	m_ctlType;
	int			m_nType;

	DECLARE_MESSAGE_MAP()
};
