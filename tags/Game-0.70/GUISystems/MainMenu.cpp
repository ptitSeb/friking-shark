#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "MainMenu.h"

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
