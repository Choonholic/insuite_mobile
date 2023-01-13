// FontInfo.h

#pragma once

#ifndef FONTLINK_KEY
#define FONTLINK_KEY		_T("SOFTWARE\\Microsoft\\FontLink")
#endif

#ifndef SYSLINK_KEY
#define SYSLINK_KEY			_T("SOFTWARE\\Microsoft\\FontLink\\SystemLink")
#define SYSLINK_DEFAULT		_T("Tahoma")
#define SYSLINK_FIXED		_T("Courier New")
#endif

#ifndef SKIP_KEY
#define SKIP_KEY			_T("SOFTWARE\\Microsoft\\FontLink\\SkipTable")
#define SKIP_DEFAULT		_T("Tahoma")
#endif

#ifndef GDI_KEY
#define GDI_KEY				_T("System\\GDI")
#define GDI_FONTLINKMETHODS	_T("FontLinkMethods")
#endif

#ifndef FONTPATH_KEY
#define FONTPATH_KEY		_T("SOFTWARE\\Microsoft\\FontPath")
#define FONTPATH_VALUE		_T("FontPath")
#endif

#ifndef EUDC_KEY
#define EUDC_KEY			_T("EUDC")
#define SYSDEFEUDC_VALUE	_T("SystemDefaultEUDCFont")
#endif

#ifndef FONTS_KEY
#define FONTS_KEY
#define SYSFNT_KEY			_T("System\\GDI\\SYSFNT")
#define MENUFNT_KEY			_T("System\\GWE\\Menu")
#define OOMFNT_KEY			_T("System\\GWE\\OOMFnt")
#define BUTTONFNT_KEY		_T("System\\GWE\\Button")
#define FONTALIAS_KEY       _T("System\\GDI\\V1")
#endif

#ifndef SKIPTABLE_I18N
#define SKIPTABLE_I18N
#define SKIPTABLE_1033		_T("")
#define SKIPTABLE_1041		_T("005c,00a5,007e,0391-03c9,2026,2116,221a,25a0-25ff")
#define SKIPTABLE_1042		_T("005c")
#define SKIPTABLE_2052		_T("005c,00a5,007e,0391-03c9,2026,2116,221a,25a0-25ff")
#define SKIPTABLE_1028		_T("005c,00a5,007e,0391-03c9,2026,2116,221a,25a0-25ff")
#endif
