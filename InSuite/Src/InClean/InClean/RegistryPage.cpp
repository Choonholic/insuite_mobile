// RegistryPage.cpp

#include "stdafx.h"
#include "InClean.h"
#include "RegistryPage.h"
#include "FoundDialog.h"
#include "FoundItems.h"
#include "RegistryMFCX.h"
#include "StringX.h"
#include "ShellX.h"

#ifndef RG_MAX_ITEMS
#define RG_MAX_ITEMS		2
#define RG_ACTIVEX_COMOBJ	0
#define RG_OBSOLETE_KEY		1
#endif

IMPLEMENT_DYNAMIC(CRegistryPage, CPropertyPageX)

CRegistryPage::CRegistryPage() : CPropertyPageX(CRegistryPage::IDD)
{
}

CRegistryPage::~CRegistryPage()
{
}

void CRegistryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RG_ITEMS, m_ctlItems);
	DDX_Control(pDX, IDC_RG_SELECT_ALL, m_ctlSelectAll);
	DDX_Control(pDX, IDC_RG_UNSELECT_ALL, m_ctlUnselectAll);
}

BEGIN_MESSAGE_MAP(CRegistryPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_RG_SELECT_ALL, &CRegistryPage::OnBnClickedSelectAll)
	ON_BN_CLICKED(IDC_RG_UNSELECT_ALL, &CRegistryPage::OnBnClickedUnselectAll)
END_MESSAGE_MAP()

void CRegistryPage::SetHeaders()
{
	CString	strItem;

	strItem.LoadString(IDS_HEADER_ITEM);
	m_ctlItems.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(200), 0);
}

