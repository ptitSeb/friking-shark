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
#include "resource.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GUISystems.h"
#include "MainWindow.h"

#define AVAILABLE_LEVELS 5

extern CSystemModuleHelper *g_pSystemModuleHelper;

CMainWindow::CMainWindow(void)
{
	m_eGameMode=eGameMode_Normal;
	m_eStage=eInterfaceStage_MainMenu;
	m_nCurrentLevel=0;
	m_dBackgroundAlpha=0;
	m_bVisible=true;
	m_nPoints=0;
	m_nLivesLeft=0;
	m_nWeaponLevel=0;
	
}

CMainWindow::~CMainWindow(void)
{
}

bool CMainWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=CGameWindowBase::InitWindow(piParent,bPopup);
	if(bResult)
	{
		m_PlayerProfile.Create(m_piSystem,"CPlayerProfile","");
		
		CViewportWrapper viewport;
		viewport.Attach("GameGUI","Viewport");
		if(viewport.m_piViewport){viewport.m_piViewport->SetCaption("Friking Shark");}

		bResult=m_GUIConfigFile.Open("Scripts/GameGUI.cfg");

		if(bResult)
		{
			m_MainMenuDialog.Create("GameGUI","CMainMenu","MainMenu");
			m_GameMenuDialog.Create("GameGUI","CGameMenu","GameMenu");
			m_ConfirmationDialog.Create("GameGUI","CConfirmationDialog","ConfirmationDialog");
			m_GameOverDialog.Create("GameGUI","CGameOverDialog","GameOverDialog");
			m_HighScoresDialog.Create("GameGUI","CHighScoresDialog","HighScoresDialog");
			m_ControlsDialog.Create("GameGUI","CControlsDialog","ControlsDialog");			
			m_LevelOptionsDialog.Create("GameGUI","CLevelOptions","LevelOptions");
			m_HighScoresTable.Create("GameGUI","CHighScoresTable","HighScoreTable");
			m_CreditsDialog.Create("GameGUI","CCreditsDialog","CreditsDialog");
			
			m_MainMenuDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\MainMenu"));
			m_GameMenuDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\GameMenu"));
			m_ConfirmationDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\ConfirmationDialog"));
			m_GameOverDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\GameOverDialog"));
			m_HighScoresDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\HighScoresDialog"));
			m_ControlsDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\ControlsDialog"));
			m_LevelOptionsDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\LevelOptions"));
			m_CreditsDialog.m_piSerializable->Unserialize(m_GUIConfigFile.GetNode("GameDialogs\\CreditsDialog"));
			
			if(m_HighScoresConfigFile.Open("HighScores.cfg"))
			{
				m_HighScoresTable.m_piSerializable->Unserialize(m_HighScoresConfigFile.GetNode("Local"));
			}
			if(m_PlayerProfileConfigFile.Open("PlayerProfiles.cfg"))
			{
				m_PlayerProfile.m_piSerializable->Unserialize(m_PlayerProfileConfigFile.GetNode("Default"));
			}
		}

		if(bResult)
		{
			m_piGameInterface->Show(false);
			m_piSTBackground->Show(true);
		}
		
		m_piGUIManager->SetFocus(this);
	}
	return bResult;
}

void CMainWindow::Destroy()
{
	if(m_HighScoresTable.m_piSerializable)
	{
		m_HighScoresTable.m_piSerializable->Serialize(m_HighScoresConfigFile.AddNode("Local"));
	}
	if(m_PlayerProfile.m_piSerializable)
	{
		m_PlayerProfile.m_piSerializable->Serialize(m_PlayerProfileConfigFile.AddNode("Default"));
	}
	m_HighScoresConfigFile.Save("HighScores.cfg");
	m_PlayerProfileConfigFile.Save("PlayerProfiles.cfg");
	m_PlayerProfile.Destroy();
	CGameWindowBase::Destroy();
}

