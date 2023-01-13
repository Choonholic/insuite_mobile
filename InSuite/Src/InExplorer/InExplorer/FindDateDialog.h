// FindDateDialog.h

#pragma once

#include "OptionsDialogX.h"

class CFindDateDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFindDateDialog)

public:
	CFindDateDialog(CWnd* pParent = NULL);
	virtual	~CFindDateDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedFindDatefrom();
	afx_msg void	OnBnClickedFindDateto();
	virtual void	OnOK();

	enum { IDD = IDD_FIND_DATE };

	CButton			m_ctlFrom;
	CDateTimeCtrl	m_ctlFromPicker;
	CButton			m_ctlTo;
	CDateTimeCtrl	m_ctlToPicker;
	BOOL			m_bFrom;
	SYSTEMTIME		m_stFrom;
	BOOL			m_bTo;
	SYSTEMTIME		m_stTo;

protected:
	void	UpdateControls();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
