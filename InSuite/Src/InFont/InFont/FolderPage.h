// FolderPage.h

#pragma once

#include "PropertyPageX.h"

class CFolderPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CFolderPage)

public:
	CFolderPage();
	virtual	~CFolderPage();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnBnClickedFontBrowse();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_FOLDERPAGE };

	CString	m_strFontFolder;
	CStatic	m_ctlFontFolderH;
	CEdit	m_ctlFontFolder;
	CButton	m_ctlFontBrowse;

protected:
	void	LoadSettings();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
