// PropertyPageX.h

#pragma once

class AFX_EXT_CLASS CPropertyPageX : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropertyPageX)

public:
	CPropertyPageX();
	CPropertyPageX(UINT nIDTemplate, UINT nIDCaption = 0);
	CPropertyPageX(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);
	virtual	~CPropertyPageX();

	void	SetHeaderItem(CWnd* pWnd);
	void	SetHeaderItem(UINT nID);
	void	DrawHeaderItem(CDC& dc, CWnd* pWnd);
	void	DrawHeaderItem(CDC& dc, UINT nID);

	afx_msg void	OnPaint();
	virtual void	OnOK();
	virtual void	OnCancel();

protected:
	CFont	m_fnHeader;

	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
