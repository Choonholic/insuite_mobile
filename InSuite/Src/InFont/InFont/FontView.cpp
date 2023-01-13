// FontView.cpp

#include "stdafx.h"
#include "InFont.h"
#include "InSuite_Settings.h"
#include "InFont_Settings.h"
#include "DisplayX.h"
#include "ShellX.h"
#include "SystemX.h"
#include "FileDialogX.h"
#include "CopyDialogX.h"
#include "FontManagerX.h"
#include "RegistryMFCX.h"
#include "FontX.h"
#include "FontView.h"
#include "InstallDialog.h"
#include "PropertiesDialog.h"
#include "ComponentsDialog.h"
#include "BackupDialog.h"
#include "FontLinkDialog.h"
#include "FontSelectDialog.h"
#include "OptionsSheet.h"
#include "AboutDialog.h"
#include "FontInfo.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

IMPLEMENT_DYNCREATE(CFontView, CFormView)

CStringArray	g_arrFontList;

CFontView::CFontView() : CFormView(CFontView::IDD)
{
}

CFontView::~CFontView()
{
}

void CFontView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONTLIST, m_ctlFontList);
}

BEGIN_MESSAGE_MAP(CFontView, CFormView)
	ON_WM_SIZE()
	ON_MESSAGE(WM_LCX_TAPANDHOLD, OnTapAndHoldList)
	ON_COMMAND(ID_INSTALL_NEW_FONT, &CFontView::OnInstallNewFont)
	ON_COMMAND(ID_ADD_DEFAULT, &CFontView::OnAddDefault)
	ON_COMMAND(ID_ADD_DEFAULT_FIXED, &CFontView::OnAddDefaultFixed)
	ON_COMMAND(ID_LINK_TO, &CFontView::OnLinkTo)
	ON_COMMAND(ID_DELETE, &CFontView::OnDelete)
	ON_COMMAND(ID_PROPERTIES, &CFontView::OnProperties)
	ON_COMMAND(ID_FONTS_DEFAULT, &CFontView::OnFontsDefault)
	ON_COMMAND(ID_FONTS_DEFAULT_FIXED, &CFontView::OnFontsDefaultFixed)
	ON_COMMAND(ID_FONTS_LINKING, &CFontView::OnFontsLinking)
	ON_COMMAND(ID_FONTS_SYSTEM, &CFontView::OnFontsSystem)
	ON_COMMAND(ID_ANTIALIASING, &CFontView::OnAntialiasing)
	ON_COMMAND(ID_TYPE_ICON, &CFontView::OnTypeIcon)
	ON_COMMAND(ID_TYPE_DETAILS, &CFontView::OnTypeDetails)
	ON_COMMAND(ID_BACKUP_AND_RESTORE, &CFontView::OnBackupAndRestore)
	ON_COMMAND(ID_RESET_DEVICE, &CFontView::OnResetDevice)
	ON_COMMAND(ID_OPTIONS, &CFontView::OnOptions)
	ON_COMMAND(ID_ABOUT, &CFontView::OnAbout)
	ON_UPDATE_COMMAND_UI(ID_TYPE_ICON, &CFontView::OnUpdateTypeIcon)
	ON_UPDATE_COMMAND_UI(ID_TYPE_DETAILS, &CFontView::OnUpdateTypeDetails)
	ON_UPDATE_COMMAND_UI(ID_ANTIALIASING, &CFontView::OnUpdateAntialiasing)
	ON_NOTIFY(NM_DBLCLK, IDC_FONTLIST, &CFontView::OnNMDblclkList)
END_MESSAGE_MAP()

