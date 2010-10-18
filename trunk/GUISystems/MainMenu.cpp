#include "StdAfx.h"
#include "GameRuntimeLib.h"
#include "GameGUILib.h"
#include ".\mainmenu.h"

CMainMenu::CMainMenu(void)
{
	m_piBTNewGame=NULL;
	m_piBTExit=NULL;
}

CMainMenu::~CMainMenu(void)
{
}

void CMainMenu::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTNewGame)
	{
		EndDialog(eMainMenuAction_NewGame);
	}
	if(piControl==m_piBTExit)
	{
		EndDialog(eMainMenuAction_Exit);
	}
}
void CMainMenu::OnInitDialog()
{

}
