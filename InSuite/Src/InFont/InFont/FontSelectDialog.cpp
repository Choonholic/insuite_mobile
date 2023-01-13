// FontSelectDialog.cpp

#include "stdafx.h"
#include "InFont.h"
#include "FontSelectDialog.h"
#include "DisplayX.h"
#include "FontX.h"
#include "ShellX.h"
#include "FontManagerX.h"
#include "RegistryMFCX.h"
#include "FontInfo.h"

IMPLEMENT_DYNAMIC(CFontSelectDialog, COptionsDialogX)

CFontSelectDialog::CFontSelectDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CFontSelectDialog::IDD, pParent)
{
	m_bLinkOnly		= FALSE;
}

CFontSelectDialog::~CFontSelectDialog()
{
}

void CFontSelectDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_FS_FONTFACE, m_nSelected);
	DDX_Control(pDX, IDC_FS_FONTFACE, m_ctlFontFace);
}

BEGIN_MESSAGE_MAP(CFontSelectDialog, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CFontSelectDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();
	FillList();

	if (m_ctlFontFace.GetCount() > 0)
	{
		m_ctlFontFace.SetCurSel(0);

		m_nSelected	= 0;
	}
	else
	{
		EndDialog(IDCANCEL);
	}

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_SELECT);

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
	return TRUE;
}

void CFontSelectDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlFontFace.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlFontFace.MoveWindow(&rc);
}

void CFontSelectDialog::FillList()
{
	CString			strFindPath;
	TCHAR			szFontPath[MAX_PATH + 1];
	HANDLE			hFind;
	WIN32_FIND_DATA	fd;
	CFontManagerX	FMX;
	CString			strFoundFile;
	DWORD			i;

	m_ctlFontFace.ResetContent();
	GetFontPath(szFontPath, MAX_PATH + 1);
	CombineFileName(strFindPath.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, szFontPath, _T("*.ttf"));
	strFindPath.ReleaseBuffer();

	hFind	= FindFirstFile(strFindPath, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CombineFileName(strFoundFile.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, szFontPath, fd.cFileName);
			strFoundFile.ReleaseBuffer();

			if (FMX.LoadFontFile(strFoundFile))
			{
				for (i = 0; i < FMX.GetFontCount(); i++)
				{
					if (FMX.m_nEncodingID[i] == 1)
					{
						m_ctlFontFace.AddString(FMX.m_szFontFace[i]);
						m_arrFontFace.Add(FMX.m_szFontFace[i]);

						if (m_bLinkOnly == FALSE)
						{
							m_arrFileName.Add(strFoundFile);
						}
					}
				}
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}

	CombineFileName(strFindPath.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, szFontPath, _T("*.ttc"));
	strFindPath.ReleaseBuffer();

	hFind	= FindFirstFile(strFindPath, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CombineFileName(strFoundFile.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, szFontPath, fd.cFileName);
			strFoundFile.ReleaseBuffer();

			if (FMX.LoadFontFile(strFoundFile))
			{
				for (i = 0; i < FMX.GetFontCount(); i++)
				{
					if (FMX.m_nEncodingID[i] == 1)
					{
						m_ctlFontFace.AddString(FMX.m_szFontFace[i]);
						m_arrFontFace.Add(FMX.m_szFontFace[i]);

						if (m_bLinkOnly == FALSE)
						{
							m_arrFileName.Add(strFoundFile);
						}
					}
				}
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}

	CombineFileName(strFindPath.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, szFontPath, _T("*.ac3"));
	strFindPath.ReleaseBuffer();

	hFind	= FindFirstFile(strFindPath, &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CombineFileName(strFoundFile.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, szFontPath, fd.cFileName);
			strFoundFile.ReleaseBuffer();

			if (FMX.LoadFontFile(strFoundFile))
			{
				for (i = 0; i < FMX.GetFontCount(); i++)
				{
					if (FMX.m_nEncodingID[i] == 1)
					{
						m_ctlFontFace.AddString(FMX.m_szFontFace[i]);
						m_arrFontFace.Add(FMX.m_szFontFace[i]);

						if (m_bLinkOnly == FALSE)
						{
							m_arrFileName.Add(strFoundFile);
						}
					}
				}
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}

	if (m_bLinkOnly == TRUE)
	{
		CRegistryMFCX	reg;
		BOOL			bFound;
		BOOL			bExist;
		CString			strValue;

		reg.Open(HKEY_LOCAL_MACHINE, SYSLINK_KEY);

		bFound	= reg.GetFirstValue(strValue);

		while (bFound == TRUE)
		{
			bExist	= FALSE;

			for (i = 0; i < (DWORD)m_arrFontFace.GetCount(); i++)
			{
				if (!strValue.CompareNoCase(m_arrFontFace.GetAt(i)))
				{
					bExist	= TRUE;

					break;
				}
			}

			if (bExist == FALSE)
			{
				m_ctlFontFace.AddString(strValue);
				m_arrFontFace.Add(strValue);
			}

			bFound	= reg.GetNextValue(strValue);
		}

		reg.Close();
	}
}
