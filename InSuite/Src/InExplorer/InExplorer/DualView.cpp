// DualView.cpp

#include "stdafx.h"
#include "InExplorer.h"
#include "InExplorer_Settings.h"
#include "MainFrm.h"
#include "DisplayX.h"
#include "ShellX.h"
#include "DeviceInfoX.h"
#include "FolderDialogX.h"
#include "CopyDialogX.h"
#include "MoveDialogX.h"
#include "DeleteDialogX.h"
#include "PropertiesDialogX.h"
#include "ZipDialogX.h"
#include "UnzipDialogX.h"
#include "FileSaveDialogX.h"
#include "RunDialog.h"
#include "FindDialog.h"
#include "ItemSearchX.h"
#include "FoundItems.h"
#include "OpenWithDialog.h"
#include "EditorName.h"
#include "RegistryMFCX.h"
#include "ZipX.h"
#include "UnzipX.h"
#include "FontX.h"
#include "OptionsSheet.h"
#include "AboutDialog.h"
#include "DualView.h"

#ifndef HDR_DES_FLAG
#define HDR_DES_FLAG		0x10
#define HDR_NAME_ASC		(HDR_NAME)
#define HDR_SIZE_ASC		(HDR_SIZE)
#define HDR_TYPE_ASC		(HDR_TYPE)
#define HDR_ATTRIBUTE_ASC	(HDR_ATTRIBUTE)
#define HDR_MODIFIED_ASC	(HDR_MODIFIED)
#define HDR_NAME_DES		(HDR_NAME + HDR_DES_FLAG)
#define HDR_SIZE_DES		(HDR_SIZE + HDR_DES_FLAG)
#define HDR_TYPE_DES		(HDR_TYPE + HDR_DES_FLAG)
#define HDR_ATTRIBUTE_DES	(HDR_ATTRIBUTE + HDR_DES_FLAG)
#define HDR_MODIFIED_DES	(HDR_MODIFIED + HDR_DES_FLAG)
#endif

#ifndef HDR_NAME_WIDTH
#define HDR_NAME_WIDTH		DRA::SCALEX(160)
#define HDR_SIZE_WIDTH		DRA::SCALEX(60)
#define HDR_TYPE_WIDTH		DRA::SCALEX(80)
#define HDR_ATTRIBUTE_WIDTH	DRA::SCALEX(60)
#define HDR_MODIFIED_WIDTH	DRA::SCALEX(100)
#endif

#ifndef CBMODE_NONE
#define CBMODE_NONE	0
#define CBMODE_CUT	1
#define CBMODE_COPY	2
#endif

#define TID_RENAME			0x0814
#define	TID_UPDATE			0x0815
#define	TID_RESIZE			0x0816
#define	MINIMUM_RENAME		500
#define	MINIMUM_UPDATE		500
#define	MINIMUM_RESIZE		200
#define FOLDERBAR_HEIGHT	DRA::SCALEY(20)
#define STATUSBAR_HEIGHT	DRA::SCALEY(20)
#define BEAM_EXEC			_T("beam.exe")

enum ITEM_TYPE
{
	ITEM_NEW,
	ITEM_FOLDER,
	ITEM_FOLDER_NOITEM
};

int	CALLBACK	SortNameAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortNameDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortSizeAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortSizeDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortTypeAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortTypeDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortAttrAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortAttrDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortModiAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
int	CALLBACK	SortModiDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

IMPLEMENT_DYNCREATE(CDualView, CFormView)

CDualView::CDualView() : CFormView(CDualView::IDD)
{
	m_bVertical			= TRUE;
	m_nShowMode			= VIEW_STANDARD;
	m_bShowFolderBar	= TRUE;
	m_bShowStatusBar	= TRUE;
	m_bAutoWidth		= FALSE;
	m_nMaxAutoWidth		= DVIEW_MAX_AUTOWIDTH;
	m_bFullRowSelect	= TRUE;
	m_bNameEditor		= TRUE;
	m_bDoubleTap		= TRUE;
	m_bMultiSelect		= FALSE;
	m_bDateFormat		= TRUE;
	m_bSizeFormat		= TRUE;
	m_bShowExt			= TRUE;
	m_bShowROM			= TRUE;
	m_bShowSystem		= TRUE;
	m_bShowHidden		= TRUE;

	m_nSplit			= 0;
	m_nSplitDistance	= DRA::SCALEY(4);
	m_nTrackDistance	= 0;
	m_nTracking			= 0;
	m_bTracking			= FALSE;
	m_bFolderBarOpen	= FALSE;
	m_nSortItem			= HDR_NAME;

	m_strFolder			= _T("");
	m_strDefaultItem	= _T("\\");

	m_nCBMode			= CBMODE_NONE;
	m_bCBSingle			= TRUE;
	m_strCBFolder		= _T("");

	if (m_arrFiles.GetSize())
	{
		m_arrFiles.RemoveAll();
	}

	m_arrFilters.Add(_T("*.*"));

	m_bUseItems[HDR_NAME]		= TRUE;
	m_bUseItems[HDR_SIZE]		= TRUE;
	m_bUseItems[HDR_TYPE]		= TRUE;
	m_bUseItems[HDR_ATTRIBUTE]	= TRUE;
	m_bUseItems[HDR_MODIFIED]	= TRUE;

	m_nRenameTimerID		= 0;
	m_nUpdateTimerID		= 0;
	m_nResizeTimerID		= 0;
	m_dwLastUpdateTicks		= 0;
	m_dwLastResizeTicks		= 0;
	m_nSizeType				= 0;
	m_nSizecx				= 0;
	m_nSizecy				= 0;
}

CDualView::~CDualView()
{
}

void CDualView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOLDERTREE, m_ctlTree);
	DDX_Control(pDX, IDC_FILELIST, m_ctlList);
}

BEGIN_MESSAGE_MAP(CDualView, CFormView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_FOLDERTREE, &CDualView::OnTvnSelchangedFoldertree)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_FOLDERTREE, &CDualView::OnTvnItemexpandedFoldertree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_FOLDERTREE, &CDualView::OnTvnItemexpandingFoldertree)
	ON_NOTIFY(TVN_GETDISPINFO, IDC_FOLDERTREE, &CDualView::OnTvnGetdispinfoFoldertree)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_FOLDERTREE, &CDualView::OnTvnBeginlabeleditFoldertree)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_FOLDERTREE, &CDualView::OnTvnEndlabeleditFoldertree)
	ON_MESSAGE(WM_TCX_TAPANDHOLD, &CDualView::OnTapAndHoldFoldertree)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CDualView::OnHeaderClickList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FILELIST, &CDualView::OnLvnGetdispinfoFilelist)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_FILELIST, &CDualView::OnLvnBeginlabeleditFilelist)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FILELIST, &CDualView::OnLvnEndlabeleditFilelist)
	ON_NOTIFY(NM_CLICK, IDC_FILELIST, &CDualView::OnNMClickFilelist)
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, &CDualView::OnNMDblclkFilelist)
	ON_NOTIFY(NM_SETFOCUS, IDC_FILELIST, &CDualView::OnNMSetfocusFilelist)
	ON_MESSAGE(WM_LCX_TAPANDHOLD, &CDualView::OnTapAndHoldFilelist)
	ON_COMMAND(ID_DUAL_RUN, &CDualView::OnDualRun)
	ON_COMMAND(ID_DUAL_RECEIVE, &CDualView::OnDualReceive)
	ON_COMMAND(ID_DUAL_FIND, &CDualView::OnDualFind)
	ON_COMMAND(ID_DUAL_FOUND_ITEMS, &CDualView::OnDualFoundItems)
	ON_COMMAND(ID_DUAL_MODE_STANDARD, &CDualView::OnDualModeStandard)
	ON_COMMAND(ID_DUAL_MODE_FOLDERONLY, &CDualView::OnDualModeFolderOnly)
	ON_COMMAND(ID_DUAL_MODE_FILEONLY, &CDualView::OnDualModeFileOnly)
	ON_COMMAND(ID_DUAL_LAYOUT, &CDualView::OnDualLayout)
	ON_COMMAND(ID_DUAL_FOLDERBAR, &CDualView::OnDualFolderBar)
	ON_COMMAND(ID_DUAL_STATUSBAR, &CDualView::OnDualStatusBar)
	ON_COMMAND(ID_DUAL_NAMEEDITOR, &CDualView::OnDualNameEditor)
	ON_COMMAND(ID_DUAL_REFRESH, &CDualView::OnDualRefresh)
	ON_COMMAND(ID_DUAL_SWITCH, &CDualView::OnDualSwitch)
	ON_COMMAND(ID_DUAL_OPTIONS, &CDualView::OnDualOptions)
	ON_COMMAND(ID_FOLDER_EXPAND, &CDualView::OnFolderExpand)
	ON_COMMAND(ID_FOLDER_COLLAPSE, &CDualView::OnFolderCollapse)
	ON_COMMAND(ID_FOLDER_CUT, &CDualView::OnFolderCut)
	ON_COMMAND(ID_FOLDER_COPY, &CDualView::OnFolderCopy)
	ON_COMMAND(ID_FOLDER_PASTE, &CDualView::OnFolderPaste)
	ON_COMMAND(ID_FOLDER_PASTE_SHORTCUT, &CDualView::OnFolderPasteShortcut)
	ON_COMMAND(ID_FOLDER_COPY_TO, &CDualView::OnFolderCopyTo)
	ON_COMMAND(ID_FOLDER_MOVE_TO, &CDualView::OnFolderMoveTo)
	ON_COMMAND(ID_FOLDER_DELETE, &CDualView::OnFolderDelete)
	ON_COMMAND(ID_FOLDER_RENAME, &CDualView::OnFolderRename)
	ON_COMMAND(ID_FOLDER_NEW_FOLDER, &CDualView::OnFolderNewFolder)
	ON_COMMAND(ID_FOLDER_PROPERTIES, &CDualView::OnFolderProperties)
	ON_COMMAND(ID_FILE_OPENFILE, &CDualView::OnFileOpen)
	ON_COMMAND(ID_FILE_OPENFILEWITH, &CDualView::OnFileOpenWith)
	ON_COMMAND(ID_FILE_SENDTO_IR, &CDualView::OnFileSendToIR)
	ON_COMMAND(ID_FILE_ZIP_ADD, &CDualView::OnFileZIPAdd)
	ON_COMMAND(ID_FILE_ZIP_EXTRACT, &CDualView::OnFileZIPExtract)
	ON_COMMAND(ID_FILE_CUT, &CDualView::OnFileCut)
	ON_COMMAND(ID_FILE_COPY, &CDualView::OnFileCopy)
	ON_COMMAND(ID_FILE_PASTE, &CDualView::OnFilePaste)
	ON_COMMAND(ID_FILE_PASTE_SHORTCUT, &CDualView::OnFilePasteShortcut)
	ON_COMMAND(ID_FILE_AUTOWIDTH, &CDualView::OnFileAutoWidth)
	ON_COMMAND(ID_FILE_FULLROWSELECT, &CDualView::OnFileFullRowSelect)
	ON_COMMAND(ID_FILE_SELECT_ALL, &CDualView::OnFileSelectAll)
	ON_COMMAND(ID_FILE_INVERT_SELECTION, &CDualView::OnFileInvertSelection)
	ON_COMMAND(ID_FILE_MULTI_SELECT, &CDualView::OnFileMultiSelect)
	ON_COMMAND(ID_FILE_TAP_ACTIVATION, &CDualView::OnFileTapActivation)
	ON_COMMAND(ID_FILE_COPY_TO, &CDualView::OnFileCopyTo)
	ON_COMMAND(ID_FILE_MOVE_TO, &CDualView::OnFileMoveTo)
	ON_COMMAND(ID_FILE_DELETE, &CDualView::OnFileDelete)
	ON_COMMAND(ID_FILE_RENAME, &CDualView::OnFileRename)
	ON_COMMAND(ID_FILE_NEW_FOLDER, &CDualView::OnFileNewFolder)
	ON_COMMAND(ID_FILE_PROPERTIES, &CDualView::OnFileProperties)
	ON_COMMAND(ID_FILE_VIEW_TILES, &CDualView::OnFileViewTiles)
	ON_COMMAND(ID_FILE_VIEW_ICONS, &CDualView::OnFileViewIcons)
	ON_COMMAND(ID_FILE_VIEW_LIST, &CDualView::OnFileViewList)
	ON_COMMAND(ID_FILE_VIEW_DETAILS, &CDualView::OnFileViewDetails)
	ON_COMMAND(ID_ABOUT, &CDualView::OnAbout)
	ON_UPDATE_COMMAND_UI(ID_DUAL_MODE_STANDARD, &CDualView::OnUpdateDualModeStandard)
	ON_UPDATE_COMMAND_UI(ID_DUAL_MODE_FOLDERONLY, &CDualView::OnUpdateDualModeFolderOnly)
	ON_UPDATE_COMMAND_UI(ID_DUAL_MODE_FILEONLY, &CDualView::OnUpdateDualModeFileOnly)
	ON_UPDATE_COMMAND_UI(ID_DUAL_LAYOUT, &CDualView::OnUpdateDualLayout)
	ON_UPDATE_COMMAND_UI(ID_DUAL_FOLDERBAR, &CDualView::OnUpdateDualFolderBar)
	ON_UPDATE_COMMAND_UI(ID_DUAL_STATUSBAR, &CDualView::OnUpdateDualStatusBar)
	ON_UPDATE_COMMAND_UI(ID_DUAL_NAMEEDITOR, &CDualView::OnUpdateDualNameEditor)
	ON_UPDATE_COMMAND_UI(ID_DUAL_SWITCH, &CDualView::OnUpdateDualSwitch)
	ON_UPDATE_COMMAND_UI(ID_FILE_VIEW_TILES, &CDualView::OnUpdateFileViewTiles)
	ON_UPDATE_COMMAND_UI(ID_FILE_VIEW_ICONS, &CDualView::OnUpdateFileViewIcons)
	ON_UPDATE_COMMAND_UI(ID_FILE_VIEW_LIST, &CDualView::OnUpdateFileViewList)
	ON_UPDATE_COMMAND_UI(ID_FILE_VIEW_DETAILS, &CDualView::OnUpdateFileViewDetails)
	ON_UPDATE_COMMAND_UI(ID_FILE_AUTOWIDTH, &CDualView::OnUpdateFileAutoWidth)
	ON_UPDATE_COMMAND_UI(ID_FILE_FULLROWSELECT, &CDualView::OnUpdateFileFullRowSelect)
	ON_UPDATE_COMMAND_UI(ID_FILE_MULTI_SELECT, &CDualView::OnUpdateFileMultiSelect)
	ON_UPDATE_COMMAND_UI(ID_FILE_TAP_ACTIVATION, &CDualView::OnUpdateFileTapActivation)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

#ifdef _DEBUG
void CDualView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDualView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CDualView::LoadGeneralOptions()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_GENERAL);

	switch (reg.ReadDWORD(RV_EX_STARTUP, 0))
	{
	case 0:
		{
			SetDefaultPath(reg.ReadString(RV_EX_LASTOPENED, _T("\\")));
		}
		break;
	case 1:
		{
			SetDefaultPath(reg.ReadString(RV_EX_OPENFOLDER, _T("\\")));
		}
		break;
	}

	SetDoubleTapActivation((BOOL)(reg.ReadDWORD(RV_EX_DOUBLETAP, 1) == 1));
	reg.Close();
}

void CDualView::LoadViewOptions()
{
	CRegistryMFCX	reg;

	reg.Open(HKEY_CURRENT_USER, RK_EX_VIEW);
	SetLayout((BOOL)(reg.ReadDWORD(RV_EX_LAYOUT, 1) == 1));
	SetShowFolderBar((BOOL)(reg.ReadDWORD(RV_EX_FOLDERBAR, 1) == 1));
	SetShowStatusBar((BOOL)(reg.ReadDWORD(RV_EX_STATUSBAR, 1) == 1));
	SetFullRowSelect((BOOL)(reg.ReadDWORD(RV_EX_FULLROWSELECT, 1) == 1));
	SetDetailedDateFormat((BOOL)(reg.ReadDWORD(RV_EX_DATEFORMAT, 1) == 1));
	SetDetailedSizeFormat((BOOL)(reg.ReadDWORD(RV_EX_SIZEFORMAT, 1) == 1));
	reg.Close();
}