#ifdef _DEBUG
void CFontView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFontView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CFontView::SetListHeader()
{
	CString		strItem;
	LVCOLUMN	lvc;

	lvc.mask	= LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt		= LVCFMT_LEFT;

	strItem.LoadString(IDS_FONTFACE);

	lvc.cx			= DRA::SCALEX(120);
	lvc.pszText		= strItem.GetBuffer(strItem.GetLength());
	lvc.iSubItem	= 0;
	lvc.iOrder		= 0;

	m_ctlFontList.InsertColumn(0, &lvc);
	strItem.ReleaseBuffer();

	strItem.LoadString(IDS_FONTFILE);

	lvc.cx			= DRA::SCALEX(120);
	lvc.pszText		= strItem.GetBuffer(strItem.GetLength());
	lvc.iSubItem	= 1;
	lvc.iOrder		= 1;

	m_ctlFontList.InsertColumn(1, &lvc);
	strItem.ReleaseBuffer();
}

void CFontView::SetViewStyle()
{
	CRegistryX	reg;
	DWORD		dwStyle;

	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);

	dwStyle	= reg.ReadDWORD(RV_FN_VIEWSTYLE, LVS_ICON);

	reg.Close();

	if (dwStyle == LVS_REPORT)
	{
		OnTypeDetails();
	}
	else
	{
		OnTypeIcon();
	}
}

void CFontView::AddFontToList(LPCTSTR lpszExt)
{
	WIN32_FIND_DATA	fd;
	CFontManagerX	FMX;
	HANDLE			hFind;
	CString			strFindPath;
	CString			strFoundFile;
	TCHAR			szFontPath[MAX_PATH + 1];
	int				nType = 0;
	DWORD			i;
	int				j;
	BOOL			bAdded;

	GetFontPath(szFontPath, MAX_PATH + 1);
	CombineFileName(strFindPath.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, szFontPath, lpszExt);
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
				if (FMX.GetFontCount() == 1)
				{
					if (FMX.m_bDigitallySigned == TRUE)
					{
						nType	= 1;
					}
					else
					{
						nType	= 0;
					}
				}
				else
				{
					nType	= 2;
				}

				for (i = 0; i < FMX.GetFontCount(); i++)
				{
					bAdded	= FALSE;

					for (j = 0; j < g_arrFontList.GetCount(); j++)
					{
						if (g_arrFontList.GetAt(j).CompareNoCase(FMX.m_szFontFace[i]) > 0)
						{
							g_arrFontList.InsertAt(j, FMX.m_szFontFace[i]);
							m_ctlFontList.InsertItem(j, FMX.m_szFontFace[i], nType);
							m_ctlFontList.SetItemText(j, 1, strFoundFile);
							m_ctlFontList.SetItemData(j, i);

							bAdded	= TRUE;
							break;
						}
					}

					if (bAdded == FALSE)
					{
						g_arrFontList.Add(FMX.m_szFontFace[i]);
						m_ctlFontList.InsertItem(g_arrFontList.GetUpperBound(), FMX.m_szFontFace[i], nType);
						m_ctlFontList.SetItemText(g_arrFontList.GetUpperBound(), 1, strFoundFile);
						m_ctlFontList.SetItemData(g_arrFontList.GetUpperBound(), i);
					}
				}
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
}

