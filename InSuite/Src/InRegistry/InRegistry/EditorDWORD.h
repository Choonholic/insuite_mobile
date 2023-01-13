// EditorDWORD.h

#pragma once

#include "OptionsDialogX.h"
#include "DWordEditX.h"

class CEditorDWORD : public COptionsDialogX
{
	DECLARE_DYNAMIC(CEditorDWORD)

public:
	CEditorDWORD(CWnd* pParent = NULL);
	virtual	~CEditorDWORD();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnCbnSelchangeEdwBase();

	enum { IDD = IDD_EDITOR_DWORD };

	CStatic		m_ctlDataH;
	CDWordEditX	m_ctlData;
	CStatic		m_ctlBaseH;
	CComboBox	m_ctlBase;
	DWORD		m_dwData;
	int			m_nBase;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);
	virtual void	OnOK();

	DECLARE_MESSAGE_MAP()
};