void CDualView::LoadBrowserOptions()
{
	CRegistryMFCX	reg;
	DWORD			dwStyle;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	SetShowMode(reg.ReadDWORD(RV_EX_MODE, VIEW_STANDARD));
	SetShowSystem((BOOL)(reg.ReadDWORD(RV_EX_SHOWSYSTEM, 1) == 1));
	SetShowHidden((BOOL)(reg.ReadDWORD(RV_EX_SHOWHIDDEN, 1) == 1));
	SetShowROM((BOOL)(reg.ReadDWORD(RV_EX_SHOWROM, 1) == 1));
	SetShowExtension((BOOL)(reg.ReadDWORD(RV_EX_SHOWEXT, 1) == 1));
	SetAutoWidth((BOOL)(reg.ReadDWORD(RV_EX_AUTOWIDTH, 0) == 1));
	SetMaxAutoWidth(reg.ReadDWORD(RV_EX_MAXAUTOWIDTH, DVIEW_MAX_AUTOWIDTH));
	SetNameEditor((BOOL)(reg.ReadDWORD(RV_EX_NAMEEDITOR, 1) == 1));
	SetMultiSelect((BOOL)(reg.ReadDWORD(RV_EX_MULTISELECT, 0) == 1));

	dwStyle	= reg.ReadDWORD(RV_EX_VIEWSTYLE, LVS_REPORT);

	reg.Close();

	switch (dwStyle)
	{
	case LVS_ICON:
		{
			ModeTiles();
		}
		break;
	case LVS_SMALLICON:
		{
			ModeIcons();
		}
		break;
	case LVS_LIST:
		{
			ModeList();
		}
		break;
	case LVS_REPORT:
		{
			ModeDetails();
		}
		break;
	}
}

void CDualView::SetDefaultPath(LPCTSTR lpszPath)
{
	m_strDefaultItem	= _T("");

	if (lpszPath == NULL)
	{
		m_strDefaultItem	= _T("\\");
	}
	else
	{
		if (lpszPath[0] != _T('\\'))
		{
			m_strDefaultItem	= _T("\\");
		}

		m_strDefaultItem	+= lpszPath;
	}

	if ((m_strDefaultItem.Right(1) == _T("\\")) && (m_strDefaultItem.GetLength() > 1))
	{
		m_strDefaultItem	= m_strDefaultItem.Left(m_strDefaultItem.GetLength() - 1);
	}
}

void CDualView::SetFilters(LPCTSTR lpszFilters)
{
	m_arrFilters.RemoveAll();

	if (lpszFilters != NULL)
	{
		CString	strBase, strParsed;
		int		nPos;

		strBase	= lpszFilters;

		while (strBase.GetLength())
		{
			if ((nPos = strBase.Find(_T('|'))) != -1)
			{
				strParsed	= strBase.Left(nPos);
				strBase		= strBase.Mid(nPos + 1);
			}
			else
			{
				strParsed	= strBase;
				strBase		= _T("");
			}

			if (strParsed.GetLength())
			{
				m_arrFilters.Add(strParsed);
			}
		}
	}
	else
	{
		m_arrFilters.Add(_T("*.*"));
	}
}

void CDualView::SetFilters(UINT nID)
{
	CString	strFilters;

	strFilters.LoadString(nID);
	SetFilters(strFilters);
}

void CDualView::SetHeaderState(int nIndex, BOOL bUse)
{
	if ((nIndex <= HDR_NAME) || (nIndex >= HDR_MAX_ITEMS))
	{
		return;
	}

	m_bUseItems[nIndex]	= bUse;
}

void CDualView::SetLayout(BOOL bVertical)
{
	m_bVertical	= bVertical;
}

void CDualView::SetShowMode(int nMode)
{
	m_nShowMode	= nMode;
}

void CDualView::SetShowFolderBar(BOOL bShow)
{
	m_bShowFolderBar	= bShow;
}

void CDualView::SetShowStatusBar(BOOL bShow)
{
	m_bShowStatusBar	= bShow;
}

void CDualView::SetAutoWidth(BOOL bAuto)
{
	m_bAutoWidth	= bAuto;
}

void CDualView::SetMaxAutoWidth(int nMax)
{
	m_nMaxAutoWidth	= nMax;
}

void CDualView::SetFullRowSelect(BOOL bSet)
{
	m_bFullRowSelect	= bSet;
}

void CDualView::SetNameEditor(BOOL bUse)
{
	m_bNameEditor	= bUse;
}

void CDualView::SetDoubleTapActivation(BOOL bSet)
{
	m_bDoubleTap	= bSet;
}

void CDualView::SetMultiSelect(BOOL bSet)
{
	m_bMultiSelect	= bSet;
}

void CDualView::SetDetailedDateFormat(BOOL bFormat)
{
	m_bDateFormat	= bFormat;
}

void CDualView::SetDetailedSizeFormat(BOOL bFormat)
{
	m_bSizeFormat	= bFormat;
}

void CDualView::SetShowExtension(BOOL bShow)
{
	m_bShowExt	= bShow;
}

void CDualView::SetShowROM(BOOL bShow)
{
	m_bShowROM	= bShow;
}

void CDualView::SetShowSystem(BOOL bShow)
{
	m_bShowSystem	= bShow;
}

void CDualView::SetShowHidden(BOOL bShow)
{
	m_bShowHidden	= bShow;
}

void CDualView::RefreshLayout()
{
	RecalcLayout();
	LayoutControls();
}

void CDualView::RecalcLayout()
{
	RECT	rc;

	GetClientRect(&rc);

	if (m_bShowFolderBar)
	{
		rc.top	+= FOLDERBAR_HEIGHT;
	}

	if (m_bShowStatusBar)
	{
		rc.bottom	-= STATUSBAR_HEIGHT;
	}

	if (m_bVertical == TRUE)
	{
		m_nSplit	= ((rc.bottom - rc.top - m_nSplitDistance) / 2) + rc.top;

		GetClientRect(&m_rcTree);

		m_rcTree.left	= rc.left;
		m_rcTree.top	= rc.top;
		m_rcTree.right	= rc.right;
		m_rcTree.bottom	= m_nSplit;

		GetClientRect(m_rcList);

		m_rcList.left	= rc.left;
		m_rcList.top	= m_nSplit + m_nSplitDistance + DRA::SCALEY(1);
		m_rcList.right	= rc.right;
		m_rcList.bottom	= rc.bottom;
	}
	else
	{
		m_nSplit	= ((rc.right - rc.left - m_nSplitDistance) / 2) + rc.left;

		GetClientRect(&m_rcTree);

		m_rcTree.left	= rc.left;
		m_rcTree.top	= rc.top;
		m_rcTree.right	= m_nSplit;
		m_rcTree.bottom	= rc.bottom;

		GetClientRect(m_rcList);

		m_rcList.left	= m_nSplit + m_nSplitDistance + DRA::SCALEY(1);
		m_rcList.top	= rc.top;
		m_rcList.right	= rc.right;
		m_rcList.bottom	= rc.bottom;
	}
}

void CDualView::LayoutControls()
{
	switch (m_nShowMode)
	{
	case VIEW_STANDARD:
		{
			ModeStandard();
		}
		break;
	case VIEW_FOLDERONLY:
		{
			ModeFolderOnly();
		}
		break;
	case VIEW_FILEONLY:
		{
			ModeFileOnly();
		}
		break;
	}
}

void CDualView::Refresh(LPCTSTR lpszDefaultPath)
{
	BeginWaitCursor();

	if (lpszDefaultPath == NULL)
	{
		SetDefaultPath(SearchFullPath(m_ctlTree.GetSelectedItem()));
	}
	else
	{
		SetDefaultPath(lpszDefaultPath);
	}

	BuildRoot();
	EndWaitCursor();
}

void CDualView::BuildHeaders()
{
	CString	strItem;
	int		nCount	= m_ctlList.GetHeaderCtrl()->GetItemCount();

	for (int i = 0; i < nCount; i++)
	{
		m_ctlList.DeleteColumn(0);
	}

	nCount	= 0;

	strItem.LoadString(IDS_HDR_NAME);
	m_ctlList.InsertColumn(0, strItem, LVCFMT_LEFT, HDR_NAME_WIDTH, 0);

	if (m_bUseItems[HDR_SIZE])
	{
		strItem.LoadString(IDS_HDR_SIZE);
		m_ctlList.InsertColumn(++nCount, strItem, LVCFMT_RIGHT, HDR_SIZE_WIDTH, nCount);
	}

	if (m_bUseItems[HDR_TYPE])
	{
		strItem.LoadString(IDS_HDR_TYPE);
		m_ctlList.InsertColumn(++nCount, strItem, LVCFMT_LEFT, HDR_TYPE_WIDTH, nCount);
	}

	if (m_bUseItems[HDR_ATTRIBUTE])
	{
		strItem.LoadString(IDS_HDR_ATTRIBUTE);
		m_ctlList.InsertColumn(++nCount, strItem, LVCFMT_LEFT, HDR_ATTRIBUTE_WIDTH, nCount);
	}

	if (m_bUseItems[HDR_MODIFIED])
	{
		strItem.LoadString(IDS_HDR_MODIFIED);
		m_ctlList.InsertColumn(++nCount, strItem, LVCFMT_LEFT, HDR_MODIFIED_WIDTH, nCount);
	}
}

void CDualView::SetHeaderDefault()
{
	CHeaderCtrl*	pHeader	= m_ctlList.GetHeaderCtrl();

	if (pHeader)
	{
		HDITEM	hdi;
		int		i;
		int		nItems;

		nItems	= pHeader->GetItemCount();

		for (i = 0; i < nItems; i++)
		{
			hdi.mask	= HDI_WIDTH;

			pHeader->GetItem(i, &hdi);

			switch (GetHeaderIndex(i))
			{
			case HDR_NAME:
				{
					hdi.cxy	= HDR_NAME_WIDTH;
				}
				break;
			case HDR_SIZE:
				{
					hdi.cxy	= HDR_SIZE_WIDTH;
				}
				break;
			case HDR_TYPE:
				{
					hdi.cxy	= HDR_TYPE_WIDTH;
				}
				break;
			case HDR_ATTRIBUTE:
				{
					hdi.cxy	= HDR_ATTRIBUTE_WIDTH;
				}
				break;
			case HDR_MODIFIED:
				{
					hdi.cxy	= HDR_MODIFIED_WIDTH;
				}
				break;
			}

			pHeader->SetItem(i, &hdi);
		}
	}
}

void CDualView::SetColumnIcons()
{
	CHeaderCtrl*	pHeader	= m_ctlList.GetHeaderCtrl();

	if (pHeader)
	{
		HDITEM	hdi;
		int		i;
		int		nItems;

		nItems	= pHeader->GetItemCount();

		for (i = 0; i < nItems; i++)
		{
			hdi.mask	= HDI_IMAGE | HDI_FORMAT;

			pHeader->GetItem(i, &hdi);

			if (i == m_nSortItem)
			{
				hdi.iImage	= 0;
				hdi.fmt		|= HDF_IMAGE | HDF_BITMAP_ON_RIGHT;
			}
			else if (i + HDR_DES_FLAG == m_nSortItem)
			{
				hdi.iImage	= 1;
				hdi.fmt		|= HDF_IMAGE | HDF_BITMAP_ON_RIGHT;
			}
			else
			{
				hdi.iImage	= 2;
				hdi.fmt		&= ~(HDF_IMAGE | HDF_BITMAP_ON_RIGHT);
			}

			pHeader->SetItem(i, &hdi);
		}
	}
}

void CDualView::RefreshHeaderState()
{
	m_arrInfo.RemoveAll();
	m_ctlList.DeleteAllItems();
	BuildHeaders();
	SearchFiles();
}

int CDualView::GetHeaderCount()
{
	int	nCount	= 0;

	for (int i = 0; i < HDR_MAX_ITEMS; i++)
	{
		if (m_bUseItems[i])
		{
			nCount++;
		}
	}

	return nCount;
}

int CDualView::GetHeaderIndex(int nItem)
{
	int	nCount	= 0;

	for (int i = 0; i < HDR_MAX_ITEMS; i++)
	{
		if (m_bUseItems[i])
		{
			nCount++;

			if ((nItem + 1) == nCount)
			{
				return i;
			}
		}
	}

	return -1;
}

void CDualView::BuildRoot()
{
	HTREEITEM		hItemRoot	= TVI_ROOT;
	TCHAR			szRoot[64];
	TVINSERTSTRUCT	tvi;
	int				iIcon		= GetSystemIconIndex(_T("\\"));

	GetDeviceIDX(szRoot, 64);
	m_ctlTree.ModifyStyle(TVS_EDITLABELS, 0);

	if (!m_bNameEditor)
	{
		m_ctlTree.ModifyStyle(0, TVS_EDITLABELS);
	}

	if (m_ctlTree.GetCount())
	{
		m_ctlTree.DeleteAllItems();
	}

	tvi.hParent				= hItemRoot;
	tvi.hInsertAfter		= TVI_LAST;
	tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText		= (LPTSTR)szRoot;
	tvi.item.iImage			= iIcon;
	tvi.item.iSelectedImage	= iIcon;
	tvi.item.cChildren		= I_CHILDRENCALLBACK;
	tvi.item.lParam			= ITEM_NEW;

	HTREEITEM	hItem	= m_ctlTree.InsertItem(&tvi);

	SearchDefaultItem(hItem);
	SearchFiles();
}

int CDualView::SearchFolders(HTREEITEM hParentItem, LPCTSTR lpszFolder)
{
	CString			strFilter;
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	BOOL			bFind;
	int				iIcon;
	TVINSERTSTRUCT	tvi;
	CString			strFile;
	int				nCount	= 0;

	BeginWaitCursor();
	strFilter.Format(_T("%s*.*"), lpszFolder);

	hFind	= FindFirstFile(strFilter, &fd);
	bFind	= (hFind != INVALID_HANDLE_VALUE);

	while (bFind)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strFile.Format(_T("%s%s"), lpszFolder, fd.cFileName);

			iIcon					= GetSystemIconIndex(strFile);
			tvi.hParent				= hParentItem;
			tvi.hInsertAfter		= TVI_SORT;
			tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
			tvi.item.pszText		= fd.cFileName;
			tvi.item.iImage			= iIcon;
			tvi.item.iSelectedImage	= iIcon;
			tvi.item.cChildren		= I_CHILDRENCALLBACK;
			tvi.item.lParam			= ITEM_NEW;

			m_ctlTree.InsertItem(&tvi);

			nCount++;
		}

		bFind	= FindNextFile(hFind, &fd);
	}

	FindClose(hFind);
	EndWaitCursor();
	return nCount;
}

int CDualView::SearchFoldersSpec(HTREEITEM hParentItem, LPCTSTR lpszFolder, LPCTSTR lpszSpec, HTREEITEM& hSpecItem)
{
	CString			strFilter;
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	BOOL			bFind;
	int				iIcon;
	TVINSERTSTRUCT	tvi;
	CString			strFile;
	int				nCount	= 0;

	BeginWaitCursor();
	strFilter.Format(_T("%s*.*"), lpszFolder);

	hSpecItem	= NULL;
	hFind		= FindFirstFile(strFilter, &fd);
	bFind		= (hFind != INVALID_HANDLE_VALUE);

	while (bFind)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strFile.Format(_T("%s%s"), lpszFolder, fd.cFileName);

			iIcon					= GetSystemIconIndex(strFile);
			tvi.hParent				= hParentItem;
			tvi.hInsertAfter		= TVI_SORT;
			tvi.item.mask			= TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
			tvi.item.pszText		= fd.cFileName;
			tvi.item.iImage			= iIcon;
			tvi.item.iSelectedImage	= iIcon;
			tvi.item.cChildren		= I_CHILDRENCALLBACK;
			tvi.item.lParam			= ITEM_NEW;

			HTREEITEM	hItem		= m_ctlTree.InsertItem(&tvi);

			nCount++;

			if (!_tcscmp(fd.cFileName, lpszSpec))
			{
				hSpecItem	= hItem;
			}
		}

		bFind	= FindNextFile(hFind, &fd);
	}

	FindClose(hFind);
	EndWaitCursor();
	return nCount;
}

