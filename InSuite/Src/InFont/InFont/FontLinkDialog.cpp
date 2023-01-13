// FontLinkDialog.cpp

#include "stdafx.h"
#include "InFont.h"
#include "FontLinkDialog.h"
#include "FontSelectDialog.h"
#include "FontInfo.h"

IMPLEMENT_DYNAMIC(CFontLinkDialog, COptionsDialogX)

CFontLinkDialog::CFontLinkDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CFontLinkDialog::IDD, pParent)
{
}

CFontLinkDialog::~CFontLinkDialog()
{
}

void CFontLinkDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FL_FONTLIST, m_ctlList);
	DDX_Control(pDX, IDC_FL_ADD, m_ctlAdd);
	DDX_Control(pDX, IDC_FL_REMOVE, m_ctlRemove);
	DDX_Control(pDX, IDC_FL_MOVEUP, m_ctlMoveUp);
	DDX_Control(pDX, IDC_FL_MOVEDOWN, m_ctlMoveDown);
	DDX_Control(pDX, IDC_FL_SKIPTABLE_H, m_ctlSkipTable);
	DDX_Control(pDX, IDC_FL_SKIPTABLE_TEXT, m_ctlSkipTableText);
	DDX_Control(pDX, IDC_FL_SKIPTABLE_DEFAULT, m_ctlSkipTableDefault);
	DDX_Text(pDX, IDC_FL_SKIPTABLE_TEXT, m_strSkipTableText);
}

BEGIN_MESSAGE_MAP(CFontLinkDialog, COptionsDialogX)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_FL_FONTLIST, &CFontLinkDialog::OnLbnSelchangeFlFontlist)
	ON_BN_CLICKED(IDC_FL_ADD, &CFontLinkDialog::OnBnClickedFlAdd)
	ON_BN_CLICKED(IDC_FL_REMOVE, &CFontLinkDialog::OnBnClickedFlRemove)
	ON_BN_CLICKED(IDC_FL_MOVEUP, &CFontLinkDialog::OnBnClickedFlMoveup)
	ON_BN_CLICKED(IDC_FL_MOVEDOWN, &CFontLinkDialog::OnBnClickedFlMovedown)
	ON_BN_CLICKED(IDC_FL_SKIPTABLE_DEFAULT, &CFontLinkDialog::OnBnClickedFlSkiptableDefault)
END_MESSAGE_MAP()

BOOL CFontLinkDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_DONE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	FillList();

	if (m_ctlList.GetCount() > 0)
	{
		m_ctlList.SetCurSel(0);
	}

	ResetButtons();
	return TRUE;
}

void CFontLinkDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rcList;
	RECT	rcButton;
	RECT	rcLabel;
	SIZE	size;

	m_ctlList.GetWindowRect(&rcList);
	ScreenToClient(&rcList);

	m_ctlAdd.GetWindowRect(&rcButton);
	ScreenToClient(&rcButton);

	size.cx			= rcButton.right - rcButton.left;
	size.cy			= rcButton.bottom - rcButton.top;
	rcButton.right	= cx - rcList.left;
	rcButton.left	= rcButton.right - size.cx;

	m_ctlAdd.MoveWindow(&rcButton);

	rcButton.top	= rcButton.bottom + rcList.left;
	rcButton.bottom	= rcButton.top + size.cy;
	rcButton.right	= cx - rcList.left;
	rcButton.left	= rcButton.right - size.cx;

	m_ctlRemove.MoveWindow(&rcButton);

	rcButton.top	= rcButton.bottom + rcList.left;
	rcButton.bottom	= rcButton.top + size.cy;
	rcButton.right	= cx - rcList.left;
	rcButton.left	= rcButton.right - size.cx;

	m_ctlMoveUp.MoveWindow(&rcButton);

	rcButton.top	= rcButton.bottom + rcList.left;
	rcButton.bottom	= rcButton.top + size.cy;
	rcButton.right	= cx - rcList.left;
	rcButton.left	= rcButton.right - size.cx;

	m_ctlMoveDown.MoveWindow(&rcButton);

	rcButton.right	= cx - rcList.left;
	rcButton.left	= rcButton.right - size.cx;
	rcButton.bottom	= cy - rcList.left;
	rcButton.top	= rcButton.bottom - size.cy;

	m_ctlSkipTableDefault.MoveWindow(&rcButton);

	rcButton.right	= rcButton.left - rcList.left;
	rcButton.left	= rcList.left;
	rcButton.bottom	= cy - rcList.left;
	rcButton.top	= rcButton.bottom - size.cy;

	m_ctlSkipTableText.MoveWindow(&rcButton);

	m_ctlSkipTable.GetWindowRect(&rcLabel);
	ScreenToClient(&rcLabel);

	size.cy			= rcLabel.bottom - rcLabel.top;
	rcLabel.left	= rcButton.left;
	rcLabel.right	= rcButton.right;
	rcLabel.bottom	= rcButton.top - rcList.left;
	rcLabel.top		= rcLabel.bottom - size.cy;

	m_ctlSkipTable.MoveWindow(&rcLabel);

	rcList.right	= rcButton.right;
	rcList.bottom	= rcLabel.top - rcList.left;

	m_ctlList.MoveWindow(&rcList);
}

void CFontLinkDialog::OnLbnSelchangeFlFontlist()
{
	ResetButtons();
}

void CFontLinkDialog::OnBnClickedFlAdd()
{
	CFontSelectDialog	dFontSelect;

	dFontSelect.SetHeader(IDS_FONTADD_TITLE);

	if (dFontSelect.DoModal() == IDOK)
	{
		CString	strFileName;
		CString	strFontFace;
		BOOL	bFound	= FALSE;

		strFileName	= dFontSelect.m_arrFileName.GetAt(dFontSelect.m_nSelected);
		strFontFace	= dFontSelect.m_arrFontFace.GetAt(dFontSelect.m_nSelected);

		for (int i = 0; i < m_ctlList.GetCount(); i++)
		{
			if ((!strFileName.CompareNoCase(m_arrFileName.GetAt(i))) && (!strFontFace.CompareNoCase(m_arrFontFace.GetAt(i))))
			{
				bFound	= TRUE;

				break;
			}
		}

		if (!bFound)
		{
			int	nCurrent	= m_ctlList.GetCurSel();

			m_arrFileName.Add(strFileName);
			m_arrFontFace.Add(strFontFace);
			FillList();

			if (m_ctlList.GetCount() > 0)
			{
				if (nCurrent < 0)
				{
					nCurrent	= 0;
				}

				if (nCurrent >= m_ctlList.GetCount())
				{
					nCurrent	= (m_ctlList.GetCount() - 1);
				}

				m_ctlList.SetCurSel(nCurrent);
			}

			ResetButtons();
		}
	}
}

void CFontLinkDialog::OnBnClickedFlRemove()
{
	int	nCurrent	= m_ctlList.GetCurSel();

	m_arrFileName.RemoveAt(nCurrent);
	m_arrFontFace.RemoveAt(nCurrent);
	FillList();

	if (m_ctlList.GetCount() > 0)
	{
		if (nCurrent < 0)
		{
			nCurrent	= 0;
		}

		if (nCurrent >= m_ctlList.GetCount())
		{
			nCurrent	= (m_ctlList.GetCount() - 1);
		}

		m_ctlList.SetCurSel(nCurrent);
	}

	ResetButtons();
}

