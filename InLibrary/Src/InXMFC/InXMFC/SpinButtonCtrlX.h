// SpinButtonCtrlX.h

#pragma once

class AFX_EXT_CLASS CSpinButtonCtrlX : public CSpinButtonCtrl
{
	DECLARE_DYNAMIC(CSpinButtonCtrlX)

public:
	CSpinButtonCtrlX();
	virtual	~CSpinButtonCtrlX();

protected:
	virtual void	PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
};
