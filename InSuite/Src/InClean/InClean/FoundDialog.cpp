// FoundDialog.cpp

#include "stdafx.h"
#include "InClean.h"
#include "FoundDialog.h"
#include "FoundItems.h"
#include "RegistryMFCX.h"

IMPLEMENT_DYNAMIC(CFoundDialog, COptionsDialogX)

CFoundDialog::CFoundDialog(CWnd* pParent /*=NULL*/) : COptionsDialogX(CFoundDialog::IDD, pParent)
{
}

CFoundDialog::~CFoundDialog()
{
}

void CFoundDialog::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FD_ITEMS, m_ctlItems);
	DDX_Control(pDX, IDC_FD_SELECT_ALL, m_ctlSelectAll);
	DDX_Control(pDX, IDC_FD_UNSELECT_ALL, m_ctlUnselectAll);
}

BEGIN_MESSAGE_MAP(CFoundDialog, COptionsDialogX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FD_SELECT_ALL, &CFoundDialog::OnBnClickedSelectAll)
	ON_BN_CLICKED(IDC_FD_UNSELECT_ALL, &CFoundDialog::OnBnClickedUnselectAll)
	ON_COMMAND(ID_CLEAN, &CFoundDialog::OnClean)
END_MESSAGE_MAP()

void CFoundDialog::SetHeaders()
{
	CString	strItem;

	strItem.LoadString(IDS_HEADER_LOCATION);
	m_ctlItems.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(100), 0);
	strItem.LoadString(IDS_HEADER_TYPE);
	m_ctlItems.InsertColumn(1, strItem, LVCFMT_LEFT, DRA::SCALEX(100), 1);
}

