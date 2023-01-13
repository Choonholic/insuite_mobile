// ConfirmCopyX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "DisplayX.h"
#include "ConfirmCopyX.h"
#include "ShellX.h"

extern HINSTANCE	g_hModule;

IMPLEMENT_DYNAMIC(CConfirmCopyX, CDialogX)

CConfirmCopyX::CConfirmCopyX(CWnd* pParent /*=NULL*/) : CDialogX(IDD_EXIST_DIALOG, pParent), m_bDoConflict(FALSE), m_nMode(CONFIRMMODE_COPY)
{
	InitializeX();
}

CConfirmCopyX::~CConfirmCopyX()
{
}

void CConfirmCopyX::DoDataExchange(CDataExchange* pDX)
{
	CDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXIS_MESSAGE, m_ctlMessage);
	DDX_Control(pDX, IDC_EXIS_REPLACE, m_ctlReplace);
	DDX_Control(pDX, IDC_EXIS_NOTCHANGE, m_ctlNotChange);
	DDX_Control(pDX, IDC_EXIS_DOCONFLICT, m_ctlDoConflict);
	DDX_Check(pDX, IDC_EXIS_DOCONFLICT, m_bDoConflict);
}

BEGIN_MESSAGE_MAP(CConfirmCopyX, CDialogX)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_EXIS_REPLACE, &CConfirmCopyX::OnBnClickedReplace)
	ON_BN_CLICKED(IDC_EXIS_NOTCHANGE, &CConfirmCopyX::OnBnClickedNotChange)
END_MESSAGE_MAP()

void CConfirmCopyX::InitializeX()
{
	m_strReplace	= _T("");
	m_strNotChange	= _T("");
	m_dwFlags		= 0x0000;
}

void CConfirmCopyX::SetMode(int nMode)
{
	m_nMode	= nMode;
}

void CConfirmCopyX::SetInfo(LPCTSTR lpszFileName, LPCTSTR lpszSource, LPCTSTR lpszTarget, DWORD dwFlags)
{
	CString		strPath;
	CString		strSize;
	CString		strDate;
	CFileStatus	status;
	int			nFind;

	CombineFileName(m_strSource.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, lpszSource, lpszFileName);
	m_strSource.ReleaseBuffer();
	AfxFormatString1(m_strMessage, IDS_CPCN_MESSAGE, lpszFileName);
	CFile::GetStatus(m_strSource, status);

	strPath	= lpszSource;
	nFind	= strPath.GetLength();

	if (nFind > 1)
	{
		strPath	= strPath.Left(nFind - 1);
	}

	nFind	= strPath.ReverseFind(_T('\\'));

	switch (m_nMode)
	{
	case CONFIRMMODE_COPY:
		{
			m_strReplace.LoadString(IDS_COPY_REPLACE);
		}
		break;
	case CONFIRMMODE_MOVE:
		{
			m_strReplace.LoadString(IDS_MOVE_REPLACE);
		}
		break;
	}

	m_strReplace	+= _T("\n");

	if (nFind == 0)
	{
		m_strReplace	+= strPath;
	}
	else
	{
		m_strReplace	+= _T("...");
		m_strReplace	+= strPath.Mid(nFind);
	}

	m_strReplace	+= _T("\n");
	m_strReplace	+= GetSizeString(status.m_size);
	m_strReplace	+= _T(", ");
	m_strReplace	+= GetTimeString(status.m_ctime);

	CombineFileName(m_strTarget.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, lpszTarget, lpszFileName);
	m_strTarget.ReleaseBuffer();
	CFile::GetStatus(m_strTarget, status);

	strPath	= lpszTarget;
	nFind	= strPath.GetLength();

	if (nFind > 1)
	{
		strPath	= strPath.Left(nFind - 1);
	}

	nFind	= strPath.ReverseFind(_T('\\'));

	switch (m_nMode)
	{
	case CONFIRMMODE_COPY:
		{
			m_strNotChange.LoadString(IDS_COPY_NOTCHANGE);
		}
		break;
	case CONFIRMMODE_MOVE:
		{
			m_strNotChange.LoadString(IDS_MOVE_NOTCHANGE);
		}
		break;
	}

	m_strNotChange	+= _T("\n");

	if (nFind == 0)
	{
		m_strNotChange	+= strPath;
	}
	else
	{
		m_strNotChange	+= _T("...");
		m_strNotChange	+= strPath.Mid(nFind);
	}

	m_strNotChange	+= _T("\n");
	m_strNotChange	+= GetSizeString(status.m_size);
	m_strNotChange	+= _T(", ");
	m_strNotChange	+= GetTimeString(status.m_ctime);
	m_dwFlags		= dwFlags;
}

