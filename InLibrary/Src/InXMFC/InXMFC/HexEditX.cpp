// HexEditX.cpp

#include "stdafx.h"
#include "HexEditX.h"

IMPLEMENT_DYNAMIC(CHexEditX, CEdit)

TCHAR	tHexTable[0x0010]	=
{
	_T('0'), _T('1'), _T('2'), _T('3'), _T('4'), _T('5'), _T('6'), _T('7'),
	_T('8'), _T('9'), _T('A'), _T('B'), _T('C'), _T('D'), _T('E'), _T('F')
};

#define TOHEX(h, s)	{ *s++ = tHexTable[h >> 4]; *s++ = tHexTable[h & 0x0F]; }
#define PAGE_SIZE	(m_nLinesPerPage * m_nBytesPerRow)
#define UNSELECTED	0xFFFFFFFF
#define HEX_ROW		3

CHexEditX::CHexEditX()
{
	InitializeX();
}

CHexEditX::~CHexEditX()
{
	CleanUpX();
}

BEGIN_MESSAGE_MAP(CHexEditX, CEdit)
	ON_COMMAND(ID_EDIT_CUT, &CHexEditX::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CHexEditX::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CHexEditX::OnEditPaste)
	ON_COMMAND(ID_EDIT_CLEAR, &CHexEditX::OnEditClear)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CHexEditX::OnEditSelectAll)
	ON_WM_GETDLGCODE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

BOOL CHexEditX::SetData(LPBYTE lpData, int& cbData)
{
	CleanUpX();
	InitializeX();

	m_lpData	= (LPBYTE)malloc(cbData + 1);

	if (m_lpData != NULL)
	{
		memcpy_s(m_lpData, cbData + 1, lpData, cbData);

		m_lpData[cbData++]	= 0x00;
		m_bIncompleted		= TRUE;

		SetSelection(-1, -1);

		m_nLength			= cbData;
		m_nCurrentAddress	= 0;
		m_ptEdit.x			= 0;
		m_ptEdit.y			= 0;
		m_nCurrentMode		= EDIT_HIGH;
		m_nTopIndex			= 0;
		m_bUpdate			= TRUE;

		return TRUE;
	}

	return FALSE;
}

BOOL CHexEditX::GetData(LPBYTE lpData, int& cbData)
{
	int	nLength	= m_nLength + (m_bIncompleted ? -1 : 0);

	if (cbData < nLength)
	{
		cbData	= nLength;

		return FALSE;
	}

	memcpy_s(lpData, cbData, m_lpData, nLength);

	cbData	= nLength;

	return TRUE;
}

void CHexEditX::SetSelection(int nBegin, int nEnd)
{
	m_nSelectionBegin	= nBegin;
	m_nSelectionEnd		= nEnd;

	if (IsWindow(m_hWnd))
	{
		DestroyCaret();
		Invalidate();
		UpdateWindow();

		if ((m_ptEdit.x == 0) && (m_bShowAddress == TRUE))
		{
			CreateAddressCaret();
		}
		else
		{
			CreateEditCaret();
		}

		SetCaretPos(m_ptEdit);
		ShowCaret();
	}
}

CSize CHexEditX::GetSelection()
{
	return CSize(m_nSelectionBegin, m_nSelectionEnd);
}

void CHexEditX::SetFontFace(LPCTSTR szFontFace)
{
	m_strFontFace	= szFontFace;
}

void CHexEditX::SetBytesPerRow(int nBytes)
{
	m_nBytesPerRow	= nBytes;
	m_bUpdate		= TRUE;
}

void CHexEditX::ShowAddress(BOOL bShow)
{
	m_bShowAddress	= bShow;
	m_bUpdate		= TRUE;
}

void CHexEditX::ShowHexData(BOOL bShow)
{
	m_bShowHex	= bShow;
	m_bUpdate	= TRUE;
}

void CHexEditX::ShowASCIIData(BOOL bShow)
{
	m_bShowASCII	= bShow;
	m_bUpdate		= TRUE;
}

void CHexEditX::UseFarAddress(BOOL bUse)
{
	m_bFarAddress	= bUse;
	m_bUpdate		= TRUE;
}

void CHexEditX::InitializeX()
{
	m_lpData				= &m_cInitBuffer;		// Included incompleted last one byte
	m_nLength				= 0x0001;
	m_lpData[m_nLength - 1]	= 0x00;					// Incompleted last one byte
	m_bIncompleted			= TRUE;
	m_nCurrentAddress		= 0;
	m_nCurrentMode			= EDIT_NONE;
	m_nSelectionBegin		= UNSELECTED;
	m_nSelectionEnd			= UNSELECTED;
	m_nTopIndex				= 0;
	m_nBytesPerRow			= 4;
	m_nLinesPerPage			= 1;
	m_bShowAddress			= TRUE;
	m_bShowHex				= TRUE;
	m_bShowASCII			= TRUE;
	m_bFarAddress			= FALSE;
	m_nOffsetAddress		= 0;
	m_nOffsetHex			= 0;
	m_nOffsetASCII			= 0;
	m_bUpdate				= TRUE;
	m_ptEdit.x				= 0;
	m_ptEdit.y				= 0;
	m_strFontFace			= _T("Courier New");
}

void CHexEditX::CleanUpX()
{
	if (m_lpData != NULL)
	{
		delete [] m_lpData;

		m_lpData	= NULL;
	}
}

inline BOOL CHexEditX::IsSelected()
{
	return (BOOL)(m_nSelectionBegin != UNSELECTED);
}

