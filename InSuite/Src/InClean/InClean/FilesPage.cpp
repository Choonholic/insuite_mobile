// FilesPage.cpp

#include "stdafx.h"
#include "InClean.h"
#include "FilesPage.h"
#include "CleanSearch.h"
#include "ShortcutSearch.h"
#include "FoundDialog.h"
#include "FoundItems.h"
#include "ShellX.h"

#pragma warning(disable:4819)
#include "regext.h"
#pragma warning(default:4819)

#ifndef FL_MAX_ITEMS
#define FL_MAX_ITEMS			11
#define FL_TEMP					0
#define FL_ACTIVESYNC			1
#define FL_INET_TEMP			2
#define FL_INET_COOKIES			3
#define FL_INET_HISTORY			4
#define FL_ERROR_REPORTS		5
#define FL_LOG_FILES			6
#define FL_ZEROSIZE_FILES		7
#define FL_INVALID_SHORTCUTS	8
#define FL_THUMBS_DB			9
#define FL_NOTIFICATIONS		10
#endif

IMPLEMENT_DYNAMIC(CFilesPage, CPropertyPageX)

CFilesPage::CFilesPage() : CPropertyPageX(CFilesPage::IDD)
{
}

CFilesPage::~CFilesPage()
{
}

void CFilesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FL_ITEMS, m_ctlItems);
	DDX_Control(pDX, IDC_FL_SELECT_ALL, m_ctlSelectAll);
	DDX_Control(pDX, IDC_FL_UNSELECT_ALL, m_ctlUnselectAll);
}

BEGIN_MESSAGE_MAP(CFilesPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_FL_SELECT_ALL, &CFilesPage::OnBnClickedSelectAll)
	ON_BN_CLICKED(IDC_FL_UNSELECT_ALL, &CFilesPage::OnBnClickedUnselectAll)
END_MESSAGE_MAP()

void CFilesPage::SetHeaders()
{
	CString	strItem;

	strItem.LoadString(IDS_HEADER_ITEM);
	m_ctlItems.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(200), 0);
}

void CFilesPage::SetItems()
{
	LVITEM	lvi;
	DWORD	dwStyle;
	CString	strItem;

	BeginWaitCursor();

	dwStyle	= m_ctlItems.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT | LVS_EX_CHECKBOXES);
	dwStyle	|= LVS_EX_CHECKBOXES;

	m_ctlItems.SetExtendedStyle(dwStyle);
	m_ctlItems.SetRedraw(FALSE);

	if (m_ctlItems.GetItemCount() > 0)
	{
		m_ctlItems.DeleteAllItems();
	}

	for (int i = 0; i < FL_MAX_ITEMS; i++)
	{
		strItem.LoadString(IDS_FL_TEMP + i);

		lvi.mask		= LVIF_TEXT | LVIF_PARAM;
		lvi.iItem		= i;
		lvi.iSubItem	= 0;
		lvi.lParam		= i;
		lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

		m_ctlItems.InsertItem(&lvi);
		strItem.ReleaseBuffer();
		m_ctlItems.SetCheck(i, TRUE);
	}

	m_ctlItems.SetRedraw(TRUE);
	EndWaitCursor();
}

