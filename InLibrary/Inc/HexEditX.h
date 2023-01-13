// HexEditX.h

#pragma once

class AFX_EXT_CLASS CHexEditX : public CEdit
{
	DECLARE_DYNAMIC(CHexEditX)

public:
	CHexEditX();
	virtual	~CHexEditX();

	// Data Manipulations
	BOOL	SetData(LPBYTE lpData, int& cbData);
	BOOL	GetData(LPBYTE lpData, int& cbData);

	// Selection Manipulations
	void	SetSelection(int nBegin, int nEnd);
	CSize	GetSelection();

	// Settings
	void	SetFontFace(LPCTSTR szFontFace = _T("Courier New"));
	void	SetBytesPerRow(int nBytes = 4);
	void	ShowAddress(BOOL bShow = TRUE);
	void	ShowHexData(BOOL bShow = TRUE);
	void	ShowASCIIData(BOOL bShow = TRUE);
	void	UseFarAddress(BOOL bUse = FALSE);

	virtual BOOL	Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	afx_msg void	OnEditCut();
	afx_msg void	OnEditCopy();
	afx_msg void	OnEditPaste();
	afx_msg void	OnEditClear();
	afx_msg void	OnEditSelectAll();
	afx_msg UINT	OnGetDlgCode();
	afx_msg void	OnPaint();
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnKillFocus(CWnd* pNewWnd);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

protected:
	void	InitializeX();
	void	CleanUpX();

	inline BOOL		IsSelected(void);
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);

	CPoint	CalcPos(int x, int y);
	void	CreateAddressCaret();
	void	CreateEditCaret();
	void	UpdateScrollbar();
	void	Move(int x, int y);
	void	RepositionCaret(int nAddress);
	void	NormalizeSelection();
	void	SelDelete(int nStart, int nEnd);
	void	SelInsert(int nStart, int nLength);

	enum EDITMODE
	{
		EDIT_NONE,
		EDIT_ASCII,
		EDIT_HIGH,
		EDIT_LOW
	};

	// Data Buffer
	LPBYTE	m_lpData;
	BYTE	m_cInitBuffer;
	int		m_nLength;

	// Indexes
	int			m_nTopIndex;
	int			m_nCurrentAddress;
	EDITMODE	m_nCurrentMode;
	BOOL		m_bIncompleted;
	int			m_nBytesPerRow;
	int			m_nLinesPerPage;

	// Selection Marks
	int		m_nSelectionBegin;
	int		m_nSelectionEnd;

	// Item Manipulations
	BOOL	m_bShowAddress;
	BOOL	m_bShowHex;
	BOOL	m_bShowASCII;
	BOOL	m_bFarAddress;

	// Internal Indexes
	BOOL	m_bUpdate;
	int		m_nLineHeight;
	int		m_nNullWidth;
	int		m_nOffsetAddress;
	int		m_nOffsetHex;
	int		m_nOffsetASCII;
	CPoint	m_ptEdit;

	// Font Face
	CString	m_strFontFace;

	DECLARE_MESSAGE_MAP()
};