CPoint CHexEditX::CalcPos(int x, int y)
{
	int	nBound;

	y	/= m_nLineHeight;
	x	/= m_nNullWidth;

	if ((y < 0) || (y >= m_nLinesPerPage))
	{
		return CPoint(-1, -1);
	}

	if ((y * m_nBytesPerRow) > m_nLength)
	{
		return CPoint(-1, -1);
	}

	if ((m_bShowAddress == TRUE) && (x < (m_bFarAddress ? 8 : 4)))
	{
		m_nCurrentAddress	= m_nTopIndex + (m_nBytesPerRow * y);
		m_nCurrentMode		= EDIT_NONE;

		return CPoint(0, y);
	}

	nBound	= ((m_nOffsetHex / m_nNullWidth) + (m_nBytesPerRow * HEX_ROW));

	if ((m_bShowHex == TRUE) && (x < nBound))
	{
		if ((x % HEX_ROW) == (m_bFarAddress ? 0x02 : 0x01))
		{
			x--;
		}

		m_nCurrentAddress	= m_nTopIndex + (m_nBytesPerRow * y) + (x - (m_nOffsetHex  / m_nNullWidth)) / HEX_ROW;
		m_nCurrentMode		= (((x % HEX_ROW) & 0x01) ? EDIT_LOW : EDIT_HIGH);

		return CPoint(x, y);
	}

	nBound	= ((m_nOffsetASCII / m_nNullWidth) + m_nBytesPerRow);

	if ((m_bShowASCII == TRUE) && (x <= nBound))
	{
		m_nCurrentAddress	= m_nTopIndex + (m_nBytesPerRow * y) + (x - (m_nOffsetASCII / m_nNullWidth));
		m_nCurrentMode		= EDIT_ASCII;

		return CPoint(x, y);
	}

	return CPoint(-1,-1);
}

void CHexEditX::CreateAddressCaret()
{
	DestroyCaret();
	CreateSolidCaret(m_nNullWidth * (m_bFarAddress ? 8 : 4), m_nLineHeight);
}

void CHexEditX::CreateEditCaret()
{
	DestroyCaret();
	CreateSolidCaret(m_nNullWidth, m_nLineHeight);
}

void CHexEditX::UpdateScrollbar()
{
	SCROLLINFO	si;

	si.cbSize	= sizeof(SCROLLINFO);
	si.fMask	= SIF_ALL;
	si.nMin		= 0;
	si.nMax		= ((m_nLength > PAGE_SIZE) ? (m_nLength - 1): 0);
	si.nPage	= PAGE_SIZE;
	si.nPos		= m_nTopIndex;

	SetScrollInfo(SB_VERT, &si, TRUE);
}

void CHexEditX::Move(int x, int y)
{
	switch (m_nCurrentMode)
	{
	case EDIT_NONE:
		return;
	case EDIT_HIGH:
		{
			if ((x == 1) && (y == 0) && (m_nCurrentAddress == (m_nLength - 1)) && (m_bIncompleted == TRUE))
			{
				break;
			}

			if (x != 0)
			{
				m_nCurrentMode	= EDIT_LOW;
			}

			if (x == -1)
			{
				m_nCurrentAddress--;
			}

			m_nCurrentAddress	+= y * m_nBytesPerRow;
		}
		break;
	case EDIT_LOW:
		{
			if ((x != 0) || (y != 0))
			{
				m_nCurrentMode	= EDIT_HIGH;
			}

			if (x == 1)
			{
				m_nCurrentAddress++;
			}

			m_nCurrentAddress	+= y * m_nBytesPerRow;
		}
		break;
	case EDIT_ASCII:
		{
			m_nCurrentAddress	+= x;
			m_nCurrentAddress	+= y * m_nBytesPerRow;
		}
		break;
	}

	if (m_nCurrentAddress < 0)
	{
		m_nCurrentAddress	= 0;
	}

	if (m_nCurrentAddress >= m_nLength)
	{
		m_nCurrentAddress	= m_nLength - 1;
	}

	if (m_nCurrentAddress < m_nTopIndex)
	{
		OnVScroll(SB_LINEUP, 0, NULL);
	}

	if (m_nCurrentAddress >= m_nTopIndex + PAGE_SIZE)
	{
		OnVScroll(SB_LINEDOWN, 0, NULL);
	}

	RepositionCaret(m_nCurrentAddress);
}

void CHexEditX::RepositionCaret(int nAddress)
{
	int	x	= ((nAddress - m_nTopIndex) % m_nBytesPerRow);
	int	y	= ((nAddress - m_nTopIndex) / m_nBytesPerRow);

	switch (m_nCurrentMode)
	{
	case EDIT_NONE:
		{
			CreateAddressCaret();

			x	= 0;
		}
		break;
	case EDIT_HIGH:
		{
			CreateEditCaret();

			x	*= m_nNullWidth * HEX_ROW;
			x	+= m_nOffsetHex;
		}
		break;
	case EDIT_LOW:
		{
			CreateEditCaret();

			x	*= m_nNullWidth * HEX_ROW;
			x	+= m_nNullWidth;
			x	+= m_nOffsetHex;
		}
		break;
	case EDIT_ASCII:
		{
			CreateEditCaret();

			x	*= m_nNullWidth;
			x	+= m_nOffsetASCII;
		}
		break;
	}

	m_ptEdit.x	= x;
	m_ptEdit.y	= y * m_nLineHeight;

	CRect	rc;

	GetClientRect(&rc);

	if (rc.PtInRect(m_ptEdit))
	{
		SetCaretPos(m_ptEdit);
		ShowCaret();
	}
}

void CHexEditX::NormalizeSelection()
{
	if (m_nSelectionBegin > m_nSelectionEnd)
	{
		m_nSelectionBegin	^= m_nSelectionEnd	^= m_nSelectionBegin	^= m_nSelectionEnd;
	}
}

