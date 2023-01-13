// InLibrary_Messages.h

#pragma once

// ButtonsX Messages
#ifndef WM_BTNX_MESSAGES
#define WM_BTNX_MESSAGES
#define WM_BTNX_TTALK_DOWN			(WM_USER + 0x0400)
#define WM_BTNX_TTALK_UP			(WM_USER + 0x0401)
#define WM_BTNX_TEND_DOWN			(WM_USER + 0x0402)
#define WM_BTNX_TEND_UP				(WM_USER + 0x0403)
#define WM_BTNX_KEYLOCK_DOWN		(WM_USER + 0x0404)
#define WM_BTNX_KEYLOCK_UP			(WM_USER + 0x0405)
#define WM_BTNX_TVOLUMEUP_DOWN		(WM_USER + 0x0406)
#define WM_BTNX_TVOLUMEUP_UP		(WM_USER + 0x0407)
#define WM_BTNX_TVOLUMEDOWN_DOWN	(WM_USER + 0x0408)
#define WM_BTNX_TVOLUMEDOWN_UP		(WM_USER + 0x0409)
#endif

// ListCtrlX Messages
#ifndef WM_LCX_MESSAGES
#define WM_LCX_MESSAGES
#define WM_LCX_TAPANDHOLD			(WM_USER + 0x0500)
#define WM_LCX_FLICK				(WM_USER + 0x0501)
#endif

// TreeCtrlX Messages
#ifndef WM_TCX_MESSAGES
#define WM_TCX_MESSAGES
#define WM_TCX_TAPANDHOLD			(WM_USER + 0x0510)
#define WM_TCX_CHECKTOGGLE			(WM_USER + 0x0511)
#endif

// CopyDialogX/MoveDialogX Messages
#ifndef WM_COPYX_MESSAGES
#define WM_COPYX_MESSAGES
#define WM_COPY_THREAD_FINISHED		(WM_USER + 0x0520)
#define WM_MOVE_THREAD_FINISHED		(WM_USER + 0x0528)
#endif

// DeleteDialogX Messages
#ifndef WM_DELETEX_MESSAGES
#define WM_DELETEX_MESSAGES
#define WM_DELETE_THREAD_FINISHED	(WM_USER + 0x0530)
#endif

// ZipDialogX/UnzipDialogX Messages
#ifndef WM_ZIPX_MESSAGES
#define WM_ZIPX_MESSAGES
#define WM_ZIP_THREAD_FINISHED		(WM_USER + 0x0540)
#define WM_UNZIP_THREAD_FINISHED	(WM_USER + 0x0548)
#endif

// HTTPDialogX/FTPDialogX Messages
#ifndef WM_HTTPX_MESSAGES
#define WM_HTTPX_MESSAGES
#define WM_HTTP_THREAD_FINISHED		(WM_USER + 0x0550)
#define WM_FTP_THREAD_FINISHED		(WM_USER + 0x0558)
#endif