void CFontView::LoadFontList()
{
	CString	strItem;

	m_ctlFontList.DeleteAllItems();
	g_arrFontList.RemoveAll();
	AddFontToList(_T("*.ttf"));
	AddFontToList(_T("*.ttc"));
	AddFontToList(_T("*.ac3"));

	if ((m_ctlFontList.GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
	{
		m_ctlFontList.SetAutoColumnWidth();
	}
}

void CFontView::SetFontLink(LPCTSTR lpszFace, LPCTSTR lpszAddFace, LPCTSTR lpszAddFile)
{
	if ((lpszAddFace != NULL) && (lpszAddFile != NULL))
	{
		CFontManagerX	FMX;

		if (!FMX.LoadFontFile(lpszAddFile))
		{
			return;
		}
	}

	CFontLinkDialog	dFontLink;
	CRegistryMFCX	reg;
	CString			strBase;
	CString			strParsed;
	CString			strFileName;
	CString			strFontFace;
	CString			strFontLink;
	CString			strTitle;
	CString			strMessage;
	int				nPos;
	DWORD			dwFlags;

	dFontLink.SetHeader(lpszFace);
	dFontLink.m_arrFileName.RemoveAll();
	dFontLink.m_arrFontFace.RemoveAll();

	reg.Open(HKEY_LOCAL_MACHINE, SYSLINK_KEY);

	strBase	= reg.ReadString(lpszFace);

	reg.Close();

	while (strBase.GetLength())
	{
		if ((nPos = strBase.Find(_T(';'))) != -1)
		{
			strParsed	= strBase.Left(nPos);
			strBase		= strBase.Mid(nPos + 1);
		}
		else
		{
			strParsed	= strBase;
			strBase		= _T("");
		}

		if (strParsed.GetLength())
		{
			nPos	= strParsed.Find(_T(','));

			dFontLink.m_arrFileName.Add(strParsed.Left(nPos));
			dFontLink.m_arrFontFace.Add(strParsed.Mid(nPos + 1));
		}
	}

	if ((lpszAddFace != NULL) && (lpszAddFile != NULL))
	{
		dFontLink.m_arrFileName.Add(lpszAddFile);
		dFontLink.m_arrFontFace.Add(lpszAddFace);
	}

	reg.Open(HKEY_LOCAL_MACHINE, SKIP_KEY);

	dFontLink.m_strSkipTableText	= reg.ReadString(lpszFace);

	reg.Close();

	if (dFontLink.DoModal() == IDOK)
	{
		if (dFontLink.m_arrFileName.GetCount() < 1)
		{
			reg.Open(HKEY_LOCAL_MACHINE, SYSLINK_KEY);
			reg.DeleteValue(lpszFace);
			reg.Close();
		}
		else
		{
			strBase.Empty();

			for (int i = 0; i < dFontLink.m_arrFileName.GetCount(); i++)
			{
				strFontLink	= dFontLink.m_arrFontFace.GetAt(i);
				strFileName	= dFontLink.m_arrFileName.GetAt(i);

				if (strFontLink.Right(8) == _T(" Regular"))
				{
					strFontFace	= strFontLink.Left(strFontLink.GetLength() - 8);
				}
				else
				{
					strFontFace	= strFontLink;
				}

				strFontLink.Format(_T("%s,%s"), strFileName, strFontFace);

				if (i < (dFontLink.m_arrFileName.GetCount() - 1))
				{
					strFontLink	+= _T(";");
				}

				strBase	+= strFontLink;
			}

			reg.Open(HKEY_LOCAL_MACHINE, SYSLINK_KEY);
			reg.WriteString(lpszFace, strBase);
			reg.Close();
		}

		reg.Open(HKEY_LOCAL_MACHINE, SKIP_KEY);

		if (dFontLink.m_strSkipTableText.GetLength())
		{
			reg.WriteString(lpszFace, dFontLink.m_strSkipTableText);
		}
		else
		{
			reg.DeleteValue(lpszFace);
		}

		reg.Close();

		strTitle.LoadString(IDR_MAINFRAME);
		strMessage.LoadString(IDS_SET_ANTIALIASING);

		if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			SetAntialiasing(TRUE);
		}
		else
		{
			SetAntialiasing(FALSE);
		}

		strMessage.LoadString(IDS_FINISH_RESET);

		if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONINFORMATION) == IDYES)
		{
			reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);
			
			if (reg.ReadDWORD(RV_FN_RESETMETHOD, 0))
			{
				dwFlags	= EWX_DEFER | EWX_PROMPT;
			}
			else
			{
				dwFlags	= 0;
			}

			reg.Close();
			Reset(dwFlags);
		}
	}
}

BOOL CFontView::GetAntialiasing()
{
	BOOL	bForceGRAY16;
	BOOL	bExpansiveAntialias;

	if (GetClearType())
	{
		RegistrySetDWORD(HKEY_CURRENT_USER, RK_FN_ROOT, RV_FN_ANTIALIASING, 1);
		return TRUE;
	}

	if (GetAntialias(bForceGRAY16, bExpansiveAntialias))
	{
		RegistrySetDWORD(HKEY_CURRENT_USER, RK_FN_ROOT, RV_FN_ANTIALIASING, 2);
		return TRUE;
	}

	RegistrySetDWORD(HKEY_CURRENT_USER, RK_FN_ROOT, RV_FN_ANTIALIASING, 0);
	return FALSE;
}

