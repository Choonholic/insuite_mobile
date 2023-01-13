// UnzipDialogX.cpp

#include "stdafx.h"
#include "resourceppc.h"
#include "InLibrary_Messages.h"
#include "DisplayX.h"
#include "UnzipDialogX.h"
#include "FileSearchX.h"
#include "ShellX.h"

IMPLEMENT_DYNAMIC(CUnzipDialogX, COptionsDialogX)

CUnzipDialogX::CUnzipDialogX(CWnd* pParent /*=NULL*/) : COptionsDialogX(IDD_PROCESS_DOUBLE_DIALOG, pParent)
{
	m_hZip			= 0;
	m_uiCodePage	= CP_ACP;
	m_ullUnzipped	= 0;
	m_ullTotal		= 0;
	m_nCurrent		= 0;
	m_nTotal		= 0;
	m_pThread		= NULL;
	m_bAbort		= FALSE;
	m_bSafeToClose	= FALSE;
	m_bShowResult	= FALSE;
}

CUnzipDialogX::~CUnzipDialogX()
{
}

void CUnzipDialogX::DoDataExchange(CDataExchange* pDX)
{
	COptionsDialogX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRCD_FILENAME, m_ctlFilename);
	DDX_Control(pDX, IDC_PRCD_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_PRCD_TOTAL, m_ctlTotal);
	DDX_Control(pDX, IDC_PRCD_STATUS, m_ctlStatus);
}

BEGIN_MESSAGE_MAP(CUnzipDialogX, COptionsDialogX)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_UNZIP_THREAD_FINISHED, &CUnzipDialogX::OnThreadFinished)
END_MESSAGE_MAP()

void CUnzipDialogX::SetSource(LPCTSTR lpszFileName, const char* szPassword)
{
	HZIP	hZip;

	m_strFileName	= lpszFileName;

	if (szPassword != NULL)
	{
		strcpy_s(m_szPassword, 1024, szPassword);
	}
	else
	{
		m_szPassword[0]	= NULL;
	}

	hZip	= OpenZip(m_strFileName, (strlen(m_szPassword) ? m_szPassword : NULL));

	if (hZip != 0)
	{
		CloseZip(hZip);
	}
	else
	{
		m_strFileName.Empty();

		m_szPassword[0]	= NULL;
		m_nTotal		= 0;
		m_ullTotal		= 0;
	}
}

void CUnzipDialogX::SetDestination(LPCTSTR lpszFolder)
{
	m_strDestination	= lpszFolder;

	if ((m_strDestination.GetLength() > 1) && (m_strDestination.Right(1) != _T("\\")))
	{
		m_strDestination += _T("\\");
	}
}

void CUnzipDialogX::SetCodePage(UINT uiCodePage)
{
	m_uiCodePage	= uiCodePage;

	UnzipCodePage(m_uiCodePage);
}

void CUnzipDialogX::AddFile(LPCTSTR lpszFile)
{
	HZIP		hZip;
	int			nIndex;
	ZIPENTRY	ze;

	hZip	= OpenZip(m_strFileName, (strlen(m_szPassword) ? m_szPassword : NULL));

	if (hZip != 0)
	{
		FindZipItem(hZip, lpszFile, 1, &nIndex, &ze);

		if (nIndex != -1)
		{
			m_nTotal++;
			m_ullTotal	+= ze.unc_size;

			m_arrIndexes.Add(ze.index);
		}

		CloseZip(hZip);
	}
}

void CUnzipDialogX::AddFiles(CStringArray &arrFiles)
{
	HZIP		hZip;
	int			nIndex;
	ZIPENTRY	ze;

	hZip	= OpenZip(m_strFileName, (strlen(m_szPassword) ? m_szPassword : NULL));

	if (hZip != 0)
	{
		for (int i = 0; i < arrFiles.GetSize(); i++)
		{
			FindZipItem(hZip, arrFiles.GetAt(i), 1, &nIndex, &ze);

			if (nIndex != -1)
			{
				m_nTotal++;
				m_ullTotal	+= ze.unc_size;

				m_arrIndexes.Add(ze.index);
			}
		}

		CloseZip(hZip);
	}
}

void CUnzipDialogX::AddAllFiles()
{
	HZIP		hZip;
	ZIPENTRY	ze;

	hZip	= OpenZip(m_strFileName, (strlen(m_szPassword) ? m_szPassword : NULL));

	if (hZip != 0)
	{
		GetZipItem(hZip, -1, &ze);

		m_nTotal	= ze.index;
		m_ullTotal	= 0;

		for (int i = 0; i < m_nTotal; i++)
		{
			GetZipItem(hZip, i, &ze);
			m_arrIndexes.Add(ze.index);

			m_ullTotal	+= ze.unc_size;
		}

		CloseZip(hZip);
	}
}

void CUnzipDialogX::ClearFiles()
{
	InitializeX();
}

void CUnzipDialogX::ShowResult(BOOL bShow)
{
	m_bShowResult	= bShow;
}

UINT CUnzipDialogX::_UnzipThread(LPVOID pParam)
{
	CUnzipDialogX*	pDialog	= (CUnzipDialogX*)pParam;

	ASSERT(pDialog);
	ASSERT(pDialog->IsKindOf(RUNTIME_CLASS(CUnzipDialogX)));
	pDialog->UnzipThread();
	return 0;
}

void CUnzipDialogX::InitializeX()
{
	m_ullUnzipped	= 0;
	m_ullTotal		= 0;
	m_nCurrent		= 0;
	m_nTotal		= 0;

	if (m_arrIndexes.GetSize())
	{
		m_arrIndexes.RemoveAll();
	}
}

#define UNZIP_BLOCK	16384

