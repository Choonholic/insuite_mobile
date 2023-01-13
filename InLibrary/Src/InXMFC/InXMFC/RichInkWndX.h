// RichInkWndX.h

#pragma once

#include "richink.h"

class AFX_EXT_CLASS CRichInkWndX : public CWnd
{
	DECLARE_DYNAMIC(CRichInkWndX)

public:
	CRichInkWndX();
	virtual	~CRichInkWndX();

	BOOL	Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// Clipboard Events
	BOOL	CanUndo();
	BOOL	CanRedo();
	BOOL	CanPaste(UINT uFormat = 0);

	void	UndoEvent();
	void	RedoEvent();
	void	ClearAll(BOOL bDirty = TRUE);

	// Retrieving / Applying Styles
	UINT	GetPageStyle();
	UINT	GetPenMode();
	UINT	GetView();
	UINT	GetWrapMode();
	UINT	GetZoomPercent();

	void	SetPageStyle(UINT nPageStyle);
	void	SetPenMode(UINT nPenMode);
	void	SetView(UINT nView);
	void	SetWrapMode(UINT nWrapMode);
	void	SetZoomPercent(UINT nZoomPercent);

	// Additional Properties
	void	SetViewAttributes(const VIEWATTRIBUTES* pViewAttr);
	UINT	SetInkLayer(UINT nInkLayer);

	void	InsertLinks(LPCTSTR pszLink, UINT nLength = -1);

	// Serialization
	BOOL	StreamIn(UINT nFormat, EDITSTREAM* pStream);
	BOOL	StreamOut(UINT nFormat, EDITSTREAM* pStream);

protected:
	DECLARE_MESSAGE_MAP()
};
