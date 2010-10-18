#include "StdAfx.h"
#include "GameGUILib.h"
#include ".\gameguigroup.h"

CGameGUIGroup::CGameGUIGroup(void)
{
}

CGameGUIGroup::~CGameGUIGroup(void)
{
}
bool CGameGUIGroup::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=CGameWindowBase::InitWindow(piParent,bPopup);
	return bResult;
}
