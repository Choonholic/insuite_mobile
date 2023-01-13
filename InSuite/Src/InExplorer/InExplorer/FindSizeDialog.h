// FindSizeDialog.h

#pragma once

#include "OptionsDialogX.h"
#include "SpinButtonCtrlX.h"

class CFindSizeDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFindSizeDialog)

public:
	CFindSizeDialog(CWnd* pParent = NULL);
	virtual	~CFindSizeDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedFindSizelarger();
	afx_msg void	OnBnClickedFindSizesmaller();

	enum { IDD = IDD_FIND_SIZE };

	CButton				m_ctlLarger;
	CEdit				m_ctlLargerEdit;
	CSpinButtonCtrlX	m_ctlLargerSpin;
	CButton				m_ctlSmaller;
	CEdit				m_ctlSmallerEdit;
	CSpinButtonCtrlX	m_ctlSmallerSpin;
	BOOL				m_bLarger;
	DWORD				m_dwLarger;
	BOOL				m_bSmaller;
	DWORD				m_dwSmaller;

protected:
	void	UpdateControls();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
