// ComboBoxX.cpp

#include "stdafx.h"
#include "ComboBoxX.h"

IMPLEMENT_DYNAMIC(CComboBoxX, CComboBox)

CComboBoxX::CComboBoxX()
{
	m_nShowType	= CBX_NORMAL;
}

CComboBoxX::~CComboBoxX()
{
}

BEGIN_MESSAGE_MAP(CComboBoxX, CComboBox)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CComboBoxX::SetShowType(int nShowType)
{
	m_nShowType	= nShowType;
}

void CComboBoxX::OnPaint()
{
	switch (m_nShowType)
	{
	case CBX_UNDERLINE:
		{
			CRect	rect;
			HDC		hDC		= ::GetDC(this->m_hWndOwner);

			GetClientRect(&rect);
			ClientToScreen(&rect);
			::IntersectClipRect(hDC, rect.left, rect.top, rect.right, rect.bottom);
			CComboBox::OnPaint();
			::SelectClipRgn(hDC, NULL);
			::ExcludeClipRect(hDC, rect.left, rect.top, rect.right, rect.bottom);
			rect.InflateRect(1, 1);

			rect.bottom--;

			::FillRect(hDC, &rect, GetSysColorBrush(COLOR_WINDOW));
			::ReleaseDC(this->m_hWndOwner, hDC);
		}
		break;
	case CBX_NORMAL:
	default:
		{
			CComboBox::OnPaint();
		}
		break;
	}
}
