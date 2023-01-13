// EditorName.h

#pragma once

#include "OptionsDialogX.h"

class CEditorName : public COptionsDialogX
{
	DECLARE_DYNAMIC(CEditorName)

public:
	CEditorName(CWnd* pParent = NULL);
	virtual	~CEditorName();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_EDITOR_NAME };

	CStatic	m_ctlNameH;
	CEdit	m_ctlName;
	CString	m_strName;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