void CMainWindow::OnDraw(IGenericRender *piRender)
{
	CGameWindowBase::OnDraw(piRender);
	
	if(m_eStage==eInterfaceStage_MainMenu 
		&& !m_MainMenuDialog.m_piDialog->IsVisible()
		&& !m_LevelOptionsDialog.m_piDialog->IsVisible()
		&& !m_HighScoresDialog.m_piDialog->IsVisible()
		&& !m_ControlsDialog.m_piDialog->IsVisible()
		&& !m_CreditsDialog.m_piDialog->IsVisible())
	{
		bool bProcessed=false;
		OnKeyDown(GK_ESCAPE,&bProcessed);
	}
	
	if(m_eStage==eInterfaceStage_LaunchNextLevel)
	{		
		char sFile[200];
		sprintf(sFile,"Level%d.ges",m_nCurrentLevel+1);
		m_eStage=eInterfaceStage_Playing;
		m_piGUIManager->ShowMouseCursor(false);
		m_piSTBackground->Show(false);
		m_piGameInterface->LoadScenario(sFile);
		m_piGameInterface->StartGame(m_PlayerProfile.m_piPlayerProfile,m_eGameMode,m_nPoints,m_nLivesLeft,m_nWeaponLevel);
		m_piGameInterface->Show(true);
	}
}

void CMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(nKey==GK_ESCAPE)
	{
		*pbProcessed=true;
		if(m_eStage==eInterfaceStage_MainMenu)
		{
			int result=m_MainMenuDialog.m_piDialog->Execute(this);
			if(result==eMainMenuAction_Exit)
			{
				m_piGUIManager->ExitGUILoop();
			}
			else if(result==eMainMenuAction_HighScores)
			{
				m_HighScoresDialog.m_piHighScoresDialog->ShowScores(this,m_HighScoresTable.m_piHighScoresTable,-1,true);
			}			
			else if(result==eMainMenuAction_Credits)
			{
				m_CreditsDialog.m_piDialog->Execute(this);
			}
			else if(result==eMainMenuAction_Controls)
			{
				m_ControlsDialog.m_piControlsDialog->SelectControls(this,m_PlayerProfile.m_piPlayerProfile);
			}
			else if(result==eMainMenuAction_NewGame)
			{
				EGameDifficulty eDifficulty=eGameDifficulty_Normal;
				if(m_PlayerProfile.m_piPlayerProfile)
				{
					double dDifficulty=m_PlayerProfile.m_piPlayerProfile->GetDifficulty();
					if(dDifficulty>=6){eDifficulty=eGameDifficulty_VeryHard;}
					else if(dDifficulty>=4){eDifficulty=eGameDifficulty_Hard;}
					else if(dDifficulty>=2){eDifficulty=eGameDifficulty_Normal;}
					else {eDifficulty=eGameDifficulty_Easy;}
				}
				if(m_LevelOptionsDialog.m_piLevelOptions->SelectOptions(this,&m_eGameMode,&eDifficulty,&m_nCurrentLevel))
				{
					char sFile[200];
					sprintf(sFile,"Level%d.ges",m_nCurrentLevel+1);
					
					m_nWeaponLevel=0;
					m_nPoints=0;
					m_nLivesLeft=3;
					
					if(m_PlayerProfile.m_piPlayerProfile)
					{
						double dDifficulty=0;
						switch(eDifficulty)
						{
							case eGameDifficulty_Easy: dDifficulty=0;break;
							case eGameDifficulty_Normal: dDifficulty=2;break;
							case eGameDifficulty_Hard: dDifficulty=4;break;
							case eGameDifficulty_VeryHard: dDifficulty=6;break;
							default: dDifficulty=0;break;
						}
						m_PlayerProfile.m_piPlayerProfile->SetDifficulty(dDifficulty);
					}
					
					SHighScoreRow sHighScore;
					if(m_HighScoresTable.m_piHighScoresTable){sHighScore=m_HighScoresTable.m_piHighScoresTable->GetRow(0);}
					
					m_eStage=eInterfaceStage_Playing;
					m_piGUIManager->ShowMouseCursor(false);
					m_piSTBackground->Show(false);
					m_piGameInterface->SetHighScore(sHighScore.nScore);
					m_piGameInterface->LoadScenario(sFile);
					m_piGameInterface->StartGame(m_PlayerProfile.m_piPlayerProfile,m_eGameMode,m_nPoints,m_nLivesLeft,m_nWeaponLevel);
					m_piGameInterface->Show(true);
				}
			}
			m_piGUIManager->SetFocus(this);
		}
		else if(m_eStage==eInterfaceStage_Playing)
		{
			m_piGUIManager->ShowMouseCursor(true);
			m_piGameInterface->Freeze(true);
			int result=m_GameMenuDialog.m_piDialog->Execute(this);
			m_piGameInterface->Freeze(false);
			if(result==eGameMenuAction_EndGame)
			{
				m_piGameInterface->StopGame();
				m_piGameInterface->CloseScenario();
				m_piGameInterface->Show(false);
				m_piSTBackground->Show(true);
				m_eStage=eInterfaceStage_MainMenu;
			}
			else
			{
				m_piGUIManager->ShowMouseCursor(false);
			}
			m_piGUIManager->SetFocus(this);
		}
	}
}

