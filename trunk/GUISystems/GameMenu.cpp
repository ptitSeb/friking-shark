#include "StdAfx.h"
#include "GameRuntimeLib.h"
#include "GameGUILib.h"
#include ".\gamemenu.h"

CGameMenu::CGameMenu(void)
{
	m_piBTContinue=NULL;
	m_piBTEndGame=NULL;
}

CGameMenu::~CGameMenu(void)
{
}

void CGameMenu::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTContinue)
	{
		EndDialog(eGameMenuAction_Continue);
	}
	if(piControl==m_piBTEndGame)
	{
		EndDialog(eGameMenuAction_EndGame);
	}
}

void CGameMenu::OnInitDialog()
{
}
