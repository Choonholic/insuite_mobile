// ComponentList.h

#pragma once

class CComponentList : public CListBox
{
	DECLARE_DYNAMIC(CComponentList)

public:
	CComponentList();
	virtual	~CComponentList();

	afx_msg void	MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};
