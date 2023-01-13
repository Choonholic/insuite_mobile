// InSuite_Messages.h

#pragma once

// InClose Messages
#ifndef WM_CL_MESSAGES
#define WM_CL_MESSAGES
#define WM_CL_MENUCHECK				(WM_USER + 0x0300)
#endif

// InGauge Messages
#ifndef WM_GG_MESSAGES
#define WM_GG_MESSAGES
#define WM_GG_BATTERYSTATUS			(WM_USER + 0x0310)
#define WM_GG_WIFISTATUS			(WM_USER + 0x0311)
#define WM_GG_BLUETOOTHSTATUS		(WM_USER + 0x0312)
#endif

// InLock Messages
#ifndef WM_LK_MESSAGES
#define WM_LK_MESSAGES
#define WM_LK_THREAD_FINISHED		(WM_USER + 0x0320)
#endif
