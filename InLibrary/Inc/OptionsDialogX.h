// OptionsDialogX.h

#pragma once

#include "DialogX.h"

class AFX_EXT_CLASS COptionsDialogX : public CDialogX
{
	DECLARE_DYNAMIC(COptionsDialogX)

public:
	COptionsDialogX();
	COptionsDialogX(UINT nIDTemplate, CWnd* pParent);
	COptionsDialogX(LPCTSTR lpszTemplateName, CWnd* pParent);
	virtual	~COptionsDialogX();

	void	SetHeader(UINT nID);
	void	SetHeader(LPCTSTR lpszHeader);
	LPCTSTR	GetHeader();
	
	int	GetHeaderHeight();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnDestroy();
	afx_msg void	OnPaint();

protected:
	void	DrawDialogTitle(HDC hDC);
	void	GetHeaderFont();

	virtual void	DoDataExchange(CDataExchange* pDX);

	CString	m_strHeader;
	HFONT	m_hHeaderFont;
	DWORD	m_dwHeaderLine;

	DECLARE_MESSAGE_MAP()
};