void CDualView::SearchFiles()
{
	CString			strFilter;
	CString			strFile;
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	BOOL			bFind;
	int				nCount;
	EXPINFOX		tEI;
	LVITEM			lvi;
	DWORD			dwStyle;

	BeginWaitCursor();
	m_ctlList.SetRedraw(FALSE);
	m_ctlList.ModifyStyle(LVS_EDITLABELS, 0);

	if (!m_bNameEditor)
	{
		m_ctlList.ModifyStyle(0, LVS_EDITLABELS);
	}

	dwStyle	= m_ctlList.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT);

	if (m_bMultiSelect)
	{
		dwStyle	|= LVS_EX_CHECKBOXES;
	}

	if ((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
	{
		if (m_bFullRowSelect)
		{
			dwStyle	|= LVS_EX_FULLROWSELECT;
		}
	}

	m_ctlList.SetExtendedStyle(dwStyle);
	m_arrInfo.RemoveAll();
	m_ctlList.DeleteAllItems();

	nCount	= 0;

	for (int i = 0; i < m_arrFilters.GetCount(); i++)
	{
		strFilter.Format(_T("%s%s"), m_strFolder, m_arrFilters.GetAt(i));

		hFind	= FindFirstFile(strFilter, &fd);
		bFind	= (hFind != INVALID_HANDLE_VALUE);

		while (bFind)
		{
			if (!m_bShowROM)
			{
				if (((fd.dwFileAttributes & FILE_ATTRIBUTE_INROM) == FILE_ATTRIBUTE_INROM) || ((fd.dwFileAttributes & FILE_ATTRIBUTE_ROMMODULE) == FILE_ATTRIBUTE_ROMMODULE))
				{
					bFind	= FindNextFile(hFind, &fd);

					continue;
				}
			}

			if (!m_bShowSystem)
			{
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == FILE_ATTRIBUTE_SYSTEM)
				{
					bFind	= FindNextFile(hFind, &fd);

					continue;
				}
			}

			if (!m_bShowHidden)
			{
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN)
				{
					bFind	= FindNextFile(hFind, &fd);

					continue;
				}
			}

			strFile	= m_strFolder;
			strFile	+= fd.cFileName;

			memcpy(&tEI.fd, &fd, sizeof(WIN32_FIND_DATA));
			wcscpy(tEI.szFolder, m_strFolder);
			m_arrInfo.Add(tEI);

			lvi.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.iItem		= nCount;
			lvi.iSubItem	= 0;
			lvi.iImage		= I_IMAGECALLBACK;
			lvi.lParam		= nCount;
			lvi.pszText		= LPSTR_TEXTCALLBACK;

			m_ctlList.InsertItem(&lvi);

			for (int i = 0; i < (GetHeaderCount() - 1); i++)
			{
				m_ctlList.SetItemText(nCount, i + 1, LPSTR_TEXTCALLBACK);
			}

			nCount++;

			bFind	= FindNextFile(hFind, &fd);
		}

		FindClose(hFind);
	}

	SetColumnIcons();
	SortFiles();

	if (m_bAutoWidth == TRUE)
	{
		if ((m_nMaxAutoWidth == 0) || (nCount < m_nMaxAutoWidth))
		{
			m_ctlList.SetAutoColumnWidth();
		}
		else
		{
			SetHeaderDefault();
		}
	}

	m_ctlList.SetRedraw(TRUE);
	EndWaitCursor();
}

void CDualView::SortFiles()
{
	switch (m_nSortItem)
	{
	case HDR_NAME_ASC:
		{
			m_ctlList.SortItems(SortNameAsc, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_NAME_DES:
		{
			m_ctlList.SortItems(SortNameDes, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_SIZE_ASC:
		{
			m_ctlList.SortItems(SortSizeAsc, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_SIZE_DES:
		{
			m_ctlList.SortItems(SortSizeDes, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_TYPE_ASC:
		{
			m_ctlList.SortItems(SortTypeAsc, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_TYPE_DES:
		{
			m_ctlList.SortItems(SortTypeDes, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_ATTRIBUTE_ASC:
		{
			m_ctlList.SortItems(SortAttrAsc, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_ATTRIBUTE_DES:
		{
			m_ctlList.SortItems(SortAttrDes, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_MODIFIED_ASC:
		{
			m_ctlList.SortItems(SortModiAsc, (DWORD)&m_arrInfo);
		}
		break;
	case HDR_MODIFIED_DES:
		{
			m_ctlList.SortItems(SortModiDes, (DWORD)&m_arrInfo);
		}
		break;
	}
}

void CDualView::SetCurrentFolder(LPCTSTR lpszFolder)
{
	CRegistryMFCX	reg;

	m_strFolder	= lpszFolder;

	reg.Open(HKEY_CURRENT_USER, RK_EX_GENERAL);
	reg.WriteString(RV_EX_LASTOPENED, m_strFolder);
	reg.Close();

	if (m_bShowFolderBar)
	{
		RECT	rc;

		GetClientRect(&rc);

		rc.bottom	= rc.top + FOLDERBAR_HEIGHT;

		InvalidateRect(&rc);
	}

	if (m_bShowStatusBar)
	{
		RECT	rc;

		GetClientRect(&rc);

		rc.top	= rc.bottom - STATUSBAR_HEIGHT;

		InvalidateRect(&rc);
	}
}

BOOL CDualView::SearchDefaultItem(HTREEITEM hParentItem)
{
	CFileStatus	fs;
	CString		strParsed;
	CString		strItem;
	BOOL		bExist	= FALSE;
	BOOL		bFound	= FALSE;
	HTREEITEM	hItem;

	if (m_strDefaultItem == _T("\\"))
	{
		ExpandSubFolder(hParentItem);
		m_ctlTree.Select(hParentItem, TVGN_CARET);

		bFound	= TRUE;

		return bFound;
	}

	if (!CFile::GetStatus(m_strDefaultItem, fs))
	{
		return bFound;
	}

	strItem		= m_strDefaultItem;
	strParsed	= SearchNextItem(strItem);
	bExist		= (BOOL)(strParsed.GetLength() > 0);

	while (bExist)
	{
		TVITEM	item;

		item.mask	= TVIF_HANDLE | TVIF_PARAM;
		item.hItem	= hParentItem;

		CString	strFullPath	= SearchFullPath(item.hItem);
		RECT	rc;

		if (SearchFoldersSpec(item.hItem, strFullPath, strParsed, hItem))
		{
			item.lParam	= ITEM_FOLDER;
		}
		else
		{
			item.lParam	= ITEM_FOLDER_NOITEM;
		}

		m_ctlTree.SetItem(&item);
		m_ctlTree.GetItemRect(item.hItem, &rc, FALSE);
		m_ctlTree.InvalidateRect(&rc);

		if (hItem == NULL)
		{
			bExist	= FALSE;
		}
		else
		{
			hParentItem	= hItem;
			strParsed	= SearchNextItem(strItem);

			if ((bExist = (BOOL)(strParsed.GetLength() > 0)) == FALSE)
			{
				m_ctlTree.Select(hItem, TVGN_CARET);

				bFound	= TRUE;
			}
		}
	}

	m_ctlTree.UpdateWindow();
	return bFound;
}

CString	CDualView::SearchNextItem(CString& strItem)
{
	CString	strParsed	= strItem;

	if (strParsed.Left(1) == _T("\\"))
	{
		strParsed	= strParsed.Mid(1);
	}

	int	nFind	= strParsed.Find(_T("\\"));

	if (nFind >= 0)
	{
		strItem		= strParsed.Mid(nFind);
		strParsed	= strParsed.Left(nFind);
	}
	else
	{
		strItem	= _T("");
	}

	return strParsed;
}

CString CDualView::SearchFullPath(HTREEITEM hItem, BOOL bIncludeSelf)
{
	CString	strFullPath, strFolder;

	if (hItem)
	{
		if (bIncludeSelf == FALSE)
		{
			hItem	= m_ctlTree.GetParentItem(hItem);
		}

		while (hItem)
		{
			if (hItem == m_ctlTree.GetRootItem())
			{
				strFolder	= _T("\\");
			}
			else
			{
				strFolder	= m_ctlTree.GetItemText(hItem);
			}

			if (strFolder != _T("\\"))
			{
				strFullPath	= strFolder + _T("\\") + strFullPath;
			}

			hItem	= m_ctlTree.GetParentItem(hItem);
		}

		strFullPath	= _T("\\") + strFullPath;
	}
	else
	{
		strFullPath	= _T("\\");
	}

	return strFullPath;
}

void CDualView::ExpandSubFolder(HTREEITEM hItem, BOOL bRefresh)
{
	HTREEITEM	hChildItem;
	HTREEITEM	hNextItem;

	if (bRefresh == TRUE)
	{
		if (m_ctlTree.GetItemData(hItem) != ITEM_NEW)
		{
			hChildItem	= m_ctlTree.GetChildItem(hItem);

			if (hChildItem != NULL)
			{
				do
				{
					hNextItem	= m_ctlTree.GetNextSiblingItem(hChildItem);

					m_ctlTree.DeleteItem(hChildItem);

					hChildItem	= hNextItem;
				}
				while (hChildItem != NULL);
			}

			m_ctlTree.SetItemData(hItem, ITEM_NEW);
		}

		if (m_ctlTree.GetItemData(hItem) == ITEM_NEW)
		{
			TVITEM	item;

			item.mask	= TVIF_HANDLE | TVIF_PARAM;
			item.hItem	= hItem;

			CString	strFullPath = SearchFullPath(item.hItem);
			RECT	rc;

			if (SearchFolders(item.hItem, strFullPath))
			{
				item.lParam	= ITEM_FOLDER;
			}
			else
			{
				item.lParam	= ITEM_FOLDER_NOITEM;
			}

			m_ctlTree.SetItem(&item);
			m_ctlTree.GetItemRect(item.hItem, &rc, FALSE);
			m_ctlTree.InvalidateRect(&rc);
			m_ctlTree.UpdateWindow();
		}
	}

	m_ctlTree.Expand(hItem, TVE_EXPAND);
	m_ctlTree.EnsureVisible(hItem);
}

void CDualView::GoParentFolder()
{
	if (m_ctlTree.GetSelectedItem() != m_ctlTree.GetRootItem())
	{
		BeginWaitCursor();
		SetDefaultPath(SearchFullPath(m_ctlTree.GetParentItem(m_ctlTree.GetSelectedItem())));
		BuildRoot();
		EndWaitCursor();
	}
}

CString CDualView::GetSizeString(WIN32_FIND_DATA fd)
{
	CString	strResult;
	CString	strUnits;

	if (m_bSizeFormat)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strResult	= _T("");
		}
		else if (fd.nFileSizeLow >= 0x40000000)
		{
			strUnits.LoadString(IDS_GIGABYTES);
			strResult.Format(_T("%.2f %s"), (double)((double)(__int64)fd.nFileSizeLow / (double)0x40000000), strUnits);
		}
		else if (fd.nFileSizeLow >= 0x100000)
		{
			strUnits.LoadString(IDS_MEGABYTES);
			strResult.Format(_T("%.2f %s"), (double)((double)(__int64)fd.nFileSizeLow / (double)0x100000), strUnits);
		}
		else if (fd.nFileSizeLow >= 0x400)
		{
			strUnits.LoadString(IDS_KILOBYTES);
			strResult.Format(_T("%.2f %s"), (double)((double)(__int64)fd.nFileSizeLow / (double)0x400), strUnits);
		}
		else
		{
			strUnits.LoadString(IDS_BYTES);
			strResult.Format(_T("%d %s"), fd.nFileSizeLow, strUnits);
		}
	}
	else
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strResult	= _T("");
		}
		else if (fd.nFileSizeLow >= 0x40000000)
		{
			strUnits.LoadString(IDS_GIGABYTES_SIMPLE);
			strResult.Format(_T("%d%s"), (DWORD)((double)(__int64)fd.nFileSizeLow / (double)0x40000000), strUnits);
		}
		else if (fd.nFileSizeLow >= 0x100000)
		{
			strUnits.LoadString(IDS_MEGABYTES_SIMPLE);
			strResult.Format(_T("%d%s"), (DWORD)((double)(__int64)fd.nFileSizeLow / (double)0x100000), strUnits);
		}
		else if (fd.nFileSizeLow >= 0x400)
		{
			strUnits.LoadString(IDS_KILOBYTES_SIMPLE);
			strResult.Format(_T("%d%s"), (DWORD)((double)(__int64)fd.nFileSizeLow / (double)0x400), strUnits);
		}
		else
		{
			strUnits.LoadString(IDS_BYTES_SIMPLE);
			strResult.Format(_T("%d%s"), fd.nFileSizeLow, strUnits);
		}
	}

	return strResult;
}

CString CDualView::GetAttributesString(WIN32_FIND_DATA fd)
{
	CString	strResult;
	CString	strItem;

	strResult	= _T("");

	if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
	{
		strItem.LoadString(IDS_ATTRIBUTE_READONLY);

		strResult	+= strItem;
	}

	if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
	{
		strItem.LoadString(IDS_ATTRIBUTE_HIDDEN);

		strResult	+= strItem;
	}

	if (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
	{
		strItem.LoadString(IDS_ATTRIBUTE_SYSTEM);

		strResult	+= strItem;
	}

	if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
	{
		strItem.LoadString(IDS_ATTRIBUTE_ARCHIVE);

		strResult	+= strItem;
	}

	if (fd.dwFileAttributes & FILE_ATTRIBUTE_INROM)
	{
		strItem.LoadString(IDS_ATTRIBUTE_INROM);

		strResult	+= strItem;
	}

	if (fd.dwFileAttributes & FILE_ATTRIBUTE_ROMMODULE)
	{
		strItem.LoadString(IDS_ATTRIBUTE_ROMMODULE);

		strResult	+= strItem;
	}

	return strResult;
}

void CDualView::UpdateSingleTap()
{
	int	iItem	= m_ctlList.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);

	if (iItem != -1)
	{
		int	nItem	= m_ctlList.GetItemData(iItem);

		if (!m_bDoubleTap)
		{
			if (m_arrInfo[nItem].fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				CString		strPath;
				HTREEITEM	hItem;
				HTREEITEM	hChildItem;
				HTREEITEM	hNextItem;
				BOOL		bFound		= FALSE;

				strPath	= m_arrInfo[nItem].fd.cFileName;
				hItem	= m_ctlTree.GetSelectedItem();

				ExpandSubFolder(hItem);

				hChildItem	= m_ctlTree.GetChildItem(hItem);

				while ((hChildItem != NULL) && (bFound == FALSE))
				{
					hNextItem	= m_ctlTree.GetNextItem(hChildItem, TVGN_NEXT);

					if (strPath == m_ctlTree.GetItemText(hChildItem))
					{
						bFound	= TRUE;
						hItem	= hChildItem;
					}

					hChildItem	= hNextItem;
				}

				m_ctlTree.SelectItem(hItem);
				m_ctlTree.EnsureVisible(hItem);
				SetCurrentFolder(SearchFullPath(hItem));
				SearchFiles();
			}
			else
			{
				OnFileOpen();
			}
		}
	}
}

void CDualView::UpdateDoubleTap()
{
	int	iItem	= m_ctlList.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);

	if (iItem != -1)
	{
		int		nItem	= m_ctlList.GetItemData(iItem);

		if (m_arrInfo[nItem].fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			CString		strPath;
			HTREEITEM	hItem;
			HTREEITEM	hChildItem;
			HTREEITEM	hNextItem;
			BOOL		bFound	= FALSE;

			strPath	= m_arrInfo[nItem].fd.cFileName;
			hItem	= m_ctlTree.GetSelectedItem();

			ExpandSubFolder(hItem);

			hChildItem	= m_ctlTree.GetChildItem(hItem);

			while ((hChildItem != NULL) && (bFound == FALSE))
			{
				hNextItem	= m_ctlTree.GetNextItem(hChildItem, TVGN_NEXT);

				if (strPath == m_ctlTree.GetItemText(hChildItem))
				{
					bFound	= TRUE;
					hItem	= hChildItem;
				}

				hChildItem	= hNextItem;
			}

			m_ctlTree.SelectItem(hItem);
			m_ctlTree.EnsureVisible(hItem);
			SetCurrentFolder(SearchFullPath(hItem));
			SearchFiles();
		}
		else
		{
			OnFileOpen();
		}
	}
}

void CDualView::AddClipboard(LPCTSTR lpszItem)
{
	m_arrCBItems.Add(lpszItem);

	m_bCBSingle	= (BOOL)(m_arrCBItems.GetCount() == 1);
}

void CDualView::ClearClipboard()
{
	if (m_arrCBItems.GetCount())
	{
		m_arrCBItems.RemoveAll();
	}

	m_nCBMode		= CBMODE_NONE;
	m_bCBSingle		= TRUE;
	m_strCBFolder	= _T("");
}

void CDualView::ModeStandard()
{
	RECT			rc;
	TBBUTTONINFO	tbButtonInfo;
	CString			strButton;
	CRegistryMFCX	reg;

	if ((!::IsWindow(m_ctlTree.m_hWnd)) || (!::IsWindow(m_ctlList.m_hWnd)))
	{
		return;
	}

	m_ctlTree.ShowWindow(SW_SHOW);
	m_ctlTree.MoveWindow(&m_rcTree);
	m_ctlList.ShowWindow(SW_SHOW);
	m_ctlList.MoveWindow(&m_rcList);

	if (m_nShowMode == VIEW_FOLDERONLY)
	{
		SearchFiles();
	}

	m_nShowMode	= VIEW_STANDARD;

	strButton.LoadString(IDS_SWITCH);
	memset(&tbButtonInfo, 0, sizeof(TBBUTTONINFO));

	tbButtonInfo.cbSize		= sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask		= TBIF_TEXT;
	tbButtonInfo.pszText	= strButton.GetBuffer();

	static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd)->m_wndCommandBar.SendMessage(TB_SETBUTTONINFO, ID_DUAL_SWITCH, (LPARAM)&tbButtonInfo);
	m_ctlTree.SetFocus();
	GetClientRect(&rc);
	InvalidateRect(&rc);
	UpdateWindow();
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_MODE, m_nShowMode);
	reg.Close();
}

void CDualView::ModeFolderOnly()
{
	RECT			rc;
	TBBUTTONINFO	tbButtonInfo;
	CString			strButton;
	CRegistryMFCX	reg;

	if ((!::IsWindow(m_ctlTree.m_hWnd)) || (!::IsWindow(m_ctlList.m_hWnd)))
	{
		return;
	}

	m_ctlTree.ShowWindow(SW_SHOW);
	m_ctlTree.MoveWindow(m_rcTree.left, m_rcTree.top, m_rcList.right - m_rcTree.left, m_rcList.bottom - m_rcTree.top);
	m_ctlList.ShowWindow(SW_HIDE);

	m_nShowMode	= VIEW_FOLDERONLY;

	strButton.LoadString(IDS_UP);
	memset(&tbButtonInfo, 0, sizeof(TBBUTTONINFO));

	tbButtonInfo.cbSize		= sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask		= TBIF_TEXT;
	tbButtonInfo.pszText	= strButton.GetBuffer();

	static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd)->m_wndCommandBar.SendMessage(TB_SETBUTTONINFO, ID_DUAL_SWITCH, (LPARAM)&tbButtonInfo);
	m_ctlTree.SetFocus();
	GetClientRect(&rc);
	InvalidateRect(&rc);
	UpdateWindow();
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_MODE, m_nShowMode);
	reg.Close();
}

void CDualView::ModeFileOnly()
{
	RECT			rc;
	TBBUTTONINFO	tbButtonInfo;
	CString			strButton;
	CRegistryMFCX	reg;

	if ((!::IsWindow(m_ctlTree.m_hWnd)) || (!::IsWindow(m_ctlList.m_hWnd)))
	{
		return;
	}

	m_ctlTree.ShowWindow(SW_HIDE);
	m_ctlList.ShowWindow(SW_SHOW);
	m_ctlList.MoveWindow(m_rcTree.left, m_rcTree.top, m_rcList.right - m_rcTree.left, m_rcList.bottom - m_rcTree.top);

	if (m_nShowMode == VIEW_FOLDERONLY)
	{
		SearchFiles();
	}

	m_nShowMode	= VIEW_FILEONLY;

	strButton.LoadString(IDS_UP);
	memset(&tbButtonInfo, 0, sizeof(TBBUTTONINFO));

	tbButtonInfo.cbSize		= sizeof(TBBUTTONINFO);
	tbButtonInfo.dwMask		= TBIF_TEXT;
	tbButtonInfo.pszText	= strButton.GetBuffer();

	static_cast<CMainFrame*>(AfxGetApp()->m_pMainWnd)->m_wndCommandBar.SendMessage(TB_SETBUTTONINFO, ID_DUAL_SWITCH, (LPARAM)&tbButtonInfo);
	m_ctlList.SetFocus();
	GetClientRect(&rc);
	InvalidateRect(&rc);
	UpdateWindow();
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_MODE, m_nShowMode);
	reg.Close();
}

void CDualView::ModeTiles()
{
	DWORD			dwStyle;
	CRegistryMFCX	reg;

	dwStyle	= m_ctlList.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT | LVS_EX_CHECKBOXES);

	if (m_bMultiSelect)
	{
		dwStyle	|= LVS_EX_CHECKBOXES;
	}

	m_ctlList.SetExtendedStyle(dwStyle);
	m_ctlList.ModifyStyle(LVS_TYPEMASK, LVS_ICON | LVS_ALIGNTOP | LVS_AUTOARRANGE);

	m_nSortItem	= HDR_NAME;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_VIEWSTYLE, LVS_ICON);
	reg.Close();
	SortFiles();
}

void CDualView::ModeIcons()
{
	DWORD			dwStyle;
	CRegistryMFCX	reg;

	dwStyle	= m_ctlList.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT | LVS_EX_CHECKBOXES);

	if (m_bMultiSelect)
	{
		dwStyle	|= LVS_EX_CHECKBOXES;
	}

	m_ctlList.SetExtendedStyle(dwStyle);
	m_ctlList.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON | LVS_ALIGNTOP | LVS_AUTOARRANGE);

	m_nSortItem	= HDR_NAME;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_VIEWSTYLE, LVS_SMALLICON);
	reg.Close();
	SortFiles();
}

void CDualView::ModeList()
{
	DWORD			dwStyle;
	CRegistryMFCX	reg;

	dwStyle	= m_ctlList.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT | LVS_EX_CHECKBOXES);

	if (m_bMultiSelect)
	{
		dwStyle	|= LVS_EX_CHECKBOXES;
	}

	m_ctlList.SetExtendedStyle(dwStyle);
	m_ctlList.ModifyStyle(LVS_TYPEMASK, LVS_LIST | LVS_ALIGNTOP | LVS_AUTOARRANGE);

	m_nSortItem	= HDR_NAME;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_VIEWSTYLE, LVS_LIST);
	reg.Close();
	SortFiles();
}

