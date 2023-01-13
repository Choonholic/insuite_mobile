// PropertiesDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "PropertiesDialogX.h"
#include "PropertiesTimeDialogX.h"
#include "PropertiesAttributesDialogX.h"
#include "DeviceInfoX.h"
#include "GraphicsX.h"
#include "ColorX.h"
#include "SearchX.h"
#include "ShellX.h"

extern HINSTANCE	g_hModule;

IMPLEMENT_DYNAMIC(CPropertiesDialogX, CDialogX)

CPropertiesDialogX::CPropertiesDialogX(CWnd* pParent /*=NULL*/) : CDialogX(IDD_PROPERTIES_DIALOG, pParent), m_bReadOnly(FALSE), m_bHidden(FALSE), m_bArchive(FALSE), m_bSystem(FALSE), m_bROM(FALSE)
{
	InitializeX();
}

CPropertiesDialogX::~CPropertiesDialogX()
{
}

void CPropertiesDialogX::DoDataExchange(CDataExchange* pDX)
{
	CDialogX::DoDataExchange(pDX);

#if (!defined(_DEBUG))
	DDX_Control(pDX, IDC_PROP_NAME_EDIT, m_ctlNameEdit);
#endif

	DDX_Text(pDX, IDC_PROP_NAME_EDIT, m_strNameEdit);
	DDX_Control(pDX, IDC_PROP_NAME, m_ctlName);
	DDX_Control(pDX, IDC_PROP_TYPE_H, m_ctlTypeH);
	DDX_Control(pDX, IDC_PROP_LOCATION_H, m_ctlLocationH);
	DDX_Control(pDX, IDC_PROP_SIZE_H, m_ctlSizeH);
	DDX_Control(pDX, IDC_PROP_CONTAINS_H, m_ctlContainsH);
	DDX_Control(pDX, IDC_PROP_CAPACITY_H, m_ctlCapacityH);
	DDX_Control(pDX, IDC_PROP_TYPE, m_ctlType);
	DDX_Control(pDX, IDC_PROP_LOCATION, m_ctlLocation);
	DDX_Control(pDX, IDC_PROP_SIZE, m_ctlSize);
	DDX_Control(pDX, IDC_PROP_CONTAINS, m_ctlContains);
	DDX_Control(pDX, IDC_PROP_CAPACITY, m_ctlCapacity);
	DDX_Control(pDX, IDC_PROP_TIME, m_ctlTime);
	DDX_Control(pDX, IDC_PROP_ATTRIBUTES, m_ctlAttributes);
}

BEGIN_MESSAGE_MAP(CPropertiesDialogX, CDialogX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_PROP_TIME, &CPropertiesDialogX::OnBnClickedPropTime)
	ON_BN_CLICKED(IDC_PROP_ATTRIBUTES, &CPropertiesDialogX::OnBnClickedPropAttributes)
END_MESSAGE_MAP()

void CPropertiesDialogX::InitializeX()
{
	m_dwChangeFlags		= PDX_CF_NONE;
	m_uiSize.QuadPart	= 0;
	m_nFiles			= 0;
	m_nFolders			= 0;

	if (m_arrFiles.GetSize())
	{
		m_arrFiles.RemoveAll();
	}

	if (m_arrTypes.GetSize())
	{
		m_arrTypes.RemoveAll();
	}
}

