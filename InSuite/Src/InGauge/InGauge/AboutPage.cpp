// AboutPage.cpp

#include "stdafx.h"
#include "InGauge.h"
#include "AboutPage.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CAboutPage, CPropertyPageX)

CAboutPage::CAboutPage() : CPropertyPageX(CAboutPage::IDD)
{
}

CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutPage, CPropertyPageX)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CAboutPage::OnPaint()
{
	CPaintDC	dc(this);
	HICON		hIcon	= (HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
	RECT		rc;
	int			nOldBkMode;
	COLORREF	crOld;
	CString		strItem;
	HFONT		hFont;
	HFONT		hOldFont;
	LOGFONT		lfFont;

	GetClientRect(&rc);
	ScreenToClient(&rc);

	// Set Background Mode
	nOldBkMode	= dc.SetBkMode(TRANSPARENT);

	// Draw Icon
	dc.DrawIcon(GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON) - DRA::SCALEX(4), DRA::SCALEY(4), hIcon);

	// Set Default Font
	GetDefaultFont(&lfFont);

	// Draw Application Name
	lfFont.lfWeight	= FW_BOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(dc.GetSafeHdc(), hFont);
	crOld			= dc.SetTextColor(RGB(0xCF, 0x32, 0x08));
	rc.left			= DRA::SCALEX(8);
	rc.top			= DRA::SCALEY(4);
	rc.right		= rc.left + DRA::SCALEX(1);
	rc.bottom		= rc.top + DRA::SCALEY(1);

	strItem.LoadString(IDS_AB_APPLICATION);
	dc.DrawText(strItem, &rc, DT_CALCRECT | DT_SINGLELINE);
	dc.DrawText(strItem, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor(crOld);
	SelectObject(dc.GetSafeHdc(), hOldFont);
	DeleteObject(hFont);

	// Draw Version Number
	lfFont.lfWeight	= FW_NORMAL;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(dc.GetSafeHdc(), hFont);
	crOld			= dc.SetTextColor(RGB(0x90, 0x90, 0x90));
	rc.left			= DRA::SCALEX(8);
	rc.top			= rc.bottom + DRA::SCALEY(2);
	rc.right		= rc.left + DRA::SCALEX(1);
	rc.bottom		= rc.top + DRA::SCALEY(1);

	strItem.LoadString(IDS_AB_VERSION);
	dc.DrawText(strItem, &rc, DT_CALCRECT | DT_SINGLELINE);
	dc.DrawText(strItem, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor(crOld);
	SelectObject(dc.GetSafeHdc(), hOldFont);
	DeleteObject(hFont);

	// Draw Copyright
	lfFont.lfWeight	= FW_NORMAL;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(dc.GetSafeHdc(), hFont);
	crOld			= dc.SetTextColor(RGB(0x90, 0x90, 0x90));
	rc.left			= DRA::SCALEX(8);
	rc.top			= rc.bottom + DRA::SCALEY(6);
	rc.right		= rc.left + DRA::SCALEX(1);
	rc.bottom		= rc.top + DRA::SCALEY(1);

	strItem.LoadString(IDS_AB_COPYRIGHT0);
	dc.DrawText(strItem, &rc, DT_CALCRECT | DT_SINGLELINE);
	dc.DrawText(strItem, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	rc.left			= DRA::SCALEX(8);
	rc.top			= rc.bottom;
	rc.right		= rc.left + DRA::SCALEX(1);
	rc.bottom		= rc.top + DRA::SCALEY(1);

	strItem.LoadString(IDS_AB_COPYRIGHT1);
	dc.DrawText(strItem, &rc, DT_CALCRECT | DT_SINGLELINE);
	dc.DrawText(strItem, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor(crOld);
	SelectObject(dc.GetSafeHdc(), hOldFont);
	DeleteObject(hFont);

	// Draw Credits
	lfFont.lfWeight	= FW_SEMIBOLD;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(dc.GetSafeHdc(), hFont);
	crOld			= dc.SetTextColor(RGB(0x35, 0x53, 0x86));
	rc.left			= DRA::SCALEX(8);
	rc.top			= rc.bottom + DRA::SCALEY(6);
	rc.right		= rc.left + DRA::SCALEX(1);
	rc.bottom		= rc.top + DRA::SCALEY(1);

	strItem.LoadString(IDS_AB_CREATED_BY);
	dc.DrawText(strItem, &rc, DT_CALCRECT | DT_SINGLELINE);
	dc.DrawText(strItem, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor(crOld);
	SelectObject(dc.GetSafeHdc(), hOldFont);
	DeleteObject(hFont);

	lfFont.lfWeight	= FW_NORMAL;
	hFont			= CreateFontIndirect(&lfFont);
	hOldFont		= (HFONT)SelectObject(dc.GetSafeHdc(), hFont);
	crOld			= dc.SetTextColor(dc.GetTextColor());
	rc.left			= DRA::SCALEX(8);
	rc.top			= rc.bottom + DRA::SCALEY(2);
	rc.right		= GetSystemMetrics(SM_CXSCREEN) - DRA::SCALEX(8);
	rc.bottom		= rc.top + DRA::SCALEY(1);

	strItem.LoadString(IDS_AB_CREDITS0);
	dc.DrawText(strItem, &rc, DT_CALCRECT | DT_SINGLELINE);
	dc.DrawText(strItem, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	rc.left			= DRA::SCALEX(8);
	rc.top			= rc.bottom;
	rc.right		= GetSystemMetrics(SM_CXSCREEN) - DRA::SCALEX(8);
	rc.bottom		= rc.top + DRA::SCALEY(1);

	strItem.LoadString(IDS_AB_CREDITS1);
	dc.DrawText(strItem, &rc, DT_CALCRECT | DT_SINGLELINE);
	dc.DrawText(strItem, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	dc.SetTextColor(crOld);
	SelectObject(dc.GetSafeHdc(), hOldFont);
	DeleteObject(hFont);

	// Restore Background Mode
	dc.SetBkMode(nOldBkMode);

	CPropertyPageX::OnPaint();
}
