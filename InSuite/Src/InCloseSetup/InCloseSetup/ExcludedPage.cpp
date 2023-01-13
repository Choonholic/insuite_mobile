// ExcludedPage.cpp

#include "stdafx.h"
#include "InCloseSetup.h"
#include "InClose_Settings.h"
#include "ExcludedPage.h"
#include "ExcludedType.h"
#include "RegistryMFCX.h"
#include "FileDialogX.h"
#include "ShellX.h"
#include "ResourceX.h"

IMPLEMENT_DYNAMIC(CExcludedPage, CPropertyPageX)

CExcludedPage::CExcludedPage() : CPropertyPageX(CExcludedPage::IDD)
{
}

CExcludedPage::~CExcludedPage()
{
}

void CExcludedPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EX_LIST, m_ctlList);
	DDX_Control(pDX, IDC_EX_ADD, m_ctlAdd);
	DDX_Control(pDX, IDC_EX_REMOVE, m_ctlRemove);
}

BEGIN_MESSAGE_MAP(CExcludedPage, CPropertyPageX)
	ON_BN_CLICKED(IDC_EX_ADD, &CExcludedPage::OnBnClickedExAdd)
	ON_BN_CLICKED(IDC_EX_REMOVE, &CExcludedPage::OnBnClickedExRemove)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CExcludedPage::LoadSettings()
{
	CRegistryMFCX	reg;
	CString			strKey;
	int				nCount;
	CString			strModule;

	m_arrModule.RemoveAll();
	m_arrType.RemoveAll();
	reg.Open(HKEY_CURRENT_USER, RK_CL_EXCLUDED);

	nCount	= reg.ReadDWORD(RV_CL_COUNT, 0);

	reg.Close();

	for (int i = 0; i < nCount; i++)
	{
		strKey.Format(_T("%s\\%03d"), RK_CL_EXCLUDED, i);
		reg.Open(HKEY_CURRENT_USER, strKey);

		strModule	= reg.ReadString(RV_CL_MODULE);

		m_arrModule.Add(strModule);
		m_arrType.Add(reg.ReadDWORD(RV_CL_TYPE));
		reg.Close();
	}
}

void CExcludedPage::SaveSettings()
{
	CRegistryMFCX	reg;
	CString			strKey;
	CString			strDefaultItem;

	reg.Open(HKEY_CURRENT_USER, RK_CL_EXCLUDED);

	strDefaultItem	= reg.ReadString(RV_CL_DEFAULTITEM);

	reg.Close();
	reg.DeleteKey(HKEY_CURRENT_USER, RK_CL_EXCLUDED);
	reg.Open(HKEY_CURRENT_USER, RK_CL_EXCLUDED);
	reg.WriteString(RV_CL_DEFAULTITEM, strDefaultItem);
	reg.WriteDWORD(RV_CL_COUNT, m_arrModule.GetSize());
	reg.Close();

	for (int i = 0; i < m_arrModule.GetSize(); i++)
	{
		strKey.Format(_T("%s\\%03d"), RK_CL_EXCLUDED, i);
		reg.Open(HKEY_CURRENT_USER, strKey);
		reg.WriteString(RV_CL_MODULE, m_arrModule[i]);
		reg.WriteDWORD(RV_CL_TYPE, m_arrType[i]);
		reg.Close();
	}

	CInCloseSetupApp*	pApp	= (CInCloseSetupApp*)AfxGetApp();
	HWND				hWnd	= pApp->FindInClose();

	if (hWnd)
	{
		::SendMessage(hWnd, pApp->WM_CL_SETTINGCHAGE, PP_EXCLUDED, 0);
	}
}

void CExcludedPage::SetList()
{
	CString	strName;

	m_ctlList.ResetContent();

	for (int i = 0; i < m_arrModule.GetSize(); i++)
	{
		TakeFileName(m_arrModule[i], strName.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
		strName.ReleaseBuffer();
		m_ctlList.AddString(strName);
	}
}

BOOL CExcludedPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	LoadSettings();
	SetList();
	return TRUE;
}

void CExcludedPage::OnOK()
{
	SaveSettings();
	CPropertyPageX::OnOK();
}

#define MAX_EXCLUDED	32
#define EXC_MAINFRAME	0x01
#define EXC_DIALOG		0x02
#define EXC_ALWAYS		0x10

void CExcludedPage::OnBnClickedExAdd()
{
	CResourceX	res(AfxGetResourceHandle());

	if (m_arrModule.GetSize() < MAX_EXCLUDED)
	{
		CRegistryMFCX	reg;
		CFileDialogX	dFile;
		CString			strDefault;

		dFile.SetHeader(IDS_EX_BROWSE);
		dFile.SetFilters(_T("*.exe"));
		reg.Open(HKEY_CURRENT_USER, RK_CL_EXCLUDED);

		strDefault	= reg.ReadString(RV_CL_DEFAULTITEM);

		dFile.SetDefaultPath(strDefault);
		reg.Close();

		if (dFile.DoModal() == IDOK)
		{
			for (int i = 0; i < m_arrModule.GetSize(); i++)
			{
				if (!m_arrModule[i].Compare(dFile.GetFullPath()))
				{
					return;
				}
			}

			reg.Open(HKEY_CURRENT_USER, RK_CL_EXCLUDED);
			reg.WriteString(RV_CL_DEFAULTITEM, dFile.GetFullPath());
			reg.Close();

			CExcludedType	dType;

			dType.SetHeader(dFile.GetFile());

			dType.m_bType	= FALSE;
			dType.m_bAlways	= FALSE;

			if (dType.DoModal() == IDOK)
			{
				m_arrModule.Add(dFile.GetFullPath());
				m_arrType.Add((dType.m_bType ? EXC_DIALOG : EXC_MAINFRAME) + (dType.m_bAlways ? EXC_ALWAYS : 0));
				SetList();
			}
		}
	}
}

void CExcludedPage::OnBnClickedExRemove()
{
	int	nIndex	= m_ctlList.GetCurSel();

	if (nIndex >= 0)
	{
		m_arrModule.RemoveAt(nIndex);
		m_arrType.RemoveAt(nIndex);
	}

	SetList();
}

void CExcludedPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rcAdd, rcRemove, rcList;
	SIZE	size;

	m_ctlList.GetWindowRect(&rcList);
	ScreenToClient(&rcList);

	m_ctlAdd.GetWindowRect(&rcAdd);
	ScreenToClient(&rcAdd);

	size.cx		= rcAdd.right - rcAdd.left;
	size.cy		= rcAdd.bottom - rcAdd.top;
	rcAdd.right	= cx - rcList.left;
	rcAdd.left	= rcAdd.right - size.cx;

	m_ctlAdd.MoveWindow(&rcAdd);

	m_ctlRemove.GetWindowRect(&rcRemove);
	ScreenToClient(&rcRemove);

	rcRemove.left	= rcAdd.left;
	rcRemove.right	= rcAdd.right;

	m_ctlRemove.MoveWindow(&rcRemove);

	rcList.right	= rcAdd.left - rcList.left;
	rcList.bottom	= cy - rcList.top;

	m_ctlList.MoveWindow(&rcList);
}
