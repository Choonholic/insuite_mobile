// DWordEditX.h

#pragma once

enum DWORD_EDIT_BASE
{
	BASE_HEX,
	BASE_DEC
};

class AFX_EXT_CLASS CDWordEditX : public CEdit
{
	DECLARE_DYNAMIC(CDWordEditX)

public:
	CDWordEditX();
	virtual	~CDWordEditX();

	void	SetDWordValue(DWORD dwValue);
	DWORD	GetDWordValue();

	void	SetBase(int nBase = BASE_DEC);

	afx_msg void	OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	int		m_nBase;
	DWORD	m_dwValue;
	BOOL	m_bProcessing;

	DECLARE_MESSAGE_MAP()
};
