// GeneralPage.cpp

#include "stdafx.h"
#include "InRegistry.h"
#include "InSuite_Settings.h"
#include "InRegistry_Settings.h"
#include "GeneralPage.h"
#include "RegistryMFCX.h"
#include "ShellX.h"
#include "FontX.h"

IMPLEMENT_DYNAMIC(CGeneralPage, CPropertyPageX)

CGeneralPage::CGeneralPage() : CPropertyPageX(CGeneralPage::IDD)
{
}

CGeneralPage::~CGeneralPage()
{
}

void CGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_GE_MAKE_DEFAULT, m_bMakeDefault);
	DDX_Control(pDX, IDC_GE_ASSOCIATION_H, m_ctlAssociationH);
}

BEGIN_MESSAGE_MAP(CGeneralPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

#define RK_EXT_REG				_T(".reg")
#define RK_CLASS_REG			_T("regfile")
#define RK_DEFAULTICON			_T("regfile\\DefaultIcon")
#define RK_SHELL_OPEN_COMMAND	_T("regfile\\Shell\\Open\\Command")
#define RV_DEFAULT				_T("Default")
#define RV_OLD_ENTRY			_T("Old Entry")
#define RD_CLASS_DESCRIPTION	_T("Registration Entries")
#define FM_ICONLINK				_T("%s,-%d")
#define FM_OPENLINK				_T("\"%s\" %%1")

void CGeneralPage::LoadSettings()
{
	CRegistryMFCX	reg;
	CString			strValue;
	CString			strFolder;
	CString			strLink;
	TCHAR			szLink[MAX_PATH + 1];

	m_bDefault	= TRUE;

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	strFolder	= reg.ReadString(RV_IS_FOLDER);

	reg.Close();
	CombineFileName(szLink, MAX_PATH + 1, strFolder, MD_RG_MAIN);

	if (reg.Open(HKEY_CLASSES_ROOT, RK_EXT_REG, FALSE))
	{
		strValue	= reg.ReadString(RV_DEFAULT);

		reg.Close();

		if (strValue.Compare(RK_CLASS_REG))
		{
			m_bDefault	= FALSE;
		}

	}
	else
	{
		m_bDefault	= FALSE;
	}

	if (reg.Open(HKEY_CLASSES_ROOT, RK_CLASS_REG, FALSE))
	{
		strValue	= reg.ReadString(RV_DEFAULT);

		reg.Close();

		if (strValue.Compare(RD_CLASS_DESCRIPTION))
		{
			m_bDefault	= FALSE;
		}
	}
	else
	{
		m_bDefault	= FALSE;
	}

	if (reg.Open(HKEY_CLASSES_ROOT, RK_DEFAULTICON, FALSE))
	{
		strValue	= reg.ReadString(RV_DEFAULT);

		reg.Close();

		strLink.Format(FM_ICONLINK, szLink, IDR_MAINFRAME);

		if (strValue.Compare(strLink))
		{
			m_bDefault	= FALSE;
		}
	}
	else
	{
		m_bDefault	= FALSE;
	}

	if (reg.Open(HKEY_CLASSES_ROOT, RK_SHELL_OPEN_COMMAND, FALSE))
	{
		strValue	= reg.ReadString(RV_DEFAULT);

		reg.Close();
		strLink.Format(FM_OPENLINK, szLink);

		if (strValue.Compare(strLink))
		{
			m_bDefault	= FALSE;
		}
	}
	else
	{
		m_bDefault	= FALSE;
	}

	m_bMakeDefault	= m_bDefault;
}

void CGeneralPage::SaveSettings()
{
	CRegistryMFCX	reg;
	CString			strOldValue;
	CString			strFolder;
	CString			strLink;
	TCHAR			szLink[MAX_PATH + 1];

	reg.Open(HKEY_CURRENT_USER, RK_IS_ROOT);

	strFolder	= reg.ReadString(RV_IS_FOLDER);

	reg.Close();
	CombineFileName(szLink, MAX_PATH + 1, strFolder, MD_RG_MAIN);

	if (m_bMakeDefault)
	{
		if (!m_bDefault)
		{
			reg.Open(HKEY_CLASSES_ROOT, RK_EXT_REG);

			strOldValue	= reg.ReadString(RV_DEFAULT);

			reg.WriteString(RV_OLD_ENTRY, strOldValue);
			reg.WriteString(RV_DEFAULT, RK_CLASS_REG);
			reg.Close();

			reg.Open(HKEY_CLASSES_ROOT, RK_CLASS_REG);

			strOldValue	= reg.ReadString(RV_DEFAULT);

			reg.WriteString(RV_OLD_ENTRY, strOldValue);
			reg.WriteString(RV_DEFAULT, RD_CLASS_DESCRIPTION);
			reg.Close();

			reg.Open(HKEY_CLASSES_ROOT, RK_DEFAULTICON);

			strOldValue	= reg.ReadString(RV_DEFAULT);

			strLink.Format(FM_ICONLINK, szLink, IDR_MAINFRAME);

			reg.WriteString(RV_OLD_ENTRY, strOldValue);
			reg.WriteString(RV_DEFAULT, strLink);
			reg.Close();

			reg.Open(HKEY_CLASSES_ROOT, RK_SHELL_OPEN_COMMAND);

			strOldValue	= reg.ReadString(RV_DEFAULT);

			strLink.Format(FM_OPENLINK, szLink);

			reg.WriteString(RV_OLD_ENTRY, strOldValue);
			reg.WriteString(RV_DEFAULT, strLink);
			reg.Close();
		}
	}
	else
	{
		if (m_bDefault)
		{
			reg.Open(HKEY_CLASSES_ROOT, RK_EXT_REG);

			strOldValue	= reg.ReadString(RV_OLD_ENTRY);

			reg.WriteString(RV_DEFAULT, strOldValue);
			reg.DeleteValue(RV_OLD_ENTRY);
			reg.Close();

			reg.Open(HKEY_CLASSES_ROOT, RK_CLASS_REG);

			strOldValue	= reg.ReadString(RV_OLD_ENTRY);

			reg.WriteString(RV_DEFAULT, strOldValue);
			reg.DeleteValue(RV_OLD_ENTRY);
			reg.Close();

			reg.Open(HKEY_CLASSES_ROOT, RK_DEFAULTICON);

			strOldValue	= reg.ReadString(RV_OLD_ENTRY);

			reg.WriteString(RV_DEFAULT, strOldValue);
			reg.DeleteValue(RV_OLD_ENTRY);
			reg.Close();

			reg.Open(HKEY_CLASSES_ROOT, RK_SHELL_OPEN_COMMAND);

			strOldValue	= reg.ReadString(RV_OLD_ENTRY);

			reg.WriteString(RV_DEFAULT, strOldValue);
			reg.DeleteValue(RV_OLD_ENTRY);
			reg.Close();
		}
	}
}

BOOL CGeneralPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetHeaderItem(&m_ctlAssociationH);
	LoadSettings();
	UpdateData(FALSE);
	return TRUE;
}

void CGeneralPage::OnOK()
{
	UpdateData(TRUE);
	SaveSettings();
	CPropertyPageX::OnOK();
}

void CGeneralPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;

	m_ctlAssociationH.GetWindowRect(&rc);
	ScreenToClient(&rc);

	rc.right	= cx - rc.left;

	m_ctlAssociationH.MoveWindow(&rc);
}

void CGeneralPage::OnPaint()
{
	CPaintDC	dc(this);

	DrawHeaderItem(dc, &m_ctlAssociationH);
	CPropertyPageX::OnPaint();
}
