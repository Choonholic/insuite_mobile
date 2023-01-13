// stdafx.h

#pragma once

#define WINVER	_WIN32_WCE

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifdef _CE_DCOM
#define _ATL_APARTMENT_THREADED
#endif

#include <ceconfig.h>

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#define SHELL_AYGSHELL
#endif

#include <afxwin.h>			// MFC core and standard components
#include <afxext.h>			// MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>			// MFC OLE classes
#include <afxodlgs.h>		// MFC OLE dialog classes
#include <afxdisp.h>		// MFC Automation classes
#endif	// _AFX_NO_OLE_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif	// _AFX_NO_AFXCMN_SUPPORT

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#ifndef _DEVICE_RESOLUTION_AWARE
#define _DEVICE_RESOLUTION_AWARE
#endif
#endif

#ifdef _DEVICE_RESOLUTION_AWARE
#include "DeviceResolutionAware.h"
#endif

#ifdef SHELL_AYGSHELL
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib") 
#endif	// SHELL_AYGSHELL

#if (_WIN32_WCE < 0x500) && (defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP))
	#pragma comment(lib, "ccrtrtti.lib")
	#ifdef _X86_
		#if defined(_DEBUG)
			#pragma comment(lib, "libcmtx86d.lib")
		#else
			#pragma comment(lib, "libcmtx86.lib")
		#endif
	#endif
#endif

#include <altcecrt.h>

// Link InLibrary
#if defined(_DEBUG)
	#pragma comment(lib, "InAPId.lib")
#else
	#pragma comment(lib, "InAPI.lib")
#endif
