// ItemSearchX.cpp

#include "StdAfx.h"
#include "InExplorer.h"
#include "ItemSearchX.h"

CItemSearchX::CItemSearchX(void)
{
}

CItemSearchX::~CItemSearchX(void)
{
}

BOOL CItemSearchX::FoundAction(LPSEARCHINFOX lpSIX)
{
	m_arrFoundFiles.Add(lpSIX->szFullPath);
	return TRUE;
}
