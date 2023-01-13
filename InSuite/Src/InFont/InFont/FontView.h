// FontView.h

#pragma once

#include "ListCtrlX.h"

class CFontView : public CFormView
{
	DECLARE_DYNCREATE(CFontView)

public:
	CFontView();
	virtual	~CFontView();

	void	SetListHeader();
	void	SetViewStyle();
	void	LoadFontList();
	void	AddFontToList(LPCTSTR lpszExt);
	void	SetFontLink(LPCTSTR lpszFace, LPCTSTR lpszAddFace = NULL, LPCTSTR lpszAddFile = NULL);
	BOOL	GetAntialiasing();
	void	SetAntialiasing(BOOL bSet = TRUE);
	void	ShowProperties();
	BOOL	IsEnableDelete();
	void	CreateStartup();

	virtual void	OnInitialUpdate();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult);

#ifdef _DEBUG
	virtual void	AssertValid() const;
#ifndef _WIN32_WCE
	virtual void	Dump(CDumpContext& dc) const;
#endif
#endif

	enum { IDD = IDD_FONTVIEW };

	CListCtrlX	m_ctlFontList;
	CImageList	m_iSmallImages;
	CImageList	m_iLargeImages;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	afx_msg LRESULT	OnTapAndHoldList(WPARAM wParam, LPARAM lParam);
	afx_msg void	OnInstallNewFont();
	afx_msg void	OnAddDefault();
	afx_msg void	OnAddDefaultFixed();
	afx_msg void	OnLinkTo();
	afx_msg void	OnDelete();
	afx_msg void	OnProperties();
	afx_msg void	OnFontsDefault();
	afx_msg void	OnFontsDefaultFixed();
	afx_msg void	OnFontsLinking();
	afx_msg void	OnFontsSystem();
	afx_msg void	OnAntialiasing();
	afx_msg void	OnTypeIcon();
	afx_msg void	OnTypeDetails();
	afx_msg void	OnBackupAndRestore();
	afx_msg void	OnResetDevice();
	afx_msg void	OnOptions();
	afx_msg void	OnAbout();
	afx_msg void	OnUpdateTypeIcon(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateTypeDetails(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateAntialiasing(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};
