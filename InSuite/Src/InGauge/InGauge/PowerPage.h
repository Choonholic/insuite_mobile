// PowerPage.h

#pragma once

#include "PropertyPageX.h"
#include "StatusBox.h"

#pragma warning(disable: 4819)
#include <regext.h>
#pragma warning(default: 4819)

class CPowerPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CPowerPage)

public:
	CPowerPage();
	virtual	~CPowerPage();

	void	SetItems();
	void	Refresh();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnDestroy();
	afx_msg LRESULT	OnBatteryStatus(WPARAM wParam, LPARAM lParam);

	enum { IDD = IDD_POWER };

	CStatusBox		m_ctlBattery;
	CStringArray	m_arrItems;

protected:
	void	RegisterCallback();
	void	UnregisterCallback();

	virtual void	DoDataExchange(CDataExchange* pDX);

	HREGNOTIFY	m_hRegNotify;

	DECLARE_MESSAGE_MAP()
};