void CPropertiesDialogX::AddFile(LPCTSTR lpszFile)
{
	CFileStatus	status;
	CString		strFile;

	strFile	= lpszFile;

	if ((strFile.GetLength() > 2) && (strFile.Right(1) == _T('\\')))
	{
		strFile	= strFile.Left(strFile.GetLength() - 1);
	}

	if (!CFile::GetStatus(strFile, status))
	{
		strFile	= _T("\\");
	}

	m_arrFiles.Add(strFile);
	m_arrTypes.Add((BOOL)((status.m_attribute & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY));
}

void CPropertiesDialogX::AddFiles(CStringArray &arrFiles)
{
	for (int i = 0; i < arrFiles.GetSize(); i++)
	{
		AddFile(arrFiles.GetAt(i));
	}
}

void CPropertiesDialogX::ClearFiles()
{
	InitializeX();
}

DWORD CPropertiesDialogX::GetChangeFlags()
{
	return m_dwChangeFlags;
}

CString CPropertiesDialogX::GetSizeString(ULARGE_INTEGER uiSize)
{
	CString	strResult;
	CString	strUnits;

	if (uiSize.QuadPart >= 0x40000000)
	{
		strUnits.LoadString(IDS_FILE_GIGABYTES);
		strResult.Format(_T("%.2f %s"), (double)((double)(ULONGLONG)uiSize.QuadPart / (double)0x40000000), strUnits);
	}
	else if (uiSize.QuadPart >= 0x100000)
	{
		strUnits.LoadString(IDS_FILE_MEGABYTES);
		strResult.Format(_T("%.2f %s"), (double)((double)(ULONGLONG)uiSize.QuadPart / (double)0x100000), strUnits);
	}
	else if (uiSize.QuadPart >= 0x400)
	{
		strUnits.LoadString(IDS_FILE_KILOBYTES);
		strResult.Format(_T("%.2f %s"), (double)((double)(ULONGLONG)uiSize.QuadPart / (double)0x400), strUnits);
	}
	else
	{
		strUnits.LoadString(IDS_FILE_BYTES);
		strResult.Format(_T("%d %s"), (DWORD)uiSize.QuadPart, strUnits);
	}

	return strResult;
}

CString CPropertiesDialogX::GetContainsString()
{
	CString	strResult;
	CString	strFiles;
	CString	strFolders;

	if ((m_arrFiles.GetSize() == 1) && (m_arrTypes.GetAt(0) == TRUE))
	{
		strResult = _T("");
	}
	else
	{
		strFiles.LoadString(IDS_PROP_FILES);
		strFolders.LoadString(IDS_PROP_FOLDERS);
		strResult.Format(_T("%d %s, %d %s"), m_nFiles, strFiles, m_nFolders, strFolders);
	}

	return strResult;
}

CString CPropertiesDialogX::GetTimeString(SYSTEMTIME &st)
{
	CString	strResult;
	TCHAR	szFormat[256];

	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, szFormat, 80);

	strResult	= szFormat;

	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, szFormat, 80);

	strResult	+= _T(" ");
	strResult	+= szFormat;

	return strResult;
}

CString	CPropertiesDialogX::GetCapacityString()
{
	CString			strResult;
	ULARGE_INTEGER	uiFree;

	GetDiskFreeSpaceEx(m_arrFiles.GetAt(0), NULL, NULL, &uiFree);
	AfxFormatString1(strResult, IDS_PROP_CAPACITY, GetSizeString(uiFree));
	return strResult;
}

