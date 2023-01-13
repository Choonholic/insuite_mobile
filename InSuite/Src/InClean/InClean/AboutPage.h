// AboutPage.h

#pragma once

#include "PropertyPageX.h"

class CAboutPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CAboutPage)

public:
	CAboutPage();
	virtual	~CAboutPage();

	afx_msg void	OnPaint();

	enum { IDD = IDD_ABOUT };

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
