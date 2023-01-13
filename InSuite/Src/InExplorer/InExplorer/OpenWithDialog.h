// OpenWithDialog.h

#pragma once

#include "DialogX.h"
#include "ListCtrlX.h"

class COpenWithDialog : public CDialogX
{
	DECLARE_DYNAMIC(COpenWithDialog)

public:
	COpenWithDialog(CWnd* pParent = NULL);
	virtual	~COpenWithDialog();

	void	SetHeaders();
	void	SetItems();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg void	OnLvnItemchangedOpenPrograms(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnBnClickedOpenBrowse();
	virtual void	OnOK();

	enum { IDD = IDD_OPEN_WITH };

	CString			m_strFile;
	CStringArray	m_arrFiles;
	CString			m_strTarget;

	CString		m_strFileName;
	CStatic		m_ctlDescription;
	CStatic		m_ctlFileName;
	CListCtrlX	m_ctlPrograms;
	CButton		m_ctlBrowse;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	CImageList	m_iSysSmall;

	DECLARE_MESSAGE_MAP()
};
