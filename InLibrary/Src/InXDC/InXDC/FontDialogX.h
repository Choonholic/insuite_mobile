// FontDialogX.h

#pragma once

#include "OptionsDialogX.h"
#include "ComboBoxX.h"
#include "ColorPickerX.h"
#include "StaticX.h"

class AFX_EXT_CLASS CFontDialogX : public COptionsDialogX
{
	DECLARE_DYNAMIC(CFontDialogX)

public:
	CFontDialogX(CWnd* pParent = NULL);
	virtual	~CFontDialogX();

	void	SetPreviewText(LPCTSTR lpszText);
	void	SetBoldWeight(LONG lWeight);
	void	SetClearTypeCompatible(BOOL bCompatible = FALSE);

	void	SetDefFont(LPLOGFONT lpLogFont);
	void	SetDefFont(HFONT hFont);
	void	SetSrcFont(LPLOGFONT lpLogFont);
	void	SetSrcFont(HFONT hFont);
	void	GetResFont(LPLOGFONT lpLogFont);
	HFONT	GetResFont();

	void		EnableColor(BOOL bEnable = FALSE);
	void		SetDefColor(COLORREF cr);
	void		SetSrcColor(COLORREF cr);
	COLORREF	GetResColor();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg void	OnBnClickedFontDefault();
	afx_msg void	OnBnClickedFontBold();
	afx_msg void	OnBnClickedFontItalic();
	afx_msg void	OnBnClickedFontUnderline();
	afx_msg void	OnBnClickedFontCleartype();
	afx_msg void	OnCbnSelchangeFontFont();
	afx_msg void	OnCbnEditchangeFontFont();
	afx_msg void	OnCbnSelchangeFontSize();
	afx_msg void	OnCbnEditchangeFontSize();
	afx_msg LRESULT	OnSelEndOKColor(WPARAM wParam, LPARAM lParam);

	CStatic			m_ctlFontH;
	CComboBoxX		m_ctlFont;
	CButton			m_ctlDefault;
	CComboBoxX		m_ctlSize;
	CStatic			m_ctlColorH;
	CColorPickerX	m_ctlColor;
	CStatic			m_ctlPreviewH;
	CStaticX		m_ctlPreview;
	CString			m_strPreview;
	BOOL			m_bEnableColor;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	void	RenderFont();
	void	SetContents();

	LONG	m_lBoldWeight;
	BYTE	m_nClearType;

	LOGFONT		m_logDefault;
	COLORREF	m_crDefault;

	LOGFONT	m_logFont;
	CFont	m_PreviewFont;

	DECLARE_MESSAGE_MAP()
};
