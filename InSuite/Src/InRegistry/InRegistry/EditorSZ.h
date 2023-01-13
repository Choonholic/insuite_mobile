// EditorSZ.h

#pragma once

#include "OptionsDialogX.h"

class CEditorSZ : public COptionsDialogX
{
	DECLARE_DYNAMIC(CEditorSZ)

public:
	CEditorSZ(CWnd* pParent = NULL);
	virtual	~CEditorSZ();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_EDITOR_SZ };

	CStatic	m_ctlDataH;
	CEdit	m_ctlData;
	CString	m_strData;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