BOOL CPropertiesDialogX::OnInitDialog()
{
	CDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_OKAY);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDOK;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);

	strButton.LoadString(IDS_MENU_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(1, &tbButton);

	CString		strMessage;
	CSearchX	Search;
	BOOL		bExternalStorage	= FALSE;

	BeginWaitCursor();

	if (!m_arrFiles.GetSize())
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	if (m_arrFiles.GetSize() == 1)
	{
		m_FI.SetFile(m_arrFiles.GetAt(0));

		if (!m_arrTypes.GetAt(0))
		{
			CString	strPath;

			strPath.Format(_T("%s%s*.*"), m_arrFiles.GetAt(0), ((m_arrFiles.GetAt(0) == _T("\\")) ? _T("") : _T("\\")));
			Search.SetPartName(strPath);
			Search.DoSearch();

			m_uiSize	= Search.GetTotalSize();
			m_nFiles	= Search.GetFoundFiles();
			m_nFolders	= Search.GetFoundFolders();

#if (!defined(_DEBUG))
			m_ctlNameEdit.ShowWindow(SW_HIDE);
			m_ctlNameEdit.EnableWindow(FALSE);
#else
			CWnd*	pNameEdit	= NULL;

			pNameEdit	= GetDlgItem(IDC_PROP_NAME_EDIT);

			if (pNameEdit != NULL)
			{
				pNameEdit->ShowWindow(SW_HIDE);
				pNameEdit->EnableWindow(FALSE);
			}
#endif

			bExternalStorage	= IsExternalStorage(m_arrFiles.GetAt(0));

			if (!bExternalStorage)
			{
				m_ctlCapacityH.ShowWindow(SW_HIDE);
				m_ctlCapacityH.EnableWindow(FALSE);
				m_ctlCapacity.ShowWindow(SW_HIDE);
				m_ctlCapacity.EnableWindow(FALSE);
			}

			m_ctlTime.EnableWindow(FALSE);
			m_ctlAttributes.EnableWindow(FALSE);
		}
		else
		{
			m_uiSize.LowPart	= m_FI.GetSize();

			m_ctlName.ShowWindow(SW_HIDE);
			m_ctlName.EnableWindow(FALSE);
			m_ctlContainsH.ShowWindow(SW_HIDE);
			m_ctlContainsH.EnableWindow(FALSE);
			m_ctlContains.ShowWindow(SW_HIDE);
			m_ctlContains.EnableWindow(FALSE);
			m_ctlCapacityH.ShowWindow(SW_HIDE);
			m_ctlCapacityH.EnableWindow(FALSE);
			m_ctlCapacity.ShowWindow(SW_HIDE);
			m_ctlCapacity.EnableWindow(FALSE);
		}

		SYSTEMTIME	st;

		if (m_arrFiles.GetAt(0) == _T("\\"))
		{
			TCHAR	szDeviceID[64];

			GetDeviceIDX(szDeviceID, 64);
			m_ctlName.SetWindowText(szDeviceID);

			m_strNameEdit	= szDeviceID;
		}
		else
		{
			TCHAR	szPath[MAX_PATH + 1];

			TakeFileName(m_arrFiles.GetAt(0), szPath, MAX_PATH + 1);
			m_ctlName.SetWindowText(szPath);

			m_strNameEdit	= szPath;
		}

		m_ctlType.SetWindowText(m_FI.GetTypeName());

		if (m_FI.GetAttributes() & FILE_ATTRIBUTE_DIRECTORY)
		{
			m_ctlLocation.SetWindowText(m_arrFiles.GetAt(0));
		}
		else
		{
			CString		strLocation;

			TakePath(m_arrFiles.GetAt(0), strLocation.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
			strLocation.ReleaseBuffer();
			m_ctlLocation.SetWindowText(strLocation);
		}

		m_ctlSize.SetWindowText(GetSizeString(m_uiSize));
		m_ctlContains.SetWindowText(GetContainsString());

		if (bExternalStorage)
		{
			m_ctlCapacity.SetWindowText(GetCapacityString());
		}

		m_FI.GetCreatedTime(st);

		m_strCreated	= GetTimeString(st);

		m_FI.GetModifiedTime(st);

		m_strModified	= GetTimeString(st);

		m_FI.GetAccessedTime(st);

		m_strAccessed	= GetTimeString(st);

		m_bReadOnly	= m_FI.IsReadOnly();
		m_bHidden	= m_FI.IsHidden();
		m_bArchive	= m_FI.IsArchive();
		m_bSystem	= m_FI.IsSystem();
		m_bROM		= ((m_FI.IsInROM() || m_FI.IsROMModule()) ? TRUE : FALSE);
	}
	else
	{
		for (int i = 0; i < m_arrFiles.GetSize(); i++)
		{
			if (m_arrTypes.GetAt(i))
			{
				m_uiSize.QuadPart++;
				m_nFiles++;
			}
			else
			{
				CString	strPath;

				strPath.Format(_T("%s%s*.*"), m_arrFiles.GetAt(i), ((m_arrFiles.GetAt(i) == _T("\\")) ? _T("") : _T("\\")));
				Search.SetPartName(strPath);
				Search.DoSearch();

				m_uiSize.QuadPart	+= Search.GetTotalSize().QuadPart;
				m_nFiles			+= Search.GetFoundFiles();
				m_nFolders			+= Search.GetFoundFolders();
			}
		}

		m_ctlName.SetWindowText(GetContainsString());

		m_strNameEdit	= GetContainsString();

		strMessage.LoadString(IDS_PROP_MULTITYPES);
		m_ctlType.SetWindowText(strMessage);
		strMessage.LoadString(IDS_PROP_MULTIITEMS);
		m_ctlLocation.SetWindowText(strMessage);
		m_ctlSize.SetWindowText(GetSizeString(m_uiSize));
		m_ctlContains.SetWindowText(GetContainsString());

#if (!defined(_DEBUG))
		m_ctlNameEdit.ShowWindow(SW_HIDE);
		m_ctlNameEdit.EnableWindow(FALSE);
#else
		CWnd*	pNameEdit	= NULL;

		pNameEdit	= GetDlgItem(IDC_PROP_NAME_EDIT);

		if (pNameEdit != NULL)
		{
			pNameEdit->ShowWindow(SW_HIDE);
			pNameEdit->EnableWindow(FALSE);
		}
#endif

		m_ctlCapacityH.ShowWindow(SW_HIDE);
		m_ctlCapacityH.EnableWindow(FALSE);
		m_ctlCapacity.ShowWindow(SW_HIDE);
		m_ctlCapacity.EnableWindow(FALSE);
		m_ctlTime.EnableWindow(FALSE);
		m_ctlAttributes.EnableWindow(FALSE);
	}

	UpdateData(FALSE);
	EndWaitCursor();
	return TRUE;
}

#define BORDER_SIZE	DRA::SCALEX(3)

void CPropertiesDialogX::OnSize(UINT nType, int cx, int cy)
{
	CDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nWidth;
	int		nLeft;

#if (!defined(_DEBUG))
	m_ctlNameEdit.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - BORDER_SIZE;

	m_ctlNameEdit.MoveWindow(&rc);
#else
	CWnd*	pNameEdit	= NULL;

	pNameEdit	= GetDlgItem(IDC_PROP_NAME_EDIT);

	if (pNameEdit != NULL)
	{
		pNameEdit->GetWindowRect(&rc);
		ScreenToClient(&rc);

		rc.right	= cx - BORDER_SIZE;

		pNameEdit->MoveWindow(&rc);
	}
#endif

	m_ctlName.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - BORDER_SIZE;

	m_ctlName.MoveWindow(&rc);

	m_ctlType.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - BORDER_SIZE;

	m_ctlType.MoveWindow(&rc);

	m_ctlLocation.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - BORDER_SIZE;

	m_ctlLocation.MoveWindow(&rc);

	m_ctlSize.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - BORDER_SIZE;

	m_ctlSize.MoveWindow(&rc);

	m_ctlContains.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - BORDER_SIZE;

	m_ctlContains.MoveWindow(&rc);

	m_ctlCapacity.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - BORDER_SIZE;

	m_ctlCapacity.MoveWindow(&rc);

	m_ctlAttributes.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nWidth		= rc.right - rc.left;
	rc.right	= cx - BORDER_SIZE;
	rc.left		= rc.right - nWidth;
	nLeft		= rc.left;

	m_ctlAttributes.MoveWindow(&rc);

	m_ctlTime.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= nLeft - BORDER_SIZE;
	rc.left		= rc.right - nWidth;

	m_ctlTime.MoveWindow(&rc);
}

void CPropertiesDialogX::OnOK()
{
	if ((!m_bROM) && (m_arrFiles.GetSize() == 1))
	{
		DWORD	dwOldAttr	= FILE_ATTRIBUTE_NORMAL;
		DWORD	dwNewAttr	= FILE_ATTRIBUTE_NORMAL;

		UpdateData(TRUE);

		dwOldAttr	= GetFileAttributes(m_arrFiles.GetAt(0));
		dwNewAttr	|= (m_bArchive  ? FILE_ATTRIBUTE_ARCHIVE  : 0);
		dwNewAttr	|= (m_bHidden   ? FILE_ATTRIBUTE_HIDDEN   : 0);
		dwNewAttr	|= (m_bReadOnly ? FILE_ATTRIBUTE_READONLY : 0);
		dwNewAttr	|= (m_bSystem   ? FILE_ATTRIBUTE_ARCHIVE  : 0);

		if (dwOldAttr != dwNewAttr)
		{
			SetFileAttributes(m_arrFiles.GetAt(0), dwNewAttr);

			m_dwChangeFlags	|= PDX_CF_ATTRIBUTES;
		}

		if (m_arrFiles.GetAt(0) == _T("\\"))
		{
			TCHAR	szDeviceID[64];

			GetDeviceIDX(szDeviceID, 64);

			if (m_strNameEdit.Compare(szDeviceID))
			{
				SetDeviceIDX(m_strNameEdit);

				m_dwChangeFlags	|= PDX_CF_NAME;
			}
		}
		else
		{
			TCHAR	szPath[MAX_PATH + 1];
			CString	strNew;

			TakePath(m_arrFiles.GetAt(0), szPath, MAX_PATH + 1, TRUE);

			strNew	= szPath + m_strNameEdit;

			if (strNew.Compare(m_arrFiles.GetAt(0)))
			{
				if (MoveFile(m_arrFiles.GetAt(0), strNew))
				{
					m_dwChangeFlags	|= PDX_CF_NAME;
				}
			}
		}
	}

	CDialogX::OnOK();
}

void CPropertiesDialogX::OnPaint()
{
	CPaintDC	dc(this);
	HICON		hIcon		= NULL;
	HICON		hFileIcon	= NULL;
	RECT		rcHeader	=
	{
		DRA::SCALEX(2), DRA::SCALEY(13) + GetSystemMetrics(SM_CYICON), GetSystemMetrics(SM_CXSCREEN) - DRA::SCALEX(2), DRA::SCALEY(14) + GetSystemMetrics(SM_CYICON)
	};

	FillHGRectEx(dc.GetSafeHdc(), &rcHeader, GetShellColor(SHCOLOR_TASKBAR_LEFTEDGE), GetShellColor(SHCOLOR_TASKBAR_RIGHTEDGE));

	if (m_arrFiles.GetSize() == 1)
	{
		hFileIcon	= m_FI.GetLargeIcon();

		if (m_arrFiles.GetAt(0) == _T("\\"))
		{
			hIcon	= GetShellIcon(SHELLICON_DEVICE, ICON_BIG);

			if (hIcon)
			{
				dc.DrawIcon(DRA::SCALEX(7), DRA::SCALEY(7), hIcon);
			}
		}
		else if (hFileIcon)
		{
			dc.DrawIcon(DRA::SCALEX(7), DRA::SCALEY(7), hFileIcon);
		}
		else
		{
			hIcon	= GetShellIcon(SHELLICON_DOCUMENT, ICON_BIG);

			if (hIcon)
			{
				dc.DrawIcon(DRA::SCALEX(7), DRA::SCALEY(7), hIcon);
			}
		}
	}
	else
	{
		hIcon	= GetShellIcon(SHELLICON_ITEMS, ICON_BIG);

		if (hIcon)
		{
			dc.DrawIcon(DRA::SCALEX(7), DRA::SCALEY(7), hIcon);
		}
	}

	if (hIcon != NULL)
	{
		DestroyIcon(hIcon);
	}
}

void CPropertiesDialogX::OnBnClickedPropTime()
{
	CPropertiesTimeDialogX	dTime;

	dTime.m_strCreated	= m_strCreated;
	dTime.m_strModified	= m_strModified;
	dTime.m_strAccessed	= m_strAccessed;

	dTime.DoModal();
}

void CPropertiesDialogX::OnBnClickedPropAttributes()
{
	CPropertiesAttributesDialogX	dAttributes;

	dAttributes.m_bArchive	= m_bArchive;
	dAttributes.m_bReadOnly	= m_bReadOnly;
	dAttributes.m_bHidden	= m_bHidden;
	dAttributes.m_bSystem	= m_bSystem;
	dAttributes.m_bROM		= m_bROM;

	if (dAttributes.DoModal() == IDOK)
	{
		m_bArchive	= dAttributes.m_bArchive;
		m_bReadOnly	= dAttributes.m_bReadOnly;
		m_bHidden	= dAttributes.m_bHidden;
		m_bSystem	= dAttributes.m_bSystem;
		m_bROM		= dAttributes.m_bROM;
	}
}
