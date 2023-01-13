// PowerPage.cpp

#include "stdafx.h"
#include "InGauge.h"
#include "InSuite_Messages.h"
#include "PowerPage.h"
#include "ItemTags.h"
#include "snapi.h"

IMPLEMENT_DYNAMIC(CPowerPage, CPropertyPageX)

CPowerPage::CPowerPage() : CPropertyPageX(CPowerPage::IDD)
{
	m_hRegNotify	= NULL;
}

CPowerPage::~CPowerPage()
{
}

void CPowerPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_POWER, m_ctlBattery);
}

BEGIN_MESSAGE_MAP(CPowerPage, CPropertyPageX)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_GG_BATTERYSTATUS, &CPowerPage::OnBatteryStatus)
END_MESSAGE_MAP()

void CPowerPage::SetItems()
{
	int						i;
	SYSTEM_POWER_STATUS_EX2	spsex2;

	GetSystemPowerStatusEx2(&spsex2, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE);
	m_arrItems.RemoveAll();

	if ((spsex2.BatteryFlag & BATTERY_FLAG_NO_BATTERY) != BATTERY_FLAG_NO_BATTERY)
	{
		m_arrItems.Add(TAG_MAIN_BATTERY);
	}

	if ((spsex2.BackupBatteryFlag & BATTERY_FLAG_NO_BATTERY) != BATTERY_FLAG_NO_BATTERY)
	{
		m_arrItems.Add(TAG_BACKUP_BATTERY);
	}

	m_ctlBattery.ResetContent();

	for (i = 0; i < m_arrItems.GetCount(); i++)
	{
		m_ctlBattery.AddString(m_arrItems.GetAt(i));
	}
}

void CPowerPage::Refresh()
{
	SetItems();
	Invalidate();
}

BOOL CPowerPage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	RegisterCallback();
	SetItems();
	return TRUE;
}

void CPowerPage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;

	rc.left		= DRA::SCALEX(0);
	rc.top		= DRA::SCALEY(1);
	rc.right	= cx - DRA::SCALEX(0);
	rc.bottom	= cy - DRA::SCALEY(1);

	m_ctlBattery.MoveWindow(&rc);
}

void CPowerPage::OnDestroy()
{
	CPropertyPageX::OnDestroy();
	UnregisterCallback();
}

LRESULT CPowerPage::OnBatteryStatus(WPARAM wParam, LPARAM lParam)
{
	Refresh();
	return 0;
}

void CPowerPage::RegisterCallback()
{
	NOTIFICATIONCONDITION	nc;

	nc.ctComparisonType	= REG_CT_ANYCHANGE;
	nc.dwMask			= SN_POWERBATTERYSTATE_BITMASK;
	nc.TargetValue.dw	= 0;

	RegistryNotifyWindow(SN_POWERBATTERYSTATE_ROOT, SN_POWERBATTERYSTATE_PATH, SN_POWERBATTERYSTATE_VALUE, this->GetSafeHwnd(), WM_GG_BATTERYSTATUS, 0, &nc, &m_hRegNotify);
}

void CPowerPage::UnregisterCallback()
{
	RegistryCloseNotification(m_hRegNotify);
}