void CFontView::SetAntialiasing(BOOL bSet)
{
	CRegistryX	reg;
	DWORD		dwAntialiasing;
	DWORD		dwApplyLS;

	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);

	dwAntialiasing	= reg.ReadDWORD(RV_FN_ANTIALIASING, 1);
	dwApplyLS		= reg.ReadDWORD(RV_FN_AA_LS, 1);

	if (bSet == FALSE)
	{
		// None
		SetClearTypeOffOnRotation(TRUE);
		SetClearType(FALSE);
		SetAntialias(FALSE, FALSE, FALSE);
		reg.WriteDWORD(RV_FN_ANTIALIASING, 0);
	}
	else
	{
		switch (dwAntialiasing)
		{
		case 2:		// Font Smoothing
			{
				CString	strMessage;
				CString	strTitle;

				SetClearTypeOffOnRotation(TRUE);
				SetClearType(FALSE);
				SetAntialias(TRUE, TRUE, TRUE);
				reg.WriteDWORD(RV_FN_ANTIALIASING, 2);

				strMessage.LoadString(IDS_TAKE_EFFECTS);
				strTitle.LoadString(IDR_MAINFRAME);
				MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
			}
			break;
		default:	// ClearType
			{
				SetAntialias(FALSE, FALSE, FALSE);
				SetClearTypeOffOnRotation(dwApplyLS ? FALSE : TRUE);
				SetClearType(TRUE);
				reg.WriteDWORD(RV_FN_ANTIALIASING, 1);
			}
			break;
		}
	}

	reg.Close();
}

