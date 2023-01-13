
 FontDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "FontDialogX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CFontDialogX, COptionsDialogX)

CFontDialogX::CFontDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_FONT_DIALOG, pParent)
{
	CString	strText;
	LOGFONT	lfFont;

	SetHeader(IDS_FONT_HEADER);
	m_strPreview.LoadString(IDS_FONT_PREVIEW);
	GetDefaultFont(&lfFont);
	SetDefFont(CreateFontIndirect(&lfFont));
	m_ctlColor.SetColor(GetSysColor(COLOR_WINDOWTEXT));
	strText.LoadString(IDS_FONT_AUTOMATIC);
	m_ctlColor.SetDefaultText(strText);
	strText.LoadString(IDS_FONT_CANCEL);
	m_ctlColor.SetCancelText(strText);

	m_lBoldWeight	= GetDefaultBoldWeight();
	m_nClearType	= CLEARTYPE_QUALITY;
	m_bEnableColor	= FALSE;
}

CFontDialogX::~CFontDialogX()
{
}

void CFontDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FNTD_FONT_H, m_ctlFontH);
	DDX_Control(pDX, IDC_FNTD_FONT, m_ctlFont);
	DDX_Control(pDX, IDC_FNTD_DEFAULT, m_ctlDefault);
	DDX_Control(pDX, IDC_FNTD_SIZE, m_ctlSize);
	DDX_Control(pDX, IDC_FNTD_COLOR_H, m_ctlColorH);
	DDX_Control(pDX, IDC_FNTD_COLOR, m_ctlColor);
	DDX_Control(pDX, IDC_FNTD_PREVIEW_H, m_ctlPreviewH);
	DDX_Control(pDX, IDC_FNTD_PREVIEW, m_ctlPreview);
	DDX_Text(pDX, IDC_FNTD_PREVIEW, m_strPreview);
}

BEGIN_MESSAGE_MAP(CFontDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FNTD_DEFAULT, &CFontDialogX::OnBnClickedFontDefault)
	ON_BN_CLICKED(IDC_FNTD_BOLD, &CFontDialogX::OnBnClickedFontBold)
	ON_BN_CLICKED(IDC_FNTD_ITALIC, &CFontDialogX::OnBnClickedFontItalic)
	ON_BN_CLICKED(IDC_FNTD_UNDERLINE, &CFontDialogX::OnBnClickedFontUnderline)
	ON_BN_CLICKED(IDC_FNTD_CLEARTYPE, &CFontDialogX::OnBnClickedFontCleartype)
	ON_CBN_SELCHANGE(IDC_FNTD_FONT, &CFontDialogX::OnCbnSelchangeFontFont)
	ON_CBN_EDITCHANGE(IDC_FNTD_FONT, &CFontDialogX::OnCbnEditchangeFontFont)
	ON_CBN_SELCHANGE(IDC_FNTD_SIZE, &CFontDialogX::OnCbnSelchangeFontSize)
	ON_CBN_EDITCHANGE(IDC_FNTD_SIZE, &CFontDialogX::OnCbnEditchangeFontSize)
	ON_MESSAGE(CPN_SELENDOK, &CFontDialogX::OnSelEndOKColor)
END_MESSAGE_MAP()

int CALLBACK FontDialogEnumProc(ENUMLOGFONT* lpelf, TEXTMETRIC* lpntm, int FontType, LPARAM lParam)
{
	CFontDialogX* pPage	= (CFontDialogX*)lParam;

	pPage->m_ctlFont.AddString(lpelf->elfLogFont.lfFaceName);
	return 1;
}

void CFontDialogX::SetPreviewText(LPCTSTR lpszText)
{
	m_strPreview	= lpszText;
}

void CFontDialogX::SetBoldWeight(LONG lWeight)
{
	m_lBoldWeight	= lWeight;
}

void CFontDialogX::SetClearTypeCompatible(BOOL bCompatible)
{
	m_nClearType	= ((bCompatible == TRUE) ? CLEARTYPE_COMPAT_QUALITY : CLEARTYPE_QUALITY);
}

void CFontDialogX::SetDefFont(LPLOGFONT lpLogFont)
{
	memcpy_s(&m_logDefault, sizeof(LOGFONT), lpLogFont, sizeof(LOGFONT));
}

void CFontDialogX::SetDefFont(HFONT hFont)
{
	CFont* pFont	= CFont::FromHandle(hFont);

	pFont->GetLogFont(&m_logDefault);
}

