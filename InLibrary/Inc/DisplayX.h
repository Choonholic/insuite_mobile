// DisplayX.h

#pragma once

#include "InAPI.h"

// Display Driver Escape Codes
#ifndef QUERYESCSUPPORT
#define QUERYESCSUPPORT				8		// Determines whether a display driver supports a particular escape mode.
#define GETVFRAMEPHYSICAL			6144	// Returns the physical memory location of the video frame buffer.
#define GETVFRAMELEN				6145	// Returns the length of the video frame buffer.
#define DBGDRIVERSTAT				6146	// Returns display driver statistics.
#define SETPOWERMANAGEMENT			6147	// Requests a particular power management state.
#define GETPOWERMANAGEMENT			6148	// Queries the current power management state.
#define CONTRASTCOMMAND				6149	// Applies a display contrast setting.
#define DRVESC_SETGAMMAVALUE		6201	// MUST set the gamma value through the SystemParametersInfo fuction with the SPI_SETFONTSMOOTHINGCONTRAST flag instead of this.
#define DRVESC_GETGAMMAVALUE		6202	// MUST retrieve the gamma value through the SystemParametersInfo fuction with the SPI_GETFONTSMOOTHINGCONTRAST flag instead of this.
#define DRVESC_SETSCREENROTATION	6301	// Sets the screen rotation value.
#define DRVESC_GETSCREENROTATION	6302	// Retrieves the screen rotation value.
#define DRVESC_SAVEVIDEOMEM			6501	// Saves the video RAM that must persist across a suspend pr resume operation.
#define DRVESC_RESTOREVIDEOMEM		6502	// Restores the contents of the video RAM after resuming operation.
#define DRVESC_QUERYVIDEOMEMUSED	6503	// Returns the amount of video memory in use.
#define DRVESC_USER					100000	// Starting point of user driver-specific escape codes.
#endif

// ContrastCmdInputParm Commands (ExtEscape and DrvEscape)
#ifndef CONTRAST_CMD_GET
#define CONTRAST_CMD_GET			0		// Gets the current contrast. The parm member is ignored.
#define CONTRAST_CMD_SET			1		// Sets the current contrast. The parm member contains the desired contrast setting.
#define CONTRAST_CMD_INCREASE		2		// Increases the contrast by the amount specified in parm.
#define CONTRAST_CMD_DECREASE		3		// Decreases the contrast by the amount specified in parm.
#define CONTRAST_CMD_DEFAULT		4		// Applies the default contrast setting. 
#define CONTRAST_CMD_MAX			5	
#endif

// GetScreenResolutionMode
#ifndef SRMODE_SQUARE
#define SRMODE_SQUARE				0		// Square Screen
#define SRMODE_PORTRAIT				1		// Portrait Screen
#define SRMODE_LANDSCAPE			2		// LandScape Screen
#endif

// GetDPI
#ifndef DPI_UNKNOWN
#define DPI_UNKNOWN					0xFFFE	// Unknown
#define DPI_DEFAULT					DPI_96	// Default DPI
#define DPI_96						96		// 96 DPI
#define DPI_192						192		// 192 DPI
#define DPI_128						128		// 128 DPI
#define DPI_131						131		// 131 DPI
#define DPI_UNREVEALED				0xFFFF	// Unrevealed (for newer versions)
#endif

// Contrast Commands
BOOL	INAPI_API	HandleContrastCommand(int nCmd, int* pParm);

// DPI Detector
DWORD	INAPI_API	GetDPI(void);

// Screen Resolution Manipulations
DWORD	INAPI_API	GetNarrowResolution(void);
DWORD	INAPI_API	GetWideResolution(void);
DWORD	INAPI_API	GetScreenResolutionMode(void);

// Display Mode Manipulations
void	INAPI_API	ToggleDisplayMode(void);
BOOL	INAPI_API	GetDisplayMode(void);
void	INAPI_API	SetDisplayMode(BOOL bState = FALSE);

// Rotation Mode Manipulations
LONG	INAPI_API	GetRotationMode(LPDWORD lpdwRotation);
LONG	INAPI_API	SetRotationMode(DWORD dwRotation = DMDO_0);
LONG	INAPI_API	ResetRotationMode(void);
