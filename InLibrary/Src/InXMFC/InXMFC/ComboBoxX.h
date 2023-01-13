// ComboBoxX.h

#pragma once

// ComboBoxX Show Types
#ifndef CBX_NORMAL
#define CBX_NORMAL		0
#define CBX_UNDERLINE	1
#endif

class AFX_EXT_CLASS CComboBoxX : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxX)

public:
	CComboBoxX();
	virtual	~CComboBoxX();

	void	SetShowType(int nShowType = CBX_NORMAL);

	afx_msg void	OnPaint();

protected:
	int	m_nShowType;

	DECLARE_MESSAGE_MAP()
};
