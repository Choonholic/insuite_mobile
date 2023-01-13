// ScreenPage.h

#pragma once

#include "PropertyPageX.h"

class CScreenPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CScreenPage)

public:
	CScreenPage();
	virtual	~CScreenPage();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();

	enum { IDD = IDD_SCREENPAGE };

	int		m_nRotation;
	BOOL	m_bWakeupLock;
	CStatic	m_ctlRotationH;
	CStatic	m_ctlDeviceLockH;

protected:
	void	LoadSettings();
	void	SaveSettings();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
