// ProgressCtrlX.h

#pragma once

class AFX_EXT_CLASS CProgressCtrlX : public CProgressCtrl
{
	DECLARE_DYNAMIC(CProgressCtrlX)

public:
	CProgressCtrlX();
	virtual	~CProgressCtrlX();

	void	SetDefaultParameters();

	void	SetDetailsColor(COLORREF cr);
	void	SetBarColor(COLORREF crFirst, COLORREF crLast, COLORREF crBack = RGB(0xCC, 0xCC, 0xCC));

	void	ShowDetails(BOOL bShow = TRUE);
	void	SetDetailsFont(LPCTSTR lpszFontFace, int nDoublePoint, LONG lWeight = FW_NORMAL);

	afx_msg void	OnPaint();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);

protected:
	COLORREF	m_crDetails;
	COLORREF	m_crBack;
	COLORREF	m_crFirst;
	COLORREF	m_crLast;
	BOOL		m_bShowDetails;
	HFONT		m_hDetailsFont;

	DECLARE_MESSAGE_MAP()
};