void CDualView::ModeDetails()
{
	DWORD			dwStyle;
	CRegistryMFCX	reg;

	dwStyle	= m_ctlList.GetExtendedStyle();
	dwStyle	&= ~(LVS_EX_FULLROWSELECT | LVS_EX_GRADIENT | LVS_EX_CHECKBOXES);

	if (m_bMultiSelect)
	{
		dwStyle	|= LVS_EX_CHECKBOXES;
	}

	if (m_bFullRowSelect)
	{
		dwStyle	|= LVS_EX_FULLROWSELECT;
	}

	m_ctlList.SetExtendedStyle(dwStyle);
	m_ctlList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_ALIGNTOP | LVS_AUTOARRANGE);

	m_nSortItem	= HDR_NAME;

	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_VIEWSTYLE, LVS_REPORT);
	reg.Close();
	SortFiles();
}

void CDualView::EditFolderName(HTREEITEM hItem)
{
	if (m_ctlTree.GetRootItem() == hItem)
	{
		return;
	}
	else
	{
		CEditorName	dName;

		dName.SetHeader(IDS_FOLDER_RENAME);

		m_strEdit		= m_ctlTree.GetItemText(hItem);
		dName.m_strName	= m_ctlTree.GetItemText(hItem);

		if (dName.DoModal() == IDOK)
		{
			CString	strOldPath;
			CString	strNewPath;
			CString	strValidName;

			if (!dName.m_strName.GetLength())
			{
				return;
			}

			strValidName	= dName.m_strName;

			if (!strValidName.Compare(m_strEdit))
			{
				return;
			}

			if (!CheckValidFileName(strValidName))
			{
				CString	strTitle;
				CString	strMessage;

				strTitle.LoadString(IDR_MAINFRAME);
				strMessage.LoadString(IDS_INVALID_FILENAME);

				strMessage	+= _T("\n\n");
				strMessage	+= _T("\\ / : * ? \" < > |");

				MessageBox(strMessage, strTitle, MB_OK | MB_ICONWARNING);
				return;
			}

			strOldPath	= SearchFullPath(hItem, FALSE) + m_strEdit;
			strNewPath	= SearchFullPath(hItem, FALSE) + strValidName;

			MoveFile(strOldPath, strNewPath);
			m_ctlTree.SetItemText(hItem, strValidName);

			m_strEdit	= _T("");
		}
	}
}

void CDualView::EditFileName(int nItem)
{
	CEditorName	dName;
	int			nID		= m_ctlList.GetItemData(nItem);

	dName.SetHeader(IDS_FILE_RENAME);

	m_strEdit		= m_arrInfo.GetAt(nID).fd.cFileName;
	dName.m_strName	= m_arrInfo.GetAt(nID).fd.cFileName;

	if (dName.DoModal() == IDOK)
	{
		CString	strOldPath;
		CString	strNewPath;
		CString	strValidName;

		if (!dName.m_strName.GetLength())
		{
			return;
		}

		strValidName	= dName.m_strName;

		if (!strValidName.Compare(m_strEdit))
		{
			return;
		}

		if (!CheckValidFileName(strValidName))
		{
			CString	strTitle;
			CString	strMessage;

			strTitle.LoadString(IDR_MAINFRAME);
			strMessage.LoadString(IDS_INVALID_FILENAME);

			strMessage	+= _T("\n\n");
			strMessage	+= _T("\\ / : * ? \" < > |");

			MessageBox(strMessage, strTitle, MB_OK | MB_ICONWARNING);
			return;
		}

		strOldPath	= m_strFolder + m_strEdit;
		strNewPath	= m_strFolder + strValidName;

		MoveFile(strOldPath, strNewPath);

		if (!m_bShowExt)
		{
			strValidName	= strValidName.Left(strValidName.ReverseFind(_T('.')));
		}

		m_ctlList.SetItemText(nItem, 0, strValidName);

		m_strEdit	= _T("");
	}
}

BOOL CDualView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style	&= ~WS_BORDER;

	return CFormView::PreCreateWindow(cs);
}

void CDualView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	SHChangeNotifyRegister(this->GetSafeHwnd(), NULL);

	HIMAGELIST	hSysIcons;
	UINT		uiHeader;
	DWORD		dwDPI	= GetDPI();

	hSysIcons	= GetSystemIconList(ICON_SMALL);

	m_iSysSmall.Attach(hSysIcons);
	m_ctlTree.SetImageList(&m_iSysSmall, TVSIL_NORMAL);
	m_ctlList.SetImageList(&m_iSysSmall, LVSIL_SMALL);
	m_iSysSmall.Detach();

	hSysIcons	= GetSystemIconList(ICON_BIG);

	m_iSysLarge.Attach(hSysIcons);
	m_ctlList.SetImageList(&m_iSysLarge, LVSIL_NORMAL);
	m_iSysLarge.Detach();

	switch (dwDPI)
	{
	case DPI_192:
		{
			uiHeader	= IDB_SORTMARK32;
		}
		break;
	case DPI_128:
		{
			uiHeader	= IDB_SORTMARK21;
		}
		break;
	default:
		{
			uiHeader	= IDB_SORTMARK16;
		}
		break;
	}

	if (m_iHdrIcons.Create(uiHeader, GetSystemMetrics(SM_CXSMICON), 3, RGB(0xFA, 0xFA, 0xFA)))
	{
		CHeaderCtrl*	pHeader	= m_ctlList.GetHeaderCtrl();

		if (pHeader)
		{
			pHeader->SetImageList(&m_iHdrIcons);
		}
	}

	LoadGeneralOptions();
	LoadViewOptions();
	LoadBrowserOptions();
	RefreshLayout();
	BuildHeaders();
	BuildRoot();
}

void CDualView::OnDestroy()
{
	SHChangeNotifyDeregister(this->GetSafeHwnd());
	CFormView::OnDestroy();
}

BOOL CDualView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch (message)
	{
	case WM_FILECHANGEINFO:
		{
			CWnd*				pWnd		= GetFocus();
			BOOL				bChanged	= FALSE;
			FILECHANGENOTIFY*	fcn			= (FILECHANGENOTIFY*)lParam;
			CString				strItem1;
			CString				strItem2;
			HTREEITEM			hItem;

			if ((fcn->fci.wEventId) & SHCNE_CREATE)	// A file was created
			{
				TakePath((LPCTSTR)fcn->fci.dwItem1, strItem1.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, TRUE);
				strItem1.ReleaseBuffer();

				if (m_strFolder == strItem1)
				{
					m_dwLastUpdateTicks	= GetTickCount();

					if (m_nUpdateTimerID == 0)
					{
						m_nUpdateTimerID	= SetTimer(TID_UPDATE, MINIMUM_UPDATE, NULL);
					}
					
					bChanged	= TRUE;
				}
			}

			if ((fcn->fci.wEventId) & SHCNE_DELETE)	// A file was deleted
			{
				TakePath((LPCTSTR)fcn->fci.dwItem1, strItem1.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, TRUE);
				strItem1.ReleaseBuffer();

				if (m_strFolder == strItem1)
				{
					m_dwLastUpdateTicks	= GetTickCount();

					if (m_nUpdateTimerID == 0)
					{
						m_nUpdateTimerID	= SetTimer(TID_UPDATE, MINIMUM_UPDATE, NULL);
					}
					
					bChanged	= TRUE;
				}
			}

			if ((fcn->fci.wEventId) & SHCNE_RENAMEITEM)	// The name of a file was changed
			{
				TakePath((LPCTSTR)fcn->fci.dwItem1, strItem1.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, TRUE);
				strItem1.ReleaseBuffer();

				if (m_strFolder == strItem1)
				{
					m_dwLastUpdateTicks	= GetTickCount();

					if (m_nUpdateTimerID == 0)
					{
						m_nUpdateTimerID	= SetTimer(TID_UPDATE, MINIMUM_UPDATE, NULL);
					}
					
					bChanged	= TRUE;
				}
			}

			if ((fcn->fci.wEventId) & SHCNE_MKDIR)	// A folder was created
			{
				strItem1	= (LPCTSTR)fcn->fci.dwItem1;
				strItem2	= strItem1.Left(strItem1.ReverseFind(_T('\\')) + 1);
				hItem		= m_ctlTree.GetSelectedItem();

				if (m_ctlTree.GetItemData(hItem) != ITEM_NEW)
				{
					ExpandSubFolder(hItem, TRUE);

					bChanged	= TRUE;
				}

				if (strItem2.Left(m_strFolder.GetLength()) == m_strFolder)
				{
					m_dwLastUpdateTicks	= GetTickCount();

					if (m_nUpdateTimerID == 0)
					{
						m_nUpdateTimerID	= SetTimer(TID_UPDATE, MINIMUM_UPDATE, NULL);
					}
					
					bChanged	= TRUE;
				}
			}

			if ((fcn->fci.wEventId) & SHCNE_RMDIR)	// A folder was removed
			{
				strItem1	= (LPCTSTR)fcn->fci.dwItem1;
				strItem2	= strItem1.Left(strItem1.ReverseFind(_T('\\')) + 1);
				hItem		= m_ctlTree.GetSelectedItem();

				if (m_ctlTree.GetItemData(hItem) != ITEM_NEW)
				{
					ExpandSubFolder(hItem, TRUE);

					bChanged	= TRUE;
				}

				if (strItem2.Left(m_strFolder.GetLength()) == m_strFolder)
				{
					m_dwLastUpdateTicks	= GetTickCount();

					if (m_nUpdateTimerID == 0)
					{
						m_nUpdateTimerID	= SetTimer(TID_UPDATE, MINIMUM_UPDATE, NULL);
					}
					
					bChanged	= TRUE;
				}
			}

			if ((fcn->fci.wEventId) & SHCNE_RENAMEFOLDER)	// The name of a folder was changed
			{
				strItem1	= (LPCTSTR)fcn->fci.dwItem1;
				strItem2	= strItem1.Left(strItem1.ReverseFind(_T('\\')) + 1);
				hItem		= m_ctlTree.GetSelectedItem();

				if (m_ctlTree.GetItemData(hItem) != ITEM_NEW)
				{
					ExpandSubFolder(hItem, TRUE);

					bChanged	= TRUE;
				}

				if (strItem2.Left(m_strFolder.GetLength()) == m_strFolder)
				{
					m_dwLastUpdateTicks	= GetTickCount();

					if (m_nUpdateTimerID == 0)
					{
						m_nUpdateTimerID	= SetTimer(TID_UPDATE, MINIMUM_UPDATE, NULL);
					}
					
					bChanged	= TRUE;
				}
			}

			if ((fcn->fci.wEventId) & SHCNE_MEDIAINSERTED)	// A media was inserted
			{
				hItem	= m_ctlTree.GetSelectedItem();

				if (hItem == m_ctlTree.GetRootItem())
				{
					ExpandSubFolder(hItem, TRUE);

					m_dwLastUpdateTicks	= GetTickCount();

					if (m_nUpdateTimerID == 0)
					{
						m_nUpdateTimerID	= SetTimer(TID_UPDATE, MINIMUM_UPDATE, NULL);
					}

					bChanged	= TRUE;
				}
			}

			if ((fcn->fci.wEventId) & SHCNE_MEDIAREMOVED)	// A media was removed
			{
				hItem	= m_ctlTree.GetSelectedItem();

				if (hItem == m_ctlTree.GetRootItem())
				{
					ExpandSubFolder(hItem, TRUE);

					m_dwLastUpdateTicks	= GetTickCount();

					if (m_nUpdateTimerID == 0)
					{
						m_nUpdateTimerID	= SetTimer(TID_UPDATE, MINIMUM_UPDATE, NULL);
					}

					bChanged	= TRUE;
				}
			}

			SHChangeNotifyFree(fcn);

			if (bChanged == TRUE)
			{
				if (pWnd == ((CWnd*)&m_ctlTree))
				{
					m_ctlList.SetFocus();
				}
				else
				{
					m_ctlTree.SetFocus();
				}
			}
		}

		return TRUE;
	}

	return CFormView::OnWndMsg(message, wParam, lParam, pResult);
}

