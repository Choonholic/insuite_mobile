// FontSelectDialog.h

#pragma once

#include "OptionsDialogX.h"

class CFontSelectDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFontSelectDialog)

public:
	CFontSelectDialog(CWnd* pParent = NULL);
	virtual	~CFontSelectDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_FONTSELECT_DIALOG };

	BOOL			m_bLinkOnly;
	CStringArray	m_arrFileName;
	CStringArray	m_arrFontFace;

	int			m_nSelected;
	CComboBox	m_ctlFontFace;

protected:
	void	FillList();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
