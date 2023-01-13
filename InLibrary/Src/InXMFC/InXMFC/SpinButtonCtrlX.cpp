// SpinButtonCtrlX.cpp

#include "stdafx.h"
#include "SpinButtonCtrlX.h"

IMPLEMENT_DYNAMIC(CSpinButtonCtrlX, CSpinButtonCtrl)

CSpinButtonCtrlX::CSpinButtonCtrlX()
{
}

CSpinButtonCtrlX::~CSpinButtonCtrlX()
{
}

BEGIN_MESSAGE_MAP(CSpinButtonCtrlX, CSpinButtonCtrl)
END_MESSAGE_MAP()

void CSpinButtonCtrlX::PreSubclassWindow()
{
	SendMessage(CCM_SETVERSION, COMCTL32_VERSION, 0);
	CSpinButtonCtrl::PreSubclassWindow();
}
