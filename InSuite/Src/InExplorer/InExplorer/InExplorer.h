// InExplorer.h

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

#include "ExPackX.h"

#define EXPACK_ENABLED

class CInExplorerApp : public CWinApp
{
public:
	CInExplorerApp();

	virtual BOOL	InitInstance();

protected:
#ifdef	EXPACK_ENABLED
	CExPackX	XP;
	HINSTANCE	m_hResInst;
#endif

	DECLARE_MESSAGE_MAP()
};

extern CInExplorerApp	theApp;
