// FolderPage.cpp

#include "stdafx.h"
#include "InFont.h"
#include "FolderPage.h"
#include "FontX.h"
#include "ShellX.h"
#include "FolderDialogX.h"
#include "CopyDialogX.h"
#include "RegistryMFCX.h"
#include "FontInfo.h"

IMPLEMENT_DYNAMIC(CFolderPage, CPropertyPageX)

CFolderPage::CFolderPage() : CPropertyPageX(CFolderPage::IDD), m_strFontFolder(_T(""))
{
}

CFolderPage::~CFolderPage()
{
}

void CFolderPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FN_FOLDER, m_strFontFolder);
	DDX_Control(pDX, IDC_FN_FOLDER_H, m_ctlFontFolderH);
	DDX_Control(pDX, IDC_FN_FOLDER, m_ctlFontFolder);
	DDX_Control(pDX, IDC_FN_BROWSE, m_ctlFontBrowse);
}

BEGIN_MESSAGE_MAP(CFolderPage, CPropertyPageX)
	ON_BN_CLICKED(IDC_FN_BROWSE, &CFolderPage::OnBnClickedFontBrowse)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CFolderPage::LoadSettings()
{
	GetFontPath(m_strFontFolder.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
}

BOOL CFolderPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlFontFolderH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CFolderPage::OnBnClickedFontBrowse()
{
	CFolderDialogX	dFolder;

	dFolder.SetHeader(IDS_FONT_FOLDER_SELECT);
	dFolder.SetDefaultPath(m_strFontFolder);

	if (dFolder.DoModal() == IDOK)
	{
		CString	strMessage;
		CString	strTitle;

		if (m_strFontFolder == dFolder.GetFullPath())
		{
			return;
		}

		strMessage.Format(IDS_FOLDER_CHANGE, dFolder.GetFullPath());
		strTitle.LoadString(IDS_WARNING);

		if (MessageBox(strMessage, strTitle, MB_YESNO | MB_ICONWARNING) == IDYES)
		{
			HANDLE			hFind;
			WIN32_FIND_DATA	fd;
			CCopyDialogX	dCD;
			CString			strFindPath;
			CString			strSource;
			CString			strTarget;

			CombineFileName(strFindPath.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strFontFolder, _T("*.ttf"));
			strFindPath.ReleaseBuffer();

			hFind	= FindFirstFile(strFindPath, &fd);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					CombineFileName(strSource.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strFontFolder, fd.cFileName);
					strSource.ReleaseBuffer();
					dCD.AddFile(strSource);
				} while (FindNextFile(hFind, &fd));

				FindClose(hFind);
			}

			CombineFileName(strFindPath.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strFontFolder, _T("*.ttc"));
			strFindPath.ReleaseBuffer();

			hFind	= FindFirstFile(strFindPath, &fd);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					CombineFileName(strSource.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strFontFolder, fd.cFileName);
					strSource.ReleaseBuffer();
					dCD.AddFile(strSource);
				} while (FindNextFile(hFind, &fd));

				FindClose(hFind);
			}

			CombineFileName(strFindPath.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strFontFolder, _T("*.ac3"));
			strFindPath.ReleaseBuffer();

			hFind	= FindFirstFile(strFindPath, &fd);

			if (hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					CombineFileName(strSource.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strFontFolder, fd.cFileName);
					strSource.ReleaseBuffer();
					dCD.AddFile(strSource);
				} while (FindNextFile(hFind, &fd));

				FindClose(hFind);
			}

			dCD.SetSource(m_strFontFolder);
			dCD.SetDestination(dFolder.GetFullPath());
			dCD.DoModal();
			SetFontPath(dFolder.GetFullPath());

			CRegistryMFCX	reg;
			BOOL			bFound;
			CString			strValue;
			CString			strData;
			CString			strFontLink;

			reg.Open(HKEY_LOCAL_MACHINE, SYSLINK_KEY);

			bFound	= reg.GetFirstValue(strValue);

			while (bFound == TRUE)
			{
				strFontLink	= reg.ReadString(strValue);

				if (strFontLink.Find(_T("\\windows\\")) != -1)
				{
					strFontLink.Replace(_T("\\windows\\"), _T("\\Windows\\"));
				}

				strFontLink.Replace(m_strFontFolder, dFolder.GetFullPath());
				reg.WriteString(strValue, strFontLink);

				bFound	= reg.GetNextValue(strValue);
			}

			reg.Close();

			m_strFontFolder	= dFolder.GetFullPath();

			UpdateData(FALSE);
		}
	}
}

void CFolderPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rcFontFolderH;
	RECT	rcFontFolder;
	RECT	rcFontBrowse;
	SIZE	size;

	m_ctlFontFolderH.GetWindowRect(&rcFontFolderH);
	m_ctlFontFolder.GetWindowRect(&rcFontFolder);
	m_ctlFontBrowse.GetWindowRect(&rcFontBrowse);
	ScreenToClient(&rcFontFolderH);
	ScreenToClient(&rcFontFolder);
	ScreenToClient(&rcFontBrowse);

	rcFontFolderH.right	= cx - rcFontFolderH.left;
	size.cx				= rcFontBrowse.right - rcFontBrowse.left;
	rcFontBrowse.right	= cx - rcFontFolderH.left;
	rcFontBrowse.left	= rcFontBrowse.right - size.cx;
	rcFontFolder.right	= rcFontBrowse.left - DRA::SCALEX(3);

	m_ctlFontFolderH.MoveWindow(&rcFontFolderH);
	m_ctlFontFolder.MoveWindow(&rcFontFolder);
	m_ctlFontBrowse.MoveWindow(&rcFontBrowse);
}

void CFolderPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlFontFolderH);
	CPropertyPageX::OnPaint();
}
