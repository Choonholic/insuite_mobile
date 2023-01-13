// ButtonsX.h

#pragma once

#include "InAPI.h"

#ifndef BTNX_DEFS
#define BTNX_DEFS
#define BTNX_TALL			0x07
#define BTNX_TTALK			0x01
#define BTNX_TEND			0x02
#define BTNX_KEYLOCK		0x04
#define BTNX_TVOLUME		0x08
#endif

#ifndef BTNX_NOERROR
#define BTNX_NOERROR		0
#define BTNX_LIMITEXCEEDED	(-1)
#define BTNX_NOPHONE		(-2)
#define BTNX_NOTASKBAR		(-3)
#define BTNX_NOMSCPROG		(-4)
#define BTNX_NOHOOK			(-5)
#define BTNX_NOWND			(-6)
#endif

INT		INAPI_API	HookButtons(HWND hWnd, UINT uButtons);
INT		INAPI_API	UnhookButtons(HWND hWnd);
LRESULT	INAPI_API	DoDefaultProc(UINT uMessage);

BOOL	INAPI_API	IsAvailableAppButton(int nIndex);
BOOL	INAPI_API	UnregisterFunction(UINT uiMod, UINT uiKey);