void CFontDialogX::SetSrcFont(LPLOGFONT lpLogFont)
{
	memcpy_s(&m_logFont, sizeof(LOGFONT), lpLogFont, sizeof(LOGFONT));
}

void CFontDialogX::SetSrcFont(HFONT hFont)
{
	CFont*	pFont	= CFont::FromHandle(hFont);

	pFont->GetLogFont(&m_logFont);
}

void CFontDialogX::GetResFont(LPLOGFONT lpLogFont)
{
	memcpy_s(lpLogFont, sizeof(LOGFONT), &m_logFont, sizeof(LOGFONT));
}

HFONT CFontDialogX::GetResFont()
{
	return CreateFontIndirect(&m_logFont);
}

void CFontDialogX::EnableColor(BOOL bEnable)
{
	m_bEnableColor	= bEnable;
}

void CFontDialogX::SetDefColor(COLORREF cr)
{
	m_crDefault	= cr;
}

void CFontDialogX::SetSrcColor(COLORREF cr)
{
	m_ctlColor.SetBackColor(cr);
}

COLORREF CFontDialogX::GetResColor()
{
	return m_ctlColor.GetBackColor();
}

void CFontDialogX::RenderFont()
{
	if (m_PreviewFont.m_hObject)
	{
		m_PreviewFont.DeleteObject();
	}

	if (m_PreviewFont.CreateFontIndirect(&m_logFont))
	{
		CWnd*	pWnd	= GetDlgItem(IDC_FNTD_PREVIEW);
		
		if (pWnd)
		{
			pWnd->SetFont(&m_PreviewFont);
		}
	}

	m_ctlPreview.SetTextColor(m_ctlColor.GetBackColor());
}