void CHexEditX::SelDelete(int nStart, int nEnd)
{
	LPBYTE	p	= (LPBYTE)malloc(m_nLength - (nEnd - nStart) + 1);

	if (p != NULL)
	{
		memcpy_s(p, m_nLength - (nEnd - nStart) + 1, m_lpData, nStart);

		if (nStart < (m_nLength - (nEnd - nStart)))
		{
			memcpy_s(p + nStart, m_nLength - nEnd + 1, m_lpData + nEnd, (m_nLength - nEnd));
		}

		free(m_lpData);

		m_lpData	= p;

		SetSelection(-1, -1);

		m_nLength	-= (nEnd - nStart);

		if (m_nCurrentAddress > m_nLength)
		{
			m_nCurrentAddress	= m_nLength;

			RepositionCaret(m_nCurrentAddress);
		}

		m_bUpdate	= TRUE;
	}
}

void CHexEditX::SelInsert(int nStart, int nLength)
{
	LPBYTE	p	= (LPBYTE)calloc(m_nLength + nLength, 1);

	if (p != NULL)
	{
		memcpy_s(p, m_nLength + nLength, m_lpData, nStart);
		memcpy_s(p + nStart + nLength, m_nLength - nStart, m_lpData + nStart, (m_nLength - nStart));
		free(m_lpData);

		m_lpData	= p;

		SetSelection(-1, -1);

		m_nLength	= m_nLength + nLength;
		m_bUpdate	= TRUE;

		RepositionCaret(m_nCurrentAddress);
	}
}

BOOL CHexEditX::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	dwStyle	|= WS_VSCROLL;
	dwStyle	&= ~WS_HSCROLL;

	return (CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext));
}

BOOL CHexEditX::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style	|= WS_VSCROLL;
	cs.style	&= ~WS_HSCROLL;

	return CEdit::PreCreateWindow(cs);
}

void CHexEditX::OnEditCut()
{
	OnEditCopy();
	SelDelete(m_nSelectionBegin, m_nSelectionEnd);
	Invalidate();
	UpdateWindow();
}

void CHexEditX::OnEditCopy()
{
	if (!OpenClipboard())
	{
		return;
	}

	EmptyClipboard();

	if (m_nCurrentMode != EDIT_ASCII)
	{
		int		dwLen	= (m_nSelectionEnd - m_nSelectionBegin);
		HGLOBAL	hMemB	= ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, dwLen);
		HGLOBAL	hMemA	= ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, sizeof(TCHAR) * (dwLen * HEX_ROW + 1));
		LPBYTE	lpB;
		TCHAR*	lpA;

		if ((!hMemA) || (!hMemB))
		{
			return;
		}

		lpB	= (LPBYTE)GlobalLock(hMemB);

		memcpy_s(lpB, dwLen, m_lpData + m_nSelectionBegin, dwLen);
		GlobalUnlock(hMemB);

		lpA	= (TCHAR*)GlobalLock(hMemA);

		for (int i = 0; i < dwLen; )
		{
			TOHEX(m_lpData[m_nSelectionBegin + i], lpA);

			*lpA++	= _T(' ');
			i++;
		}

		GlobalUnlock(hMemA);
		SetClipboardData((CLIPFORMAT)RegisterClipboardFormat(_T("BinaryData")), hMemB);
		SetClipboardData(CF_UNICODETEXT, hMemA);
	}
	else
	{
		int		dwLen	= (m_nSelectionEnd - m_nSelectionBegin);
		HGLOBAL	hMemB	= ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, dwLen);
		HGLOBAL	hMemA	= ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT, sizeof(TCHAR) * (dwLen + 1));
		LPBYTE	lpB;
		TCHAR*	lpA;

		if ((!hMemA) || (!hMemB))
		{
			return;
		}

		lpB	= (LPBYTE)GlobalLock(hMemB);

		memcpy_s(lpB, dwLen, m_lpData + m_nSelectionBegin, dwLen);
		GlobalUnlock(hMemB);

		lpA	= (TCHAR*)GlobalLock(hMemA);

		for (int i = 0; i < dwLen; lpA++, i++)
		{
			if (iswprint(m_lpData[m_nSelectionBegin + i]))
			{
				*lpA	= (TCHAR)(m_lpData[m_nSelectionBegin + i] & 0xFF);
			}
			else
			{
				*lpA	= _T('.');
			}
		}

		GlobalUnlock(hMemA);
		SetClipboardData((CLIPFORMAT)RegisterClipboardFormat(_T("BinaryData")), hMemB);
		SetClipboardData(CF_UNICODETEXT, hMemA);
	}

	CloseClipboard();
}

void CHexEditX::OnEditPaste()
{
	if (!OpenClipboard())
	{
		return;
	}

	HGLOBAL	hMem	= NULL;

	if (IsClipboardFormatAvailable((CLIPFORMAT)RegisterClipboardFormat(_T("BinaryData"))))
	{
		hMem	= GetClipboardData((CLIPFORMAT)RegisterClipboardFormat(_T("BinaryData")));
	}
	else if (IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		hMem	= GetClipboardData(CF_UNICODETEXT);
	}

	if (hMem)
	{
		LPBYTE	p		= (LPBYTE)GlobalLock(hMem);
		DWORD	dwLen	= GlobalSize(hMem);
		int		nBefore	= m_nCurrentAddress;
		int		nInsert;

		NormalizeSelection();

		if (m_nSelectionBegin == UNSELECTED)
		{
			if (m_nCurrentMode == EDIT_LOW)
			{
				m_nCurrentAddress++;
			}

			nInsert	= m_nCurrentAddress;

			SelInsert(m_nCurrentAddress, dwLen);
		}
		else
		{
			nInsert	= m_nSelectionBegin;

			SelDelete(m_nSelectionBegin, m_nSelectionEnd);
			SelInsert(nInsert, dwLen);
			SetSel(-1, -1);
		}

		memcpy_s(m_lpData + nInsert, dwLen, p, dwLen);

		m_nCurrentAddress	= nBefore;

		Invalidate();
		UpdateWindow();
		GlobalUnlock(hMem);
	}

	CloseClipboard();
}

