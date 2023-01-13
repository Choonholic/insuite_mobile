// EditorMultiSZ.h

#pragma once

#include "OptionsDialogX.h"

class CEditorMultiSZ : public COptionsDialogX
{
	DECLARE_DYNAMIC(CEditorMultiSZ)

public:
	CEditorMultiSZ(CWnd* pParent = NULL);
	virtual	~CEditorMultiSZ();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_EDITOR_MULTI_SZ };

	CStatic	m_ctlDataH;
	CEdit	m_ctlData;
	CString	m_strData;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
