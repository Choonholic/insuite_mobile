// SystemPage.h

#pragma once

#include "PropertyPageX.h"
#include "ListCtrlX.h"

#pragma warning(disable: 4819)
#include <regext.h>
#pragma warning(default: 4819)

class CSystemPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CSystemPage)

public:
	CSystemPage();
	virtual	~CSystemPage();

	void	SetHeaders();
	void	SetItems();
	void	Refresh();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnDestroy();
#if (_WIN32_WCE >= 0x0502)
	afx_msg LRESULT	OnWiFiStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnBluetoothStatus(WPARAM wParam, LPARAM lParam);
#endif

	enum { IDD = IDD_SYSTEM };

	CListCtrlX	m_ctlSystem;

protected:
#if (_WIN32_WCE >= 0x0502)
	void	RegisterCallback();
	void	UnregisterCallback();
#endif

	virtual void	DoDataExchange(CDataExchange* pDX);

#if (_WIN32_WCE >= 0x0502)
	HREGNOTIFY	m_hRegNotify[2];
#endif

	DECLARE_MESSAGE_MAP()
};
