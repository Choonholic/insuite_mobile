// RegisterDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "RegisterDialogX.h"
#include "LicenseX.h"
#include "TimeX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CRegisterDialogX, COptionsDialogX)

CRegisterDialogX::CRegisterDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_REGISTER_DIALOG, pParent), m_strEMail(_T("")), m_strSerial(_T("")), m_strPeriod(_T("")), m_wProductID(0), m_wVersion(0), m_strBinary(_T("")), m_nPeriod(15), m_nStatus(ILM_TRIAL)
{
}

CRegisterDialogX::~CRegisterDialogX()
{
}

void CRegisterDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REGI_EMAIL_H, m_ctlEMailH);
	DDX_Control(pDX, IDC_REGI_EMAIL, m_ctlEMail);
	DDX_Control(pDX, IDC_REGI_SERIAL_H, m_ctlSerialH);
	DDX_Control(pDX, IDC_REGI_SERIAL, m_ctlSerial);
	DDX_Control(pDX, IDC_REGI_PERIOD, m_ctlPeriod);
	DDX_Text(pDX, IDC_REGI_EMAIL, m_strEMail);
	DDX_Text(pDX, IDC_REGI_SERIAL, m_strSerial);
	DDX_Text(pDX, IDC_REGI_PERIOD, m_strPeriod);
}

BEGIN_MESSAGE_MAP(CRegisterDialogX, COptionsDialogX)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CRegisterDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	TCHAR		szWindows[MAX_PATH + 1];
	TCHAR		szPath[MAX_PATH + 1];
	ULONGLONG	ullPeriod;
	DWORD		dwVersion	= MakeVersionNumberX(m_wProductID, m_wVersion);
	int			nRemain;

	SetHeader(IDS_REGI_TITLE);
	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	CombineFileName(szPath, MAX_PATH + 1, szWindows, m_strBinary);

	m_strBinary	= szPath;
	ullPeriod	= (m_nPeriod * OD_100NS);
	m_nStatus	= ReadActivateStatusX(m_strBinary, dwVersion, ullPeriod);
	nRemain		= (int)(ullPeriod / OD_100NS);

	CCommandBar*	pCommandBar	= GetCommandBarCtrl();
	TBBUTTON		tbButton;
	CString			strButton;

	switch (m_nStatus)
	{
	case ILM_ACTIVATED:
		{
			strButton.LoadString(IDS_REGI_CLOSE);

			tbButton.iBitmap	= I_IMAGENONE;
			tbButton.idCommand	= IDCANCEL;
			tbButton.fsState	= TBSTATE_ENABLED;
			tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
			tbButton.dwData		= 0;
			tbButton.iString	= (int)(LPCTSTR)strButton;

			pCommandBar->InsertButton(0, &tbButton);

			m_strPeriod.LoadString(IDS_REGI_TIMEREGISTERED);
			m_ctlEMailH.EnableWindow(FALSE);
			m_ctlEMail.EnableWindow(FALSE);
			m_ctlEMail.SetReadOnly(TRUE);
			m_ctlSerialH.EnableWindow(FALSE);
			m_ctlSerial.EnableWindow(FALSE);
			m_ctlSerial.SetReadOnly(TRUE);
		}
		break;
	case ILM_TRIAL:
		{
			strButton.LoadString(IDS_REGI_REGISTER);

			tbButton.iBitmap	= I_IMAGENONE;
			tbButton.idCommand	= IDOK;
			tbButton.fsState	= TBSTATE_ENABLED;
			tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
			tbButton.dwData		= 0;
			tbButton.iString	= (int)(LPCTSTR)strButton;

			pCommandBar->InsertButton(0, &tbButton);

			strButton.LoadString(IDS_REGI_TRYMORE);

			tbButton.iBitmap	= I_IMAGENONE;
			tbButton.idCommand	= IDCANCEL;
			tbButton.fsState	= TBSTATE_ENABLED;
			tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
			tbButton.dwData		= 0;
			tbButton.iString	= (int)(LPCTSTR)strButton;

			pCommandBar->InsertButton(1, &tbButton);

			m_strPeriod.Format(IDS_REGI_TIMELEFT, nRemain);
		}
		break;
	case ILM_INVALID:
	case ILM_EXPIRED:
		{
			strButton.LoadString(IDS_REGI_REGISTER);

			tbButton.iBitmap	= I_IMAGENONE;
			tbButton.idCommand	= IDOK;
			tbButton.fsState	= TBSTATE_ENABLED;
			tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
			tbButton.dwData		= 0;
			tbButton.iString	= (int)(LPCTSTR)strButton;

			pCommandBar->InsertButton(0, &tbButton);

			strButton.LoadString(IDS_REGI_CLOSE);

			tbButton.iBitmap	= I_IMAGENONE;
			tbButton.idCommand	= IDCANCEL;
			tbButton.fsState	= TBSTATE_ENABLED;
			tbButton.fsStyle	= TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
			tbButton.dwData		= 0;
			tbButton.iString	= (int)(LPCTSTR)strButton;

			pCommandBar->InsertButton(1, &tbButton);

			m_strPeriod.LoadString(IDS_REGI_TIMEEXPIRED);
		}
		break;
	}

	UpdateData(FALSE);
	return TRUE;
}

void CRegisterDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlEMailH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlEMailH.MoveWindow(&rc);

	m_ctlEMail.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlEMail.MoveWindow(&rc);

	m_ctlSerialH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlSerialH.MoveWindow(&rc);

	m_ctlSerial.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlSerial.MoveWindow(&rc);

	m_ctlPeriod.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlPeriod.MoveWindow(&rc);
}

void CRegisterDialogX::OnOK()
{
	CString	strSerial	= _T("00000000");
	CString	strMessage;
	CString	strTitle;

	if (m_nStatus == ILM_ACTIVATED)
	{
		COptionsDialogX::OnCancel();
		return;
	}

	UpdateData(TRUE);

	strTitle.LoadString(IDS_REGI_TITLE);

	if (IsValidMailX(m_strEMail))
	{
		DWORD	dwVersion	= MakeVersionNumberX(m_wProductID, m_wVersion);
		DWORD	dwSerial	= CalculateSerialNumberX(m_strEMail, m_wProductID, m_wVersion);

		strSerial.Format(_T("%08X"), dwSerial);

		if (!m_strSerial.CompareNoCase(strSerial))
		{
			if (WriteActivateBinaryX(m_strBinary, dwVersion, dwSerial))
			{
				strMessage.LoadString(IDS_REGI_SUCCESS);
				MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);

				m_nStatus	= ILM_ACTIVATED;

				COptionsDialogX::OnOK();
				return;
			}
		}
	}

	strMessage.LoadString(IDS_REGI_FAILED);
	MessageBox(strMessage, strTitle, MB_OK | MB_ICONERROR);
}
