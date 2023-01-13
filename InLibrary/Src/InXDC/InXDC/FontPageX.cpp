// FontPageX.cpp

#include "stdafx.h"
#include "FontPageX.h"
#include "resourceppc.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CFontPageX, CPropertyPageX)

CFontPageX::CFontPageX() : CPropertyPageX(IDD_FONT_PAGE)
{
	CString	strText;
	LOGFONT	lfFont;

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

CFontPageX::~CFontPageX()
{
	if (m_PreviewFont.m_hObject)
	{
		m_PreviewFont.DeleteObject();
	}
}

void CFontPageX::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FNTP_FONT, m_ctlFont);
	DDX_Control(pDX, IDC_FNTP_DEFAULT, m_ctlDefault);
	DDX_Control(pDX, IDC_FNTP_SIZE, m_ctlSize);
	DDX_Control(pDX, IDC_FNTP_COLOR_H, m_ctlColorH);
	DDX_Control(pDX, IDC_FNTP_COLOR, m_ctlColor);
	DDX_Control(pDX, IDC_FNTP_PREVIEW_H, m_ctlPreviewH);
	DDX_Control(pDX, IDC_FNTP_PREVIEW, m_ctlPreview);
	DDX_Text(pDX, IDC_FNTP_PREVIEW, m_strPreview);
}

BEGIN_MESSAGE_MAP(CFontPageX, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_FNTP_DEFAULT, &CFontPageX::OnBnClickedFontDefault)
	ON_BN_CLICKED(IDC_FNTP_BOLD, &CFontPageX::OnBnClickedFontBold)
	ON_BN_CLICKED(IDC_FNTP_ITALIC, &CFontPageX::OnBnClickedFontItalic)
	ON_BN_CLICKED(IDC_FNTP_UNDERLINE, &CFontPageX::OnBnClickedFontUnderline)
	ON_BN_CLICKED(IDC_FNTP_CLEARTYPE, &CFontPageX::OnBnClickedFontCleartype)
	ON_CBN_SELCHANGE(IDC_FNTP_FONT, &CFontPageX::OnCbnSelchangeFontFont)
	ON_CBN_EDITCHANGE(IDC_FNTP_FONT, &CFontPageX::OnCbnEditchangeFontFont)
	ON_CBN_SELCHANGE(IDC_FNTP_SIZE, &CFontPageX::OnCbnSelchangeFontSize)
	ON_CBN_EDITCHANGE(IDC_FNTP_SIZE, &CFontPageX::OnCbnEditchangeFontSize)
	ON_MESSAGE(CPN_SELENDOK, &CFontPageX::OnSelEndOKColor)
END_MESSAGE_MAP()

int CALLBACK FontPageEnumProc(ENUMLOGFONT* lpelf, TEXTMETRIC* lpntm, int FontType, LPARAM lParam)
{
	CFontPageX*	pPage	= (CFontPageX*)lParam;

	pPage->m_ctlFont.AddString(lpelf->elfLogFont.lfFaceName);
	return 1;
}

void CFontPageX::SetPreviewText(LPCTSTR lpszText)
{
	m_strPreview	= lpszText;
}

void CFontPageX::SetBoldWeight(LONG lWeight)
{
	m_lBoldWeight	= lWeight;
}

void CFontPageX::SetClearTypeCompatible(BOOL bCompatible)
{
	m_nClearType	= ((bCompatible == TRUE) ? CLEARTYPE_COMPAT_QUALITY : CLEARTYPE_QUALITY);
}

void CFontPageX::SetDefFont(LPLOGFONT lpLogFont)
{
	memcpy_s(&m_logDefault, sizeof(LOGFONT), lpLogFont, sizeof(LOGFONT));
}

void CFontPageX::SetDefFont(HFONT hFont)
{
	CFont*	pFont	= CFont::FromHandle(hFont);

	pFont->GetLogFont(&m_logDefault);
}

void CFontPageX::SetSrcFont(LPLOGFONT lpLogFont)
{
	memcpy_s(&m_logFont, sizeof(LOGFONT), lpLogFont, sizeof(LOGFONT));
}

void CFontPageX::SetSrcFont(HFONT hFont)
{
	CFont*	pFont	= CFont::FromHandle(hFont);

	pFont->GetLogFont(&m_logFont);
}

