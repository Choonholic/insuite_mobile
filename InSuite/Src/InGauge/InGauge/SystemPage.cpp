// SystemPage.cpp

#include "stdafx.h"
#include "InGauge.h"
#include "InSuite_Messages.h"
#include "SystemPage.h"
#include "DeviceInfoX.h"
#include "OwnerInfoX.h"
#include "PhoneX.h"
#include "WiFiX.h"
#include "BluetoothX.h"
#include "snapi.h"

IMPLEMENT_DYNAMIC(CSystemPage, CPropertyPageX)

CSystemPage::CSystemPage() : CPropertyPageX(CSystemPage::IDD)
{
#if (_WIN32_WCE >= 0x0502)
	m_hRegNotify[0]	= NULL;
	m_hRegNotify[1]	= NULL;
#endif
}

CSystemPage::~CSystemPage()
{
}

void CSystemPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SYSTEM, m_ctlSystem);
}

BEGIN_MESSAGE_MAP(CSystemPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_DESTROY()
#if (_WIN32_WCE >= 0x0502)
	ON_MESSAGE(WM_GG_WIFISTATUS, &CSystemPage::OnWiFiStatus)
	ON_MESSAGE(WM_GG_BLUETOOTHSTATUS, &CSystemPage::OnBluetoothStatus)
#endif
END_MESSAGE_MAP()

void CSystemPage::SetHeaders()
{
	CString	strItem;

	strItem.LoadString(IDS_HEADER_ITEM);
	m_ctlSystem.InsertColumn(0, strItem, LVCFMT_LEFT, DRA::SCALEX(80), 0);
	strItem.LoadString(IDS_HEADER_VALUE);
	m_ctlSystem.InsertColumn(1, strItem, LVCFMT_LEFT, DRA::SCALEX(140), 1);
}

void CSystemPage::SetItems()
{
	int			nIndex;
	LVITEM		lvi;
	DWORD		dwStyle;
	CString		strItem;
	CString		strValue;
	TCHAR		szBuffer[128];
	DWORD		dwMajor;
	DWORD		dwMinor;
	DWORD		dwValue;
	COwnerInfoX	oix;

	BeginWaitCursor();

	dwStyle	= m_ctlSystem.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT);
	dwStyle	|= LVS_EX_FULLROWSELECT;

	m_ctlSystem.SetExtendedStyle(dwStyle);
	m_ctlSystem.SetRedraw(FALSE);

	if (m_ctlSystem.GetItemCount() > 0)
	{
		m_ctlSystem.DeleteAllItems();
	}

	nIndex	= 0;

	// Model
	strItem.LoadString(IDS_SY_MODEL);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	GetMachineNameX(szBuffer, 128);
	strValue.Format(_T("%s"), szBuffer);
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Version
	strItem.LoadString(IDS_SY_VERSION);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	GetVersionX(&dwMajor, &dwMinor, &dwValue, szBuffer, 128);
	strValue.Format(IDS_SY_VERSION_FORMAT, dwMajor, dwMinor, dwValue, szBuffer);
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Device ID
	strItem.LoadString(IDS_SY_DEVICEID);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	GetDeviceIDX(szBuffer, 128);
	strValue.Format(_T("%s"), szBuffer);
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Name
	strItem.LoadString(IDS_SY_NAME);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	strValue.Format(_T("%s"), oix.GetName());
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Company
	strItem.LoadString(IDS_SY_COMPANY);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	strValue.Format(_T("%s"), oix.GetCompany());
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Address
	strItem.LoadString(IDS_SY_ADDRESS);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	strValue.Format(_T("%s"), oix.GetAddress());
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Telephone
	strItem.LoadString(IDS_SY_TELEPHONE);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	strValue.Format(_T("%s"), oix.GetTelephone());
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// E-Mail
	strItem.LoadString(IDS_SY_EMAIL);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	strValue.Format(_T("%s"), oix.GetEMail());
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Keyboard
	strItem.LoadString(IDS_SY_KEYBOARD);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	RegistryGetDWORD(SN_KEYBOARDPRESENT_ROOT, SN_KEYBOARDPRESENT_PATH, SN_KEYBOARDPRESENT_VALUE, &dwValue);
	strValue.LoadString(dwValue ? IDS_SY_PRESENT : IDS_SY_NOT_PRESENT);
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Camera
	strItem.LoadString(IDS_SY_CAMERA);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	RegistryGetDWORD(SN_CAMERAPRESENT_ROOT, SN_CAMERAPRESENT_PATH, SN_CAMERAPRESENT_VALUE, &dwValue);
	strValue.LoadString(dwValue ? IDS_SY_PRESENT : IDS_SY_NOT_PRESENT);
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// PC Connection
	strItem.LoadString(IDS_SY_PC_CONNECTION);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	RegistryGetDWORD(SN_CRADLEPRESENT_ROOT, SN_CRADLEPRESENT_PATH, SN_CRADLEPRESENT_VALUE, &dwValue);
	strValue.LoadString(dwValue ? IDS_SY_CONNECTED : IDS_SY_NOT_CONNECTED);
	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	if (DetectPhoneEdition())
	{
		// Phone
		BOOL	bPhoneIsWorking	= FALSE;

		strItem.LoadString(IDS_SY_PHONE);

		lvi.mask		= LVIF_TEXT | LVIF_PARAM;
		lvi.iItem		= nIndex;
		lvi.iSubItem	= 0;
		lvi.lParam		= nIndex;
		lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

		m_ctlSystem.InsertItem(&lvi);
		strItem.ReleaseBuffer();

		RegistryGetDWORD(SN_PHONENOSIM_ROOT, SN_PHONENOSIM_PATH, SN_PHONENOSIM_VALUE, &dwValue);

		if (dwValue & SN_PHONENOSIM_BITMASK)
		{
			strValue.LoadString(IDS_SY_NOSIM);
		}
		else if (dwValue & SN_PHONEINVALIDSIM_BITMASK)
		{
			strValue.LoadString(IDS_SY_INVALIDSIM);
		}
		else if (dwValue & SN_PHONEBLOCKEDSIM_BITMASK)
		{
			strValue.LoadString(IDS_SY_BLOCKEDSIM);
		}
		else if (dwValue & SN_PHONERADIOOFF_BITMASK)
		{
			strValue.LoadString(IDS_SY_PHONEOFF);
		}
		else
		{
			strValue.LoadString(IDS_SY_WORKING);

			bPhoneIsWorking	= TRUE;
		}

		m_ctlSystem.SetItemText(nIndex, 1, strValue);

		nIndex++;

		if (bPhoneIsWorking == TRUE)
		{
			// Phone Operator
			strItem.LoadString(IDS_SY_OPERATOR);

			lvi.mask		= LVIF_TEXT | LVIF_PARAM;
			lvi.iItem		= nIndex;
			lvi.iSubItem	= 0;
			lvi.lParam		= nIndex;
			lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

			m_ctlSystem.InsertItem(&lvi);
			strItem.ReleaseBuffer();

			RegistryGetString(SN_PHONEOPERATORNAME_ROOT, SN_PHONEOPERATORNAME_PATH, SN_PHONEOPERATORNAME_VALUE, szBuffer, 128);
			strValue.Format(_T("%s"), szBuffer);
			m_ctlSystem.SetItemText(nIndex, 1, strValue);

			nIndex++;
		}
	}

	// WiFi
	strItem.LoadString(IDS_SY_WIFI);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	dwValue	= GetWiFiStatus();

	if (dwValue == WIFI_ERROR)
	{
		strValue.LoadString(IDS_SY_NOT_PRESENT);
	}
	else
	{
		if (dwValue & WIFI_CONNECTED)
		{
			strValue.LoadString(IDS_SY_CONNECTED);
		}
		else if (dwValue & WIFI_CONNECTING)
		{
			strValue.LoadString(IDS_SY_CONNECTING);
		}
		else if (dwValue & WIFI_NETWORKSAVAILABLE)
		{
			strValue.LoadString(IDS_SY_AVAILABLE);
		}
		else if (dwValue & WIFI_POWERON)
		{
			strValue.LoadString(IDS_SY_POWERON);
		}
		else
		{
			strValue.LoadString(IDS_SY_PRESENT);
		}
	}

	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	// Bluetooth
	strItem.LoadString(IDS_SY_BLUETOOTH);

	lvi.mask		= LVIF_TEXT | LVIF_PARAM;
	lvi.iItem		= nIndex;
	lvi.iSubItem	= 0;
	lvi.lParam		= nIndex;
	lvi.pszText		= strItem.GetBuffer(strItem.GetLength());

	m_ctlSystem.InsertItem(&lvi);
	strItem.ReleaseBuffer();

	dwValue	= GetBluetoothStatus();

	if (dwValue == BT_ERROR)
	{
		strValue.LoadString(IDS_SY_NOT_PRESENT);
	}
	else
	{
		if (dwValue & BT_HANDSFREE_AUDIO)
		{
			strValue.LoadString(IDS_SY_HFAUDIO);
		}
		else if (dwValue & BT_HANDSFREE_CONTROL)
		{
			strValue.LoadString(IDS_SY_HFCONTROL);
		}
		else if (dwValue & BT_A2DP_CONNECTED)
		{
			strValue.LoadString(IDS_SY_A2DPCONNECTED);
		}
		else if (dwValue & BT_DISCOVERABLE)
		{
			strValue.LoadString(IDS_SY_DISCOVERABLE);
		}
		else if (dwValue & WIFI_POWERON)
		{
			strValue.LoadString(IDS_SY_POWERON);
		}
		else
		{
			strValue.LoadString(IDS_SY_PRESENT);
		}
	}

	m_ctlSystem.SetItemText(nIndex, 1, strValue);

	nIndex++;

	m_ctlSystem.SetRedraw(TRUE);
	EndWaitCursor();
}