void CFilesPage::Scan()
{
	CFoundDialog	dFound;
	BOOL			bScanned	= FALSE;

	BeginWaitCursor();
	g_arrFoundItems.RemoveAll();

	for (int i = 0; i < FL_MAX_ITEMS; i++)
	{
		if (m_ctlItems.GetCheck(i))
		{
			switch (i)
			{
			case FL_TEMP:
				{
					ScanTemporaryFiles();

					bScanned	= TRUE;
				}
				break;
			case FL_ACTIVESYNC:
				{
					ScanActiveSync();

					bScanned	= TRUE;
				}
				break;
			case FL_INET_TEMP:
				{
					ScanTemporaryInternetFiles();

					bScanned	= TRUE;
				}
				break;
			case FL_INET_COOKIES:
				{
					ScanInternetCookies();

					bScanned	= TRUE;
				}
				break;
			case FL_INET_HISTORY:
				{
					ScanInternetHistory();

					bScanned	= TRUE;
				}
				break;
			case FL_ERROR_REPORTS:
				{
					ScanErrorReports();

					bScanned	= TRUE;
				}
				break;
			case FL_LOG_FILES:
				{
					ScanLogFiles();

					bScanned	= TRUE;
				}
				break;
			case FL_ZEROSIZE_FILES:
				{
					ScanZeroSizeFiles();

					bScanned	= TRUE;
				}
				break;
			case FL_INVALID_SHORTCUTS:
				{
					ScanInvalidShortcuts();

					bScanned	= TRUE;
				}
				break;
			case FL_THUMBS_DB:
				{
					ScanThumbnailCaches();

					bScanned	= TRUE;
				}
				break;
			case FL_NOTIFICATIONS:
				{
					ScanNotifications();

					bScanned	= TRUE;
				}
				break;
			}
		}
	}

	EndWaitCursor();

	if (g_arrFoundItems.GetCount())
	{
		dFound.SetHeader(IDS_FD_TITLE);
		dFound.DoModal();
	}
	else
	{
		if (bScanned == TRUE)
		{
			CString	strMessage;
			CString	strTitle;

			strMessage.LoadString(IDS_NOITEM);
			strTitle.LoadString(IDS_INCLEAN);
			MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
		}
	}
}