void CHexEditX::OnEditClear()
{
	m_nCurrentAddress	= m_nSelectionBegin;

	SelDelete(m_nSelectionBegin, m_nSelectionEnd);
	RepositionCaret(m_nCurrentAddress);
	Invalidate();
	UpdateWindow();
}

void CHexEditX::OnEditSelectAll()
{
	m_nSelectionBegin	= 0;
	m_nSelectionEnd		= m_nLength;

	DestroyCaret();
	Invalidate();
	UpdateWindow();
}

UINT CHexEditX::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

void CHexEditX::OnPaint()
{
	CPaintDC	dc(this);
	CRect		rc;
	CRect		rcText;
	CDC			memDC;
	CBitmap		Bitmap;
	CBrush		Brush;
	CFont		Font;
	CFont*		oldFont;
	int			nHeight	= 0;
	int			x;
	int			y;
	TCHAR		buf[1024];

	GetClientRect(rc);
	memDC.CreateCompatibleDC(&dc);
	Bitmap.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	memDC.SelectObject(&Bitmap);
	Brush.CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	memDC.FillRect(rc, &Brush);
	Brush.DeleteObject();
	Font.CreatePointFont(120, m_strFontFace, &memDC);

	oldFont	= memDC.SelectObject(&Font);

	ASSERT(m_nCurrentAddress >= 0);
	ASSERT(m_nTopIndex >= 0);

	if (m_lpData)
	{
		if (m_bUpdate)
		{
			::GetCharWidth32(memDC.GetSafeHdc(), _T('0'), _T('0'), &m_nNullWidth);

			CSize	size		= memDC.GetTextExtent(_T("0"), 1);

			m_nLineHeight		= size.cy;
			m_nOffsetAddress	= 0;
			m_nOffsetHex		= (m_bShowAddress ? m_nNullWidth * ((m_bFarAddress ? 8 : 4) + 1) : 0);
			m_nOffsetASCII		= (m_bShowAddress ? m_nNullWidth * ((m_bFarAddress ? 8 : 4) + 1) : 0) + (m_bShowHex	? ((m_nBytesPerRow * HEX_ROW + 1) * m_nNullWidth) : 0);
			m_nLinesPerPage		= rc.Height() / m_nLineHeight;
			m_bUpdate			= FALSE;

			UpdateScrollbar();
		}

		nHeight	= (rc.Height() / m_nLineHeight);
		nHeight	*= m_nLineHeight;

		if (m_bShowAddress)
		{
			TCHAR	fmt[6]	= { _T('%'), _T('0'), _T('8'), _T('l'), _T('X'), NULL };
			int		nWidth	= (m_bFarAddress ? 8 : 4);

			rcText				= rc;
			fmt[2]				= (m_bFarAddress ? _T('8') : _T('4'));
			y					= 0;
			rcText.TopLeft().x	= m_nOffsetAddress;

			memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			memDC.SetBkColor(GetSysColor(COLOR_WINDOW));

			for (int i = m_nTopIndex; (i < m_nLength) && (rcText.TopLeft().y < nHeight); i += m_nBytesPerRow)
			{
				wsprintf(buf, fmt, i);
				memDC.DrawText(buf, nWidth, rcText, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

				rcText.TopLeft().y	+= m_nLineHeight;
			}
		}

		if (m_bShowHex)
		{
			x					= m_nOffsetHex;
			y					= 0;
			rcText				= rc;
			rcText.TopLeft().x	= m_nOffsetHex;

			if ((m_nSelectionBegin != UNSELECTED) && ((m_nCurrentMode == EDIT_HIGH) || (m_nCurrentMode == EDIT_LOW)))
			{
				int	i;
				int	n				= 0;
				int	nSelectBegin	= m_nSelectionBegin;
				int	nSelectEnd		= m_nSelectionEnd;

				if (nSelectBegin > nSelectEnd)
				{
					nSelectBegin ^= nSelectEnd ^= nSelectBegin ^= nSelectEnd;
				}

				memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				memDC.SetBkColor(GetSysColor(COLOR_WINDOW));

				for (i = m_nTopIndex; (i < nSelectBegin) && (y < nHeight); i++)
				{
					TCHAR*	p	= &buf[0];

					if ((i == (m_nLength - 1)) && (m_bIncompleted == TRUE))
					{
						*p++	= _T(' ');
						*p++	= _T(' ');
					}
					else
					{
						TOHEX(m_lpData[i], p);
					}

					*p++	= _T(' ');

					memDC.ExtTextOut(x, y, ETO_OPAQUE, NULL, buf, HEX_ROW, NULL);

					x	+= m_nNullWidth * HEX_ROW;
					n++;

					if (n == m_nBytesPerRow)
					{
						n	= 0;
						x	= m_nOffsetHex;
						y	+= m_nLineHeight;
					}
				}

				memDC.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				memDC.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));

				for( ; (i < nSelectEnd) && (i < m_nLength) && (y < nHeight); i++)
				{
					TCHAR*	p	= &buf[0];

					if ((i == (m_nLength - 1)) && (m_bIncompleted == TRUE))
					{
						*p++	= _T(' ');
						*p++	= _T(' ');
					}
					else
					{
						TOHEX(m_lpData[i], p);
					}

					*p++	= _T(' ');

					memDC.ExtTextOut(x, y, ETO_OPAQUE, NULL, buf, HEX_ROW, NULL);

					x	+= m_nNullWidth * HEX_ROW;
					n++;

					if (n == m_nBytesPerRow)
					{
						n	= 0;
						x	= m_nOffsetHex;
						y	+= m_nLineHeight;
					}
				}

				memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				memDC.SetBkColor(GetSysColor(COLOR_WINDOW));

				for( ; (i < m_nLength) && (y < nHeight); i++)
				{
					TCHAR*	p	= &buf[0];

					if ((i == (m_nLength - 1)) && (m_bIncompleted == TRUE))
					{
						*p++	= _T(' ');
						*p++	= _T(' ');
					}
					else
					{
						TOHEX(m_lpData[i], p);
					}

					*p++	= _T(' ');

					memDC.ExtTextOut(x, y, ETO_OPAQUE, NULL, buf, HEX_ROW, NULL);

					x	+= m_nNullWidth * HEX_ROW;
					n++;

					if (n == m_nBytesPerRow)
					{
						n	= 0;
						x	= m_nOffsetHex;
						y	+= m_nLineHeight;
					}
				}
			}
			else
			{
				for (int i = m_nTopIndex; (i < m_nLength) && (rcText.TopLeft().y < nHeight); )
				{
					TCHAR*	p	= &buf[0];
					int		n;

					for (n = 0; (n < m_nBytesPerRow) && (i < m_nLength); n++)
					{
						if ((i == (m_nLength - 1)) && (m_bIncompleted == TRUE))
						{
							*p++	= _T(' ');
							*p++	= _T(' ');
						}
						else
						{
							TOHEX(m_lpData[i], p);
						}

						*p++	= _T(' ');
						i++;
					}

					while (n < m_nBytesPerRow)
					{
						*p++	= _T(' ');
						*p++	= _T(' ');
						*p++	= _T(' ');
						n++;
					}

					memDC.DrawText(buf, m_nBytesPerRow * HEX_ROW, rcText, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

					rcText.TopLeft().y	+= m_nLineHeight;
				}
			}
		}

		if (m_bShowASCII)
		{
			x					= m_nOffsetASCII;
			y					= 0;
			rcText				= rc;
			rcText.TopLeft().x	= m_nOffsetASCII;

			if ((m_nSelectionBegin != UNSELECTED) && (m_nCurrentMode == EDIT_ASCII))
			{
				int	i;
				int	n				= 0;
				int	nSelectBegin	= m_nSelectionBegin;
				int	nSelectEnd		= m_nSelectionEnd;

				if (nSelectBegin > nSelectEnd)
				{
					nSelectBegin ^= nSelectEnd ^= nSelectBegin ^= nSelectEnd;
				}

				memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				memDC.SetBkColor(GetSysColor(COLOR_WINDOW));

				for (i = m_nTopIndex; (i < nSelectBegin) && (y < nHeight); i++)
				{
					if ((i == (m_nLength - 1)) && (m_bIncompleted == TRUE))
					{
						buf[0]	= _T(' ');
					}
					else
					{
						buf[0]	= (isprint(m_lpData[i]) ? (TCHAR)(m_lpData[i]) : _T('.'));
					}

					memDC.ExtTextOut(x, y, ETO_OPAQUE, NULL, buf, 1, NULL);

					x	+= m_nNullWidth;
					n++;

					if (n == m_nBytesPerRow)
					{
						n	= 0;
						x	= m_nOffsetASCII;
						y	+= m_nLineHeight;
					}
				}

				memDC.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				memDC.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));

				for(; (i < nSelectEnd) && (i < m_nLength) && (y < nHeight); i++)
				{
					if ((i == (m_nLength - 1)) && (m_bIncompleted == TRUE))
					{
						buf[0]	= _T(' ');
					}
					else
					{
						buf[0]	= (isprint(m_lpData[i]) ? (TCHAR)(m_lpData[i]) : _T('.'));
					}

					memDC.ExtTextOut(x, y, ETO_OPAQUE, NULL, buf, 1, NULL);

					x	+= m_nNullWidth;
					n++;

					if (n == m_nBytesPerRow)
					{
						n	= 0;
						x	= m_nOffsetASCII;
						y	+= m_nLineHeight;
					}
				}

				memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				memDC.SetBkColor(GetSysColor(COLOR_WINDOW));

				for(; (i < m_nLength) && (y < nHeight); i++)
				{
					if ((i == (m_nLength - 1)) && (m_bIncompleted == TRUE))
					{
						buf[0]	= _T(' ');
					}
					else
					{
						buf[0]	= (isprint(m_lpData[i]) ? (TCHAR)(m_lpData[i]) : _T('.'));
					}

					memDC.ExtTextOut(x, y, ETO_OPAQUE, NULL, buf, 1, NULL);

					x	+= m_nNullWidth;
					n++;

					if (n == m_nBytesPerRow)
					{
						n	= 0;
						x	= m_nOffsetASCII;
						y	+= m_nLineHeight;
					}
				}
			}
			else
			{
				for (int i = m_nTopIndex; (i < m_nLength) && (rcText.TopLeft().y < nHeight); )
				{
					TCHAR*	p	= &buf[0];
					int		n;

					for (n = 0; (n < m_nBytesPerRow) && (i < m_nLength); n++)
					{
						if ((i == (m_nLength - 1)) && (m_bIncompleted == TRUE))
						{
							*p++	= _T(' ');
						}
						else
						{
							*p++	= (isprint(m_lpData[i]) ? (TCHAR)(m_lpData[i]) : _T('.'));
						}
						i++;
					}

					memDC.DrawText(buf, n, rcText, DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX);

					rcText.TopLeft().y	+= m_nLineHeight;
				}
			}
		}
	}

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(oldFont);
	memDC.DeleteDC();
	Bitmap.DeleteObject();
}