void CFontLinkDialog::OnBnClickedFlMoveup()
{
	int		nCurrent	= m_ctlList.GetCurSel();
	CString	strItem;

	if ((nCurrent > 0) && (nCurrent != LB_ERR))
	{
		strItem	= m_arrFileName.GetAt(nCurrent);

		m_arrFileName.SetAt(nCurrent, m_arrFileName.GetAt(nCurrent - 1));
		m_arrFileName.SetAt(nCurrent - 1, strItem);

		strItem	= m_arrFontFace.GetAt(nCurrent);

		m_arrFontFace.SetAt(nCurrent, m_arrFontFace.GetAt(nCurrent - 1));
		m_arrFontFace.SetAt(nCurrent - 1, strItem);
	}

	--nCurrent;

	FillList();

	if (m_ctlList.GetCount() > 0)
	{
		if (nCurrent < 0)
		{
			nCurrent	= 0;
		}

		if (nCurrent >= m_ctlList.GetCount())
		{
			nCurrent	= (m_ctlList.GetCount() - 1);
		}

		m_ctlList.SetCurSel(nCurrent);
	}

	ResetButtons();
}

void CFontLinkDialog::OnBnClickedFlMovedown()
{
	int		nCurrent	= m_ctlList.GetCurSel();
	CString	strItem;

	if ((nCurrent < (m_ctlList.GetCount() - 1)) && (nCurrent != LB_ERR))
	{
		strItem	= m_arrFileName.GetAt(nCurrent);

		m_arrFileName.SetAt(nCurrent, m_arrFileName.GetAt(nCurrent + 1));
		m_arrFileName.SetAt(nCurrent + 1, strItem);

		strItem	= m_arrFontFace.GetAt(nCurrent);

		m_arrFontFace.SetAt(nCurrent, m_arrFontFace.GetAt(nCurrent + 1));
		m_arrFontFace.SetAt(nCurrent + 1, strItem);
	}

	++nCurrent;

	FillList();

	if (m_ctlList.GetCount() > 0)
	{
		if (nCurrent < 0)
		{
			nCurrent	= 0;
		}

		if (nCurrent >= m_ctlList.GetCount())
		{
			nCurrent	= (m_ctlList.GetCount() - 1);
		}

		m_ctlList.SetCurSel(nCurrent);
	}

	ResetButtons();
}

void CFontLinkDialog::OnBnClickedFlSkiptableDefault()
{
	if (m_ctlList.GetCount() > 0)
	{
		LCID	LocaleID	= GetSystemDefaultLCID();
		WORD	nLCID		= (WORD)(LocaleID & 0x0000FFFF);

		switch (nLCID)
		{
		case 0x0411:	// Japanese
			{
				m_strSkipTableText	= SKIPTABLE_1041;
			}
			break;
		case 0x0412:	// Korean
			{
				m_strSkipTableText	= SKIPTABLE_1042;
			}
			break;
		case 0x0804:	// Chinese (PRC)
			{
				m_strSkipTableText	= SKIPTABLE_2052;
			}
			break;
		case 0x0404:	// Chinese (Taiwan)
			{
				m_strSkipTableText	= SKIPTABLE_1028;
			}
			break;
		default:		// English (U.S.) and other languages
			{
				m_strSkipTableText	= SKIPTABLE_1033;
			}
			break;
		}
	}
	else
	{
		m_strSkipTableText.Empty();
	}

	UpdateData(FALSE);
}

void CFontLinkDialog::FillList()
{
	m_ctlList.ResetContent();

	for (int i = 0; i < m_arrFontFace.GetCount(); i++)
	{
		m_ctlList.AddString(m_arrFontFace.GetAt(i));
	}
}

void CFontLinkDialog::ResetButtons()
{
	m_ctlRemove.EnableWindow((BOOL)((m_arrFontFace.GetCount() > 0) && (m_ctlList.GetCurSel() != LB_ERR)));
	m_ctlMoveUp.EnableWindow((BOOL)((m_ctlList.GetCurSel() != LB_ERR) && (m_ctlList.GetCurSel() > 0)));
	m_ctlMoveDown.EnableWindow((BOOL)((m_ctlList.GetCurSel() != LB_ERR) && (m_ctlList.GetCurSel() < (m_ctlList.GetCount() - 1))));
}
