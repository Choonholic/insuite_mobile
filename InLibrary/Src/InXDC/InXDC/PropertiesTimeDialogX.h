// PropertiesTimeDialogX.h

#pragma once

#include "OptionsDialogX.h"

class CPropertiesTimeDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CPropertiesTimeDialogX)

public:
	CPropertiesTimeDialogX(CWnd* pParent = NULL);
	virtual	~CPropertiesTimeDialogX();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	CString	m_strCreated;
	CString	m_strModified;
	CString	m_strAccessed;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	CStatic	m_ctlCreatedH;
	CStatic	m_ctlModifiedH;
	CStatic	m_ctlAccessedH;
	CStatic	m_ctlCreated;
	CStatic	m_ctlModified;
	CStatic	m_ctlAccessed;

	DECLARE_MESSAGE_MAP()
};
