// ExportRange.h

#pragma once

#include "OptionsDialogX.h"

class CExportRange : public COptionsDialogX
{
	DECLARE_DYNAMIC(CExportRange)

public:
	CExportRange(CWnd* pParent = NULL);
	virtual	~CExportRange();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_EXPORT };

	CButton	m_ctlExportAll;
	CEdit	m_ctlSelectedBrench;
	int		m_nExportRange;
	CString	m_strSelectedBrench;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