void CFontView::ShowProperties()
{
	int	nItem	= m_ctlFontList.GetNextItem(-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		CPropertiesDialog	dProperties;
		CFontManagerX		FMX;
		CString				strFontFace	= m_ctlFontList.GetItemText(nItem, 0);

		if (FMX.LoadFontFile(m_ctlFontList.GetItemText(nItem, 1)))
		{
			CString	strFilename;

			TakeFileName(m_ctlFontList.GetItemText(nItem, 1), strFilename.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
			strFilename.ReleaseBuffer();

			dProperties.m_strFontFace	= strFontFace;
			dProperties.m_strFileName	= strFilename;
			dProperties.m_strVersion	= FMX.m_szFontVersion[m_ctlFontList.GetItemData(nItem)];
			dProperties.m_strCopyright	= FMX.m_szFontCopyrights[m_ctlFontList.GetItemData(nItem)];
			dProperties.m_nEncodingID	= FMX.m_nEncodingID[m_ctlFontList.GetItemData(nItem)];

			dProperties.SetHeader(strFontFace);
			dProperties.DoModal();
		}
	}
}

LPCTSTR	g_lpszDefaultFF_1033[]	=
{
	_T("Tahoma"),
	_T("Tahoma Bold"),
	_T("Courier New"),
	NULL
};

LPCTSTR	g_lpszDefaultFF_1041[]	=
{
	_T("MS Gothic"),
	_T("MS PGothic"),
	_T("MS UI Gothic"),
	NULL
};

LPCTSTR	g_lpszDefaultFF_1042[]	=
{
	_T("Gulim"),
	NULL
};

LPCTSTR	g_lpszDefaultFF_2052[]	=
{
	_T("NSimSun"),
	NULL
};

LPCTSTR	g_lpszDefaultFF_1028[]	=
{
	_T("PMingLiu"),
	NULL
};

BOOL CFontView::IsEnableDelete()
{
	CRegistryMFCX	reg;
	int				nItem	= m_ctlFontList.GetNextItem(-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		LCID	LocaleID	= GetSystemDefaultLCID();
		WORD	nLCID		= (WORD)(LocaleID & 0x0000FFFF);
		CString	strFontFace;
		CString	strFontLink;
		CString	strValue;
		DWORD	dwType;
		int		i;

		strFontLink	= m_ctlFontList.GetItemText(nItem, 0);

		if (strFontLink.Right(8) == _T(" Regular"))
		{
			strFontFace	= strFontLink.Left(strFontLink.GetLength() - 8);
		}
		else
		{
			strFontFace	= strFontLink;
		}

		switch (nLCID)
		{
		case 0x0411:	// Japanese
			{
				for (i = 0; g_lpszDefaultFF_1033[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_1033[i]))
					{
						return FALSE;
					}
				}

				for (i = 0; g_lpszDefaultFF_1041[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_1041[i]))
					{
						return FALSE;
					}
				}
			}
			break;
		case 0x0412:	// Korean
			{
				for (i = 0; g_lpszDefaultFF_1033[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_1033[i]))
					{
						return FALSE;
					}
				}

				for (i = 0; g_lpszDefaultFF_1042[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_1042[i]))
					{
						return FALSE;
					}
				}
			}
			break;
		case 0x0804:	// Chinese (PRC)
			{
				for (i = 0; g_lpszDefaultFF_1033[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_1033[i]))
					{
						return FALSE;
					}
				}

				for (i = 0; g_lpszDefaultFF_2052[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_2052[i]))
					{
						return FALSE;
					}
				}
			}
			break;
		case 0x0404:	// Chinese (Taiwan)
			{
				for (i = 0; g_lpszDefaultFF_1033[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_1033[i]))
					{
						return FALSE;
					}
				}

				for (i = 0; g_lpszDefaultFF_1028[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_1028[i]))
					{
						return FALSE;
					}
				}
			}
			break;
		default:		// English (U.S.) and other languages
			{
				for (i = 0; g_lpszDefaultFF_1033[i] != NULL; i++)
				{
					if (!strFontFace.CompareNoCase(g_lpszDefaultFF_1033[i]))
					{
						return FALSE;
					}
				}
			}
			break;
		}

		reg.Open(HKEY_LOCAL_MACHINE, SYSLINK_KEY);

		if (reg.GetFirstValue(strValue, &dwType))
		{
			do
			{
				strFontLink	= reg.ReadString(strValue);

				if (strFontLink.Find(strFontFace) != -1)
				{
					reg.Close();
					return FALSE;
				}
			} while (reg.GetNextValue(strValue, &dwType));
		}

		reg.Close();
		return TRUE;
	}

	return FALSE;
}

void CFontView::CreateStartup()
{
	CString			strPath;
	TCHAR			szStartup[MAX_PATH + 1];
	TCHAR			szModule[MAX_PATH + 1];
	TCHAR			szShortcut[MAX_PATH + 1];
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	strPath	= reg.ReadString(RV_IS_FOLDER);

	reg.Close();

	GetSpecialDirectoryEx(CSIDL_STARTUP, szStartup, MAX_PATH + 1);
	CombineFileName(szModule, MAX_PATH + 1, strPath, MD_FN_MAIN);
	CombineFileName(szShortcut, MAX_PATH + 1, szStartup, MD_FN_SHORTCUT);
	strPath.Format(_T("\"%s\" -C"), szModule);
	_tcscpy_s(szModule, MAX_PATH + 1, strPath);
	DeleteFile(szShortcut);
	SHCreateShortcut(szShortcut, szModule);
}

