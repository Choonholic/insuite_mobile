// ColorX.h

#pragma once

#include "InAPI.h"

// Color Manipulations
DWORD	INAPI_API	GetShellBaseHue(void);

#ifndef SHCOLOR_SET
#define SHCOLOR_SET					0x00
#define SHCOLOR_UNKNOWN_00			0x00
#define SHCOLOR_UNKNOWN_01			0x01
#define SHCOLOR_UNKNOWN_02			0x02
#define SHCOLOR_UNKNOWN_03			0x03
#define SHCOLOR_TODAY_TEXT			0x04	// Today, Text
#define SHCOLOR_TASKBAR_LEFTEDGE	0x05	// Task Bar, Left Edge
#define SHCOLOR_TODAY_SEPARATEBAR	0x06	// Today, Separate Bar
#define SHCOLOR_SYSTRAY_BACKGROUND	0x07	// System Tray, Background
#define SHCOLOR_TASKBAR_RIGHTEDGE	0x08	// Task Bar, Right Edge
#define SHCOLOR_TASKBAR_TEXT		0x09	// Task Bar, Text
#define SHCOLOR_NOTITRAY_HIGHLIGHT	0x0A	// Notification Tray, Highlight
#define SHCOLOR_NOTITRAY_BACKGROUND	0x0B	// Notification Tray, Background
#define SHCOLOR_NOTIICON_HIGHLIGHT	0x0C	// Notification Icon, Highlight
#define SHCOLOR_MESSAGE_LEFTEDGE	0x0D	// Notification, Title Left Edge
#define SHCOLOR_MESSAGE_RIGHTEDGE	0x0E	// Notification, Title Right Edge
#define SHCOLOR_LIST_LEFTEDGE2		0x0F	// Extended List Control, 2nd Background Left Edge
#define SHCOLOR_LIST_RIGHTEDGE2		0x10	// Extended List Control, 2nd Background Right Edge
#define SHCOLOR_UNKNOWN_17			0x11
#define SHCOLOR_UNKNOWN_18			0x12
#define SHCOLOR_UNKNOWN_19			0x13
#define SHCOLOR_UNKNOWN_20			0x14
#define SHCOLOR_UNKNOWN_21			0x15
#define SHCOLOR_UNKNOWN_22			0x16
#define SHCOLOR_UNKNOWN_23			0x17
#define SHCOLOR_UNKNOWN_24			0x18
#define SHCOLOR_CRITICAL_LEFTEDGE	0x19	// Critical Notification, Title Left Edge
#define SHCOLOR_CRITICAL_RIGHTEDGE	0x1A	// Critical Notification, Title Right Edge
#define SHCOLOR_MESSAGE_TITLE		0x1B	// Notification, Title Text
#define SHCOLOR_CRITICAL_TITLE		0x1C	// Critical Notification, Title Text
#define SHCOLOR_UNKNOWN_29			0x1D
#define SHCOLOR_UNKNOWN_30			0x1E
#define SHCOLOR_UNKNOWN_31			0x1F
#define SHCOLOR_UNKNOWN_32			0x20
#define SHCOLOR_UNKNOWN_33			0x21
#define SHCOLOR_UNKNOWN_34			0x22
#define SHCOLOR_UNKNOWN_35			0x23
#define SHCOLOR_LIST_LEFTEDGE1		0x24	// Extended List Control, 1st Background Left Edge
#define SHCOLOR_LIST_RIGHTEDGE1		0x25	// Extended List Control, 1st Background Right Edge
#define SHCOLOR_UNKNOWN_38			0x26	// Used in Spiral.tsk
#define SHCOLOR_STARTMENU_TEXT		0x27	// Start Menu, Text
#define SHCOLOR_TAPNHOLD_DOTS		0x28	// Tap and Hold Dots
#endif

DWORD	INAPI_API	GetUserShellColor(int nIndex);
DWORD	INAPI_API	GetShellColor(int nIndex);
DWORD	INAPI_API	GetDefShellColor(int nIndex);

// SetDefShellColor is not implemented because values of DefSHColor are unique.
BOOL	INAPI_API	SetUserShellColor(int nIndex, DWORD dwColor);
BOOL	INAPI_API	SetShellColor(int nIndex, DWORD dwColor);

#ifndef SYSCOLOR_SET
#define SYSCOLOR_SET				0x00
#define SYSCOLOR_UNKNOWN_00			0x00
#define SYSCOLOR_DESKTOP_BACK		0x01	// Desktop, Background
#define SYSCOLOR_ACTIVE_BACK		0x02	// Active Title Bar, Background
#define SYSCOLOR_INACTIVE_BACK		0x03	// Inactive Title Bar, Background
#define SYSCOLOR_POPUP_BACK			0x04	// Popup Menu, Background
#define SYSCOLOR_WINDOW_BACK		0x05	// Window, Background
#define SYSCOLOR_UNKNOWN_06			0x06
#define SYSCOLOR_POPUP_TEXT			0x07	// Popup Menu, Text
#define SYSCOLOR_WINDOW_TEXT		0x08	// Window, Text
#define SYSCOLOR_ACTIVE_TEXT		0x09	// Active Title Bar, Text
#define SYSCOLOR_UNKNOWN_10			0x0A
#define SYSCOLOR_3DOBJECT_BACK		0x0B	// 3D Objects, Background
#define SYSCOLOR_UNKNOWN_12			0x0C
#define SYSCOLOR_SELECTED_BACK		0x0D	// Selected Item, Background
#define SYSCOLOR_SELECTED_TEXT		0x0E	// Selected Item, Text
#define SYSCOLOR_MENU_BACK			0x0F	// Menu Bar, Background
#define SYSCOLOR_UNKNOWN_16			0x10
#define SYSCOLOR_DISABLED_TEXT		0x11	// Disabled Item, Text
#define SYSCOLOR_MENU_TEXT			0x12	// Menu Bar, Text
#define SYSCOLOR_3DOBJECT_TEXT		0x12	// 3D Objects, Text
#define SYSCOLOR_INACTIVE_TEXT		0x13	// Inactive Title Bar, Text
#define SYSCOLOR_UNKNOWN_20			0x14
#define SYSCOLOR_UNKNOWN_21			0x15
#define SYSCOLOR_UNKNOWN_22			0x16
#define SYSCOLOR_UNKNOWN_23			0x17
#define SYSCOLOR_UNKNOWN_24			0x18
#define SYSCOLOR_DIALOG_BACK		0x19	// Dialog Box, Background
#define SYSCOLOR_DIALOG_TEXT		0x1A	// Dialog Box, Text
#define SYSCOLOR_UNKNOWN_27			0x1B
#define SYSCOLOR_UNKNOWN_28			0x1C
#endif

DWORD	INAPI_API	GetSystemColor(int nIndex);
DWORD	INAPI_API	GetDefSystemColor(int nIndex);

// SetDefSystemColor is not implemented because values of DefSysColor are unique.
BOOL	INAPI_API	SetSystemColor(int nIndex, DWORD dwColor);
