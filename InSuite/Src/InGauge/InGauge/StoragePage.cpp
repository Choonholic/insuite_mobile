// StoragePage.cpp

#include "stdafx.h"
#include "InGauge.h"
#include "StoragePage.h"
#include "ItemTags.h"
#include "dbt.h"

IMPLEMENT_DYNAMIC(CStoragePage, CPropertyPageX)

CStoragePage::CStoragePage() : CPropertyPageX(CStoragePage::IDD)
{

}

CStoragePage::~CStoragePage()
{
}

void CStoragePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageX::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STORAGE, m_ctlStorage);
}

BEGIN_MESSAGE_MAP(CStoragePage, CPropertyPageX)
	ON_WM_SIZE()
	ON_MESSAGE(WM_DEVICECHANGE, &CStoragePage::OnDeviceChange)
END_MESSAGE_MAP()

void CStoragePage::SetItems()
{
	int				i;
	HANDLE			hStorage;
	WIN32_FIND_DATA	fdMount;

	m_arrItems.RemoveAll();
	m_arrItems.Add(TAG_PROGRAM_MEMORY);
	m_arrItems.Add(TAG_STORAGE_MEMORY);

	hStorage	= FindFirstFlashCard(&fdMount);

	if (hStorage != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (_tcslen(fdMount.cFileName) > 0)
			{
				m_arrItems.Add(fdMount.cFileName);
			}
		}
		while (FindNextFlashCard(hStorage, &fdMount));

		FindClose(hStorage);
	}

	m_ctlStorage.ResetContent();

	for (i = 0; i < m_arrItems.GetCount(); i++)
	{
		m_ctlStorage.AddString(m_arrItems.GetAt(i));
	}
}

void CStoragePage::Refresh()
{
	SetItems();
	Invalidate();
}

BOOL CStoragePage::OnInitDialog()
{
	CPropertyPageX::OnInitDialog();
	SetItems();
	return TRUE;
}

LRESULT CStoragePage::OnDeviceChange(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case DBT_DEVICEARRIVAL:
	case DBT_DEVICEREMOVECOMPLETE:
		{
			Refresh();
		}
		break;
	}

	return 0;
}

void CStoragePage::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPageX::OnSize(nType, cx, cy);

	RECT	rc;

	rc.left		= DRA::SCALEX(0);
	rc.top		= DRA::SCALEY(1);
	rc.right	= cx - DRA::SCALEX(0);
	rc.bottom	= cy - DRA::SCALEY(1);

	m_ctlStorage.MoveWindow(&rc);
}
