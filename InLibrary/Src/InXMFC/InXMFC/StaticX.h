// StaticX.h

#pragma once

class AFX_EXT_CLASS CStaticX : public CStatic
{
	DECLARE_DYNAMIC(CStaticX)

public:
	CStaticX();
	virtual	~CStaticX();

	void	SetTextColor(COLORREF cr);
	void	SetBackColor(COLORREF cr);

	afx_msg HBRUSH	CtlColor(CDC* pDC, UINT nCtlColor);

protected:
	CBrush		m_brBkgnd;
	COLORREF	m_crText;
	COLORREF	m_crBack;

	DECLARE_MESSAGE_MAP()
};