void CFontPageX::GetResFont(LPLOGFONT lpLogFont)
{
	memcpy_s(lpLogFont, sizeof(LOGFONT), &m_logFont, sizeof(LOGFONT));
}

HFONT CFontPageX::GetResFont()
{
	return CreateFontIndirect(&m_logFont);
}

void CFontPageX::EnableColor(BOOL bEnable)
{
	m_bEnableColor	= bEnable;
}

void CFontPageX::SetDefColor(COLORREF cr)
{
	m_crDefault	= cr;
}

void CFontPageX::SetSrcColor(COLORREF cr)
{
	m_ctlColor.SetBackColor(cr);
}

COLORREF CFontPageX::GetResColor()
{
	return m_ctlColor.GetBackColor();
}

void CFontPageX::RenderFont()
{
	if (m_PreviewFont.m_hObject)
	{
		m_PreviewFont.DeleteObject();
	}

	if (m_PreviewFont.CreateFontIndirect(&m_logFont))
	{
		CWnd*	pWnd	= GetDlgItem(IDC_FNTP_PREVIEW);

		if (pWnd)
		{
			pWnd->SetFont(&m_PreviewFont);
		}
	}

	m_ctlPreview.SetTextColor(m_ctlColor.GetBackColor());
}

void CFontPageX::SetContents()
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

	CheckDlgButton(IDC_FNTP_BOLD, ((m_logFont.lfWeight >= m_lBoldWeight) ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(IDC_FNTP_ITALIC, ((m_logFont.lfItalic) ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(IDC_FNTP_UNDERLINE, ((m_logFont.lfUnderline) ? BST_CHECKED : BST_UNCHECKED));
	CheckDlgButton(IDC_FNTP_CLEARTYPE, (((m_logFont.lfQuality == CLEARTYPE_COMPAT_QUALITY) || (m_logFont.lfQuality == CLEARTYPE_QUALITY)) ? BST_CHECKED : BST_UNCHECKED));
	RenderFont();
}

BOOL CFontPageX::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();

	SetHeaderItem(IDC_FNTP_STYLE_H);
	SetHeaderItem(IDC_FNTP_PREVIEW_H);

	CDC*	pDC	= GetDC();

	EnumFontFamilies(*pDC, NULL, (FONTENUMPROC)FontPageEnumProc, (LPARAM)this);
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

void CFontPageX::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

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

void CFontPageX::OnPaint()
{
	CPaintDC dc(this);
	DrawHeaderItem(dc, IDC_FNTP_STYLE_H);
	DrawHeaderItem(dc, IDC_FNTP_PREVIEW_H);
	CPropertyPageX::OnPaint();
}

void CFontPageX::OnBnClickedFontDefault()
{
	memcpy_s(&m_logFont, sizeof(LOGFONT), &m_logDefault, sizeof(LOGFONT));
	m_ctlColor.SetBackColor(m_crDefault);
	SetContents();
	RenderFont();
}

void CFontPageX::OnBnClickedFontBold()
{
	m_logFont.lfWeight	= (IsDlgButtonChecked(IDC_FNTP_BOLD) ? m_lBoldWeight : FW_NORMAL);

	RenderFont();
}

void CFontPageX::OnBnClickedFontItalic()
{
	m_logFont.lfItalic	= IsDlgButtonChecked(IDC_FNTP_ITALIC);

	RenderFont();
}

void CFontPageX::OnBnClickedFontUnderline()
{
	m_logFont.lfUnderline	= IsDlgButtonChecked(IDC_FNTP_UNDERLINE);

	RenderFont();
}

void CFontPageX::OnBnClickedFontCleartype()
{
	m_logFont.lfQuality	= (IsDlgButtonChecked(IDC_FNTP_CLEARTYPE) ? m_nClearType : DEFAULT_QUALITY);

	RenderFont();
}

void CFontPageX::OnCbnSelchangeFontFont()
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

void CFontPageX::OnCbnEditchangeFontFont()
{
	CString	strFont;

	m_ctlFont.GetWindowText(strFont);
	_tcscpy_s(m_logFont.lfFaceName, LF_FACESIZE, strFont);
	RenderFont();
}

void CFontPageX::OnCbnSelchangeFontSize()
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

void CFontPageX::OnCbnEditchangeFontSize()
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

LRESULT CFontPageX::OnSelEndOKColor(WPARAM wParam, LPARAM lParam)
{
	RenderFont();
	return 0;
}
