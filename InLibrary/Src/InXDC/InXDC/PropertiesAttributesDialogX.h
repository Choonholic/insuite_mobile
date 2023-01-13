// PropertiesAttributesDialogX.h

#pragma once

#include "OptionsDialogX.h"

class CPropertiesAttributesDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CPropertiesAttributesDialogX)

public:
	CPropertiesAttributesDialogX(CWnd* pParent = NULL);
	virtual	~CPropertiesAttributesDialogX();

	virtual BOOL	OnInitDialog();

	BOOL	m_bReadOnly;
	BOOL	m_bHidden;
	BOOL	m_bArchive;
	BOOL	m_bSystem;
	BOOL	m_bROM;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	CButton	m_ctlReadOnly;
	CButton	m_ctlHidden;
	CButton	m_ctlArchive;
	CButton	m_ctlSystem;
	CButton	m_ctlROM;

	DECLARE_MESSAGE_MAP()
};