void CHexEditX::OnSetFocus(CWnd* pOldWnd)
{
	if ((m_lpData != NULL) && (!IsSelected()))
	{
		if ((m_ptEdit.x == 0) && (m_bShowAddress == TRUE))
		{
			CreateAddressCaret();
		}
		else
		{
			CreateEditCaret();
		}

		SetCaretPos(m_ptEdit);
		ShowCaret();
	}

	Invalidate();
	UpdateWindow();
	CEdit::OnSetFocus(pOldWnd);
}

void CHexEditX::OnKillFocus(CWnd* pNewWnd)
{
	DestroyCaret();
	CEdit::OnKillFocus(pNewWnd);
}

void CHexEditX::OnSize(UINT nType, int cx, int cy)
{
	m_bUpdate	= TRUE;

	CEdit::OnSize(nType, cx, cy);
}

void CHexEditX::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (m_lpData == NULL)
	{
		return;
	}

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		{
			if (m_nTopIndex < (m_nLength - PAGE_SIZE))
			{
				m_nTopIndex	+= m_nBytesPerRow;

				if (m_nTopIndex > m_nCurrentAddress)
				{
					m_nCurrentAddress	+= m_nBytesPerRow;
				}

				if (m_nCurrentAddress >= m_nLength)
				{
					m_nCurrentAddress	= m_nLength;
				}
			}
			else
			{
				m_nCurrentAddress	+= m_nBytesPerRow;

				if (m_nCurrentAddress >= m_nLength)
				{
					m_nCurrentAddress	= m_nLength - 1;
				}
			}

			Invalidate();
			UpdateWindow();
		}
		break;
	case SB_LINEUP:
		{
			if (m_nTopIndex > 0)
			{
				m_nTopIndex	-= m_nBytesPerRow;

				if (m_nTopIndex < 0)
				{
					m_nTopIndex	= 0;
				}

				if (m_nCurrentAddress > m_nTopIndex + PAGE_SIZE)
				{
					m_nCurrentAddress	-= m_nBytesPerRow;
				}

				if (m_nCurrentAddress < 0)
				{
					m_nCurrentAddress	= 0;
				}
			}
			else
			{
				m_nCurrentAddress	-= m_nBytesPerRow;

				if (m_nCurrentAddress < 0)
				{
					m_nCurrentAddress	= 0;
				}
			}

			Invalidate();
			UpdateWindow();
		}
		break;
	case SB_PAGEDOWN:
		{
			if ((m_nTopIndex + PAGE_SIZE) <= (m_nLength - PAGE_SIZE))
			{
				m_nTopIndex			+= PAGE_SIZE;
				m_nCurrentAddress	+= PAGE_SIZE;
			}
			else
			{
				int	nLast	= m_nBytesPerRow - (m_nLength % m_nBytesPerRow);

				m_nTopIndex	= m_nLength - PAGE_SIZE + ((nLast != m_nBytesPerRow) ? nLast : 0);

				if (m_nTopIndex < 0)
				{
					m_nTopIndex	= 0;
				}

				m_nCurrentAddress	+= PAGE_SIZE;

				if (m_nCurrentAddress >= m_nLength)
				{
					m_nCurrentAddress	= m_nLength - 1;
				}
			}

			Invalidate();
			UpdateWindow();
		}
		break;
	case SB_PAGEUP:
		{
			if (m_nTopIndex > 0)
			{
				m_nTopIndex			-= PAGE_SIZE;
				m_nCurrentAddress	-= PAGE_SIZE;

				if (m_nTopIndex < 0)
				{
					m_nTopIndex	= 0;
				}

				if (m_nCurrentAddress < 0)
				{
					m_nCurrentAddress	= 0;
				}
			}
			else
			{
				if (m_nCurrentAddress > 0)
				{
					m_nCurrentAddress	= 0;
				}
			}

			Invalidate();
			UpdateWindow();
		}
		break;
	case SB_THUMBTRACK:
		{
			SCROLLINFO	si;

			ZeroMemory(&si, sizeof(SCROLLINFO));

			si.cbSize	= sizeof(SCROLLINFO);
			si.fMask	= SIF_TRACKPOS;

			if (!GetScrollInfo(SB_VERT, &si))
			{
				return;
			}

			m_nTopIndex	= (si.nTrackPos / m_nBytesPerRow) * m_nBytesPerRow;

			if (m_nCurrentAddress < m_nTopIndex)
			{
				while (m_nCurrentAddress < m_nTopIndex)
				{
					m_nCurrentAddress	+= m_nBytesPerRow;
				}

				if (m_nCurrentAddress >= m_nLength)
				{
					m_nCurrentAddress	= m_nLength - 1;
				}
			}

			if (m_nCurrentAddress >= (m_nTopIndex + PAGE_SIZE))
			{
				while (m_nCurrentAddress >= (m_nTopIndex + PAGE_SIZE))
				{
					m_nCurrentAddress	-= m_nBytesPerRow;
				}

				if (m_nCurrentAddress < 0)
				{
					m_nCurrentAddress	= 0;
				}
			}

			Invalidate();
			UpdateWindow();
		}
		break;
	}

	SetScrollPos(SB_VERT, m_nTopIndex, TRUE);
	RepositionCaret(m_nCurrentAddress);
}

