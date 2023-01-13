// StaticX.cpp

#include "stdafx.h"
#include "StaticX.h"

IMPLEMENT_DYNAMIC(CStaticX, CStatic)

CStaticX::CStaticX()
{
	m_crBack	= GetSysColor(COLOR_WINDOW);
	m_crText	= GetSysColor(COLOR_STATICTEXT);

	m_brBkgnd.CreateSolidBrush(m_crBack);
}

CStaticX::~CStaticX()
{
}

BEGIN_MESSAGE_MAP(CStaticX, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CStaticX::CtlColor(CDC* pDC, UINT nCtlColor)
{
	HBRUSH	hbr	= (HBRUSH)m_brBkgnd;

	pDC->SetBkColor(m_crBack);
	pDC->SetTextColor(m_crText);
	return hbr;
}

void CStaticX::SetBackColor(COLORREF cr)
{
	m_crBack	= cr;

	m_brBkgnd.DeleteObject();
	m_brBkgnd.CreateSolidBrush(cr);
	RedrawWindow();
}

void CStaticX::SetTextColor(COLORREF cr)
{
	m_crText	= cr;

	RedrawWindow();
}
