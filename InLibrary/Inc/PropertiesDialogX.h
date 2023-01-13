// PropertiesDialogX.h

#pragma once

#include "DialogX.h"
#include "FileInfoX.h"

#ifndef PDX_CF_NONE
#define PDX_CF_NONE			0x00
#define PDX_CF_NAME			0x01
#define PDX_CF_ATTRIBUTES	0x02
#endif	// PDX_CF_NONE

class AFX_EXT_CLASS CPropertiesDialogX : public CDialogX
{
	DECLARE_DYNAMIC(CPropertiesDialogX)

public:
	CPropertiesDialogX(CWnd* pParent = NULL);
	virtual	~CPropertiesDialogX();

	void	AddFile(LPCTSTR lpszFile);
	void	AddFiles(CStringArray &arrFiles);
	void	ClearFiles();
	DWORD	GetChangeFlags();

	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnPaint();
	afx_msg void	OnBnClickedPropTime();
	afx_msg void	OnBnClickedPropAttributes();

	BOOL	m_bReadOnly;
	BOOL	m_bHidden;
	BOOL	m_bArchive;
	BOOL	m_bSystem;
	BOOL	m_bROM;

#if (!defined(_DEBUG))
	CEdit	m_ctlNameEdit;
#endif

	CString	m_strNameEdit;
	CStatic	m_ctlName;
	CStatic	m_ctlTypeH;
	CStatic	m_ctlLocationH;
	CStatic	m_ctlSizeH;
	CStatic	m_ctlContainsH;
	CStatic	m_ctlCapacityH;
	CStatic	m_ctlType;
	CStatic	m_ctlLocation;
	CStatic	m_ctlSize;
	CStatic	m_ctlContains;
	CStatic	m_ctlCapacity;
	CButton	m_ctlTime;
	CButton m_ctlAttributes;

protected:
	void	InitializeX();

	CString	GetSizeString(ULARGE_INTEGER uiSize);
	CString	GetContainsString();
	CString	GetTimeString(SYSTEMTIME &st);
	CString	GetCapacityString();

	virtual void	OnOK();
	virtual void	DoDataExchange(CDataExchange* pDX);

	DWORD			m_dwChangeFlags;
	CStringArray	m_arrFiles;
	CByteArray		m_arrTypes;
	ULARGE_INTEGER	m_uiSize;
	int				m_nFiles;
	int				m_nFolders;
	CFileInfoX		m_FI;
	CString			m_strCreated;
	CString			m_strModified;
	CString			m_strAccessed;

	DECLARE_MESSAGE_MAP()
};
