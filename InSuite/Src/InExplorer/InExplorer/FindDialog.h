// FindDialog.h

#pragma once

#include "OptionsDialogX.h"

class CFindDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFindDialog)

public:
	CFindDialog(CWnd* pParent = NULL);
	virtual	~CFindDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedFindBrowse();
	afx_msg void	OnBnClickedFindDate();
	afx_msg void	OnBnClickedFindSize();
	virtual void	OnOK();

	enum { IDD = IDD_FIND };

	CStatic		m_ctlFilterH;
	CEdit		m_ctlFilter;
	CStatic		m_ctlFromH;
	CEdit		m_ctlFrom;
	CButton		m_ctlBrowse;
	CButton		m_ctlSubFolders;
	CButton		m_ctlDate;
	CButton		m_ctlSize;
	CString		m_strFilter;
	CString		m_strFrom;
	BOOL		m_bSubFolders;
	BOOL		m_bFrom;
	SYSTEMTIME	m_stFrom;
	BOOL		m_bTo;
	SYSTEMTIME	m_stTo;
	BOOL		m_bLarger;
	DWORD		m_dwLarger;
	BOOL		m_bSmaller;
	DWORD		m_dwSmaller;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