BOOL CHexEditX::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CHexEditX::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	if (!m_lpData)
	{
		return;
	}

	CPoint	pt	= CalcPos(point.x, point.y);

	if (m_nCurrentAddress > (m_nLength - 1))
	{
		m_nCurrentAddress	= m_nLength - 1;
		pt.x				= -1;
		pt.y				= -1;
	}

	if ((m_nCurrentAddress == (m_nLength - 1)) && (m_bIncompleted == TRUE) && (m_nCurrentMode == EDIT_LOW))
	{
		m_nCurrentMode		= EDIT_HIGH;

		if (pt.x > 1)
		{
			pt.x--;
		}
	}

	if (pt.x > -1)
	{
		m_ptEdit	= pt;
		pt.x		*= m_nNullWidth;
		pt.y		*= m_nLineHeight;

		if ((pt.x == 0) && (m_bShowAddress == TRUE))
		{
			CreateAddressCaret();
		}
		else
		{
			CreateEditCaret();
		}

		SetCaretPos(pt);

		if (nFlags & MK_SHIFT)
		{
			m_nSelectionEnd	= m_nCurrentAddress;

			if ((m_nCurrentMode == EDIT_HIGH) || (m_nCurrentMode == EDIT_LOW))
			{
				m_nSelectionEnd++;
			}

			Invalidate();
			UpdateWindow();
		}
	}

	if (!IsSelected())
	{
		ShowCaret();
	}
}

