// StatusBox.h

#pragma once

class CStatusBox : public CListBox
{
	DECLARE_DYNAMIC(CStatusBox)

public:
	CStatusBox();
	virtual	~CStatusBox();

	afx_msg void	MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	void	DrawGauge(HDC hDC, LPRECT lpRect, int nPercent, LPCTSTR lpszMessage, BOOL bReversed = FALSE);
	void	DrawMainBattery(HDC hDC, LPRECT lprc, LPCTSTR lpszText);
	void	DrawBackupBattery(HDC hDC, LPRECT lprc, LPCTSTR lpszText);
	void	DrawProgramMemory(HDC hDC, LPRECT lprc, LPCTSTR lpszText);
	void	DrawStorageMemory(HDC hDC, LPRECT lprc, LPCTSTR lpszText);
	void	DrawExternalMemory(HDC hDC, LPRECT lprc, LPCTSTR lpszText);

	DECLARE_MESSAGE_MAP()
};
