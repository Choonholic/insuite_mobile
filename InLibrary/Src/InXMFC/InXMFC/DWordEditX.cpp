// DWordEditX.cpp

#include "stdafx.h"
#include "DWordEditX.h"
#include "ConvertersX.h"

IMPLEMENT_DYNAMIC(CDWordEditX, CEdit)

CDWordEditX::CDWordEditX()
{
	m_nBase			= BASE_DEC;
	m_dwValue		= 0;
	m_bProcessing	= FALSE;
}

CDWordEditX::~CDWordEditX()
{
}

BEGIN_MESSAGE_MAP(CDWordEditX, CEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

#define HEXBUFSIZE	16

void CDWordEditX::SetDWordValue(DWORD dwValue)
{
	CString	strText;
	DWORD	dwSize	= HEXBUFSIZE;

	m_dwValue	= dwValue;

	switch (m_nBase)
	{
	case BASE_HEX:
		{
			DWORDToHexString(m_dwValue, strText.GetBuffer(HEXBUFSIZE), &dwSize);
		}
		break;
	case BASE_DEC:
		{
			DWORDToDecString(m_dwValue, strText.GetBuffer(HEXBUFSIZE), &dwSize);
		}
		break;
	}

	strText.ReleaseBuffer();
	SetWindowText(strText);
}

DWORD CDWordEditX::GetDWordValue()
{
	CString	strText;

	GetWindowText(strText);

	switch (m_nBase)
	{
	case BASE_HEX:
		{
			HexStringToDWORD(strText, m_dwValue);
		}
		break;
	case BASE_DEC:
		{
			DecStringToDWORD(strText, m_dwValue);
		}
		break;
	}

	return m_dwValue;
}

void CDWordEditX::SetBase(int nBase)
{
	GetDWordValue();

	m_nBase	= nBase;

	SetDWordValue(m_dwValue);
}

void CDWordEditX::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 0x08)
	{
		// Backspace: Do nothing
	}
	else
	{
		switch (m_nBase)
		{
		case BASE_DEC:
			{
				if ((nChar >= _T('0')) && (nChar <= _T('9')))
				{
					// Numeric: Do nothing
				}
				else
				{
					// Ignore Input
					return;
				}
			}
			break;
		case BASE_HEX:
			{
				if (((nChar >= _T('0')) && (nChar <= _T('9'))) || ((nChar >= _T('A')) && (nChar <= _T('F'))) || ((nChar >= _T('a')) && (nChar <= _T('f'))))
				{
					// Hexadecimal Values: Do nothing
				}
				else
				{
					// Ignore Input
					return;
				}
			}
			break;
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CDWordEditX::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_PROCESSKEY)
	{
		m_bProcessing	= TRUE;

		Clear();
		return;
	}

	if (m_bProcessing)
	{
		m_bProcessing	= FALSE;

		Clear();
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