void CHexEditX::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_lpData)
	{
		return;
	}

	if ((nFlags & MK_LBUTTON) && (m_nSelectionBegin != UNSELECTED))
	{
		CRect	rc;

		GetClientRect(&rc);

		if (!rc.PtInRect(point))
		{
			if (point.y < 0)
			{
				OnVScroll(SB_LINEUP, 0, NULL);

				point.y	= 0;
			}
			else if (point.y > rc.Height())
			{
				OnVScroll(SB_LINEDOWN, 0, NULL);

				point.y	= rc.Height() - 1;
			}
		}

		int		nEndBefore	= m_nSelectionEnd;
		CPoint	pt			= CalcPos(point.x, point.y);

		if (pt.x > -1)
		{
			m_nSelectionEnd	= m_nCurrentAddress;

			if ((m_nCurrentMode == EDIT_HIGH) || (m_nCurrentMode == EDIT_LOW))
			{
				m_nSelectionEnd++;
			}
		}

		if (IsSelected())
		{
			DestroyCaret();
		}

		if (nEndBefore != m_nSelectionEnd)
		{
			Invalidate();
			UpdateWindow();
		}
	}
}

void CHexEditX::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsSelected())
	{
		ReleaseCapture();
	}

	CEdit::OnLButtonUp(nFlags, point);
}


void CHexEditX::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!m_lpData)
	{
		return;
	}

	if (nChar == _T('\t'))
	{
		return;
	}

	if (GetKeyState(VK_CONTROL) & 0x80000000)
	{
		switch (nChar)
		{
		case 0x03:
			{
				if (IsSelected())
				{
					OnEditCopy();
				}
			}
			return;
		case 0x16:
			{
				OnEditPaste();
			}
			return;
		case 0x18:
			{
				if (IsSelected())
				{
					OnEditCut();
				}
			}
			return;
		}
	}

	if (nChar == 0x08)
	{
		if (m_nCurrentAddress > 0)
		{
			m_nCurrentAddress--;

			SelDelete(m_nCurrentAddress, m_nCurrentAddress + 1);
			RepositionCaret(m_nCurrentAddress);
			Invalidate();
			UpdateWindow();
		}

		return;
	}

	SetSelection(-1, -1);

	switch (m_nCurrentMode)
	{
	case EDIT_NONE:
		return;
	case EDIT_HIGH:
	case EDIT_LOW:
		{
			if (((nChar >= _T('0')) && (nChar <= _T('9'))) || ((nChar >= _T('a')) && (nChar <= _T('f'))))
			{
				UINT	b	= nChar - _T('0');

				if (b > 0x09)
				{
					b	= 0x0A + nChar - _T('a');
				}

				if (m_nCurrentMode == EDIT_HIGH)
				{
					m_lpData[m_nCurrentAddress]	= (BYTE)((m_lpData[m_nCurrentAddress] & 0x0F) | (b << 4));

					if (m_nCurrentAddress == (m_nLength - 1))
					{
						m_bIncompleted	= FALSE;
					}
				}
				else
				{
					m_lpData[m_nCurrentAddress]	= (BYTE)((m_lpData[m_nCurrentAddress] & 0xF0) | b);

					if (m_nCurrentAddress == (m_nLength - 1))
					{
						SelInsert(m_nCurrentAddress + 1, 1);

						m_bIncompleted	= TRUE;
					}
				}

				Move(1, 0);
			}
		}
		break;
	case EDIT_ASCII:
		{
			m_lpData[m_nCurrentAddress]	= (BYTE)(nChar & 0xFF);

			if (m_nCurrentAddress == (m_nLength - 1))
			{
				SelInsert(m_nCurrentAddress + 1, 1);

				m_bIncompleted	= TRUE;
			}

			Move(1, 0);
		}
		break;
	}

	Invalidate();
	UpdateWindow();
}

