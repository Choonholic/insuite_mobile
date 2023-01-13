// ScreenX.h

#pragma once

#include "InAPI.h"

// Touch Sensitivity
DWORD	INAPI_API	GetErrorAnalysisSensitivity(void);
void	INAPI_API	SetErrorAnalysisSensitivity(DWORD dwMaxError = 7);