void CFontDialogX::SetContents()
{
	int		iIndex;
	int		nPoint;
	CString	strPoint;

	iIndex	= m_ctlFont.FindStringExact(-1, m_logFont.lfFaceName);

	if (iIndex != CB_ERR)
	{
		m_ctlFont.SetCurSel(iIndex);
	}
	else
	{
		m_ctlFont.SetWindowText(m_logFont.lfFaceName);
	}

	nPoint	= HtToPts(m_logFont.lfHeight);

	strPoint.Format(_T("%d"), nPoint);

	iIndex	= m_ctlSize.SelectString(-1, strPoint);

	if (iIndex == CB_ERR)
	{
		m_ctlSize.SetWindowText(strPoint);
	}

	CheckDlgButton(IDC_FNTD_BOLD, ((m_logFont.lfWeight >= m_lBoldWeight) ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(IDC_FNTD_ITALIC, ((m_logFont.lfItalic) ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(IDC_FNTD_UNDERLINE, ((m_logFont.lfUnderline) ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(IDC_FNTD_CLEARTYPE, (((m_logFont.lfQuality == CLEARTYPE_COMPAT_QUALITY) || (m_logFont.lfQuality == CLEARTYPE_QUALITY)) ? BST_CHECKED : BST_UNCHECKED));
	RenderFont();
}

BOOL CFontDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();
	SetHeaderItem(IDC_FNTD_STYLE_H);
	SetHeaderItem(IDC_FNTD_PREVIEW_H);

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_OKAY);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_MENU_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	CDC*	pDC	= GetDC();

	EnumFontFamilies(*pDC, NULL, (FONTENUMPROC)FontDialogEnumProc, (LPARAM)this);
	ReleaseDC(pDC);
	m_ctlSize.AddString(_T("8"));
	m_ctlSize.AddString(_T("9"));
	m_ctlSize.AddString(_T("10"));
	m_ctlSize.AddString(_T("11"));
	m_ctlSize.AddString(_T("12"));
	m_ctlSize.AddString(_T("14"));
	m_ctlSize.AddString(_T("16"));
	m_ctlSize.AddString(_T("20"));
	m_ctlSize.AddString(_T("24"));
	m_ctlSize.AddString(_T("28"));
	m_ctlSize.AddString(_T("36"));
	SetContents();
	m_ctlColorH.EnableWindow(m_bEnableColor);
	m_ctlColor.EnableWindow(m_bEnableColor);
	m_ctlColorH.ShowWindow(m_bEnableColor ? SW_SHOW : SW_HIDE);
	m_ctlColor.ShowWindow(m_bEnableColor ? SW_SHOW : SW_HIDE);
	return TRUE;
}

void CFontDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc, rcDefault;
	SIZE	size;
	int		nGap;

	m_ctlFontH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nGap	= rc.left;

	m_ctlFont.GetWindowRect(&rc);
	ScreenToClient(&rc);

	size.cy	= rc.bottom - rc.top;

	m_ctlDefault.GetWindowRect(&rcDefault);
	ScreenToClient(&rcDefault);

	size.cx	= rcDefault.right - rcDefault.left;

	rcDefault.right		= cx - nGap;
	rcDefault.left		= rcDefault.right - size.cx;
	rcDefault.bottom	= rcDefault.top + size.cy;

	m_ctlDefault.MoveWindow(&rcDefault);

	rc.right	= rcDefault.left - DRA::SCALEX(3);

	m_ctlFont.MoveWindow(&rc);

	m_ctlColor.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.bottom	= rc.top + size.cy;

	m_ctlColor.MoveWindow(&rc);

	m_ctlPreviewH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nGap;

	m_ctlPreviewH.MoveWindow(&rc);

	m_ctlPreview.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nGap;
	rc.bottom	= cy - nGap;

	m_ctlPreview.MoveWindow(&rc);
}

void CFontDialogX::OnPaint()
{
	CPaintDC dc(this);
	DrawHeaderItem(dc, IDC_FNTD_STYLE_H);
	DrawHeaderItem(dc, IDC_FNTD_PREVIEW_H);
	COptionsDialogX::OnPaint();
}

void CFontDialogX::OnBnClickedFontDefault()
{
	memcpy_s(&m_logFont, sizeof(LOGFONT), &m_logDefault, sizeof(LOGFONT));
	m_ctlColor.SetBackColor(m_crDefault);
	SetContents();
	RenderFont();
}

void CFontDialogX::OnBnClickedFontBold()
{
	m_logFont.lfWeight	= (IsDlgButtonChecked(IDC_FNTD_BOLD) ? m_lBoldWeight : FW_NORMAL);

	RenderFont();
}

void CFontDialogX::OnBnClickedFontItalic()
{
	m_logFont.lfItalic	= IsDlgButtonChecked(IDC_FNTD_ITALIC);

	RenderFont();
}

void CFontDialogX::OnBnClickedFontUnderline()
{
	m_logFont.lfUnderline	= IsDlgButtonChecked(IDC_FNTD_UNDERLINE);

	RenderFont();
}

void CFontDialogX::OnBnClickedFontCleartype()
{
	m_logFont.lfQuality	= (IsDlgButtonChecked(IDC_FNTD_CLEARTYPE) ? m_nClearType : DEFAULT_QUALITY);

	RenderFont();
}

void CFontDialogX::OnCbnSelchangeFontFont()
{
	int		iSel;
	CString	strFont;

	iSel	= m_ctlFont.GetCurSel();

	if (iSel != CB_ERR)
	{
		m_ctlFont.GetLBText(iSel, strFont);
		_tcscpy_s(m_logFont.lfFaceName, LF_FACESIZE, strFont);
		RenderFont();
	}
}

void CFontDialogX::OnCbnEditchangeFontFont()
{
	CString	strFont;

	m_ctlFont.GetWindowText(strFont);
	_tcscpy_s(m_logFont.lfFaceName, LF_FACESIZE, strFont);
	RenderFont();
}

void CFontDialogX::OnCbnSelchangeFontSize()
{
	int		iSel;
	int		nPoint;
	CString	strSize;

	iSel	= m_ctlSize.GetCurSel();

	if (iSel != CB_ERR)
	{
		m_ctlSize.GetLBText(iSel, strSize);

		nPoint				= _ttoi(strSize);
		m_logFont.lfHeight	= PtsToHt(nPoint);

		RenderFont();
	}
}

void CFontDialogX::OnCbnEditchangeFontSize()
{
	CString	strSize;
	int		nPoint;

	m_ctlSize.GetWindowText(strSize);

	nPoint	= _ttoi(strSize);

	if (nPoint > 0)
	{
		m_logFont.lfHeight	= PtsToHt(nPoint);

		RenderFont();
	}
}

LRESULT CFontDialogX::OnSelEndOKColor(WPARAM wParam, LPARAM lParam)
{
	RenderFont();
	return 0;
}