void CFontView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	UINT	uiSmall	= 0;
	UINT	uiLarge	= 0;
	DWORD	dwDPI	= GetDPI();

	switch (dwDPI)
	{
	case DPI_192:
		{
			uiSmall	= IDB_FONTICONS32;
			uiLarge	= IDB_FONTICONS64;
		}
		break;
	case DPI_128:
		{
			uiSmall	= IDB_FONTICONS21;
			uiLarge	= IDB_FONTICONS43;
		}
		break;
	default:
		{
			uiSmall	= IDB_FONTICONS16;
			uiLarge	= IDB_FONTICONS32;
		}
		break;
	}

	m_iSmallImages.Create(uiSmall, GetSystemMetrics(SM_CXSMICON), 3, RGB(0xFA, 0xFA, 0xFA));
	m_iLargeImages.Create(uiLarge, GetSystemMetrics(SM_CXICON), 3, RGB(0xFA, 0xFA, 0xFA));
	m_ctlFontList.SetImageList(&m_iSmallImages, LVSIL_SMALL);
	m_ctlFontList.SetImageList(&m_iLargeImages, LVSIL_NORMAL);
	SetListHeader();
	SetViewStyle();
	LoadFontList();
}

void CFontView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	SIPINFO	SipInfo;
	int		nWidth	= GetSystemMetrics(SM_CXSCREEN);
	int		nHeight	= GetSystemMetrics(SM_CYSCREEN) - DRA::SCALEY(52);

	SipInfo.cbSize			= sizeof(SIPINFO);
	SipInfo.dwImDataSize	= 0;

	if (SipGetInfo(&SipInfo))
	{
		if (SipInfo.fdwFlags & (SIPF_LOCKED | SIPF_ON))
		{
			nHeight	-= SipInfo.rcSipRect.bottom - SipInfo.rcSipRect.top;
		}
	}

	if ((cx == nWidth) && (cy == nHeight))
	{
		CRect rcList;

		rcList.left		= DRA::SCALEX(0);
		rcList.top		= DRA::SCALEY(0);
		rcList.right	= cx;
		rcList.bottom	= cy;

		if (m_ctlFontList.GetSafeHwnd() != NULL)
		{
			m_ctlFontList.MoveWindow(rcList);

			if ((m_ctlFontList.GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
			{
				m_ctlFontList.SetAutoColumnWidth();
			}
		}
	}
}

void CFontView::OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	int	nItem	= m_ctlFontList.GetNextItem(-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		ShowProperties();
	}

	*pResult = 0;
}

BOOL CFontView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style	&= ~WS_BORDER;

	return CFormView::PreCreateWindow(cs);
}

LRESULT CFontView::OnTapAndHoldList(WPARAM wParam, LPARAM lParam)
{
	int				nItem		= (int)wParam;
	CMenu			ctlMenu;
	CString			strMenu;
	UINT			uFlags		= ((m_ctlFontList.GetNextItem(-1, LVNI_SELECTED) != -1) ? MF_ENABLED : MF_GRAYED);
	UINT			uLinkFlags	= MF_GRAYED;
	CFontManagerX	FMX;

	if (nItem != -1)
	{
		if (FMX.LoadFontFile(m_ctlFontList.GetItemText(nItem, 1)))
		{
			if (FMX.m_nEncodingID[m_ctlFontList.GetItemData(nItem)] == 1)
			{
				uLinkFlags	= MF_ENABLED;
			}
		}
	}

	ctlMenu.CreatePopupMenu();
	strMenu.LoadString(IDS_INSTALL_NEW_FONT);
	ctlMenu.AppendMenu(MF_ENABLED, ID_INSTALL_NEW_FONT, strMenu);
	ctlMenu.AppendMenu(MF_SEPARATOR);
	strMenu.LoadString(IDS_ADD_DEFAULT);
	ctlMenu.AppendMenu(uLinkFlags, ID_ADD_DEFAULT, strMenu);
	strMenu.LoadString(IDS_ADD_DEFAULT_FIXED);
	ctlMenu.AppendMenu(uLinkFlags, ID_ADD_DEFAULT_FIXED, strMenu);
	ctlMenu.AppendMenu(MF_SEPARATOR);
	strMenu.LoadString(IDS_LINK_TO);
	ctlMenu.AppendMenu(uLinkFlags, ID_LINK_TO, strMenu);
	strMenu.LoadString(IDS_DELETE);
	ctlMenu.AppendMenu((IsEnableDelete() ? MF_ENABLED : MF_GRAYED), ID_DELETE, strMenu);
	ctlMenu.AppendMenu(MF_SEPARATOR);
	strMenu.LoadString(IDS_PROPERTIES);
	ctlMenu.AppendMenu(uFlags, ID_PROPERTIES, strMenu);
	ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	return 0;
}