void CDualView::OnPaint()
{
	CPaintDC	dc(this);
	RECT		rc;
	RECT		rcSplit;
	int			nBkOldMode;
	LOGFONT		lfFont;
	CFont		Font;
	CFont*		pOldFont;

	GetClientRect(&rc);
	GetDefaultFont(&lfFont);
	Font.CreateFontIndirect(&lfFont);

	pOldFont	= dc.SelectObject(&Font);

	if (m_bShowFolderBar)
	{
		CString	strFolder;
		int		nPos;
		HICON	hIcon;

		SetRect(&rcSplit, rc.left, rc.top, rc.right, rc.top + FOLDERBAR_HEIGHT);
		dc.MoveTo(rcSplit.left, rcSplit.bottom - 1);
		dc.LineTo(rcSplit.right, rcSplit.bottom - 1);

		rcSplit.bottom--;

		dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));

		hIcon	= GetFileIcon(m_strFolder, ICON_SMALL);

		if (hIcon != NULL)
		{
			DrawIconEx(dc.GetSafeHdc(), rc.left + DRA::SCALEX(2), DRA::SCALEY(2), hIcon, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0, NULL, DI_NORMAL);
			DestroyIcon(hIcon);
		}

		nBkOldMode		= dc.SetBkMode(TRANSPARENT);
		rcSplit.left	= GetSystemMetrics(SM_CXSMICON) + DRA::SCALEX(6);
		rcSplit.right	= rc.right - DRA::SCALEX(4);

		if (!m_strFolder.CompareNoCase(_T("\\")))
		{
			strFolder	= m_strFolder;
		}
		else
		{
			strFolder	= m_strFolder.Left(m_strFolder.GetLength() - 1);
			nPos		= strFolder.ReverseFind(_T('\\'));
			strFolder	= strFolder.Mid(nPos + 1);
		}

		DrawText(dc.GetSafeHdc(), strFolder, -1, &rcSplit, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_WORD_ELLIPSIS);
		dc.SetBkMode(nBkOldMode);

		rc.top	+= FOLDERBAR_HEIGHT;
	}

	if (m_bShowStatusBar)
	{
		int		i;
		int		nItems;
		CString	strStatus;

		SetRect(&rcSplit, rc.left, rc.bottom - STATUSBAR_HEIGHT, rc.right, rc.bottom);
		dc.MoveTo(rcSplit.left, rcSplit.top);
		dc.LineTo(rcSplit.right, rcSplit.top);
		
		rcSplit.top++;

		dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));

		nBkOldMode		= dc.SetBkMode(TRANSPARENT);
		rcSplit.left	= rc.left + DRA::SCALEX(4);
		rcSplit.right	= rc.right - DRA::SCALEX(4);
		nItems			= 0;

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_bMultiSelect)
			{
				if (m_ctlList.GetCheck(i))
				{
					nItems++;
				}
			}
			else
			{
				if (m_ctlList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				{
					nItems++;
				}
			}
		}

		if (nItems > 0)
		{
			strStatus.Format(((nItems == 1) ? IDS_STATUS_SELECTED0 : IDS_STATUS_SELECTED1), nItems);
		}
		else
		{
			nItems	= m_ctlList.GetItemCount();

			strStatus.Format(((nItems == 1) ? IDS_STATUS_TOTAL0 : IDS_STATUS_TOTAL1), nItems);
		}

		DrawText(dc.GetSafeHdc(), strStatus, -1, &rcSplit, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_WORD_ELLIPSIS);
		dc.SetBkMode(nBkOldMode);

		rc.bottom	-= STATUSBAR_HEIGHT;
	}

	dc.SelectObject(pOldFont);

	if (m_nShowMode == VIEW_STANDARD)
	{
		if (m_bVertical == TRUE)
		{
			dc.MoveTo(rc.left, m_nSplit);
			dc.LineTo(rc.right, m_nSplit);
			dc.MoveTo(rc.left, m_nSplit + m_nSplitDistance);
			dc.LineTo(rc.right, m_nSplit + m_nSplitDistance);
			SetRect(&rcSplit, rc.left, m_nSplit + 1, rc.right, m_nSplit + m_nSplitDistance);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));
		}
		else
		{
			dc.MoveTo(m_nSplit, rc.top);
			dc.LineTo(m_nSplit, rc.bottom);
			dc.MoveTo(m_nSplit + m_nSplitDistance, rc.top);
			dc.LineTo(m_nSplit + m_nSplitDistance, rc.bottom);
			SetRect(&rcSplit, m_nSplit + 1, rc.top, m_nSplit + m_nSplitDistance, rc.bottom);
			dc.FillSolidRect(&rcSplit, ::GetSysColor(COLOR_3DFACE));
		}
	}

	CFormView::OnPaint();
}

void CDualView::OnLButtonDown(UINT nFlags, CPoint point)
{
	RECT	rc;
	RECT	rcFolderBar;

	GetClientRect(&rc);

	if (m_bShowFolderBar)
	{
		POINT	pt	= { point.x, point.y };

		SetRect(&rcFolderBar, rc.left, rc.top, rc.right, rc.top + FOLDERBAR_HEIGHT);

		if (PtInRect(&rcFolderBar, pt))
		{
			m_bFolderBarOpen	= TRUE;
		}
	}

	if (!m_bFolderBarOpen)
	{
		if (m_nShowMode == VIEW_STANDARD)
		{
			SetCapture();

			m_bTracking			= TRUE;
			m_nTrackDistance	= ((m_bVertical == TRUE) ? point.y : point.x) - m_nSplit;
			m_nTracking			= m_nSplit;
		}
	}

	Default();
}

void CDualView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bFolderBarOpen)
	{
		CFolderDialogX	dFD;

		m_bFolderBarOpen	= FALSE;

		dFD.SetDefaultPath(m_strFolder);

		if (dFD.DoModal() == IDOK)
		{
			Refresh(dFD.GetFullPath());
		}
	}
	else
	{
		if (m_nShowMode == VIEW_STANDARD)
		{
			ReleaseCapture();

			m_bTracking	= FALSE;

			InvalidateRect(NULL);
			UpdateWindow();
		}
	}

	CFormView::OnLButtonUp(nFlags, point);
}

void CDualView::OnMouseMove(UINT nFlags, CPoint point)
{
	RECT	rc;

	GetClientRect(&rc);

	if ((m_nShowMode == VIEW_STANDARD) && (m_bTracking == TRUE))
	{
		m_nTracking	= ((m_bVertical == TRUE) ? point.y : point.x) - m_nTracking;
		m_nSplit	= ((m_bVertical == TRUE) ? point.y : point.x) - m_nTrackDistance;

		if (m_bVertical == TRUE)
		{
			if (m_nSplit < 0)
			{
				m_nSplit	= 0;
			}
			if (m_nSplit >= rc.bottom - m_nSplitDistance)
			{
				m_nSplit	= rc.bottom - m_nSplitDistance - DRA::SCALEY(1);
			}

			m_rcTree.bottom	= m_nSplit;
			m_rcList.top	= m_nSplit + m_nSplitDistance + DRA::SCALEY(1);
		}
		else
		{
			if (m_nSplit < 0)
			{
				m_nSplit	= 0;
			}
			if (m_nSplit >= rc.right - m_nSplitDistance)
			{
				m_nSplit	= rc.right - m_nSplitDistance - DRA::SCALEY(1);
			}

			m_rcTree.right	= m_nSplit;
			m_rcList.left	= m_nSplit + m_nSplitDistance + DRA::SCALEY(1);
		}

		m_ctlTree.MoveWindow(&m_rcTree);
		m_ctlList.MoveWindow(&m_rcList);
		InvalidateRect(NULL);
		UpdateWindow();
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CDualView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	m_dwLastResizeTicks	= GetTickCount();

	if (m_nResizeTimerID == 0)
	{
		m_nResizeTimerID	= SetTimer(TID_RESIZE, MINIMUM_RESIZE, NULL);
	}
}

void CDualView::OnTvnSelchangedFoldertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView	= reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM		hItem		= pNMTreeView->itemNew.hItem;

	if (hItem)
	{
		SetCurrentFolder(SearchFullPath(hItem));

		if ((pNMTreeView->action) && (m_nShowMode != VIEW_FOLDERONLY))
		{
			SearchFiles();
		}
	}

	*pResult	= 0;
}

void CDualView::OnTvnItemexpandedFoldertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView	= reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM		hItem		= pNMTreeView->itemNew.hItem;

	if ((pNMTreeView->action & TVE_COLLAPSE) && (hItem))
	{
		SetCurrentFolder(SearchFullPath(hItem));

		if (m_nShowMode != VIEW_FOLDERONLY)
		{
			SearchFiles();
		}
	}

	*pResult	= 0;
}

void CDualView::OnTvnItemexpandingFoldertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView	= reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if (pNMTreeView->action & TVE_EXPAND)
	{
		if (pNMTreeView->itemNew.lParam == ITEM_NEW)
		{
			TVITEM	item;

			item.mask	= TVIF_HANDLE | TVIF_PARAM;
			item.hItem	= pNMTreeView->itemNew.hItem;

			CString	strFullPath	= SearchFullPath(item.hItem);
			RECT	rc;

			if (SearchFolders(item.hItem, strFullPath))
			{
				item.lParam	= ITEM_FOLDER;
			}
			else
			{
				item.lParam	= ITEM_FOLDER_NOITEM;
			}

			m_ctlTree.SetItem(&item);
			m_ctlTree.GetItemRect(item.hItem, &rc, FALSE);
			m_ctlTree.InvalidateRect(&rc);
			m_ctlTree.UpdateWindow();
		}
	}

	*pResult	= 0;
}

void CDualView::OnTvnGetdispinfoFoldertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if (pTVDispInfo->item.mask & TVIF_CHILDREN)
	{
		if ((pTVDispInfo->item.lParam == ITEM_NEW) || (pTVDispInfo->item.lParam == ITEM_FOLDER))
		{
			pTVDispInfo->item.cChildren	= 1;
		}
		else
		{
			pTVDispInfo->item.cChildren	= 0;
		}
	}

	*pResult	= 0;
}

void CDualView::OnTvnBeginlabeleditFoldertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	if (m_ctlTree.GetRootItem() == pTVDispInfo->item.hItem)
	{
		*pResult	= 1;
	}
	else
	{
		m_strEdit	= m_ctlTree.GetItemText(pTVDispInfo->item.hItem);

		::SHSipPreference(this->GetSafeHwnd(), SIP_UP);
		m_ctlTree.EnsureVisible(pTVDispInfo->item.hItem);

		*pResult	= 0;
	}
}

void CDualView::OnTvnEndlabeleditFoldertree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVDISPINFO	pTVDispInfo	= reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	CString	strOldPath;
	CString	strNewPath;

	::SHSipPreference(this->GetSafeHwnd(), SIP_DOWN);

	if (!pTVDispInfo->item.pszText)
	{
		*pResult	= 0;

		return;
	}

	CString	strValidName	= pTVDispInfo->item.pszText;

	if (!CheckValidFileName(strValidName))
	{
		CString	strTitle;
		CString	strMessage;

		strTitle.LoadString(IDR_MAINFRAME);
		strMessage.LoadString(IDS_INVALID_FILENAME);

		strMessage	+= _T("\n\n");
		strMessage	+= _T("\\ / : * ? \" < > |");

		MessageBox(strMessage, strTitle, MB_OK | MB_ICONWARNING);

		*pResult	= 0;

		return;
	}

	strOldPath	= SearchFullPath(pTVDispInfo->item.hItem, FALSE) + m_strEdit;
	strNewPath	= SearchFullPath(pTVDispInfo->item.hItem, FALSE) + strValidName;

	m_strEdit	= _T("");
	*pResult	= (BOOL)MoveFile(strOldPath, strNewPath);
}

LRESULT CDualView::OnTapAndHoldFoldertree(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM	hItem	= (HTREEITEM)wParam;
	CString		strItem;
	CMenu		ctlMenu;
	UINT		uFlags;

	if (hItem == NULL)
	{
		ctlMenu.CreatePopupMenu();
		strItem.LoadString(IDS_DUAL_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_DUAL_REFRESH, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}
	else
	{
		SetCurrentFolder(SearchFullPath(hItem));

		if (m_nShowMode != VIEW_FOLDERONLY)
		{
			SearchFiles();
		}

		m_ctlTree.Select(hItem, TVGN_CARET);
		ctlMenu.CreatePopupMenu();

		if ((m_ctlTree.GetItemState(hItem, TVIS_EXPANDED) & TVIS_EXPANDED) != TVIS_EXPANDED)
		{
			strItem.LoadString(IDS_FOLDER_EXPAND);
			ctlMenu.AppendMenu(((m_ctlTree.GetItemData(hItem) != ITEM_FOLDER_NOITEM) ? MF_ENABLED : MF_GRAYED), ID_FOLDER_EXPAND, strItem);
		}
		else
		{
			strItem.LoadString(IDS_FOLDER_COLLAPSE);
			ctlMenu.AppendMenu(MF_ENABLED, ID_FOLDER_COLLAPSE, strItem);
		}

		strItem.LoadString(IDS_DUAL_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_DUAL_REFRESH, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);

		uFlags	= ((m_ctlTree.GetRootItem() == hItem) ? MF_GRAYED : MF_ENABLED);

		strItem.LoadString(IDS_FOLDER_CUT);
		ctlMenu.AppendMenu(uFlags, ID_FOLDER_CUT, strItem);
		strItem.LoadString(IDS_FOLDER_COPY);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLDER_COPY, strItem);

		uFlags	= (((m_nCBMode != CBMODE_NONE) && (m_strFolder != m_strCBFolder)) ? MF_ENABLED : MF_GRAYED);

		strItem.LoadString(IDS_FOLDER_PASTE);
		ctlMenu.AppendMenu(uFlags, ID_FOLDER_PASTE, strItem);

		uFlags	= (((m_nCBMode != CBMODE_NONE) && (m_bCBSingle == TRUE)) ? MF_ENABLED : MF_GRAYED);

		strItem.LoadString(IDS_FOLDER_PASTE_SHORTCUT);
		ctlMenu.AppendMenu(uFlags, ID_FOLDER_PASTE_SHORTCUT, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FOLDER_COPY_TO);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLDER_COPY_TO, strItem);
		strItem.LoadString(IDS_FOLDER_MOVE_TO);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLDER_MOVE_TO, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FOLDER_DELETE);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLDER_DELETE, strItem);
		strItem.LoadString(IDS_FOLDER_RENAME);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLDER_RENAME, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FOLDER_NEW_FOLDER);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLDER_NEW_FOLDER, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FOLDER_PROPERTIES);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FOLDER_PROPERTIES, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}

	return 0;
}

void CDualView::OnHeaderClickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	HD_NOTIFY*	phdn	= reinterpret_cast<HD_NOTIFY*>(pNMHDR);
	int			iItem;

	iItem	= GetHeaderIndex(phdn->iItem);

	if (m_nSortItem == iItem)
	{
		m_nSortItem	= iItem + HDR_DES_FLAG;
	}
	else
	{
		m_nSortItem	= iItem;
	}

	SetColumnIcons();
	SortFiles();

	*pResult	= 0;
}

void CDualView::OnLvnGetdispinfoFilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO*	pDispInfo	= reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	int				nItem		= m_ctlList.GetItemData(pDispInfo->item.iItem);
	SHFILEINFO		SFI;
	CString			strFile;
	FILETIME		ftLastWriteTime;
	SYSTEMTIME		stLastWriteTime;
	TCHAR			szDate[256];
	TCHAR			szTime[256];
	CString			strDateTime;

	strFile	= m_arrInfo.GetAt(nItem).szFolder;
	strFile	+= m_arrInfo.GetAt(nItem).fd.cFileName;

	if (pDispInfo->item.mask & LVIF_IMAGE)
	{
		SHGetFileInfo(strFile, 0, &SFI, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);

		pDispInfo->item.iImage	= SFI.iIcon;
	}

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		switch (GetHeaderIndex(pDispInfo->item.iSubItem))
		{
		case HDR_NAME:
			{
				CString	strFileName	= m_arrInfo.GetAt(nItem).fd.cFileName;

				if (!m_bShowExt)
				{
					int	nPos	= strFileName.ReverseFind(_T('.'));

					if (nPos > 0)
					{
						strFileName	= strFileName.Left(nPos);
					}
				}

				_tcscpy(pDispInfo->item.pszText, strFileName);
			}
			break;
		case HDR_SIZE:
			{
				_tcscpy(pDispInfo->item.pszText, GetSizeString(m_arrInfo.GetAt(nItem).fd));
			}
			break;
		case HDR_TYPE:
			{
				SHGetFileInfo(strFile, 0, &SFI, sizeof(SHFILEINFO), SHGFI_TYPENAME);
				_tcscpy(pDispInfo->item.pszText, SFI.szTypeName);
			}
			break;
		case HDR_ATTRIBUTE:
			{
				_tcscpy(pDispInfo->item.pszText, GetAttributesString(m_arrInfo.GetAt(nItem).fd));
			}
			break;
		case HDR_MODIFIED:
			{
				FileTimeToLocalFileTime(&m_arrInfo.GetAt(nItem).fd.ftLastWriteTime, &ftLastWriteTime);
				FileTimeToSystemTime(&ftLastWriteTime, &stLastWriteTime);

				if (m_bDateFormat)
				{
					GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stLastWriteTime, NULL, szDate, 256);
					GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &stLastWriteTime, NULL, szTime, 256);
					strDateTime.Format(_T("%s %s"), szDate, szTime);
				}
				else
				{
					GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &stLastWriteTime, NULL, szDate, 256);
					strDateTime.Format(_T("%s"), szDate);
				}

				_tcscpy(pDispInfo->item.pszText, strDateTime);
			}
			break;
		}
	}

	*pResult	= 0;
}

void CDualView::OnLvnBeginlabeleditFilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO*	pDispInfo	= reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	m_strEdit	= m_ctlList.GetItemText(pDispInfo->item.iItem, 0);

	::SHSipPreference(this->GetSafeHwnd(), SIP_UP);
	m_ctlList.EnsureVisible(pDispInfo->item.iItem, FALSE);

	*pResult	= 0;
}

void CDualView::OnLvnEndlabeleditFilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO*	pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	CString			strOldPath;
	CString			strNewPath;

	::SHSipPreference(this->GetSafeHwnd(), SIP_DOWN);

	if (!pDispInfo->item.pszText)
	{
		*pResult	= 0;

		return;
	}

	CString	strValidName	= pDispInfo->item.pszText;

	if (!m_bShowExt)
	{
		CString	strExt	= m_arrInfo.GetAt(pDispInfo->item.lParam).fd.cFileName;

		strExt			= strExt.Mid(strExt.ReverseFind(_T('.')) + 1);
		strValidName	+= strExt;
	}

	if (!CheckValidFileName(strValidName))
	{
		CString	strTitle;
		CString	strMessage;

		strTitle.LoadString(IDR_MAINFRAME);
		strMessage.LoadString(IDS_INVALID_FILENAME);

		strMessage	+= _T("\n\n");
		strMessage	+= _T("\\ / : * ? \" < > |");

		MessageBox(strMessage, strTitle, MB_OK | MB_ICONWARNING);

		*pResult	= 0;

		return;
	}

	strOldPath	= m_strFolder + m_strEdit;
	strNewPath	= m_strFolder + strValidName;
	m_strEdit	= _T("");
	*pResult	= (BOOL)MoveFile(strOldPath, strNewPath);
}

void CDualView::OnNMClickFilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateSingleTap();

	if (m_bShowStatusBar)
	{
		RECT	rc;

		GetClientRect(&rc);

		rc.top	= rc.bottom - STATUSBAR_HEIGHT;

		InvalidateRect(&rc);
	}

	*pResult	= 0;
}

void CDualView::OnNMDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateDoubleTap();

	if (m_bShowStatusBar)
	{
		RECT	rc;

		GetClientRect(&rc);

		rc.top	= rc.bottom - STATUSBAR_HEIGHT;

		InvalidateRect(&rc);
	}

	*pResult	= 0;
}

void CDualView::OnNMSetfocusFilelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	if ((m_ctlList.GetItemCount() > 0) && (m_ctlList.GetSelectedCount() == 0))
	{
		m_ctlList.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}

	*pResult	= 0;
}

LRESULT CDualView::OnTapAndHoldFilelist(WPARAM wParam, LPARAM lParam)
{
	int		nItem	= wParam;
	CString	strItem;
	CMenu	ctlMenu;
	UINT	uFlags;

	if (nItem == -1)
	{
		CMenu	ctlView;

		ctlView.CreatePopupMenu();
		strItem.LoadString(IDS_FILE_VIEW_TILES);
		ctlView.AppendMenu(MF_ENABLED | (((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_ICON) ? MF_CHECKED : 0), ID_FILE_VIEW_TILES, strItem);
		strItem.LoadString(IDS_FILE_VIEW_ICONS);
		ctlView.AppendMenu(MF_ENABLED | (((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_SMALLICON) ? MF_CHECKED : 0), ID_FILE_VIEW_ICONS, strItem);
		strItem.LoadString(IDS_FILE_VIEW_LIST);
		ctlView.AppendMenu(MF_ENABLED | (((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_LIST) ? MF_CHECKED : 0), ID_FILE_VIEW_LIST, strItem);
		strItem.LoadString(IDS_FILE_VIEW_DETAILS);
		ctlView.AppendMenu(MF_ENABLED | (((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_REPORT) ? MF_CHECKED : 0), ID_FILE_VIEW_DETAILS, strItem);

		CMenu	ctlMode;

		ctlMode.CreatePopupMenu();
		strItem.LoadString(IDS_DUAL_MODE_STANDARD);
		ctlMode.AppendMenu(MF_ENABLED | ((m_nShowMode == VIEW_STANDARD) ? MF_CHECKED : 0), ID_DUAL_MODE_STANDARD, strItem);
		strItem.LoadString(IDS_DUAL_MODE_FOLDERONLY);
		ctlMode.AppendMenu(MF_ENABLED | ((m_nShowMode == VIEW_FOLDERONLY) ? MF_CHECKED : 0), ID_DUAL_MODE_FOLDERONLY, strItem);
		strItem.LoadString(IDS_DUAL_MODE_FILEONLY);
		ctlMode.AppendMenu(MF_ENABLED | ((m_nShowMode == VIEW_FILEONLY) ? MF_CHECKED : 0), ID_DUAL_MODE_FILEONLY, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(((m_bVertical == TRUE) ? IDS_DUAL_HORIZONTAL_LAYOUT : IDS_DUAL_VERTICAL_LAYOUT));
		ctlMode.AppendMenu(MF_ENABLED, ID_DUAL_LAYOUT, strItem);

		ctlMenu.CreatePopupMenu();
		strItem.LoadString(IDS_FILE_VIEW);
		ctlMenu.AppendMenu(MF_ENABLED | MF_POPUP, (UINT_PTR)ctlView.GetSafeHmenu(), strItem);
		strItem.LoadString(IDS_DUAL_MODE);
		ctlMenu.AppendMenu(MF_ENABLED | MF_POPUP, (UINT_PTR)ctlMode.GetSafeHmenu(), strItem);
		strItem.LoadString(IDS_DUAL_REFRESH);
		ctlMenu.AppendMenu(MF_ENABLED, ID_DUAL_REFRESH, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);

		uFlags	= (((m_nCBMode != CBMODE_NONE) && (m_strFolder != m_strCBFolder)) ? MF_ENABLED : MF_GRAYED);

		strItem.LoadString(IDS_FILE_PASTE);
		ctlMenu.AppendMenu(uFlags, ID_FILE_PASTE, strItem);

		uFlags	= (((m_nCBMode != CBMODE_NONE) && (m_bCBSingle == TRUE)) ? MF_ENABLED : MF_GRAYED);

		strItem.LoadString(IDS_FILE_PASTE_SHORTCUT);
		ctlMenu.AppendMenu(uFlags, ID_FILE_PASTE_SHORTCUT, strItem);

		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FILE_NEW_FOLDER);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_NEW_FOLDER, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FILE_PROPERTIES);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_PROPERTIES, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}
	else
	{
		ctlMenu.CreatePopupMenu();

		uFlags	= ((m_ctlList.GetSelectedCount() == 1) ? MF_ENABLED : MF_GRAYED);

		strItem.LoadString(IDS_FILE_OPENFILE);
		ctlMenu.AppendMenu(uFlags, ID_FILE_OPENFILE, strItem);
		strItem.LoadString(IDS_FILE_OPENFILEWITH);
		ctlMenu.AppendMenu(uFlags, ID_FILE_OPENFILEWITH, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);

		CMenu	ctlSend;

		ctlSend.CreatePopupMenu();

		uFlags	= (((m_ctlList.GetSelectedCount() == 1) && ((m_arrInfo[m_ctlList.GetItemData(nItem)].fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)) ? MF_ENABLED : MF_GRAYED);

		strItem.LoadString(IDS_FILE_SENDTO_IR);
		ctlSend.AppendMenu(uFlags, ID_FILE_SENDTO_IR, strItem);
		strItem.LoadString(IDS_FILE_SENDTO);
		ctlMenu.AppendMenu(uFlags | MF_POPUP, (UINT_PTR)ctlSend.GetSafeHmenu(), strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);

		CMenu	ctlZIP;
		TCHAR	szFileName[MAX_PATH + 1];
		TCHAR	szExtension[MAX_PATH + 1];

		ctlZIP.CreatePopupMenu();
		strItem.LoadString(IDS_FILE_ZIP_ADD);
		ctlZIP.AppendMenu(MF_ENABLED, ID_FILE_ZIP_ADD, strItem);

		uFlags	= MF_GRAYED;

		if (m_ctlList.GetSelectedCount() == 1)
		{
			CombineFileName(szFileName, MAX_PATH + 1, m_strFolder, m_arrInfo[m_ctlList.GetItemData(nItem)].fd.cFileName);
			TakeExtension(szFileName, szExtension, MAX_PATH + 1);

			uFlags	= (_tcsicmp(szExtension, _T("zip")) == 0 ? MF_ENABLED : MF_GRAYED);
		}

		strItem.LoadString(IDS_FILE_ZIP_EXTRACT);
		ctlZIP.AppendMenu(uFlags, ID_FILE_ZIP_EXTRACT, strItem);
		strItem.LoadString(IDS_FILE_ZIP);
		ctlMenu.AppendMenu(MF_ENABLED | MF_POPUP, (UINT_PTR)ctlZIP.GetSafeHmenu(), strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);

		strItem.LoadString(IDS_FILE_CUT);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_CUT, strItem);
		strItem.LoadString(IDS_FILE_COPY);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_COPY, strItem);

		uFlags	= (((m_nCBMode != CBMODE_NONE) && (m_strFolder != m_strCBFolder)) ? MF_ENABLED : MF_GRAYED);

		strItem.LoadString(IDS_FILE_PASTE);
		ctlMenu.AppendMenu(uFlags, ID_FILE_PASTE, strItem);

		uFlags	= (((m_nCBMode != CBMODE_NONE) && (m_bCBSingle == TRUE)) ? MF_ENABLED : MF_GRAYED);

		strItem.LoadString(IDS_FILE_PASTE_SHORTCUT);
		ctlMenu.AppendMenu(uFlags, ID_FILE_PASTE_SHORTCUT, strItem);

		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FILE_COPY_TO);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_COPY_TO, strItem);
		strItem.LoadString(IDS_FILE_MOVE_TO);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_MOVE_TO, strItem);
		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FILE_DELETE);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_DELETE, strItem);

		uFlags	= ((m_ctlList.GetSelectedCount() > 1) ? MF_GRAYED : MF_ENABLED);

		strItem.LoadString(IDS_FILE_RENAME);
		ctlMenu.AppendMenu(uFlags, ID_FILE_RENAME, strItem);

		ctlMenu.AppendMenu(MF_SEPARATOR);
		strItem.LoadString(IDS_FILE_PROPERTIES);
		ctlMenu.AppendMenu(MF_ENABLED, ID_FILE_PROPERTIES, strItem);
		ctlMenu.TrackPopupMenu(TPM_LEFTALIGN, LOWORD(lParam), HIWORD(lParam), this);
	}

	return 0;
}

void CDualView::OnDualRun()
{
	CRunDialog	dRun;

	dRun.SetHeader(IDS_RUN_TITLE);

	if (dRun.DoModal() == IDOK)
	{
		CreateProcess(dRun.m_strFile, dRun.m_strParameters, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
	}
}

void CDualView::OnDualReceive()
{
	TCHAR	szWindows[MAX_PATH + 1];
	TCHAR	szBeam[MAX_PATH + 1];

	GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
	CombineFileName(szBeam, MAX_PATH + 1, szWindows, BEAM_EXEC);
	CreateProcess(szBeam, NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
}

void CDualView::OnDualFind()
{
	CFindDialog	dFind;

	dFind.SetHeader(IDS_FIND_TITLE);

	if (dFind.DoModal() == IDOK)
	{
		TCHAR			szFilter[MAX_PATH + 1];
		CItemSearchX	Search;

		BeginWaitCursor();
		m_arrFoundFiles.RemoveAll();
		CombineFileName(szFilter, MAX_PATH + 1, dFind.m_strFrom, dFind.m_strFilter);
		Search.SetPartName(szFilter);
		Search.SetSearchSubFolders(dFind.m_bSubFolders);
		Search.SetIncludeFolders(TRUE);

		if (dFind.m_bFrom == TRUE)
		{
			Search.SetSearchDateFrom(&dFind.m_stFrom);
		}

		if (dFind.m_bTo == TRUE)
		{
			Search.SetSearchDateTo(&dFind.m_stTo);
		}

		if (dFind.m_bLarger == TRUE)
		{
			Search.SetSearchSizeFrom(dFind.m_dwLarger);
		}

		if (dFind.m_bSmaller == TRUE)
		{
			Search.SetSearchSizeTo(dFind.m_dwSmaller);
		}

		Search.DoSearch();
		m_arrFoundFiles.Copy(Search.m_arrFoundFiles);
		EndWaitCursor();
		OnDualFoundItems();
	}
}

void CDualView::OnDualFoundItems()
{
	CFoundItems	dFound;

	if (!m_arrFoundFiles.GetCount())
	{
		CString	strMessage;
		CString	strTitle;

		strMessage.LoadString(IDS_FIND_NO_MATCH);
		strTitle.LoadString(IDR_MAINFRAME);
		MessageBox(strMessage, strTitle, MB_OK | MB_ICONINFORMATION);
		return;
	}

	dFound.m_arrFiles.Copy(m_arrFoundFiles);
	dFound.SetHeader(IDS_FOUND_ITEMS_TITLE);

	UINT	nResponse	= dFound.DoModal();

	m_arrFoundFiles.RemoveAll();
	m_arrFoundFiles.Copy(dFound.m_arrFiles);

	if (nResponse == IDOK)
	{
		DWORD	dwAttributes	= GetFileAttributes(m_arrFoundFiles[dFound.m_nSelected]);

		if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			Refresh(m_arrFoundFiles[dFound.m_nSelected]);
			m_ctlTree.SetFocus();
		}
		else
		{
			int		nPos		= m_arrFoundFiles[dFound.m_nSelected].ReverseFind(_T('\\'));
			CString	strName		= m_arrFoundFiles[dFound.m_nSelected].Mid(nPos + 1);
			CString	strFolder	= m_arrFoundFiles[dFound.m_nSelected].Left(nPos);

			Refresh(strFolder);
			m_ctlList.SetFocus();

			for (int i = 0; i < m_ctlList.GetItemCount(); i++)
			{
				if (strName == m_ctlList.GetItemText(i, 0))
				{
					m_ctlList.EnsureVisible(i, FALSE);
					m_ctlList.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
					break;
				}
			}
		}
	}
}

void CDualView::OnDualModeStandard()
{
	ModeStandard();
}

void CDualView::OnDualModeFolderOnly()
{
	ModeFolderOnly();
}

void CDualView::OnDualModeFileOnly()
{
	ModeFileOnly();
}

void CDualView::OnDualLayout()
{
	CRegistryMFCX	reg;

	SetLayout(((m_bVertical == TRUE) ? FALSE : TRUE));
	RefreshLayout();
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_LAYOUT, (m_bVertical ? 1 : 0));
	reg.Close();
}

void CDualView::OnDualFolderBar()
{
	CRegistryMFCX	reg;

	SetShowFolderBar(((m_bShowFolderBar == TRUE) ? FALSE : TRUE));
	RefreshLayout();
	Refresh();
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_FOLDERBAR, (m_bShowFolderBar ? 1 : 0));
	reg.Close();
}

void CDualView::OnDualStatusBar()
{
	CRegistryMFCX	reg;

	SetShowStatusBar(((m_bShowStatusBar == TRUE) ? FALSE : TRUE));
	RefreshLayout();
	Refresh();
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_STATUSBAR, (m_bShowStatusBar ? 1 : 0));
	reg.Close();
}

void CDualView::OnDualNameEditor()
{
	CRegistryMFCX	reg;

	SetNameEditor(((m_bNameEditor == TRUE) ? FALSE : TRUE));
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_NAMEEDITOR, (m_bNameEditor ? 1 : 0));
	reg.Close();
}

void CDualView::OnDualRefresh()
{
	Refresh();
}

void CDualView::OnDualSwitch()
{
	if (m_nShowMode != VIEW_STANDARD)
	{
		GoParentFolder();
	}
	else
	{
		CWnd*	pWnd	= GetFocus();

		if (pWnd == ((CWnd*)&m_ctlTree))
		{
			m_ctlList.SetFocus();
		}
		else
		{
			m_ctlTree.SetFocus();
		}
	}
}

