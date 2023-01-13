// StoragePage.h

#pragma once

#include "PropertyPageX.h"
#include "StatusBox.h"

class CStoragePage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CStoragePage)

public:
	CStoragePage();
	virtual	~CStoragePage();

	void	SetItems();
	void	Refresh();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT	OnDeviceChange(WPARAM wParam, LPARAM lParam);

	enum { IDD = IDD_STORAGE };

	CStatusBox		m_ctlStorage;
	CStringArray	m_arrItems;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