void CMainWindow::OnGameOverCourtainClosed()
{
	m_eStage=eInterfaceStage_MainMenu;
	m_piGameInterface->Show(false);
	m_piSTBackground->Show(true);
	m_piGameInterface->StopGame();
	m_piGameInterface->CloseScenario();
}

void CMainWindow::OnScenarioFinished(eScenarioFinishedReason eReason,unsigned int nPoints, unsigned int nLivesLeft, unsigned int nWeaponLevel)
{
	m_piGUIManager->ShowMouseCursor(true);
	m_piGameInterface->Freeze(true);
	if(eReason!=eScenarioFinishedReason_Completed)
	{
		m_GameOverDialog.m_piDialog->Execute(this);
		m_piGameInterface->Freeze(false);
		m_eStage=eInterfaceStage_HighScores;
		
		EGameDifficulty eDifficulty=eGameDifficulty_Normal;
		if(m_PlayerProfile.m_piPlayerProfile)
		{
			double dDifficulty=m_PlayerProfile.m_piPlayerProfile->GetDifficulty();
			if(dDifficulty>=6){eDifficulty=eGameDifficulty_VeryHard;}
			else if(dDifficulty>=4){eDifficulty=eGameDifficulty_Hard;}
			else if(dDifficulty>=2){eDifficulty=eGameDifficulty_Normal;}
			else {eDifficulty=eGameDifficulty_Easy;}
		}
		
		SHighScoreRow row;
		row.sName="<type your name>";
		row.eDifficulty=eDifficulty;
		row.nScore=nPoints;
		
		unsigned int nRow=m_HighScoresTable.m_piHighScoresTable?m_HighScoresTable.m_piHighScoresTable->AddRow(row):-1;
		m_HighScoresDialog.m_piHighScoresDialog->ShowScores(this,m_HighScoresTable.m_piHighScoresTable,nRow,false);
	}
	else
	{
		m_piGameInterface->Freeze(false);
		m_nPoints=nPoints;
		m_nLivesLeft=nLivesLeft;
		m_nWeaponLevel=nWeaponLevel;
		
		m_nCurrentLevel++;
		if(m_nCurrentLevel>=AVAILABLE_LEVELS)
		{
			m_nCurrentLevel=0;
		}
		m_eStage=eInterfaceStage_LaunchNextLevel;
		m_piGameInterface->Show(false);
		m_piGameInterface->StopGame();
		m_piGameInterface->CloseScenario();
	}
}

void CMainWindow::OnWantFocus(bool *pbWant){*pbWant=true;}