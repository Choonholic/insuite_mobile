// RegistryPage.h

#pragma once

#include "PropertyPageX.h"
#include "ListCtrlX.h"

class CRegistryPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CRegistryPage)

public:
	CRegistryPage();
	virtual	~CRegistryPage();

	void	Scan();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedSelectAll();
	afx_msg void	OnBnClickedUnselectAll();

	enum { IDD = IDD_REGISTRY };

	CListCtrlX	m_ctlItems;
	CButton		m_ctlSelectAll;
	CButton		m_ctlUnselectAll;

protected:
	void	SetHeaders();
	void	SetItems();
	void	ScanActiveXCOMObjects();
	void	ScanObsoleteKeys();

	void	DeepScanCLSID();
	void	DeepScanClasses();
	void	DeepScanAppID();
	void	DeepScanObsoleteKeys(HKEY hKey, LPCTSTR lpszSubKey);

	BOOL	IsAppIDExist(LPCTSTR lpszAppID);
	BOOL	IsAppIconExist(LPCTSTR lpszIcon);
	BOOL	IsProgIDExist(LPCTSTR lpszProgID);
	BOOL	IsCLSIDExist(LPCTSTR lpszCLSID);

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
