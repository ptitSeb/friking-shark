#include "./stdafx.h"
#include "GameGUILib.h"
#include "GameGUIGroup.h"

CGameGUIGroup::CGameGUIGroup(void)
{
  m_bActive=false;
}

CGameGUIGroup::~CGameGUIGroup(void)
{
}
bool CGameGUIGroup::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=CGameWindowBase::InitWindow(piParent,bPopup);
	return bResult;
}