DWORD CConfirmCopyX::GetInfo()
{
	return m_dwFlags;
}

CString CConfirmCopyX::GetSizeString(ULONGLONG ulSize)
{
	CString	strResult;
	CString	strUnits;

	if (ulSize >= 0x40000000)
	{
		strUnits.LoadString(IDS_FILE_GIGABYTES);
		strResult.Format(_T("%d %s"), (DWORD)((double)ulSize / (double)0x40000000), strUnits);
	}
	else if (ulSize >= 0x100000)
	{
		strUnits.LoadString(IDS_FILE_MEGABYTES);
		strResult.Format(_T("%d %s"), (DWORD)((double)ulSize / (double)0x100000), strUnits);
	}
	else if (ulSize >= 0x400)
	{
		strUnits.LoadString(IDS_FILE_KILOBYTES);
		strResult.Format(_T("%d %s"), (DWORD)((double)ulSize / (double)0x400), strUnits);
	}
	else
	{
		strUnits.LoadString(IDS_FILE_BYTES);
		strResult.Format(_T("%d %s"), (DWORD)ulSize, strUnits);
	}

	return strResult;
}

CString CConfirmCopyX::GetTimeString(CTime time)
{
	CString		strTime;
	SYSTEMTIME	stTime;
	TCHAR		szDate[64];
	TCHAR		szTime[64];

	time.GetAsSystemTime(stTime);
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stTime, NULL, szDate, 64);
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &stTime, NULL, szTime, 64);
	strTime.Format(_T("%s %s"), szDate, szTime);

	return strTime;
}

BOOL CConfirmCopyX::OnInitDialog()
{
	CDialogX::OnInitDialog();

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	strButton.LoadString(IDS_MENU_CANCEL);

	tbButton.iBitmap	= I_IMAGENONE;
	tbButton.idCommand	= IDCANCEL;
	tbButton.fsState	= TBSTATE_ENABLED;
	tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	tbButton.dwData		= 0;
	tbButton.iString	= (int)(LPCTSTR)strButton;

	pCommandBar->InsertButton(0, &tbButton);
	m_ctlMessage.SetWindowText(m_strMessage);
	m_ctlReplace.SetWindowText(m_strReplace);
	m_ctlNotChange.SetWindowText(m_strNotChange);
	return TRUE;
}

void CConfirmCopyX::OnSize(UINT nType, int cx, int cy)
{
	CDialogX::OnSize(nType, cx, cy);

	RECT	rc;
	int		nLeft;

	m_ctlMessage.GetWindowRect(&rc);
	ScreenToClient(&rc);

	nLeft		= rc.top;
	rc.right	= cx - nLeft;

	m_ctlMessage.MoveWindow(&rc);

	m_ctlReplace.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlReplace.MoveWindow(&rc);

	m_ctlNotChange.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - nLeft;

	m_ctlNotChange.MoveWindow(&rc);
}

void CConfirmCopyX::OnPaint()
{
	CPaintDC	dc(this);
	HICON		hReplace	= GetFileIcon(m_strTarget, ICON_BIG);
	HICON		hNotChange	= GetFileIcon(m_strSource, ICON_BIG);
	RECT		rc;

	if (hReplace != NULL)
	{
		m_ctlReplace.GetWindowRect(&rc);
		ScreenToClient(&rc);
		dc.DrawIcon(DRA::SCALEX(4), rc.top, hReplace);
		DestroyIcon(hReplace);
	}

	if (hNotChange != NULL)
	{
		m_ctlNotChange.GetWindowRect(&rc);
		ScreenToClient(&rc);
		dc.DrawIcon(DRA::SCALEX(4), rc.top, hNotChange);
		DestroyIcon(hNotChange);
	}
}

void CConfirmCopyX::OnBnClickedReplace()
{
	UpdateData(TRUE);

	if (m_bDoConflict)
	{
		m_dwFlags	&= ~(0x0001);
		m_dwFlags	|= 0x0001;
	}

	EndDialog(IDYES);
}

void CConfirmCopyX::OnBnClickedNotChange()
{
	UpdateData(TRUE);

	if (m_bDoConflict)
	{
		m_dwFlags	&= ~(0x0001);
		m_dwFlags	|= 0x0001;
	}

	EndDialog(IDNO);
}

void CConfirmCopyX::OnCancel()
{
	CDialogX::OnCancel();
}
