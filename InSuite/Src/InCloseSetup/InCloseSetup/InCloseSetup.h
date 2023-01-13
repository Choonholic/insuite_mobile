// InCloseSetup.h

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif

#include "ExPackX.h"

#ifndef PP_GENERAL
#define PP_GENERAL		0x01
#define PP_ACTIONS		0x02
#define PP_BACKGROUND	0x04
#define PP_MENU			0x08
#define PP_EXCLUDED		0x10
#endif

#define EXPACK_ENABLED

class CInCloseSetupApp : public CWinApp
{
public:
	CInCloseSetupApp();

	HWND	FindInClose();

	virtual BOOL	InitInstance();

	UINT	WM_CL_SETTINGCHAGE;

protected:
#ifdef	EXPACK_ENABLED
	CExPackX	XP;
	HINSTANCE	m_hResInst;
#endif

	DECLARE_MESSAGE_MAP()
};

extern CInCloseSetupApp	theApp;
