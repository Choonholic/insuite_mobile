// ClipboardX.h

#pragma once

#include "InAPI.h"

// Standard Clipboard Commands
void	INAPI_API	Cut(void);
void	INAPI_API	Copy(void);
void	INAPI_API	Paste(void);

// Customized Clipboard Commands
BOOL	INAPI_API	CopyText(LPCTSTR lpszText);