void CHexEditX::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL	bShift	= (GetKeyState(VK_SHIFT) & 0x80000000);

	switch (nChar)
	{
	case VK_UP:
		{
			if (bShift)
			{
				if (!IsSelected())
				{
					m_nSelectionBegin	= m_nCurrentAddress;
				}

				Move(0, -1);

				m_nSelectionEnd	= m_nCurrentAddress;

				Invalidate();
				UpdateWindow();
				break;
			}
			else
			{
				SetSelection(-1, -1);
			}

			Move(0,-1);
		}
		break;
	case VK_DOWN:
		{
			if (bShift)
			{
				if (!IsSelected())
				{
					m_nSelectionBegin	= m_nCurrentAddress;
				}

				Move(0, 1);

				m_nSelectionEnd	= m_nCurrentAddress;

				if ((m_nCurrentMode == EDIT_HIGH) || (m_nCurrentMode == EDIT_LOW))
				{
					m_nSelectionEnd++;
				}

				Invalidate();
				UpdateWindow();
				break;
			}
			else
			{
				SetSelection(-1, -1);
			}

			Move(0, 1);
		}
		break;
	case VK_LEFT:
		{
			if (bShift)
			{
				if (!IsSelected())
				{
					m_nSelectionBegin	= m_nCurrentAddress;
				}

				Move(-1, 0);

				m_nSelectionEnd	= m_nCurrentAddress;

				Invalidate();
				UpdateWindow();
				break;
			}
			else
			{
				SetSelection(-1, -1);
			}

			Move(-1, 0);
		}
		break;
	case VK_RIGHT:
		{
			if (bShift)
			{
				if (!IsSelected())
				{
					m_nSelectionBegin	= m_nCurrentAddress;
				}

				Move(1, 0);

				m_nSelectionEnd	= m_nCurrentAddress;

				if ((m_nCurrentMode == EDIT_HIGH) || (m_nCurrentMode == EDIT_LOW))
				{
					m_nSelectionEnd++;
				}

				Invalidate();
				UpdateWindow();
				break;
			}
			else
			{
				SetSelection(-1, -1);
			}

			Move(1, 0);
		}
		break;
	case VK_PRIOR:
		{
			if (bShift)
			{
				if (!IsSelected())
				{
					m_nSelectionBegin	= m_nCurrentAddress;
				}

				OnVScroll(SB_PAGEUP, 0, NULL);
				Move(0, 0);

				m_nSelectionEnd	= m_nCurrentAddress;

				Invalidate();
				UpdateWindow();
				break;
			}
			else
			{
				SetSelection(-1, -1);
			}

			OnVScroll(SB_PAGEUP, 0, NULL);
			Move(0, 0);
		}
		break;
	case VK_NEXT:
		{
			if (bShift)
			{
				if (!IsSelected())
				{
					m_nSelectionBegin	= m_nCurrentAddress;
				}

				OnVScroll(SB_PAGEDOWN, 0, NULL);
				Move(0, 0);

				m_nSelectionEnd	= m_nCurrentAddress;

				Invalidate();
				UpdateWindow();
				break;
			}
			else
			{
				SetSelection(-1, -1);
			}

			OnVScroll(SB_PAGEDOWN, 0, NULL);
			Move(0, 0);
		}
		break;
	case VK_HOME:
		{
			if (bShift)
			{
				if (!IsSelected())
				{
					m_nSelectionBegin	= m_nCurrentAddress;
				}

				if (GetKeyState(VK_CONTROL) & 0x80000000)
				{
					OnVScroll(SB_THUMBTRACK, 0, NULL);
					Move(0, 0);
				}
				else
				{
					m_nCurrentAddress	/= m_nBytesPerRow;
					m_nCurrentAddress	*= m_nBytesPerRow;

					Move(0, 0);
				}

				m_nSelectionEnd	= m_nCurrentAddress;

				Invalidate();
				UpdateWindow();
				break;
			}
			else
			{
				SetSelection(-1, -1);
			}

			if (GetKeyState(VK_CONTROL) & 0x80000000)
			{
				OnVScroll(SB_THUMBTRACK, 0, NULL);

				m_nCurrentAddress	= 0;

				Move(0, 0);
			}
			else
			{
				m_nCurrentAddress	/= m_nBytesPerRow;
				m_nCurrentAddress	*= m_nBytesPerRow;

				Move(0, 0);
			}
		}
		break;
	case VK_END:
		{
			if (bShift)
			{
				if (!IsSelected())
				{
					m_nSelectionBegin	= m_nCurrentAddress;
				}

				if (GetKeyState(VK_CONTROL) & 0x80000000)
				{
					m_nCurrentAddress	= m_nLength - 1;

					OnVScroll(SB_THUMBTRACK, ((m_nLength + (m_nBytesPerRow / 2)) / m_nBytesPerRow) - m_nLinesPerPage, NULL);
					Move(0, 0);
				}
				else
				{
					m_nCurrentAddress	/= m_nBytesPerRow;
					m_nCurrentAddress	*= m_nBytesPerRow;
					m_nCurrentAddress	+= m_nBytesPerRow - 1;

					if (m_nCurrentAddress > m_nLength)
					{
						m_nCurrentAddress	= m_nLength - 1;
					}

					Move(0, 0);
				}

				m_nSelectionEnd	= m_nCurrentAddress;

				Invalidate();
				UpdateWindow();
				break;
			}
			else
			{
				SetSelection(-1, -1);
			}

			if (GetKeyState(VK_CONTROL) & 0x80000000)
			{
				m_nCurrentAddress	= m_nLength - 1;

				OnVScroll(SB_THUMBTRACK, ((m_nLength + (m_nBytesPerRow / 2)) / m_nBytesPerRow) - m_nLinesPerPage, NULL);
				Move(0, 0);
			}
			else
			{
				m_nCurrentAddress	/= m_nBytesPerRow;
				m_nCurrentAddress	*= m_nBytesPerRow;
				m_nCurrentAddress	+= m_nBytesPerRow - 1;

				if (m_nCurrentAddress > m_nLength)
				{
					m_nCurrentAddress	= m_nLength - 1;
				}

				Move(0, 0);
			}
		}
		break;
	case VK_INSERT:
		{
			SelInsert(m_nCurrentAddress, max(1, m_nSelectionEnd - m_nSelectionBegin));
			Invalidate();
			UpdateWindow();
		}
		break;
	case VK_DELETE:
		{
			if (IsSelected())
			{
				OnEditClear();
			}
			else
			{
				if (m_nLength > 0)
				{
					SelDelete(m_nCurrentAddress, m_nCurrentAddress + 1);
					Invalidate();
					UpdateWindow();
				}
			}
		}
		break;
	case VK_TAB:
		{
			switch (m_nCurrentMode)
			{
			case EDIT_NONE:
				{
					m_nCurrentMode	= EDIT_HIGH;
				}
				break;
			case EDIT_HIGH:
			case EDIT_LOW:
				{
					m_nCurrentMode	= EDIT_ASCII;
				}
				break;
			case EDIT_ASCII:
				{
					m_nCurrentMode	= EDIT_HIGH;
				}
				break;
			}

			Move(0, 0);
		}
		break;
	}
}