void CUnzipDialogX::ExecUnzip()
{
	TCHAR		szFileName[MAX_PATH + 1];
	ZIPENTRY	ze;
	ZRESULT		zr;
	HANDLE		hFile;
	DWORD		dwPos;
	DWORD		dwSize;
	DWORD		dwWritten;
	BYTE		cBuffer[UNZIP_BLOCK];

	GetZipItem(m_hZip, m_arrIndexes.GetAt(m_nCurrent), &ze);
	wsprintf(szFileName, _T("%s%s"), m_strDestination, ze.name);

	hFile	= CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	dwPos	= 0;
	zr		= ZR_MORE;

	UpdateUI(szFileName, dwPos, (DWORD)(ze.unc_size));

	while (zr == ZR_MORE)
	{
		zr				= UnzipItem(m_hZip, m_nCurrent, cBuffer, UNZIP_BLOCK);
		dwSize			= ((zr == ZR_OK) ? ze.unc_size - dwPos : UNZIP_BLOCK);
		dwPos			+= dwSize;
		m_ullUnzipped	+= dwSize;

		WriteFile(hFile, cBuffer, dwSize, &dwWritten, NULL);
		UpdateUI(szFileName, dwPos, (DWORD)(ze.unc_size));
	}

	CloseHandle(hFile);
	UpdateUI(szFileName, 100, 100);
}

void CUnzipDialogX::UpdateUI(LPCTSTR lpszFilename, DWORD dwUnzipped, DWORD dwSize)
{
	CString	strMessage;
	CString	strName;
	CString	strCurrent;
	CString	strTotal;
	DWORD	dwProgress;

	TakeName(lpszFilename, strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
	strName.ReleaseBuffer();
	AfxFormatString1(strMessage, IDS_ZIPX_DECOMPRESSING, strName);
	m_ctlFilename.SetWindowText(strMessage);
	strCurrent.Format(_T("%d"), m_nTotal - m_nCurrent);
	strTotal.Format(_T("%d"), m_nTotal);
	AfxFormatString2(strMessage, IDS_ZIPX_REMAINU, strCurrent, strTotal);
	m_ctlStatus.SetWindowText(strMessage);
	m_ctlTotal.SetPos(static_cast<int>(m_ullUnzipped));

	dwProgress	= (DWORD)((ULONGLONG)((ULONGLONG)dwUnzipped * 100) / (ULONGLONG)dwSize);

	m_ctlProgress.SetPos(dwProgress);
}

void CUnzipDialogX::UnzipThread()
{
	CString	strTitle;
	CString	strMessage;

	m_hZip	= OpenZip(m_strFileName, (strlen(m_szPassword) ? m_szPassword : NULL));

	if (m_hZip == 0)
	{
		PostMessage(WM_UNZIP_THREAD_FINISHED);
		return;
	}

	m_nCurrent	= 0;

	for (int i = 0; i < m_nTotal; i++)
	{
		ExecUnzip();

		m_nCurrent++;

		if (m_bAbort)
		{
			CloseZip(m_hZip);
			PostMessage(WM_UNZIP_THREAD_FINISHED);
			return;
		}
	}

	CloseZip(m_hZip);

	if (m_bShowResult)
	{
		strTitle.LoadString(IDS_ZIPX_HEADERU);
		strMessage.LoadString(IDS_ZIPX_COMPLETE);
		MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
	}

	PostMessage(WM_UNZIP_THREAD_FINISHED);
}

BOOL CUnzipDialogX::OnInitDialog()
{
	COptionsDialogX::OnInitDialog();

	SetHeader(IDS_ZIPX_HEADERU);

	CString	strMessage;

	if (m_strFileName.IsEmpty())
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	if (m_nTotal < 1)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	m_ctlProgress.SetRange32(0, 100);
	m_ctlTotal.SetRange32(0, static_cast<int>(m_ullTotal));
	strMessage.LoadString(IDS_ZIPX_PREPARINGU);
	m_ctlFilename.SetWindowText(strMessage);
	strMessage.LoadString(IDS_ZIPX_CALCULATING);
	m_ctlStatus.SetWindowText(strMessage);

	m_pThread	= AfxBeginThread(_UnzipThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	if (m_pThread == NULL)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	HideDoneButton();

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

	m_pThread->m_bAutoDelete	= FALSE;

	m_pThread->ResumeThread();
	return TRUE;
}

void CUnzipDialogX::OnDestroy()
{
	ShowDoneButton();

	if (m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);

		delete m_pThread;

		m_pThread	= NULL;
	}

	COptionsDialogX::OnDestroy();
}

void CUnzipDialogX::OnSize(UINT nType, int cx, int cy)
{
	COptionsDialogX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlFilename.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlFilename.MoveWindow(&rc);

	m_ctlProgress.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlProgress.MoveWindow(&rc);

	m_ctlTotal.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlTotal.MoveWindow(&rc);

	m_ctlStatus.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlStatus.MoveWindow(&rc);
}

void CUnzipDialogX::OnOK()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CUnzipDialogX::OnCancel()
{
	m_bAbort	= TRUE;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
}

void CUnzipDialogX::OnClose()
{
	if (m_bSafeToClose)
	{
		COptionsDialogX::OnClose();
	}
	else
	{
		m_bAbort	= TRUE;

		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	}
}

LRESULT CUnzipDialogX::OnThreadFinished(WPARAM wParam, LPARAM lParam)
{
	m_bSafeToClose	= TRUE;

	if (m_hZip != 0)
	{
		CloseZip(m_hZip);
	}

	if (m_bAbort)
	{
		EndDialog(IDCANCEL);
	}
	else if (wParam)
	{
		EndDialog(IDCANCEL);
	}
	else
	{
		EndDialog(IDOK);
	}

	return 0L;
}
