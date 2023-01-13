// ExcludedType.h

#pragma once

#include "OptionsDialogX.h"

class CExcludedType : public COptionsDialogX
{
	DECLARE_DYNAMIC(CExcludedType)

public:
	CExcludedType(CWnd* pParent = NULL);
	virtual	~CExcludedType();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_EXCLUDEDTYPE };

	BOOL	m_bType;
	BOOL	m_bAlways;
	CStatic	m_ctlAppTypeH;
	CStatic	m_ctlAddOptionsH;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
