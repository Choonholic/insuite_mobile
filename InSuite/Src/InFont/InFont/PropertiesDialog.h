// PropertiesDialog.h

#pragma once

#include "OptionsDialogX.h"
#include "StaticX.h"

class CPropertiesDialog : public COptionsDialogX
{
	DECLARE_DYNAMIC(CPropertiesDialog)

public:
	CPropertiesDialog(CWnd* pParent = NULL);
	virtual	~CPropertiesDialog();

	void	SetPreviewFont();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);

	enum { IDD = IDD_PROPERTIES_DIALOG };

	CString		m_strFileName;
	CString		m_strVersion;
	CString		m_strCopyright;
	CStatic		m_ctlFileName;
	CStatic		m_ctlVersion;
	CStatic		m_ctlCopyright;
	CStaticX	m_ctlPreview;
	CString		m_strFontFace;
	CFont		m_PreviewFont;
	int			m_nEncodingID;

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
