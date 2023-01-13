// MenuPage.h

#pragma once

#include "PropertyPageX.h"
#include "TreeCtrlX.h"

#ifndef MNU_ITEMS
#define MNU_ITEMS	14
#endif

typedef struct _tagMENUINFO
{
	BOOL	bEnable;
	DWORD	dwItem;
} MENUINFO, FAR* LPMENUINFO;

class CMenuPage : public CPropertyPageX
{
	DECLARE_DYNAMIC(CMenuPage)

public:
	CMenuPage();
	virtual	~CMenuPage();

	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedUp();
	afx_msg void	OnBnClickedDown();
	afx_msg void	OnNMClickMenu(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT	OnTcxCheckToggle(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMenuCheck(WPARAM wParam, LPARAM lParam);

	enum { IDD = IDD_MENUPAGE };

	CTreeCtrlX	m_ctlMenu;
	CButton		m_ctlUp;
	CButton		m_ctlDown;
	CImageList	m_iMenuImages;

protected:
	void	LoadSettings();
	void	SaveSettings();
	void	SetTree();
	void	GetTree();

	virtual void	DoDataExchange(CDataExchange* pDX);

	MENUINFO	m_MenuInfo[MNU_ITEMS];

	DECLARE_MESSAGE_MAP()
};