void CFontView::OnInstallNewFont()
{
	CFileDialogX	dFD;
	CCopyDialogX	dCD;
	CFontManagerX	FMX;

	dFD.SetFilters(_T("*.ttf|*.ttc|*.ac3"));
	dFD.SetHeader(IDS_INSTALL_SELECT);

	if (dFD.DoModal() == IDOK)
	{
		if (FMX.LoadFontFile(dFD.GetFullPath()))
		{
			CInstallDialog	dInstall;

			dInstall.SetHeader(IDS_INSTALL_TITLE);

			dInstall.m_strFileName	= dFD.GetFile();
			dInstall.m_strFontFace	= FMX.m_szFontFace[0];

			for (DWORD i = 1; i < FMX.GetFontCount(); i++)
			{
				dInstall.m_strFontFace	+= _T(" && ");
				dInstall.m_strFontFace	+= FMX.m_szFontFace[i];
			}

			dInstall.m_strVersion	= FMX.m_szFontVersion[0];
			dInstall.m_strCopyright	= FMX.m_szFontCopyrights[0];

			if (dInstall.DoModal() == IDOK)
			{
				TCHAR	szFontPath[MAX_PATH + 1];
				CString	strTarget;

				GetFontPath(szFontPath, MAX_PATH + 1);
				dCD.AddFile(dFD.GetFullPath());
				dCD.SetSource(dFD.GetPath());
				dCD.SetDestination(szFontPath);
				dCD.DoModal();
				CombineFileName(strTarget.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, szFontPath, dFD.GetFile());
				strTarget.ReleaseBuffer();
				AddFontResource(strTarget);
				::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
				LoadFontList();
				EndWaitCursor();
			}
		}
	}
}

void CFontView::OnAddDefault()
{
	int	nItem	= m_ctlFontList.GetNextItem(-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		SetFontLink(SYSLINK_DEFAULT, m_ctlFontList.GetItemText(nItem, 0), m_ctlFontList.GetItemText(nItem, 1));
	}
}

void CFontView::OnAddDefaultFixed()
{
	int	nItem	= m_ctlFontList.GetNextItem(-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		SetFontLink(SYSLINK_FIXED, m_ctlFontList.GetItemText(nItem, 0), m_ctlFontList.GetItemText(nItem, 1));
	}
}

void CFontView::OnLinkTo()
{
	int	nItem	= m_ctlFontList.GetNextItem(-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		CFontSelectDialog	dFontSelect;

		dFontSelect.m_bLinkOnly	= TRUE;

		dFontSelect.SetHeader(IDS_FONTSELECT_TITLE);

		if (dFontSelect.DoModal() == IDOK)
		{
			SetFontLink(dFontSelect.m_arrFontFace.GetAt(dFontSelect.m_nSelected), m_ctlFontList.GetItemText(nItem, 0), m_ctlFontList.GetItemText(nItem, 1));
		}
	}
}

void CFontView::OnDelete()
{
	int	nItem	= m_ctlFontList.GetNextItem(-1, LVNI_SELECTED);

	if (nItem != -1)
	{
		CString	strSource	= m_ctlFontList.GetItemText(nItem, 1);
		CString	strTarget	= strSource + _T(".mdf");
		CString	strTitle;
		CString	strMessage;

		RemoveFontResource(strSource);
		::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
		MoveFile(strSource, strTarget);
		CreateStartup();
		strTitle.LoadString(IDS_NOTICE);
		strMessage.LoadString(IDS_DELETE_RESTART);
		MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
		LoadFontList();
	}
}

void CFontView::OnProperties()
{
	ShowProperties();
}