void CRegistryPage::SetItems()
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

	for (int i = 0; i < RG_MAX_ITEMS; i++)
	{
		strItem.LoadString(IDS_RG_ACTIVEX_COMOBJ + i);

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

void CRegistryPage::Scan()
{
	CFoundDialog	dFound;
	BOOL			bScanned	= FALSE;

	BeginWaitCursor();
	g_arrFoundItems.RemoveAll();

	for (int i = 0; i < RG_MAX_ITEMS; i++)
	{
		if (m_ctlItems.GetCheck(i))
		{
			switch (i)
			{
			case RG_ACTIVEX_COMOBJ:
				{
					ScanActiveXCOMObjects();

					bScanned	= TRUE;
				}
				break;
			case RG_OBSOLETE_KEY:
				{
					ScanObsoleteKeys();

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

void CRegistryPage::ScanActiveXCOMObjects()
{
	DeepScanCLSID();
	DeepScanClasses();
	DeepScanAppID();
}

void CRegistryPage::ScanObsoleteKeys()
{
	DeepScanObsoleteKeys(HKEY_LOCAL_MACHINE, _T("SOFTWARE"));
	DeepScanObsoleteKeys(HKEY_CURRENT_USER, _T("Software"));
}

BOOL CRegistryPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaders();
	SetItems();
	return TRUE;
}

void CRegistryPage::OnSize(UINT nType, int cx, int cy)
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

void CRegistryPage::OnBnClickedSelectAll()
{
	for (int i = 0; i < m_ctlItems.GetItemCount(); i++)
	{
		m_ctlItems.SetCheck(i, TRUE);
	}
}

void CRegistryPage::OnBnClickedUnselectAll()
{
	for (int i = 0; i < m_ctlItems.GetItemCount(); i++)
	{
		m_ctlItems.SetCheck(i, FALSE);
	}
}

void CRegistryPage::DeepScanCLSID()
{
	FOUND_ITEM		tFound;
	CRegistryMFCX	reg;
	CRegistryMFCX	regSub;
	CRegistryMFCX	regItem;
	BOOL			bFound;
	CString			strKey;
	CString			strSubKey;
	CString			strItemKey;

	if (!reg.Open(HKEY_CLASSES_ROOT, _T("CLSID"), FALSE))
	{
		return;
	}

	bFound	= reg.GetFirstSubKey(strKey);

	while (bFound)
	{
		strSubKey.Format(_T("CLSID\\%s"), strKey);

		if (regSub.Open(HKEY_CLASSES_ROOT, strSubKey, FALSE))
		{
			// AppID
			if (regSub.IsValueExist(_T("AppID")))
			{
				CString	strAppID	= regSub.ReadString(_T("AppID"));

				if (!strAppID.IsEmpty())
				{
					if (!IsAppIDExist(strAppID))
					{
						tFound.nType	= FI_MISSING_APPID;
						tFound.hKey		= HKEY_CLASSES_ROOT;

						_tcscpy_s(tFound.szName, MAX_PATH + 1, strSubKey);
						_tcscpy_s(tFound.szValue, MAX_PATH + 1, _T("AppID"));
						g_arrFoundItems.Add(tFound);
					}
				}
			}

			// DefaultIcon
			strItemKey.Format(_T("%s\\DefaultIcon"), strSubKey);

			if (regItem.Open(HKEY_CLASSES_ROOT, strItemKey, FALSE))
			{
				CString	strIconPath	= regItem.ReadString(_T(""));

				if (!strIconPath.IsEmpty())
				{
					if (!IsAppIconExist(strIconPath))
					{
						tFound.nType	= FI_MISSING_ICON;
						tFound.hKey		= HKEY_CLASSES_ROOT;

						_tcscpy_s(tFound.szName, MAX_PATH + 1, strItemKey);
						g_arrFoundItems.Add(tFound);
					}
				}

				regItem.Close();
			}

			// InprocServer
			strItemKey.Format(_T("%s\\InprocServer32"), strSubKey);

			if (regItem.Open(HKEY_CLASSES_ROOT, strItemKey, FALSE))
			{
				CString	strServerPath	= regItem.ReadString(_T(""));

				if (!strServerPath.IsEmpty())
				{
					if (!IsFileExist(strServerPath))
					{
						tFound.nType	= FI_MISSING_INPROC32;
						tFound.hKey		= HKEY_CLASSES_ROOT;

						_tcscpy_s(tFound.szName, MAX_PATH + 1, strItemKey);
						g_arrFoundItems.Add(tFound);
					}
				}

				regItem.Close();
			}

			regSub.Close();
		}

		bFound	= reg.GetNextSubKey(strKey);
	}

	reg.Close();
}

void CRegistryPage::DeepScanClasses()
{
	FOUND_ITEM		tFound;
	CRegistryMFCX	reg;
	CRegistryMFCX	regSub;
	BOOL			bFound;
	CString			strKey;

	if (!reg.Open(HKEY_CLASSES_ROOT, _T(""), FALSE))
	{
		return;
	}

	bFound	= reg.GetFirstSubKey(strKey);

	while (bFound)
	{
		if (strKey.GetAt(0) == _T('.'))
		{
			if (regSub.Open(HKEY_CLASSES_ROOT, strKey, FALSE))
			{
				CString	strProgID	= regSub.ReadString(_T(""));

				if (!strProgID.IsEmpty())
				{
					if (!IsProgIDExist(strProgID))
					{
						tFound.nType	= FI_MISSING_PROGID;
						tFound.hKey		= HKEY_CLASSES_ROOT;

						_tcscpy_s(tFound.szName, MAX_PATH + 1, strKey);
						g_arrFoundItems.Add(tFound);
					}
				}
			}
		}
		else
		{
			CLSID	clsid;

			if (CLSIDFromProgID(strKey, &clsid) == S_OK)
			{
				LPOLESTR	psz;

				if (StringFromCLSID(clsid, &psz) == S_OK)
				{
					if (!IsCLSIDExist(psz))
					{
						tFound.nType	= FI_MISSING_CLSID;
						tFound.hKey		= HKEY_CLASSES_ROOT;

						_tcscpy_s(tFound.szName, MAX_PATH + 1, strKey);
						g_arrFoundItems.Add(tFound);
					}

					CoTaskMemFree(psz);
				}
			}
 		}

		if (regSub.Open(HKEY_CLASSES_ROOT, strKey, FALSE))
		{
			if ((regSub.GetSubKeyCount() == 0) && (regSub.GetValueCount() == 0))
			{
				tFound.nType	= FI_UNUSED_CLASS;
				tFound.hKey		= HKEY_CLASSES_ROOT;

				_tcscpy_s(tFound.szName, MAX_PATH + 1, strKey);
				g_arrFoundItems.Add(tFound);
			}

			regSub.Close();
		}

		bFound	= reg.GetNextSubKey(strKey);
	}

	reg.Close();
}

void CRegistryPage::DeepScanAppID()
{
	FOUND_ITEM		tFound;
	CRegistryMFCX	reg;
	CRegistryMFCX	regSub;
	BOOL			bFound;
	CString			strKey;
	CString			strSubKey;

	if (!reg.Open(HKEY_CLASSES_ROOT, _T("AppID"), FALSE))
	{
		return;
	}

	bFound	= reg.GetFirstSubKey(strKey);

	while (bFound)
	{
		strSubKey.Format(_T("AppID\\%s"), strKey);

		if (regSub.Open(HKEY_CLASSES_ROOT, strSubKey, FALSE))
		{
			CString	strCLSID	= regSub.ReadString(_T("AppID"));

			if (!strCLSID.IsEmpty())
			{
				if (!IsAppIDExist(strCLSID))
				{
					tFound.nType	= FI_MISSING_APPIDREF;
					tFound.hKey		= HKEY_CLASSES_ROOT;

					_tcscpy_s(tFound.szName, MAX_PATH + 1, strSubKey);
					g_arrFoundItems.Add(tFound);
				}
			}

			regSub.Close();
		}

		bFound	= reg.GetNextSubKey(strKey);
	}

	reg.Close();
}

void CRegistryPage::DeepScanObsoleteKeys(HKEY hKey, LPCTSTR lpszSubKey)
{
	FOUND_ITEM		tFound;
	CRegistryMFCX	reg;
	CRegistryMFCX	regSub;
	BOOL			bFound;
	CString			strKey;
	CString			strSubKey;

	if (!reg.Open(hKey, lpszSubKey, FALSE))
	{
		return;
	}

	bFound	= reg.GetFirstSubKey(strKey);

	while (bFound)
	{
		strSubKey.Format(_T("%s\\%s"), lpszSubKey, strKey);

		if (regSub.Open(hKey, strSubKey, FALSE))
		{
			if ((regSub.GetSubKeyCount() == 0) && (regSub.GetValueCount() == 0))
			{
				tFound.nType	= FI_OBSOLETE_KEY;
				tFound.hKey		= hKey;

				_tcscpy_s(tFound.szName, MAX_PATH + 1, strSubKey);
				g_arrFoundItems.Add(tFound);
			}

			regSub.Close();
		}

		bFound	= reg.GetNextSubKey(strKey);
	}

	reg.Close();
}

BOOL CRegistryPage::IsAppIDExist(LPCTSTR lpszAppID)
{
	CRegistryMFCX	reg;
	CString			strKey;

	strKey.Format(_T("AppID\\%s"), lpszAppID);

	if (reg.Open(HKEY_CLASSES_ROOT, strKey, FALSE))
	{
		reg.Close();
		return TRUE;
	}

	return FALSE;
}

BOOL CRegistryPage::IsAppIconExist(LPCTSTR lpszIcon)
{
	TCHAR	szFullPath[MAX_PATH + 1];
	TCHAR	szIconPath[MAX_PATH + 1];
	TCHAR	szIconFile[MAX_PATH + 1];
	TCHAR	szIconID[7];
	HICON	hLargeIcon;
	HICON	hSmallIcon;
	int		nSep;
	int		nID;

	_tcscpy_s(szIconPath, MAX_PATH + 1, lpszIcon);

	nSep	= ReverseFindX(szIconPath, _T(','));

	if (nSep != -1)
	{
		LeftX(szIconFile, MAX_PATH + 1, szIconPath, nSep);
		MidX(szIconID, 7, szIconPath, nSep + 1);

		nID	= _ttoi(szIconID);
	}
	else
	{
		_tcscpy_s(szIconFile, MAX_PATH + 1, szIconPath);

		nID	= 0;
	}

	if (szIconFile[0] != _T('\\'))
	{
		TCHAR	szWindows[MAX_PATH + 1];

		GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
		CombineFileName(szFullPath, MAX_PATH + 1, szWindows, szIconFile);
	}
	else
	{
		_tcscpy_s(szFullPath, MAX_PATH + 1, szIconFile);
	}

	return (BOOL)(ExtractIconEx(szFullPath, nID, &hLargeIcon, &hSmallIcon, 1) != NULL);
}

BOOL CRegistryPage::IsProgIDExist(LPCTSTR lpszProgID)
{
	CRegistryMFCX	reg;

	if (reg.Open(HKEY_CLASSES_ROOT, lpszProgID, FALSE))
	{
		reg.Close();
		return TRUE;
	}

	return FALSE;
}

BOOL CRegistryPage::IsCLSIDExist(LPCTSTR lpszCLSID)
{
	CRegistryMFCX	reg;
	CString			strKey;

	strKey.Format(_T("CLSID\\%s"), lpszCLSID);

	if (reg.Open(HKEY_CLASSES_ROOT, strKey, FALSE))
	{
		reg.Close();
		return TRUE;
	}

	return FALSE;
}
