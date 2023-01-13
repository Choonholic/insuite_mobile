// RegisterDialogX.h

#pragma once

#include "OptionsDialogX.h"

class AFX_EXT_CLASS CRegisterDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CRegisterDialogX)

public:
	CRegisterDialogX(CWnd* pParent = NULL);
	virtual	~CRegisterDialogX();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	WORD	m_wProductID;
	WORD	m_wVersion;
	CString	m_strBinary;
	int		m_nPeriod;
	int		m_nStatus;

protected:
	virtual void	OnOK();
	virtual void	DoDataExchange(CDataExchange* pDX);

	CStatic	m_ctlEMailH;
	CEdit	m_ctlEMail;
	CStatic	m_ctlSerialH;
	CEdit	m_ctlSerial;
	CStatic	m_ctlPeriod;
	CString m_strEMail;
	CString m_strSerial;
	CString	m_strPeriod;

	DECLARE_MESSAGE_MAP()
};