void CFontView::OnFontsDefault()
{
	SetFontLink(SYSLINK_DEFAULT);
}

void CFontView::OnFontsDefaultFixed()
{
	SetFontLink(SYSLINK_FIXED);
}

void CFontView::OnFontsLinking()
{
	CFontSelectDialog	dFontSelect;

	dFontSelect.m_bLinkOnly	= TRUE;

	dFontSelect.SetHeader(IDS_FONTSELECT_TITLE);

	if (dFontSelect.DoModal() == IDOK)
	{
		SetFontLink(dFontSelect.m_arrFontFace.GetAt(dFontSelect.m_nSelected));
	}
}

void CFontView::OnFontsSystem()
{
	CComponentsDialog	dComponents;

	dComponents.SetHeader(IDS_COMPONENTS_TITLE);
	dComponents.DoModal();
}

void CFontView::OnAntialiasing()
{
	if (GetAntialiasing())
	{
		SetAntialiasing(FALSE);
	}
	else
	{
		SetAntialiasing(TRUE);
	}
}

void CFontView::OnTypeIcon()
{
	DWORD			dwStyle;
	CRegistryMFCX	reg;

	dwStyle	= m_ctlFontList.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT);

	m_ctlFontList.SetExtendedStyle(dwStyle);
	m_ctlFontList.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);
	reg.WriteDWORD(RV_FN_VIEWSTYLE, LVS_ICON);
	reg.Close();
}

void CFontView::OnTypeDetails()
{
	DWORD			dwStyle;
	CRegistryMFCX	reg;

	dwStyle	= m_ctlFontList.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT);
	dwStyle	|= LVS_EX_FULLROWSELECT;

	m_ctlFontList.SetExtendedStyle(dwStyle);
	m_ctlFontList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_ctlFontList.SetAutoColumnWidth();
	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);
	reg.WriteDWORD(RV_FN_VIEWSTYLE, LVS_REPORT);
	reg.Close();
}

void CFontView::OnBackupAndRestore()
{
	CBackupDialog	dBackup;

	dBackup.SetHeader(IDS_BACKUP_TITLE);
	dBackup.DoModal();
}

void CFontView::OnResetDevice()
{
	CRegistryX	reg;
	CString		strTitle;
	CString		strMessage;
	DWORD		dwFlags;

	strTitle.LoadString(IDR_MAINFRAME);
	strMessage.LoadString(IDS_RESET_NOTICE);

	if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);
		
		if (reg.ReadDWORD(RV_FN_RESETMETHOD, 0))
		{
			dwFlags	= EWX_DEFER | EWX_PROMPT;
		}
		else
		{
			dwFlags	= 0;
		}

		reg.Close();
		Reset(dwFlags);
	}
}

void CFontView::OnOptions()
{
	COptionsSheet	Sheet(IDR_MAINFRAME);
	CString			strMessage;
	CString			strTitle;

	Sheet.SetTitle(IDS_OPTIONS);
	Sheet.DoModal();
	LoadFontList();

	strMessage.LoadString(IDS_TAKE_EFFECTS);
	strTitle.LoadString(IDR_MAINFRAME);
	MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
}

void CFontView::OnAbout()
{
	CAboutDialog	dAbout;

	dAbout.DoModal();
}

void CFontView::OnUpdateTypeIcon(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)((m_ctlFontList.GetStyle() & LVS_TYPEMASK) == LVS_ICON));
}

void CFontView::OnUpdateTypeDetails(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)((m_ctlFontList.GetStyle() & LVS_TYPEMASK) == LVS_REPORT));
}

void CFontView::OnUpdateAntialiasing(CCmdUI* pCmdUI)
{
	DWORD			dwAntialiasing;
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_FN_ROOT);

	dwAntialiasing	= reg.ReadDWORD(RV_FN_ANTIALIASING, 1);

	reg.Close();

	pCmdUI->SetCheck(GetAntialiasing());
	pCmdUI->Enable(dwAntialiasing != 2);
}
