// PhoneX.h

#pragma once

#include "InAPI.h"

// Phone Edition Detection
#define DetectWindowsPhone()		DetectPhoneEdition()
#define DetectProfessionalEdition()	DetectPhoneEdition()

BOOL	INAPI_API	DetectPhoneEdition(void);

HWND	INAPI_API	FindPhoneWindow(void);

// Retrieve Phone Number from SIM
HRESULT	INAPI_API	GetPhoneNumberX(LPTSTR szNumber, UINT cchNumber, UINT nLineNumber);

BOOL	INAPI_API	GetRingSoundFromSpeakerOnly(void);
void	INAPI_API	SetRingSoundFromSpeakerOnly(BOOL bSpeaker = TRUE);