void CFoundDialog::SetItems()
{
	LVITEM	lvi;
	DWORD	dwStyle;
	CString	strItem;

	BeginWaitCursor();

	dwStyle	= m_ctlItems.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT | LVS_EX_CHECKBOXES);
	dwStyle	|= LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES;

	m_ctlItems.SetExtendedStyle(dwStyle);
	m_ctlItems.SetRedraw(FALSE);

	if (m_ctlItems.GetItemCount() > 0)
	{
		m_ctlItems.DeleteAllItems();
	}

	for (int i = 0; i < g_arrFoundItems.GetCount(); i++)
	{
		switch (g_arrFoundItems.GetAt(i).nType)
		{
		case FI_TEMP_FILE:
		case FI_TEMP_FOLDER:
		case FI_ASYNC_FILE:
		case FI_ASYNC_FOLDER:
		case FI_INTERNET_FILE:
		case FI_INTERNET_FOLDER:
		case FI_COOKIE_FILE:
		case FI_COOKIE_FOLDER:
		case FI_HISTORY_FILE:
		case FI_HISTORY_FOLDER:
		case FI_ERRREP_FILE:
		case FI_ERRREP_FOLDER:
		case FI_LOG_FILE:
		case FI_ZEROSIZE_FILE:
		case FI_INVALID_SHORTCUT:
		case FI_THUMBS_DB:
			{
				strItem.Format(_T("%s"), g_arrFoundItems.GetAt(i).szName);
			}
			break;
		case FI_NOTI_QUEUE:
		case FI_NOTI_EVENT:
			{
				strItem.Format(_T("%s"), g_arrFoundItems.GetAt(i).szName);
			}
			break;
		case FI_OBSOLETE_KEY:
		case FI_MISSING_ICON:
		case FI_MISSING_INPROC32:
		case FI_MISSING_APPIDREF:
		case FI_MISSING_PROGID:
		case FI_MISSING_CLSID:
		case FI_UNUSED_CLASS:
			{
				if (g_arrFoundItems.GetAt(i).hKey == HKEY_CLASSES_ROOT)
				{
					strItem.Format(_T("HKEY_CLASSES_ROOT\\%s"), g_arrFoundItems.GetAt(i).szName);
				}
				else if (g_arrFoundItems.GetAt(i).hKey == HKEY_CURRENT_USER)
				{
					strItem.Format(_T("HKEY_CURRENT_USER\\%s"), g_arrFoundItems.GetAt(i).szName);
				}
				else if (g_arrFoundItems.GetAt(i).hKey == HKEY_LOCAL_MACHINE)
				{
					strItem.Format(_T("HKEY_LOCAL_MACHINE\\%s"), g_arrFoundItems.GetAt(i).szName);
				}
				else if (g_arrFoundItems.GetAt(i).hKey == HKEY_USERS)
				{
					strItem.Format(_T("HKEY_USERS\\%s"), g_arrFoundItems.GetAt(i).szName);
				}
			}
			break;
		case FI_MISSING_APPID:
			{
				if (g_arrFoundItems.GetAt(i).hKey == HKEY_CLASSES_ROOT)
				{
					strItem.Format(_T("HKEY_CLASSES_ROOT\\%s\\%s"), g_arrFoundItems.GetAt(i).szName, g_arrFoundItems.GetAt(i).szValue);
				}
				else if (g_arrFoundItems.GetAt(i).hKey == HKEY_CURRENT_USER)
				{
					strItem.Format(_T("HKEY_CURRENT_USER\\%s\\%s"), g_arrFoundItems.GetAt(i).szName, g_arrFoundItems.GetAt(i).szValue);
				}
				else if (g_arrFoundItems.GetAt(i).hKey == HKEY_LOCAL_MACHINE)
				{
					strItem.Format(_T("HKEY_LOCAL_MACHINE\\%s\\%s"), g_arrFoundItems.GetAt(i).szName, g_arrFoundItems.GetAt(i).szValue);
				}
				else if (g_arrFoundItems.GetAt(i).hKey == HKEY_USERS)
				{
					strItem.Format(_T("HKEY_USERS\\%s\\%s"), g_arrFoundItems.GetAt(i).szName, g_arrFoundItems.GetAt(i).szValue);
				}
			}
			break;
		}

		lvi.mask		= LVIF_TEXT | LVIF_PARAM;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.lParam		= i;
		lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

		m_ctlItems.InsertItem(&lvi);
		strItem.ReleaseBuffer();

		switch (g_arrFoundItems.GetAt(i).nType)
		{
		case FI_TEMP_FILE:
		case FI_TEMP_FOLDER:
			{
				strItem.LoadString(IDS_FD_TEMP);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_ASYNC_FILE:
		case FI_ASYNC_FOLDER:
			{
				strItem.LoadString(IDS_FD_ASYNC);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_INTERNET_FILE:
		case FI_INTERNET_FOLDER:
			{
				strItem.LoadString(IDS_FD_INET_TEMP);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_COOKIE_FILE:
		case FI_COOKIE_FOLDER:
			{
				strItem.LoadString(IDS_FD_INET_COOKIE);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_HISTORY_FILE:
		case FI_HISTORY_FOLDER:
			{
				strItem.LoadString(IDS_FD_INET_HISTORY);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_ERRREP_FILE:
		case FI_ERRREP_FOLDER:
			{
				strItem.LoadString(IDS_FD_ERRREP);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_LOG_FILE:
			{
				strItem.LoadString(IDS_FD_LOG);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_ZEROSIZE_FILE:
			{
				strItem.LoadString(IDS_FD_ZEROSIZE);
			}
			break;
		case FI_INVALID_SHORTCUT:
			{
				strItem.LoadString(IDS_FD_INVALID_SHORTCUT);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_THUMBS_DB:
			{
				strItem.LoadString(IDS_FD_THUMBS_DB);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_NOTI_QUEUE:
		case FI_NOTI_EVENT:
			{
				strItem.LoadString(IDS_FD_NOTIFICATION);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_OBSOLETE_KEY:
			{
				strItem.LoadString(IDS_FD_OBSOLETE_KEY);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_MISSING_APPID:
			{
				strItem.LoadString(IDS_FD_MISSING_APPID);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_MISSING_ICON:
			{
				strItem.LoadString(IDS_FD_MISSING_ICON);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_MISSING_INPROC32:
			{
				strItem.LoadString(IDS_FD_MISSING_INPROC32);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_MISSING_APPIDREF:
			{
				strItem.LoadString(IDS_FD_MISSING_APPIDREF);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_MISSING_PROGID:
			{
				strItem.LoadString(IDS_FD_MISSING_PROGID);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_MISSING_CLSID:
			{
				strItem.LoadString(IDS_FD_MISSING_CLSID);
				m_ctlItems.SetCheck(i);
			}
			break;
		case FI_UNUSED_CLASS:
			{
				strItem.LoadString(IDS_FD_UNUSED_CLASS);
				m_ctlItems.SetCheck(i);
			}
			break;
		}

		m_ctlItems.SetItemText(i, 1, strItem);
	}

	m_ctlItems.SetRedraw(TRUE);
	EndWaitCursor();
}

BOOL CFoundDialog::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_CLEAN);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= ID_CLEAN;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_CLOSE);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	SetHeaders();
	SetItems();
	return TRUE;
}

void CFoundDialog::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	SIZE	size;

	m_ctlUnselectAll.GetWindowRect(&rc);

	size.cx	= rc.right - rc.left;
	size.cy	= rc.bottom - rc.top;

	rc.right	= cx - DRA::SCALEX(2);
	rc.bottom	= cy - DRA::SCALEY(2);
	rc.left		= rc.right - size.cx;
	rc.top		= rc.bottom - size.cy;

	m_ctlUnselectAll.MoveWindow(&rc);

	rc.right	= rc.left - DRA::SCALEX(2);
	rc.left		= rc.right - size.cx;

	m_ctlSelectAll.MoveWindow(&rc);

	m_ctlItems.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.left		= DRA::SCALEX(2);
	rc.top		= GetHeaderHeight() + DRA::SCALEY(2);
	rc.right	= cx - DRA::SCALEX(2);
	rc.bottom	= cy - size.cy - DRA::SCALEY(4);

	m_ctlItems.MoveWindow(&rc);
	m_ctlItems.SetAutoColumnWidth();
}

void CFoundDialog::OnBnClickedSelectAll()
{
	for (int i = 0; i < m_ctlItems.GetItemCount(); i++)
	{
		m_ctlItems.SetCheck(i, TRUE);
	}
}

void CFoundDialog::OnBnClickedUnselectAll()
{
	for (int i = 0; i < m_ctlItems.GetItemCount(); i++)
	{
		m_ctlItems.SetCheck(i, FALSE);
	}
}

void CFoundDialog::OnClean()
{
	int		nCount	= 0;
	int		i;
	CString	strMessage;
	CString	strTitle;

	for (i = 0; i < m_ctlItems.GetItemCount(); i++)
	{
		if (m_ctlItems.GetCheck(i))
		{
			nCount++;
		}
	}

	if (!nCount)
	{
		OnOK();
	}

	strMessage.LoadString(IDS_FD_WARNING_MESSAGE);
	strTitle.LoadString(IDS_INCLEAN);

	if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		Clean();
		OnOK();
	}
}

#pragma warning(disable:4995)

void CFoundDialog::Clean()
{
	CRegistryMFCX	reg;
	int				i;
	DWORD			dwCount;
	ULARGE_INTEGER	uiTotalSize;
	CString			strMessage;
	CString			strTitle;

	dwCount					= 0;
	uiTotalSize.QuadPart	= 0;

	BeginWaitCursor();

	for (i = 0; i < g_arrFoundItems.GetCount(); i++)
	{
		if (m_ctlItems.GetCheck(i))
		{
			switch (g_arrFoundItems.GetAt(i).nType)
			{
			case FI_TEMP_FILE:
			case FI_ASYNC_FILE:
			case FI_INTERNET_FILE:
			case FI_COOKIE_FILE:
			case FI_HISTORY_FILE:
			case FI_ERRREP_FILE:
			case FI_LOG_FILE:
			case FI_ZEROSIZE_FILE:
			case FI_INVALID_SHORTCUT:
			case FI_THUMBS_DB:
				{

					SetFileAttributes(g_arrFoundItems.GetAt(i).szName, FILE_ATTRIBUTE_NORMAL);

					if (DeleteFile(g_arrFoundItems.GetAt(i).szName))
					{
						dwCount++;
						uiTotalSize.QuadPart	+= g_arrFoundItems.GetAt(i).uiSize.QuadPart;
					}
				}
				break;
			case FI_TEMP_FOLDER:
			case FI_ASYNC_FOLDER:
			case FI_INTERNET_FOLDER:
			case FI_COOKIE_FOLDER:
			case FI_HISTORY_FOLDER:
			case FI_ERRREP_FOLDER:
				{
					if (RemoveDirectory(g_arrFoundItems.GetAt(i).szName))
					{
						dwCount++;
					}
				}
				break;
			case FI_NOTI_QUEUE:
				{
					HANDLE	hDatabase;
					CEGUID	ceguid;
					CEOID	ceoid;
					TCHAR	szVolume[MAX_PATH];
					TCHAR	szDatabase[MAX_PATH];

					_tcscpy_s(szVolume, MAX_PATH, _T("\\mxip_notify.vol"));

					if (CeMountDBVol(&ceguid, szVolume, OPEN_EXISTING))
					{
						_tcscpy_s(szDatabase, MAX_PATH, _T("DB_notify_queue"));

						ceoid		= 0;
						hDatabase	= CeOpenDatabaseEx2(&ceguid, &ceoid, szDatabase, NULL, 0, NULL);

						if (hDatabase != INVALID_HANDLE_VALUE)
						{
							if (CeDeleteRecord(hDatabase, g_arrFoundItems.GetAt(i).oidRec))
							{
								dwCount++;
							}

							CloseHandle(hDatabase);
						}

						CeUnmountDBVol(&ceguid);
					}
				}
				break;
			case FI_NOTI_EVENT:
				{
					HANDLE	hDatabase;
					CEGUID	ceguid;
					CEOID	ceoid;
					TCHAR	szVolume[MAX_PATH];
					TCHAR	szDatabase[MAX_PATH];

					_tcscpy_s(szVolume, MAX_PATH, _T("\\mxip_notify.vol"));

					if (CeMountDBVol(&ceguid, szVolume, OPEN_EXISTING))
					{
						_tcscpy_s(szDatabase, MAX_PATH, _T("DB_notify_events"));

						ceoid		= 0;
						hDatabase	= CeOpenDatabaseEx2(&ceguid, &ceoid, szDatabase, NULL, 0, NULL);

						if (hDatabase != INVALID_HANDLE_VALUE)
						{
							if (CeDeleteRecord(hDatabase, g_arrFoundItems.GetAt(i).oidRec))
							{
								dwCount++;
							}

							CloseHandle(hDatabase);
						}

						CeUnmountDBVol(&ceguid);
					}
				}
				break;
			case FI_OBSOLETE_KEY:
			case FI_MISSING_ICON:
			case FI_MISSING_INPROC32:
			case FI_MISSING_APPIDREF:
			case FI_MISSING_PROGID:
			case FI_MISSING_CLSID:
			case FI_UNUSED_CLASS:
				{
					if (reg.DeleteKey(g_arrFoundItems.GetAt(i).hKey, g_arrFoundItems.GetAt(i).szName))
					{
						dwCount++;
					}
				}
				break;
			case FI_MISSING_APPID:
				{
					reg.Open(g_arrFoundItems.GetAt(i).hKey, g_arrFoundItems.GetAt(i).szName);

					if (reg.DeleteValue(g_arrFoundItems.GetAt(i).szValue))
					{
						dwCount++;
					}

					reg.Close();
				}
				break;
			}
		}
	}

	EndWaitCursor();
	g_arrFoundItems.RemoveAll();

	if ((dwCount == 1) && (uiTotalSize.QuadPart == 1))
	{
		strMessage.Format(IDS_FD_RESULT_FORMAT0, dwCount, uiTotalSize.QuadPart);
	}
	else if ((dwCount == 1) && (uiTotalSize.QuadPart > 1))
	{
		strMessage.Format(IDS_FD_RESULT_FORMAT1, dwCount, uiTotalSize.QuadPart);
	}
	else if ((dwCount > 1) && (uiTotalSize.QuadPart == 1))
	{
		strMessage.Format(IDS_FD_RESULT_FORMAT2, dwCount, uiTotalSize.QuadPart);
	}
	else if ((dwCount > 1) && (uiTotalSize.QuadPart > 1))
	{
		strMessage.Format(IDS_FD_RESULT_FORMAT3, dwCount, uiTotalSize.QuadPart);
	}
	else if ((dwCount == 1) && (uiTotalSize.QuadPart == 0))
	{
		strMessage.Format(IDS_FD_RESULT_FORMAT4, dwCount);
	}
	else
	{
		strMessage.Format(IDS_FD_RESULT_FORMAT5, dwCount);
	}

	strTitle.Format(IDS_INCLEAN);
	MessageBox(strMessage, strTitle, MB_ICONINFORMATION | MB_OK);
}

#pragma warning(default:4995)
