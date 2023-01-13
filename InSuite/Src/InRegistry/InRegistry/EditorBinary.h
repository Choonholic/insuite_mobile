// EditorBinary.h

#pragma once

#include "OptionsDialogX.h"
#include "HexEditX.h"

class CEditorBinary : public COptionsDialogX
{
	DECLARE_DYNAMIC(CEditorBinary)

public:
	CEditorBinary(CWnd* pParent = NULL);
	virtual	~CEditorBinary();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_EDITOR_BINARY };

	CStatic		m_ctlDataH;
	CHexEditX	m_ctlData;
	BYTE		m_cBuffer[4096];
	int			m_cbSize;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();

	DECLARE_MESSAGE_MAP()
};