void CFilesPage::ScanTemporaryFiles()
{
	FOUND_ITEM		tFound;
	int				i;
	TCHAR			szWindows[MAX_PATH + 1];
	CString			strFolder;
	CCleanSearch	Search;

	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	strFolder.Format(_T("%s\\Temp\\*.*"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();
	strFolder.Format(_T("%s\\Cache\\*.*"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();

	strFolder	= _T("\\Temp\\*.*");

	Search.SetPartName(strFolder);
	Search.DoSearch();

	strFolder	= _T("\\Application Data\\Volatile\\*.*");

	Search.SetPartName(strFolder);
	Search.DoSearch();

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_TEMP_FILE;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));

		g_arrFoundItems.Add(tFound);
	}

	for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType	= FI_TEMP_FOLDER;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFolders[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanActiveSync()
{
	FOUND_ITEM		tFound;
	int				i;
	TCHAR			szWindows[MAX_PATH + 1];
	CString			strFolder;
	CCleanSearch	Search;

	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	strFolder.Format(_T("%s\\Activesync\\*.txt"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_ASYNC_FILE;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}

	for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType	= FI_ASYNC_FOLDER;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFolders[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanTemporaryInternetFiles()
{
	FOUND_ITEM		tFound;
	int				i;
	TCHAR			szWindows[MAX_PATH + 1];
	CString			strFolder;
	CCleanSearch	Search;

	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	strFolder.Format(_T("%s\\Temporary Internet Files\\*.*"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();

	RegistryGetString(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("Cache"), szWindows, MAX_PATH + 1);

	strFolder.Format(_T("%s\\desktop.ini"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();

	strFolder.Format(_T("%s\\Content.IE5\\*.*"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_INTERNET_FILE;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}

	for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType	= FI_INTERNET_FOLDER;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFolders[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanInternetCookies()
{
	FOUND_ITEM		tFound;
	int				i;
	TCHAR			szWindows[MAX_PATH + 1];
	CString			strFolder;
	CCleanSearch	Search;

	RegistryGetString(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("Cookies"), szWindows, MAX_PATH + 1);

	strFolder.Format(_T("%s\\*.*"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_COOKIE_FILE;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}

	for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType	= FI_COOKIE_FOLDER;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFolders[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanInternetHistory()
{
	FOUND_ITEM		tFound;
	int				i;
	TCHAR			szWindows[MAX_PATH + 1];
	CString			strFolder;
	CCleanSearch	Search;

	RegistryGetString(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("History"), szWindows, MAX_PATH + 1);

	strFolder.Format(_T("%s\\desktop.ini"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();

	strFolder.Format(_T("%s\\History.IE5\\*.*"), szWindows);
	Search.SetPartName(strFolder);
	Search.DoSearch();

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_HISTORY_FILE;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}

	for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType	= FI_HISTORY_FOLDER;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFolders[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanErrorReports()
{
	FOUND_ITEM		tFound;
	int				i;
	CString			strFolder;
	CCleanSearch	Search;

	strFolder	= _T("\\ErrorRep\\*.*");

	Search.SetPartName(strFolder);
	Search.DoSearch();

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_ERRREP_FILE;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}

	for (i = 0; i < Search.m_arrFolders.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType	= FI_ERRREP_FOLDER;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFolders[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanLogFiles()
{
	FOUND_ITEM		tFound;
	int				i;
	CString			strFolder;
	CCleanSearch	Search;

	strFolder	= _T("\\*.log");

	Search.SetPartName(strFolder);
	Search.SetIncludeFolders(FALSE);
	Search.DoSearch();
	Search.SetIncludeFolders(TRUE);

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_LOG_FILE;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanZeroSizeFiles()
{
	FOUND_ITEM		tFound;
	int				i;
	CString			strFolder;
	CCleanSearch	Search;

	strFolder	= _T("\\*.*");

	Search.SetPartName(strFolder);
	Search.SetSearchSize(0, 0);
	Search.SetIncludeFolders(FALSE);
	Search.DoSearch();
	Search.SetIncludeFolders(TRUE);

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_ZEROSIZE_FILE;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanInvalidShortcuts()
{
	FOUND_ITEM		tFound;
	int				i;
	CString			strFolder;
	CShortcutSearch	Search;

	strFolder	= _T("\\*.lnk");

	Search.SetPartName(strFolder);
	Search.SetIncludeFolders(FALSE);
	Search.DoSearch();
	Search.SetIncludeFolders(TRUE);

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_INVALID_SHORTCUT;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}
}

void CFilesPage::ScanThumbnailCaches()
{
	FOUND_ITEM		tFound;
	int				i;
	CString			strFolder;
	CCleanSearch	Search;

	strFolder	= _T("\\Thumbs.db");

	Search.SetPartName(strFolder);
	Search.SetIncludeFolders(FALSE);
	Search.DoSearch();
	Search.SetIncludeFolders(TRUE);

	for (i = 0; i < Search.m_arrFiles.GetSize(); i++)
	{
		memset(&tFound, 0, sizeof(FOUND_ITEM));

		tFound.nType			= FI_THUMBS_DB;
		tFound.uiSize.QuadPart	= Search.m_arrFiles[i].uiSize.QuadPart;

		_tcscpy_s(tFound.szName, MAX_PATH + 1, Search.GetFullPath(&Search.m_arrFiles[i]));
		g_arrFoundItems.Add(tFound);
	}
}

#pragma warning(disable:4995)

void CFilesPage::ScanNotifications()
{
	FOUND_ITEM	tFound;
	CEGUID		ceguid;
	CEOID		ceoid;
	CEOID		ceoidRec;
	HANDLE		hDatabase;
	WORD		wProps;
	LPBYTE		lpRecBuffer;
	DWORD		dwRecSize;
	PCEPROPVAL	pRecProps;
	TCHAR		szVolume[MAX_PATH];
	TCHAR		szDatabase[MAX_PATH];
	int			i;
	int			j;
	FILETIME	ft;
	SYSTEMTIME	st;
	NOTI_ITEM	tItem;
	CEOID		ceoid1;
	CEOID		ceoid2;

	CArray<NOTI_ITEM, NOTI_ITEM>	arrItems;
	CByteArray						arrDuplicated;

	_tcscpy_s(szVolume, MAX_PATH, _T("\\mxip_notify.vol"));

	if (CeMountDBVol(&ceguid, szVolume, OPEN_EXISTING))
	{
		arrItems.RemoveAll();
		arrDuplicated.RemoveAll();

		// Notification Queue
		_tcscpy_s(szDatabase, MAX_PATH, _T("DB_notify_queue"));

		ceoid		= 0;
		hDatabase	= CeOpenDatabaseEx2(&ceguid, &ceoid, szDatabase, NULL, 0, NULL);

		if (hDatabase != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (CeSeekDatabaseEx(hDatabase, CEDB_SEEK_BEGINNING, arrItems.GetCount(), 0, NULL))
				{
					ceoidRec	= CeReadRecordPropsEx(hDatabase, CEDB_ALLOWREALLOC, &wProps, NULL, &lpRecBuffer, &dwRecSize, NULL);

					if (ceoidRec)
					{
						pRecProps	= (PCEPROPVAL)lpRecBuffer;

						memset(&tItem, 0, sizeof(NOTI_ITEM));

						for (i = 0; i < wProps; i++)
						{
							switch (HIWORD(pRecProps->propid))
							{
							case 0x0001:
								{
									memcpy_s(&tItem.ftTime, sizeof(FILETIME), &pRecProps->val.filetime, sizeof(FILETIME));
								}
								break;
							case 0x0002:
								{
									DWORD	dwCount	= pRecProps->val.blob.dwCount;

									if (dwCount > NOTIFICATION_MAXBUFFER)
									{
										dwCount	= NOTIFICATION_MAXBUFFER;
									}

									memcpy_s(tItem.szBuffer, sizeof(BYTE) * NOTIFICATION_MAXBUFFER, pRecProps->val.blob.lpb, dwCount);
									_tcscpy_s(tItem.szEvent, MAX_PATH + 1, (LPCTSTR)(pRecProps->val.blob.lpb + 0x30));
								}
								break;
							case 0x0004:
								{
									tItem.dwFlags1	= pRecProps->val.ulVal;
								}
								break;
							}

							pRecProps++;
						}

						tItem.oid	= ceoidRec;

						GetSystemTime(&st);
						SystemTimeToFileTime(&st, &ft);

						if (CompareFileTime(&tItem.ftTime, &ft) < 0)
						{
							arrDuplicated.Add(1);
						}
						else
						{
							arrDuplicated.Add(0);
						}

						arrItems.Add(tItem);
						LocalFree(lpRecBuffer);
					}
				}
				else
				{
					ceoidRec	= 0;
				}
			} while (ceoidRec);

			CloseHandle(hDatabase);
		}

		if (arrItems.GetCount() > 1)
		{
			for (i = 0; i < arrItems.GetCount(); i++)
			{
				if (arrDuplicated.GetAt(i) == 0)
				{
					for (j = i; j < arrItems.GetCount(); j++)
					{
						if (i == j)
						{
							continue;
						}

						ceoid1	= arrItems.GetAt(i).oid;
						ceoid2	= arrItems.GetAt(j).oid;

						arrItems.GetAt(i).oid	= 0;
						arrItems.GetAt(j).oid	= 0;

						if (!memcmp(&arrItems.GetAt(i), &arrItems.GetAt(j), sizeof(NOTI_ITEM)))
						{
							arrDuplicated.SetAt(j, 1);
						}

						arrItems.GetAt(i).oid	= ceoid1;
						arrItems.GetAt(j).oid	= ceoid2;
					}
				}
			}
		}

		for (i = 0; i < arrDuplicated.GetCount(); i++)
		{
			if (arrDuplicated.GetAt(i) == 1)
			{
				memset(&tFound, 0, sizeof(FOUND_ITEM));

				tFound.nType			= FI_NOTI_QUEUE;
				tFound.oidRec			= arrItems.GetAt(i).oid;

				_tcscpy_s(tFound.szName, MAX_PATH + 1, arrItems.GetAt(i).szEvent);
				g_arrFoundItems.Add(tFound);
			}
		}

		arrItems.RemoveAll();
		arrDuplicated.RemoveAll();

		// Notification Event
		_tcscpy_s(szDatabase, MAX_PATH, _T("DB_notify_events"));

		ceoid		= 0;
		hDatabase	= CeOpenDatabaseEx2(&ceguid, &ceoid, szDatabase, NULL, 0, NULL);

		if (hDatabase != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (CeSeekDatabaseEx(hDatabase, CEDB_SEEK_BEGINNING, arrItems.GetCount(), 0, NULL))
				{
					ceoidRec	= CeReadRecordPropsEx(hDatabase, CEDB_ALLOWREALLOC, &wProps, NULL, &lpRecBuffer, &dwRecSize, NULL);

					if (ceoidRec)
					{
						pRecProps	= (PCEPROPVAL)lpRecBuffer;

						memset(&tItem, 0, sizeof(NOTI_ITEM));

						for (i = 0; i < wProps; i++)
						{
							switch (HIWORD(pRecProps->propid))
							{
							case 0x0001:
								{
									_tcscpy_s(tItem.szEvent, MAX_PATH + 1, pRecProps->val.lpwstr);
								}
								break;
							case 0x0002:
								{
									_tcscpy_s(tItem.szType, MAX_PATH + 1, pRecProps->val.lpwstr);
								}
								break;
							case 0x0003:
								{
									tItem.dwFlags1	= pRecProps->val.ulVal;
								}
								break;
							case 0x0004:
								{
									tItem.dwFlags2	= pRecProps->val.ulVal;
								}
								break;
							}

							pRecProps++;
						}

						tItem.oid	= ceoidRec;

						arrItems.Add(tItem);
						arrDuplicated.Add(0);
						LocalFree(lpRecBuffer);
					}
				}
				else
				{
					ceoidRec	= 0;
				}
			} while (ceoidRec);

			CloseHandle(hDatabase);
		}

		if (arrItems.GetCount() > 1)
		{
			for (i = 0; i < arrItems.GetCount(); i++)
			{
				if (arrDuplicated.GetAt(i) == 0)
				{
					for (j = i; j < arrItems.GetCount(); j++)
					{
						if (i == j)
						{
							continue;
						}

						ceoid1	= arrItems.GetAt(i).oid;
						ceoid2	= arrItems.GetAt(j).oid;

						arrItems.GetAt(i).oid	= 0;
						arrItems.GetAt(j).oid	= 0;

						if (!memcmp(&arrItems.GetAt(i), &arrItems.GetAt(j), sizeof(NOTI_ITEM)))
						{
							arrDuplicated.SetAt(j, 1);
						}

						arrItems.GetAt(i).oid	= ceoid1;
						arrItems.GetAt(j).oid	= ceoid2;
					}
				}
			}
		}

		for (i = 0; i < arrDuplicated.GetCount(); i++)
		{
			if (arrDuplicated.GetAt(i) == 1)
			{
				memset(&tFound, 0, sizeof(FOUND_ITEM));

				tFound.nType			= FI_NOTI_EVENT;
				tFound.oidRec			= arrItems.GetAt(i).oid;

				_tcscpy_s(tFound.szName, MAX_PATH + 1, arrItems.GetAt(i).szEvent);
				g_arrFoundItems.Add(tFound);
			}
		}

		CeUnmountDBVol(&ceguid);
	}
}

#pragma warning(default:4995)

BOOL CFilesPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaders();
	SetItems();
	return TRUE;
}

void CFilesPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

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

	rc.left		= DRA::SCALEX(2);
	rc.top		= DRA::SCALEY(2);
	rc.right	= cx - DRA::SCALEX(2);
	rc.bottom	= cy - size.cy - DRA::SCALEY(4);

	m_ctlItems.MoveWindow(&rc);
	m_ctlItems.SetAutoColumnWidth();
}

void CFilesPage::OnBnClickedSelectAll()
{
	for (int i = 0; i < m_ctlItems.GetItemCount(); i++)
	{
		m_ctlItems.SetCheck(i, TRUE);
	}
}

void CFilesPage::OnBnClickedUnselectAll()
{
	for (int i = 0; i < m_ctlItems.GetItemCount(); i++)
	{
		m_ctlItems.SetCheck(i, FALSE);
	}
}
