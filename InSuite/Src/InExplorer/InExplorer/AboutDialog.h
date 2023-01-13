// AboutDialog.h

#pragma once

#include "DialogX.h"

class CAboutDialog : public CDialogX
{
	DECLARE_DYNAMIC(CAboutDialog)

public:
	CAboutDialog(CWnd* pParent = NULL);
	virtual	~CAboutDialog();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnPaint();
	afx_msg void	OnRegister();

	enum { IDD = IDD_ABOUT_DIALOG };

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