void CDualView::OnDualOptions()
{
	COptionsSheet	dOptions(IDR_MAINFRAME);

	dOptions.SetTitle(IDS_OPTIONS_TITLE);
	dOptions.DoModal();
	LoadGeneralOptions();
	LoadViewOptions();
	LoadBrowserOptions();
	RefreshLayout();
	Refresh();
}

void CDualView::OnFolderExpand()
{
	HTREEITEM	hItem	= m_ctlTree.GetSelectedItem();

	m_ctlTree.Expand(hItem, TVE_EXPAND);
	m_ctlTree.EnsureVisible(hItem);
}

void CDualView::OnFolderCollapse()
{
	HTREEITEM	hItem	= m_ctlTree.GetSelectedItem();

	m_ctlTree.Expand(hItem, TVE_COLLAPSE);
	m_ctlTree.EnsureVisible(hItem);
}

void CDualView::OnFolderCut()
{
	HTREEITEM	hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		ClearClipboard();
		m_ctlTree.SetItemState(hItem, TVIS_CUT, TVIS_CUT);
		AddClipboard(m_strFolder);

		m_nCBMode		= CBMODE_CUT;
		m_strCBFolder	= m_strFolder;
	}
}

void CDualView::OnFolderCopy()
{
	HTREEITEM	hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		ClearClipboard();
		AddClipboard(m_strFolder);

		m_nCBMode		= CBMODE_COPY;
		m_strCBFolder	= m_strFolder;
	}
}

void CDualView::OnFolderPaste()
{
	int	i;

	switch (m_nCBMode)
	{
	case CBMODE_CUT:
		{
			CMoveDialogX	dMD;

			for (i = 0; i < m_arrCBItems.GetCount(); i++)
			{
				dMD.AddFile(m_arrCBItems[i]);
			}

			dMD.SetSource(m_strCBFolder);
			dMD.SetDestination(m_strFolder);
			dMD.DoModal();
			ClearClipboard();
		}
		break;
	case CBMODE_COPY:
		{
			CCopyDialogX	dCD;

			for (i = 0; i < m_arrCBItems.GetCount(); i++)
			{
				dCD.AddFile(m_arrCBItems[i]);
			}

			dCD.SetSource(m_strCBFolder);
			dCD.SetDestination(m_strFolder);
			dCD.DoModal();
		}
		break;
	}
}

void CDualView::OnFolderPasteShortcut()
{
	CFileStatus	status;
	CString		strSource;
	CString		strShortcut;

	if (m_bCBSingle == FALSE)
	{
		return;
	}

	if (CFile::GetStatus(m_arrCBItems[0], status))
	{
		strSource	= m_arrCBItems[0].Mid(m_arrCBItems[0].GetLength());

		if (status.m_attribute & FILE_ATTRIBUTE_DIRECTORY)
		{
			strSource	= m_arrCBItems[0].Mid(m_arrCBItems[0].ReverseFind(_T('\\')) + 1);
		}
		else
		{
			TakeTitle(m_arrCBItems[0], strSource.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
			strSource.ReleaseBuffer();
		}

		strSource	+= _T(".lnk");

		CombineFileName(strShortcut.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strFolder, strSource);
		strShortcut.ReleaseBuffer();
		strSource.Format(_T("\"%s\""), m_arrCBItems[0]);
		SHCreateShortcut(strShortcut.GetBuffer(strShortcut.GetLength()), strSource.GetBuffer(strSource.GetLength()));
		strShortcut.ReleaseBuffer();
		strSource.ReleaseBuffer();
	}
}

void CDualView::OnFolderCopyTo()
{
	CCopyDialogX	dCD;
	CFolderDialogX	dFD;
	HTREEITEM		hItem;
	CString			strPath;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		if (m_strFolder == _T("\\"))
		{
			for (int i = 0; i < m_ctlList.GetItemCount(); i++)
			{
				strPath	= m_strFolder + m_ctlList.GetItemText(i, 0);

				dCD.AddFile(strPath);
			}
		}
		else
		{
			dCD.AddFile(m_strFolder);
			GoParentFolder();
		}

		if (dFD.DoModal() == IDOK)
		{
			dCD.SetSource(m_strFolder);
			dCD.SetDestination(dFD.GetFullPath());
			dCD.DoModal();
		}
	}
}

void CDualView::OnFolderMoveTo()
{
	CMoveDialogX	dMD;
	CFolderDialogX	dFD;
	HTREEITEM		hItem;
	CString			strPath;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		if (m_strFolder == _T("\\"))
		{
			for (int i = 0; i < m_ctlList.GetItemCount(); i++)
			{
				strPath	= m_strFolder + m_ctlList.GetItemText(i, 0);

				dMD.AddFile(strPath);
			}
		}
		else
		{
			dMD.AddFile(m_strFolder);
			GoParentFolder();
		}

		if (dFD.DoModal() == IDOK)
		{
			dMD.SetSource(m_strFolder);
			dMD.SetDestination(dFD.GetFullPath());
			dMD.DoModal();
		}
	}
}

void CDualView::OnFolderDelete()
{
	CDeleteDialogX	dDD;
	HTREEITEM		hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		HTREEITEM	hNextItem;

		if ((hNextItem = m_ctlTree.GetPrevSiblingItem(hItem)) == NULL)
		{
			if ((hNextItem = m_ctlTree.GetNextSiblingItem(hItem)) == NULL)
			{
				hNextItem	= m_ctlTree.GetParentItem(hItem);
			}
		}

		dDD.ClearFiles();
		dDD.AddFile(m_strFolder);
		dDD.DoModal();
		Refresh(SearchFullPath(hNextItem));
	}
}

void CDualView::OnFolderRename()
{
	if (m_bNameEditor)
	{
		EditFolderName(m_ctlTree.GetSelectedItem());
	}
	else
	{
		m_ctlTree.EditLabel(m_ctlTree.GetSelectedItem());
	}
}

void CDualView::OnFolderNewFolder()
{
	HTREEITEM	hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		CFileStatus	fs;
		CString		strPath;
		CString		strTemplate;
		int			nCount	= 0;

		strTemplate.LoadString(IDS_NEW_FOLDER_TEMPLATE);

		strPath	= m_strFolder + strTemplate;

		while (CFile::GetStatus(strPath, fs))
		{
			strPath.Format(_T("%s%s #%d"), m_strFolder, strTemplate, ++nCount);
		}

		if (CreateDirectory(strPath, NULL))
		{
			Refresh(strPath);

			if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
			{
				if (m_bNameEditor)
				{
					EditFolderName(hItem);
				}
				else
				{
					m_ctlTree.EditLabel(hItem);
				}
			}
		}
	}
}

void CDualView::OnFolderProperties()
{
	CPropertiesDialogX	dPD;
	HTREEITEM			hItem;

	if ((hItem = m_ctlTree.GetSelectedItem()) != NULL)
	{
		dPD.AddFile(m_strFolder);
		dPD.DoModal();
	}
}

void CDualView::OnFileOpen()
{
	if (m_ctlList.GetSelectedCount() != 1)
	{
		return;
	}

	int	nItem	= m_ctlList.GetNextItem(-1, LVNI_SELECTED);

	if (m_arrInfo[m_ctlList.GetItemData(nItem)].fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		UpdateDoubleTap();
	}
	else
	{
		CString				strPath;
		SHELLEXECUTEINFO	shei;

		strPath	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

		memset(&shei, 0, sizeof(SHELLEXECUTEINFO));

		shei.cbSize			= sizeof(SHELLEXECUTEINFO);
		shei.fMask			= 0;
		shei.hwnd			= this->GetSafeHwnd();
		shei.lpVerb			= _T("open");
		shei.lpFile			= strPath;
		shei.lpDirectory	= m_strFolder;
		shei.nShow			= SW_SHOW;

		ShellExecuteEx(&shei);
	}
}

void CDualView::OnFileOpenWith()
{
	if (m_ctlList.GetSelectedCount() != 1)
	{
		return;
	}

	int	nItem	= m_ctlList.GetNextItem(-1, LVNI_SELECTED);

	if (m_arrInfo[m_ctlList.GetItemData(nItem)].fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		UpdateDoubleTap();
	}
	else
	{
		COpenWithDialog	dOpenWith;
		TCHAR			szFilter[MAX_PATH + 1];
		TCHAR			szStartMenu[MAX_PATH + 1];
		CItemSearchX	Search;

		BeginWaitCursor();

		dOpenWith.m_strFile	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

		dOpenWith.m_arrFiles.RemoveAll();
		GetSpecialDirectoryEx(CSIDL_STARTMENU, szStartMenu, MAX_PATH + 1);
		CombineFileName(szFilter, MAX_PATH + 1, szStartMenu, _T("*.lnk"));
		Search.SetPartName(szFilter);
		Search.SetSearchSubFolders(TRUE);
		Search.SetIncludeFolders(FALSE);
		Search.DoSearch();
		dOpenWith.m_arrFiles.Copy(Search.m_arrFoundFiles);
		EndWaitCursor();
		dOpenWith.DoModal();
	}
}

void CDualView::OnFileSendToIR()
{
	if (m_ctlList.GetSelectedCount() != 1)
	{
		return;
	}

	int	nItem	= m_ctlList.GetNextItem(-1, LVNI_SELECTED);

	if (m_arrInfo[m_ctlList.GetItemData(nItem)].fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return;
	}
	else
	{
		TCHAR	szWindows[MAX_PATH + 1];
		TCHAR	szBeam[MAX_PATH + 1];
		TCHAR	szPath[MAX_PATH + 1];

		CombineFileName(szPath, MAX_PATH + 1, m_strFolder, m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName);
		GetSpecialDirectoryEx(CSIDL_WINDOWS, szWindows, MAX_PATH + 1);
		CombineFileName(szBeam, MAX_PATH + 1, szWindows, BEAM_EXEC);
		CreateProcess(szBeam, szPath, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
	}
}

void CDualView::OnFileZIPAdd()
{
	CRegistryMFCX		reg;
	CZipDialogX			dZD;
	CFileSaveDialogX	dSD;
	POSITION			pos;
	CString				strPath;
	int					nItem;
	HTREEITEM			hItem;

	if (m_bMultiSelect)
	{
		int	i;

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetItemState(i, LVNI_FOCUSED))
			{
				m_ctlList.SetCheck(i);
				break;
			}
		}

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetCheck(i))
			{
				m_ctlList.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
			}
			else
			{
				m_ctlList.SetItemState(i, 0, LVNI_SELECTED);
			}
		}
	}

	pos	= m_ctlList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		return;
	}
	else
	{
		reg.Open(HKEY_CURRENT_USER, RK_EX_ZIP);
		dZD.SetLevel(reg.ReadDWORD(RV_EX_COMPRESSLEVEL, 9));

		if (reg.ReadDWORD(RV_EX_CODEPAGESET, 0))
		{
			dZD.SetCodePage(reg.ReadDWORD(RV_EX_CODEPAGE, 1252));
		}

		reg.Close();
		dZD.SetSource(m_strFolder);

		while (pos)
		{
			nItem	= m_ctlList.GetNextSelectedItem(pos);
			strPath	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

			dZD.AddFile(strPath);
		}

		hItem	= m_ctlTree.GetSelectedItem();

		dSD.SetDefaultFilename(m_ctlTree.GetItemText(hItem));
		dSD.SetHeader(IDS_ZIP_ADDHEADER);
		dSD.SetTypePairs(IDS_ZIP_FILTERS);

		if (dSD.DoModal() != IDOK)
		{
			return;
		}
	}

	dZD.SetZIPFile(dSD.GetFullPath(), NULL);
	dZD.DoModal();
}

void CDualView::OnFileZIPExtract()
{
	int				nItem	= m_ctlList.GetNextItem(-1, LVNI_SELECTED);
	TCHAR			szPath[MAX_PATH + 1];
	CRegistryMFCX	reg;
	CFolderDialogX	dFD;
	CUnzipDialogX	dUZ;

	CombineFileName(szPath, MAX_PATH + 1, m_strFolder, m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName);
	dFD.SetDefaultPath(m_strFolder);

	if (dFD.DoModal() == IDOK)
	{
		reg.Open(HKEY_CURRENT_USER, RK_EX_ZIP);

		if (reg.ReadDWORD(RV_EX_CODEPAGESET, 0))
		{
			dUZ.SetCodePage(reg.ReadDWORD(RV_EX_CODEPAGE, 1252));
		}

		reg.Close();
		dUZ.SetSource(szPath, NULL);
		dUZ.SetDestination(dFD.GetFullPath());
		dUZ.AddAllFiles();
		dUZ.DoModal();
	}
}

void CDualView::OnFileCut()
{
	POSITION	pos;
	CString		strPath;
	int			nItem;

	if (m_bMultiSelect)
	{
		int	i;

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetItemState(i, LVNI_FOCUSED))
			{
				m_ctlList.SetCheck(i);
				break;
			}
		}

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetCheck(i))
			{
				m_ctlList.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
			}
			else
			{
				m_ctlList.SetItemState(i, 0, LVNI_SELECTED);
			}
		}
	}

	pos	= m_ctlList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		return;
	}
	else
	{
		ClearClipboard();

		while (pos)
		{
			nItem	= m_ctlList.GetNextSelectedItem(pos);
			strPath	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

			m_ctlList.SetItemState(nItem, LVIS_CUT, LVIS_CUT);
			AddClipboard(strPath);
		}

		m_nCBMode		= CBMODE_CUT;
		m_strCBFolder	= m_strFolder;
	}
}

void CDualView::OnFileCopy()
{
	POSITION	pos;
	CString		strPath;
	int			nItem;

	if (m_bMultiSelect)
	{
		int	i;

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetItemState(i, LVNI_FOCUSED))
			{
				m_ctlList.SetCheck(i);
				break;
			}
		}

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetCheck(i))
			{
				m_ctlList.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
			}
			else
			{
				m_ctlList.SetItemState(i, 0, LVNI_SELECTED);
			}
		}
	}

	pos	= m_ctlList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		return;
	}
	else
	{
		ClearClipboard();

		while (pos)
		{
			nItem	= m_ctlList.GetNextSelectedItem(pos);
			strPath	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

			AddClipboard(strPath);
		}

		m_nCBMode		= CBMODE_COPY;
		m_strCBFolder	= m_strFolder;
	}
}

void CDualView::OnFilePaste()
{
	int	i;

	switch (m_nCBMode)
	{
	case CBMODE_CUT:
		{
			CMoveDialogX	dMD;

			for (i = 0; i < m_arrCBItems.GetCount(); i++)
			{
				dMD.AddFile(m_arrCBItems[i]);
			}

			dMD.SetSource(m_strCBFolder);
			dMD.SetDestination(m_strFolder);
			dMD.DoModal();
			ClearClipboard();
		}
		break;
	case CBMODE_COPY:
		{
			CCopyDialogX	dCD;

			for (i = 0; i < m_arrCBItems.GetCount(); i++)
			{
				dCD.AddFile(m_arrCBItems[i]);
			}

			dCD.SetSource(m_strCBFolder);
			dCD.SetDestination(m_strFolder);
			dCD.DoModal();
		}
		break;
	}
}

void CDualView::OnFilePasteShortcut()
{
	CFileStatus	status;
	CString		strSource;
	CString		strShortcut;

	if (m_bCBSingle == FALSE)
	{
		return;
	}

	if (CFile::GetStatus(m_arrCBItems[0], status))
	{
		strSource	= m_arrCBItems[0].Mid(m_arrCBItems[0].GetLength());

		if (status.m_attribute & FILE_ATTRIBUTE_DIRECTORY)
		{
			strSource	= m_arrCBItems[0].Mid(m_arrCBItems[0].ReverseFind(_T('\\')) + 1);
		}
		else
		{
			TakeTitle(m_arrCBItems[0], strSource.GetBuffer(MAX_PATH + 1), MAX_PATH + 1);
			strSource.ReleaseBuffer();
		}

		strSource	+= _T(".lnk");

		CombineFileName(strShortcut.GetBuffer(MAX_PATH + 1), MAX_PATH + 1, m_strFolder, strSource);
		strShortcut.ReleaseBuffer();
		strSource.Format(_T("\"%s\""), m_arrCBItems[0]);
		SHCreateShortcut(strShortcut.GetBuffer(strShortcut.GetLength()), strSource.GetBuffer(strSource.GetLength()));
		strShortcut.ReleaseBuffer();
		strSource.ReleaseBuffer();
	}
}

void CDualView::OnFileAutoWidth()
{
	CRegistryX	reg;

	SetAutoWidth(((m_bAutoWidth == TRUE) ? FALSE : TRUE));
	RefreshLayout();
	Refresh();
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_AUTOWIDTH, (m_bAutoWidth ? 1 : 0));
	reg.Close();
}

void CDualView::OnFileFullRowSelect()
{
	CRegistryX	reg;

	SetFullRowSelect(((m_bFullRowSelect == TRUE) ? FALSE : TRUE));
	Refresh();
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_FULLROWSELECT, (m_bFullRowSelect ? 1 : 0));
	reg.Close();
}

