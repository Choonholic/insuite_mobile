// ConnectX.h

#pragma once

#include "InAPI.h"

// Connection Manager
HANDLE	INAPI_API	EstablishConnection(void);
BOOL	INAPI_API	ReleaseConnection(HANDLE hConnection);
void	INAPI_API	EndAllDataCalls(void);
BOOL	INAPI_API	IsActiveDataCall(void);

// ActiveSync Status
BOOL	INAPI_API	IsCradled(void);
BOOL	INAPI_API	IsSynchronizing(void);

// RAS Connections
int		INAPI_API	GetRasConnections(void);
BOOL	INAPI_API	EnableRasConnection(int nIndex, BOOL bEnable = TRUE);
LPCTSTR	INAPI_API	GetRasConnectionString(int nIndex);
