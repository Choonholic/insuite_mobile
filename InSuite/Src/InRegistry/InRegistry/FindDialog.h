// FindDialog.h

#pragma once

#include "OptionsDialogX.h"

class CFindDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFindDialog)

public:
	CFindDialog(CWnd* pParent = NULL);
	virtual	~CFindDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_FIND };

	CEdit	m_ctlFindWhat;
	CStatic	m_ctlLookAt;
	CButton	m_ctlKeys;
	CButton	m_ctlValues;
	CButton	m_ctlData;
	CString	m_strFindWhat;
	BOOL	m_bKeys;
	BOOL	m_bValues;
	BOOL	m_bData;
	BOOL	m_bMatchOnly;
	BOOL	m_bCaseSensitive;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
