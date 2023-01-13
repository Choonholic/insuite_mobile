// FilesPage.h

#pragma once

#include "PropertyPageX.h"
#include "ListCtrlX.h"

class CFilesPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CFilesPage)

public:
	CFilesPage();
	virtual	~CFilesPage();

	void	Scan();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedSelectAll();
	afx_msg void	OnBnClickedUnselectAll();

	enum { IDD = IDD_FILES };

	CListCtrlX	m_ctlItems;
	CButton		m_ctlSelectAll;
	CButton		m_ctlUnselectAll;

protected:
	void	SetHeaders();
	void	SetItems();
	void	ScanTemporaryFiles();
	void	ScanActiveSync();
	void	ScanTemporaryInternetFiles();
	void	ScanInternetCookies();
	void	ScanInternetHistory();
	void	ScanErrorReports();
	void	ScanLogFiles();
	void	ScanZeroSizeFiles();
	void	ScanInvalidShortcuts();
	void	ScanThumbnailCaches();
	void	ScanNotifications();

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
