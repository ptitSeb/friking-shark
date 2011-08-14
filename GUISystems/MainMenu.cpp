//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GUISystems.h"
#include "MainMenu.h"

CMainMenu::CMainMenu(void)
{
	m_bAllowLoad=false;
	m_bAllowContinue=false;
}

CMainMenu::~CMainMenu(void)
{
}

void CMainMenu::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE)
	{
		*pbProcessed=true;
		return;
	}
	if(nKey==GK_RETURN){return;}
	CGameDialogBase::OnKeyDown(nKey,pbProcessed);
}

void CMainMenu::OnButtonClicked(IGameGUIButton *piControl)
{
	if(piControl==m_piBTNewGame)
	{
		EndDialog(eMainMenuAction_NewGame);
	}
	if(piControl==m_piBTContinue)
	{
		EndDialog(eMainMenuAction_Continue);
	}
	if(piControl==m_piBTLoad)
	{
		EndDialog(eMainMenuAction_Load);
	}
	if(piControl==m_piBTControls)
	{
		EndDialog(eMainMenuAction_Controls);
	}
	if(piControl==m_piBTHighScores)
	{
		EndDialog(eMainMenuAction_HighScores);
	}
	if(piControl==m_piBTCredits)
	{
		EndDialog(eMainMenuAction_Credits);
	}
	if(piControl==m_piBTExit)
	{
		EndDialog(eMainMenuAction_Exit);
	}
}
void CMainMenu::OnInitDialog()
{
	CGameDialogBase::OnInitDialog();
	
	if(m_piBTContinue){m_piBTContinue->Activate(m_bAllowContinue);}
	if(m_piBTLoad){m_piBTLoad->Activate(m_bAllowLoad);}
	if(m_piLastFocusedWindow && m_piLastFocusedWindow->IsActive())
	{
		IGameGUIButton *piButton=QI(IGameGUIButton,m_piLastFocusedWindow);
		if(piButton){piButton->DisableSounds();}
		m_piGUIManager->SetFocus(m_piLastFocusedWindow);
		if(piButton){piButton->EnableSounds();}
		REL(piButton);
	}
	else if(m_piBTNewGame)
	{
		m_piBTNewGame->DisableSounds();
		m_piGUIManager->SetFocus(m_piBTNewGame);
		m_piBTNewGame->EnableSounds();
	}
}

void CMainMenu::OnEndDialog()
{
	REL(m_piLastFocusedWindow);
	m_piLastFocusedWindow=GetFocusedDescendant();
	CGameDialogBase::OnEndDialog();
}

eMainMenuAction CMainMenu::Show(IGameWindow *piParent,bool bAllowContinue, bool bAllowLoad)
{
	m_bAllowLoad=bAllowLoad;
	m_bAllowContinue=bAllowContinue;
	int nRes=Execute(piParent);
	return (eMainMenuAction)nRes;
}