void CDualView::OnFileSelectAll()
{
	for (int i = 0; i < m_ctlList.GetItemCount(); i++)
	{
		if (m_bMultiSelect)
		{
			m_ctlList.SetCheck(i, TRUE);
		}

		m_ctlList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CDualView::OnFileInvertSelection()
{
	for (int i = 0; i < m_ctlList.GetItemCount(); i++)
	{
		if (m_bMultiSelect)
		{
			if (m_ctlList.GetCheck(i))
			{
				m_ctlList.SetCheck(i, FALSE);
				m_ctlList.SetItemState(i, 0, LVIS_SELECTED);
			}
			else
			{
				m_ctlList.SetCheck(i, TRUE);
				m_ctlList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			}
		}
		else
		{
			if (m_ctlList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
			{
				m_ctlList.SetItemState(i, 0, LVIS_SELECTED);
			}
			else
			{
				m_ctlList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			}
		}
	}
}

void CDualView::OnFileMultiSelect()
{
	DWORD			dwStyle;
	CRegistryMFCX	reg;

	SetMultiSelect(m_bMultiSelect ? FALSE : TRUE);
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_MULTISELECT, (m_bMultiSelect ? 1 : 0));
	reg.Close();

	dwStyle	= m_ctlList.GetExtendedStyle();
	dwStyle	&= ~LVS_EX_CHECKBOXES;

	if (m_bMultiSelect)
	{
		dwStyle	|= LVS_EX_CHECKBOXES;
	}

	m_ctlList.SetExtendedStyle(dwStyle);
}

void CDualView::OnFileTapActivation()
{
	CRegistryMFCX	reg;

	SetDoubleTapActivation(m_bDoubleTap ? FALSE : TRUE);
	reg.Open(HKEY_CURRENT_USER, RK_EX_BROWSER);
	reg.WriteDWORD(RV_EX_DOUBLETAP, (m_bDoubleTap ? 1 : 0));
	reg.Close();
}

void CDualView::OnFileCopyTo()
{
	CCopyDialogX	dCD;
	CFolderDialogX	dFD;
	POSITION		pos;
	CString			strPath;
	int				nItem;

	if (m_bMultiSelect)
	{
		int	i;

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetItemState(i, LVNI_FOCUSED))
			{
				m_ctlList.SetCheck(i);
				break;
			}
		}

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetCheck(i))
			{
				m_ctlList.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
			}
			else
			{
				m_ctlList.SetItemState(i, 0, LVNI_SELECTED);
			}
		}
	}

	pos	= m_ctlList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		return;
	}
	else
	{
		while (pos)
		{
			nItem	= m_ctlList.GetNextSelectedItem(pos);
			strPath	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

			dCD.AddFile(strPath);
		}

		if (dFD.DoModal() != IDOK)
		{
			return;
		}
	}

	dCD.SetSource(m_strFolder);
	dCD.SetDestination(dFD.GetFullPath());
	dCD.DoModal();
}

void CDualView::OnFileMoveTo()
{
	CMoveDialogX	dMD;
	CFolderDialogX	dFD;
	POSITION		pos;
	CString			strPath;
	int				nItem;

	if (m_bMultiSelect)
	{
		int	i;

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetItemState(i, LVNI_FOCUSED))
			{
				m_ctlList.SetCheck(i);
				break;
			}
		}

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetCheck(i))
			{
				m_ctlList.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
			}
			else
			{
				m_ctlList.SetItemState(i, 0, LVNI_SELECTED);
			}
		}
	}

	pos	= m_ctlList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		return;
	}
	else
	{
		while (pos)
		{
			nItem	= m_ctlList.GetNextSelectedItem(pos);
			strPath	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

			dMD.AddFile(strPath);
		}

		if (dFD.DoModal() != IDOK)
		{
			return;
		}
	}

	dMD.SetSource(m_strFolder);
	dMD.SetDestination(dFD.GetFullPath());
	dMD.DoModal();
}

void CDualView::OnFileDelete()
{
	CDeleteDialogX	dDD;
	POSITION		pos;
	CString			strPath;
	int				nItem;

	if (m_bMultiSelect)
	{
		int	i;

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetItemState(i, LVNI_FOCUSED))
			{
				m_ctlList.SetCheck(i);
				break;
			}
		}

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetCheck(i))
			{
				m_ctlList.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
			}
			else
			{
				m_ctlList.SetItemState(i, 0, LVNI_SELECTED);
			}
		}
	}

	pos	= m_ctlList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		return;
	}
	else
	{
		while (pos)
		{
			nItem	= m_ctlList.GetNextSelectedItem(pos);
			strPath	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

			dDD.AddFile(strPath);
		}
	}

	dDD.DoModal();
}

void CDualView::OnFileRename()
{
	if (m_bNameEditor)
	{
		EditFileName(m_ctlList.GetNextItem(-1, LVNI_SELECTED));
	}
	else
	{
		m_ctlList.EditLabel(m_ctlList.GetNextItem(-1, LVNI_SELECTED));
	}
}

void CDualView::OnFileNewFolder()
{
	CFileStatus	fs;
	CString		strPath;
	CString		strTemplate;
	CString		strName;
	int			nCount	= 0;

	strTemplate.LoadString(IDS_NEW_FOLDER_TEMPLATE);

	strName	= strTemplate;
	strPath	= m_strFolder + strName;

	while (CFile::GetStatus(strPath, fs))
	{
		strName.Format(_T("%s %d"), strTemplate, ++nCount);
		strPath.Format(_T("%s%s"), m_strFolder, strName);
	}

	if (CreateDirectory(strPath, NULL))
	{
		m_strFind			= strName;
		m_nRenameTimerID	= SetTimer(TID_RENAME, MINIMUM_RENAME, NULL);
	}
}

void CDualView::OnFileProperties()
{
	CPropertiesDialogX	dPD;
	POSITION			pos;
	CString				strPath;
	int					nItem;

	if (m_bMultiSelect)
	{
		int	i;

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetItemState(i, LVNI_FOCUSED))
			{
				m_ctlList.SetCheck(i);
				break;
			}
		}

		for (i = 0; i < m_ctlList.GetItemCount(); i++)
		{
			if (m_ctlList.GetCheck(i))
			{
				m_ctlList.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);
			}
			else
			{
				m_ctlList.SetItemState(i, 0, LVNI_SELECTED);
			}
		}
	}

	pos	= m_ctlList.GetFirstSelectedItemPosition();

	if (pos == NULL)
	{
		dPD.AddFile(m_strFolder);
	}
	else
	{
		while (pos)
		{
			nItem	= m_ctlList.GetNextSelectedItem(pos);
			strPath	= m_strFolder + m_arrInfo.GetAt(m_ctlList.GetItemData(nItem)).fd.cFileName;

			dPD.AddFile(strPath);
		}
	}

	dPD.DoModal();
}

void CDualView::OnFileViewTiles()
{
	ModeTiles();
}

void CDualView::OnFileViewIcons()
{
	ModeIcons();
}

void CDualView::OnFileViewList()
{
	ModeList();
}

void CDualView::OnFileViewDetails()
{
	ModeDetails();
}

void CDualView::OnAbout()
{
	CAboutDialog	dAbout;

	dAbout.DoModal();
}

void CDualView::OnUpdateDualModeStandard(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_nShowMode == VIEW_STANDARD));
}

void CDualView::OnUpdateDualModeFolderOnly(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_nShowMode == VIEW_FOLDERONLY));
}

void CDualView::OnUpdateDualModeFileOnly(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_nShowMode == VIEW_FILEONLY));
}

void CDualView::OnUpdateDualLayout(CCmdUI* pCmdUI)
{
	CString	strItem;

	strItem.LoadString(((m_bVertical == TRUE) ? IDS_DUAL_HORIZONTAL_LAYOUT : IDS_DUAL_VERTICAL_LAYOUT));
	pCmdUI->SetText(strItem);
}

void CDualView::OnUpdateDualFolderBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_bShowFolderBar == TRUE));
}

void CDualView::OnUpdateDualStatusBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_bShowStatusBar == TRUE));
}

void CDualView::OnUpdateDualNameEditor(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((BOOL)(m_bNameEditor == TRUE));
}

void CDualView::OnUpdateDualSwitch(CCmdUI* pCmdUI)
{
	CString	strItem;

	if (m_nShowMode != VIEW_STANDARD)
	{
		strItem.LoadString(IDS_UP);
		pCmdUI->SetText(strItem);
		pCmdUI->Enable((BOOL)(m_ctlTree.GetSelectedItem() != m_ctlTree.GetRootItem()));
	}
	else
	{
		strItem.LoadString(IDS_SWITCH);
		pCmdUI->SetText(strItem);
		pCmdUI->Enable(TRUE);
	}
}

void CDualView::OnUpdateFileViewTiles(CCmdUI* pCmdUI)
{
	if (m_nShowMode == VIEW_FOLDERONLY)
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->SetCheck((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_ICON);
		pCmdUI->Enable(TRUE);
	}
}

void CDualView::OnUpdateFileViewIcons(CCmdUI* pCmdUI)
{
	if (m_nShowMode == VIEW_FOLDERONLY)
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->SetCheck((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_SMALLICON);
		pCmdUI->Enable(TRUE);
	}
}

void CDualView::OnUpdateFileViewList(CCmdUI* pCmdUI)
{
	if (m_nShowMode == VIEW_FOLDERONLY)
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->SetCheck((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_LIST);
		pCmdUI->Enable(TRUE);
	}
}

void CDualView::OnUpdateFileViewDetails(CCmdUI* pCmdUI)
{
	if (m_nShowMode == VIEW_FOLDERONLY)
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->SetCheck((m_ctlList.GetStyle() & LVS_TYPEMASK) == LVS_REPORT);
		pCmdUI->Enable(TRUE);
	}
}

void CDualView::OnUpdateFileAutoWidth(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bAutoWidth);
}

void CDualView::OnUpdateFileFullRowSelect(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bFullRowSelect);
}

void CDualView::OnUpdateFileMultiSelect(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bMultiSelect);
}

void CDualView::OnUpdateFileTapActivation(CCmdUI* pCmdUI)
{
	CString	strItem;

	strItem.LoadString(((m_bDoubleTap == TRUE) ? IDS_DUAL_SINGLE_TAP : IDS_DUAL_DOUBLE_TAP));
	pCmdUI->SetText(strItem);
}

void CDualView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent != 0)
	{
		if (nIDEvent == m_nRenameTimerID)
		{
			KillTimer(m_nRenameTimerID);

			LVFINDINFO	lvfi;

			lvfi.flags	= LVFI_STRING;
			lvfi.psz	= m_strFind;

			if (m_bNameEditor)
			{
				EditFileName(m_ctlList.FindItem(&lvfi));
			}
			else
			{
				m_ctlList.EditLabel(m_ctlList.FindItem(&lvfi));
			}

			m_nRenameTimerID	= 0;
		}

		if (nIDEvent == m_nUpdateTimerID)
		{
			DWORD	dwTicks	= GetTickCount() - m_dwLastUpdateTicks;

			if (dwTicks > MINIMUM_UPDATE)
			{
				KillTimer(m_nUpdateTimerID);
				SearchFiles();

				m_nUpdateTimerID	= 0;
			}
		}

		if (nIDEvent == m_nResizeTimerID)
		{
			DWORD	dwTicks	= GetTickCount() - m_dwLastResizeTicks;

			if (dwTicks > MINIMUM_RESIZE)
			{
				KillTimer(m_nResizeTimerID);
				RefreshLayout();

				m_nResizeTimerID	= 0;
			}
		}
	}

	CFormView::OnTimer(nIDEvent);
}

static int SortNameAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	return _wcsicmp(pInfo->GetAt(lParam1).fd.cFileName, pInfo->GetAt(lParam2).fd.cFileName);
}

static int SortNameDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	return _wcsicmp(pInfo->GetAt(lParam2).fd.cFileName, pInfo->GetAt(lParam1).fd.cFileName);
}

static int SortSizeAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int								nComp	= 0;
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	nComp	= (int)((__int64)(pInfo->GetAt(lParam1).fd.nFileSizeLow) - (__int64)(pInfo->GetAt(lParam2).fd.nFileSizeLow));

	if (nComp == 0)
	{
		return _wcsicmp(pInfo->GetAt(lParam1).fd.cFileName, pInfo->GetAt(lParam2).fd.cFileName);
	}

	return nComp;
}

static int SortSizeDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int								nComp	= 0;
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	nComp	= (int)((__int64)(pInfo->GetAt(lParam2).fd.nFileSizeLow) - (__int64)(pInfo->GetAt(lParam1).fd.nFileSizeLow));

	if (nComp == 0)
	{
		return _wcsicmp(pInfo->GetAt(lParam2).fd.cFileName, pInfo->GetAt(lParam1).fd.cFileName);
	}

	return nComp;
}

static int SortTypeAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SHFILEINFO						SFI1;
	SHFILEINFO						SFI2;
	CString							strFile1;
	CString							strFile2;
	int								nComp	= 0;
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	strFile1	=  pInfo->GetAt(lParam1).szFolder;
	strFile1	+= pInfo->GetAt(lParam1).fd.cFileName;

	SHGetFileInfo(strFile1, 0, &SFI1, sizeof(SHFILEINFO), SHGFI_TYPENAME);

	strFile2	=  pInfo->GetAt(lParam2).szFolder;
	strFile2	+= pInfo->GetAt(lParam2).fd.cFileName;

	SHGetFileInfo(strFile2, 0, &SFI2, sizeof(SHFILEINFO), SHGFI_TYPENAME);

	nComp	= _wcsicmp(SFI1.szTypeName, SFI2.szTypeName);

	if (nComp == 0)
	{
		return _wcsicmp(pInfo->GetAt(lParam1).fd.cFileName, pInfo->GetAt(lParam2).fd.cFileName);
	}

	return nComp;
}

static int SortTypeDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	SHFILEINFO						SFI1;
	SHFILEINFO						SFI2;
	CString							strFile1;
	CString							strFile2;
	int								nComp	= 0;
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	strFile1	=  pInfo->GetAt(lParam1).szFolder;
	strFile1	+= pInfo->GetAt(lParam1).fd.cFileName;

	SHGetFileInfo(strFile1, 0, &SFI1, sizeof(SHFILEINFO), SHGFI_TYPENAME);

	strFile2	=  pInfo->GetAt(lParam2).szFolder;
	strFile2	+= pInfo->GetAt(lParam2).fd.cFileName;

	SHGetFileInfo(strFile2, 0, &SFI2, sizeof(SHFILEINFO), SHGFI_TYPENAME);

	nComp	= _wcsicmp(SFI2.szTypeName, SFI1.szTypeName);

	if (nComp == 0)
	{
		return _wcsicmp(pInfo->GetAt(lParam2).fd.cFileName, pInfo->GetAt(lParam1).fd.cFileName);
	}

	return nComp;
}

static int SortAttrAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int								nComp	= 0;
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	nComp	= (int)((__int64)(pInfo->GetAt(lParam1).fd.dwFileAttributes) - (__int64)(pInfo->GetAt(lParam2).fd.dwFileAttributes));

	if (nComp == 0)
	{
		return _wcsicmp(pInfo->GetAt(lParam1).fd.cFileName, pInfo->GetAt(lParam2).fd.cFileName);
	}

	return nComp;
}

static int SortAttrDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int								nComp	= 0;
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	nComp	= (int)((__int64)(pInfo->GetAt(lParam2).fd.dwFileAttributes) - (__int64)(pInfo->GetAt(lParam1).fd.dwFileAttributes));

	if (nComp == 0)
	{
		return _wcsicmp(pInfo->GetAt(lParam2).fd.cFileName, pInfo->GetAt(lParam1).fd.cFileName);
	}

	return nComp;
}

static int SortModiAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int								nComp	= 0;
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	nComp	= CompareFileTime(&(pInfo->GetAt(lParam1).fd.ftLastWriteTime), &(pInfo->GetAt(lParam2).fd.ftLastWriteTime));

	if (nComp == 0)
	{
		return _wcsicmp(pInfo->GetAt(lParam1).fd.cFileName, pInfo->GetAt(lParam2).fd.cFileName);
	}

	return nComp;
}

static int SortModiDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int								nComp	= 0;
	CArray<EXPINFOX, EXPINFOX>*	pInfo	= (CArray<EXPINFOX, EXPINFOX>*)lParamSort;

	if ((pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return -1;
	}

	if (!(pInfo->GetAt(lParam2).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (pInfo->GetAt(lParam1).fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return 1;
	}

	nComp	= CompareFileTime(&(pInfo->GetAt(lParam2).fd.ftLastWriteTime), &(pInfo->GetAt(lParam1).fd.ftLastWriteTime));

	if (nComp == 0)
	{
		return _wcsicmp(pInfo->GetAt(lParam2).fd.cFileName, pInfo->GetAt(lParam1).fd.cFileName);
	}

	return nComp;
}