void CSystemPage::Refresh()
{
	SetItems();
}

BOOL CSystemPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();

#if (_WIN32_WCE >= 0x0502)
	RegisterCallback();
#endif

	SetHeaders();
	Refresh();
	return TRUE;
}

void CSystemPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;

	rc.left		= 0;
	rc.top		= 0;
	rc.right	= cx;
	rc.bottom	= cy - DRA::SCALEY(1);

	m_ctlSystem.MoveWindow(&rc);
	m_ctlSystem.SetAutoColumnWidth();
}

void CSystemPage::OnDestroy()
{
	CPropertyPageX::OnDestroy();

#if (_WIN32_WCE >= 0x0502)
	UnregisterCallback();
#endif
}

#if (_WIN32_WCE >= 0x0502)
LRESULT CSystemPage::OnWiFiStatus(WPARAM wParam, LPARAM lParam)
{
	Refresh();
	return 0;
}

LRESULT CSystemPage::OnBluetoothStatus(WPARAM wParam, LPARAM lParam)
{
	Refresh();
	return 0;
}

void CSystemPage::RegisterCallback()
{
	RegistryNotifyWindow(SN_WIFISTATEPOWERON_ROOT, SN_WIFISTATEPOWERON_PATH, SN_WIFISTATEPOWERON_VALUE, this->GetSafeHwnd(), WM_GG_WIFISTATUS, 0, NULL, &m_hRegNotify[0]);
	RegistryNotifyWindow(SN_BLUETOOTHSTATEPOWERON_ROOT, SN_BLUETOOTHSTATEPOWERON_PATH, SN_BLUETOOTHSTATEPOWERON_VALUE, this->GetSafeHwnd(), WM_GG_BLUETOOTHSTATUS, 0, NULL, &m_hRegNotify[1]);
}

void CSystemPage::UnregisterCallback()
{
	RegistryCloseNotification(m_hRegNotify[0]);
	RegistryCloseNotification(m_hRegNotify[1]);
}
#endif
