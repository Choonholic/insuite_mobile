// FontLinkDialog.h

#pragma once

#include "OptionsDialogX.h"

class CFontLinkDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFontLinkDialog)

public:
	CFontLinkDialog(CWnd* pParent = NULL);
	virtual	~CFontLinkDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnLbnSelchangeFlFontlist();
	afx_msg void	OnBnClickedFlAdd();
	afx_msg void	OnBnClickedFlRemove();
	afx_msg void	OnBnClickedFlMoveup();
	afx_msg void	OnBnClickedFlMovedown();
	afx_msg void	OnBnClickedFlSkiptableDefault();

	enum { IDD = IDD_FONTLINK_DIALOG };

	CStringArray	m_arrFileName;
	CStringArray	m_arrFontFace;
	CString			m_strSkipTableText;

	CListBox	m_ctlList;
	CButton		m_ctlAdd;
	CButton		m_ctlRemove;
	CButton		m_ctlMoveUp;
	CButton		m_ctlMoveDown;
	CStatic		m_ctlSkipTable;
	CEdit		m_ctlSkipTableText;
	CButton		m_ctlSkipTableDefault;

protected:
	void	FillList();
	void	ResetButtons();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
