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

#define MAX_LEVELS 64
extern CSystemModuleHelper *g_pSystemModuleHelper;

CMainWindow::CMainWindow(void)
{
	m_eStage=eInterfaceStage_None;
	m_dBackgroundAlpha=0;
	m_nContinuePauseStartTime=0;
	m_nAvailableLevels=0;
	m_bVisible=true;
}

CMainWindow::~CMainWindow(void)
{
}

bool CMainWindow::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=CGameWindowBase::InitWindow(piParent,bPopup);
	if(bResult)
	{
		CViewportWrapper viewport;
		viewport.Attach("GameGUI","Viewport");
		if(viewport.m_piViewport){viewport.m_piViewport->SetCaption("Friking Shark");}

		bResult=m_GUIConfigFile.Open("Scripts/GameGUI.cfg");

		if(bResult)
		{
			m_SoundManager.Attach("GameGUI","SoundManager");
			m_MainMenuDialog.Attach("GameGUI","MainMenu");
			m_GameMenuDialog.Attach("GameGUI","GameMenu");
			m_OptionsMenuDialog.Attach("GameGUI","OptionsMenu");
			m_AudioOptionsDialog.Attach("GameGUI","AudioOptions");
			m_GameOverDialog.Attach("GameGUI","GameOverDialog");
			m_HighScoresDialog.Attach("GameGUI","HighScoresDialog");
			m_ControlsDialog.Attach("GameGUI","ControlsDialog");			
			m_LevelOptionsDialog.Attach("GameGUI","LevelOptions");
			m_CreditsDialog.Attach("GameGUI","CreditsDialog");
			m_LoadDialog.Attach("GameGUI","LoadDialog");
			m_SaveDialog.Attach("GameGUI","SaveDialog");
			m_HighScoresTable.Create("GameGUI","CHighScoresTable","HighScoreTable");
			if(m_HighScoresConfigFile.Open("../Player/HighScores.cfg"))
			{
				m_HighScoresTable.m_piSerializable->Unserialize(m_HighScoresConfigFile.GetNode("Local"));
			}
			if(m_PlayerProfileConfigFile.Open("../Player/PlayerProfiles.cfg"))
			{
				m_PlayerData.Unserialize(m_PlayerProfileConfigFile.GetNode("Default"));
			}
			if(m_PlayerData.m_PlayerProfile.m_piPlayerProfile==NULL)
			{
				m_PlayerData.m_PlayerProfile.Create(m_piSystem,"CPlayerProfile","");
			}
			if(m_SoundManager.m_piSoundManager)
			{
				m_SoundManager.m_piSoundManager->SetMasterVolume(m_PlayerData.m_nMasterVolume);
				m_SoundManager.m_piSoundManager->SetGroupVolume("Music",m_PlayerData.m_nMusicVolume);
				m_SoundManager.m_piSoundManager->SetGroupVolume("SoundFX",m_PlayerData.m_nSoundFXVolume);
			}
		}
		
		if(m_piSTBackground)
		{
			m_piSTBackground->Show(true);
			m_piSTBackground->SetText("Loading...");
		}
		
		m_piGUIManager->SetFocus(this);
	}
	
	for(int x=0;x<MAX_LEVELS;x++)
	{
		char sFile[200];
		sprintf(sFile,"Level%d.ges",x+1);
		if(!afexists(sFile)){break;}
		m_nAvailableLevels++;
	}
	return bResult;
}

void CMainWindow::Destroy()
{
	if(m_SoundManager.m_piSoundManager)
	{
		m_PlayerData.m_nMasterVolume=m_SoundManager.m_piSoundManager->GetMasterVolume();
		m_PlayerData.m_nMusicVolume=m_SoundManager.m_piSoundManager->GetGroupVolume("Music");
		m_PlayerData.m_nSoundFXVolume=m_SoundManager.m_piSoundManager->GetGroupVolume("SoundFX");
	}
	
	if(m_HighScoresTable.m_piSerializable)
	{
		m_HighScoresTable.m_piSerializable->Serialize(m_HighScoresConfigFile.AddNode("Local"));
	}
	m_PlayerData.Serialize(m_PlayerProfileConfigFile.AddNode("Default"));
	
	m_HighScoresConfigFile.Save("../Player/HighScores.cfg");
	m_PlayerProfileConfigFile.Save("../Player/PlayerProfiles.cfg");
	m_SoundManager.Detach();
	CGameWindowBase::Destroy();
}

void CMainWindow::OnDrawBackground(IGenericRender *piRender)
{
	if(m_piGameInterface==NULL){CGameWindowBase::OnDrawBackground(piRender);return;}
	
	SGameRect sInterfaceRect;
	m_piGameInterface->GetRect(&sInterfaceRect);
	if(sInterfaceRect==m_rRealRect){return;}
	
	if(m_dBackgroundAlpha>0.0)
	{
		SGameRect leftRect=m_rRealRect;
		SGameRect rightRect=m_rRealRect;
		leftRect.w=sInterfaceRect.x-m_rRealRect.x;
		rightRect.x=sInterfaceRect.x+sInterfaceRect.w;
		rightRect.w=m_rRealRect.x+m_rRealRect.w-rightRect.x;
		// avoid problems with decimals (dirty :P)
		leftRect.x--;leftRect.w+=2.0;
		rightRect.x--;rightRect.w+=2.0;
		// Render interface lateral bands
		piRender->RenderRect(leftRect.x,leftRect.y,leftRect.w,leftRect.h,m_vBackgroundColor,m_dBackgroundAlpha);
		piRender->RenderRect(rightRect.x,rightRect.y,rightRect.w,rightRect.h,m_vBackgroundColor,m_dBackgroundAlpha);
	}
}
void CMainWindow::OnDraw(IGenericRender *piRender)
{
	if(m_eStage==eInterfaceStage_None)
	{
		m_eStage=eInterfaceStage_Initializing;
	}
	else if(m_eStage==eInterfaceStage_Initializing)
	{
		if(m_piGameInterface)
		{
			m_piGameInterface->InitializeGameSystem();
			m_piGameInterface->LoadScenario("./Background.ges");
			m_piGameInterface->StartDemo();
			m_piSTBackground->SetText("");
		}
		m_eStage=eInterfaceStage_MainMenu;
	}
	else if(m_eStage==eInterfaceStage_MainMenu 
		&& !m_MainMenuDialog.m_piDialog->IsVisible()
		&& !m_OptionsMenuDialog.m_piDialog->IsVisible()
		&& !m_AudioOptionsDialog.m_piDialog->IsVisible()
		&& !m_LevelOptionsDialog.m_piDialog->IsVisible()
		&& !m_HighScoresDialog.m_piDialog->IsVisible()
		&& !m_ControlsDialog.m_piDialog->IsVisible()
		&& !m_CreditsDialog.m_piDialog->IsVisible()
		&& !m_LoadDialog.m_piDialog->IsVisible()
		&& !m_SaveDialog.m_piDialog->IsVisible())
	{
		bool bProcessed=false;
		OnKeyDown(GK_ESCAPE,&bProcessed);
	}
	else if(m_eStage==eInterfaceStage_WaitingForDemoEndCourtain)
	{
		m_piSTBackground->SetText("");
		
		char sFile[200];
		sprintf(sFile,"Level%d.ges",m_PlayerData.m_CurrentGame.nLevel+1);
		
		SHighScoreRow sHighScore;
		if(m_HighScoresTable.m_piHighScoresTable){sHighScore=m_HighScoresTable.m_piHighScoresTable->GetRow(0);}
		m_eStage=eInterfaceStage_Playing;
		m_piGUIManager->ShowMouseCursor(false);
		m_piSTBackground->Show(false);
		m_piGameInterface->SetHighScore(sHighScore.nScore);
		
		m_piGameInterface->LoadScenario(sFile);
		m_piGameInterface->StartGame(m_PlayerData.m_PlayerProfile.m_piPlayerProfile,&m_PlayerData.m_CurrentGame);
	}
	else if(m_eStage==eInterfaceStage_LaunchNextLevel)
	{
		char sFile[200];
		sprintf(sFile,"Level%d.ges",m_PlayerData.m_CurrentGame.nLevel+1);
		m_eStage=eInterfaceStage_Playing;
		m_piGUIManager->ShowMouseCursor(false);
		m_piSTBackground->SetText("");
		m_piSTBackground->Show(false);
		m_piGameInterface->LoadScenario(sFile);
		m_piGameInterface->StartGame(m_PlayerData.m_PlayerProfile.m_piPlayerProfile,&m_PlayerData.m_CurrentGame);
	}
	else if(m_eStage==eInterfaceStage_WaitingForContinuePause)
	{
		if(m_nContinuePauseStartTime+3000<GetTimeStamp())
		{
			m_eStage=eInterfaceStage_Playing;
			m_piGameInterface->Freeze(false);
			m_piSTBackground->Show(false);
			m_piSTBackground->SetText("");
		}
		else if(m_nContinuePauseStartTime+2000<GetTimeStamp())
		{
			m_piSTBackground->SetText("Ready in 1");
		}
		else if(m_nContinuePauseStartTime+1000<GetTimeStamp())
		{
			m_piSTBackground->SetText("Ready in 2");
		}
	}
	CGameWindowBase::OnDraw(piRender);
}

void CMainWindow::OnKeyDown(int nKey,bool *pbProcessed)
{
	if(m_eStage==eInterfaceStage_WaitingForContinuePause)
	{
		m_eStage=eInterfaceStage_Playing;
		m_piGameInterface->Freeze(false);
		m_piSTBackground->Show(false);
		m_piSTBackground->SetText("");
		
		if(nKey==GK_ESCAPE){*pbProcessed=true;return;}
	}
	
	if(nKey==GK_ESCAPE)
	{
		*pbProcessed=true;
		if(m_eStage==eInterfaceStage_MainMenu)
		{
			eMainMenuAction result=m_MainMenuDialog.m_piMainMenu->Show(this,m_PlayerData.m_CurrentGame.nLevel!=-1,m_PlayerData.m_vSavedGames.size()!=0);
			if(result==eMainMenuAction_Exit)
			{
				m_eStage=eInterfaceStage_WaitingForExitCourtain;
				m_piGameInterface->StopManuallyWithCourtain();
			}
			else if(result==eMainMenuAction_Load)
			{
				SGameState sSelectedGame;
				if(m_LoadDialog.m_piLoadDialog->LoadGame(this,&m_PlayerData.m_vSavedGames,&sSelectedGame))
				{
					m_PlayerData.m_CurrentGame=sSelectedGame;
					m_piGameInterface->StopManuallyWithCourtain();
					m_eStage=eInterfaceStage_WaitingForDemoEndCourtainLoading;
				}
			}
			else if(result==eMainMenuAction_Continue)
			{
				m_piGameInterface->StopManuallyWithCourtain();
				m_eStage=eInterfaceStage_WaitingForDemoEndCourtainLoading;
			}	
			else if(result==eMainMenuAction_HighScores)
			{
				m_HighScoresDialog.m_piHighScoresDialog->ShowScores(this,m_HighScoresTable.m_piHighScoresTable,-1,true);
			}			
			else if(result==eMainMenuAction_Credits)
			{
				m_CreditsDialog.m_piDialog->Execute(this);
			}
			else if(result==eMainMenuAction_Options)
			{
				eOptionsMenuAction eOptionsResult=eOptionsMenuAction_Back;
				do
				{
					eOptionsResult=m_OptionsMenuDialog.m_piOptionsMenu->Show(this);
					if(eOptionsResult==eOptionsMenuAction_Controls)
					{
						m_ControlsDialog.m_piControlsDialog->SelectControls(this,m_PlayerData.m_PlayerProfile.m_piPlayerProfile);
					}
					else if(eOptionsResult==eOptionsMenuAction_Audio)
					{
						m_AudioOptionsDialog.m_piAudioOptions->Show(this);
					}
				}
				while(eOptionsResult!=eOptionsMenuAction_Back);
			}
			else if(result==eMainMenuAction_NewGame)
			{
				if(m_LevelOptionsDialog.m_piLevelOptions->SelectOptions(this,m_nAvailableLevels,&m_PlayerData.m_eLastMode,&m_PlayerData.m_eLastDifficulty,&m_PlayerData.m_nLastLevel))
				{
					m_PlayerData.m_CurrentGame=SGameState();
					m_PlayerData.m_CurrentGame.nLivesLeft=3;
					m_PlayerData.m_CurrentGame.nBombs=3;
					m_PlayerData.m_CurrentGame.nWeapon=0;
					m_PlayerData.m_CurrentGame.nLevel=m_PlayerData.m_nLastLevel;
					m_PlayerData.m_CurrentGame.nCheckpoint=-1;
					m_PlayerData.m_CurrentGame.nPoints=0;
					m_PlayerData.m_CurrentGame.eDifficulty=m_PlayerData.m_eLastDifficulty;
					m_PlayerData.m_CurrentGame.eMode=m_PlayerData.m_eLastMode;
					m_piGameInterface->StopManuallyWithCourtain();
					m_eStage=eInterfaceStage_WaitingForDemoEndCourtainLoading;
				}
			}
			m_piGUIManager->SetFocus(this);
		}
		else if(m_eStage==eInterfaceStage_Playing)
		{
			m_piSTBackground->Show(true);
			m_piSTBackground->SetText("");
			m_piGUIManager->ShowMouseCursor(true);
			m_piGameInterface->Freeze(true);
			eGameMenuAction result=eGameMenuAction_Continue;
			
			bool bContinue=false;
			m_eStage=eInterfaceStage_GameMenu;

			do
			{			
				result=m_GameMenuDialog.m_piGameMenu->Show(this);
				
				if(result==eGameMenuAction_EndGame)
				{
					if(ConfirmDialog("End game?","Friking shark",eMessageDialogType_Warning))
					{
						m_eStage=eInterfaceStage_WaitingForManualGameEndCourtain;
						m_piGameInterface->StopManuallyWithCourtain();
						break;
					}
				}
				else if(result==eGameMenuAction_Save)
				{
					if(m_SaveDialog.m_piSaveDialog->SaveGame(this,&m_PlayerData.m_CurrentGame,&m_PlayerData.m_vSavedGames))
					{
						bContinue=true;
						break;
					}
				}
				else if(result==eGameMenuAction_Options)
				{
					eOptionsMenuAction eOptionsResult=eOptionsMenuAction_Back;
					do
					{
						eOptionsResult=m_OptionsMenuDialog.m_piOptionsMenu->Show(this);
						if(eOptionsResult==eOptionsMenuAction_Controls)
						{
							m_ControlsDialog.m_piControlsDialog->SelectControls(this,m_PlayerData.m_PlayerProfile.m_piPlayerProfile);
						}
						else if(eOptionsResult==eOptionsMenuAction_Audio)
						{
							m_AudioOptionsDialog.m_piAudioOptions->Show(this);
						}
					}
					while(eOptionsResult!=eOptionsMenuAction_Back);
				}
				else 
				{
					bContinue=true;
					break;
				}
			}
			while(true);

			if(bContinue)
			{
				if(!m_piGameInterface->IsPaused() && m_piGameInterface->IsPlayerInControl())
				{
					m_eStage=eInterfaceStage_WaitingForContinuePause;
					m_piSTBackground->Show(true);
					m_piSTBackground->SetText("Ready in 3");
					m_nContinuePauseStartTime=GetTimeStamp();
				}
				else
				{
					m_eStage=eInterfaceStage_Playing;
					m_piGameInterface->Freeze(false);
					m_piSTBackground->Show(m_piGameInterface->IsPaused());
					m_piSTBackground->SetText(m_piGameInterface->IsPaused()?"Paused":"");
				}
				m_piGUIManager->ShowMouseCursor(false);
			}
			
			m_piGUIManager->SetFocus(this);
		}
	}
}

void CMainWindow::OnGameOverCourtainClosed()
{
	m_eStage=eInterfaceStage_MainMenu;
	m_piGameInterface->StopGame();
	m_piGameInterface->CloseScenario();
	m_piGameInterface->LoadScenario("./Background.ges");
	m_piGameInterface->StartDemo();
	m_piSTBackground->Show(true);
}

void CMainWindow::OnScenarioFinished(eScenarioFinishedReason eReason)
{
	SGameState sGameState;
	m_piGameInterface->GetGameState(&sGameState);
	m_piGUIManager->ShowMouseCursor(true);
	m_piGameInterface->Freeze(true);
	if(eReason!=eScenarioFinishedReason_Completed)
	{
		m_PlayerData.m_CurrentGame=SGameState();
		m_piSTBackground->Show(true);
		m_GameOverDialog.m_piDialog->Execute(this);
		m_piGameInterface->Freeze(false);
		m_eStage=eInterfaceStage_HighScores;

		SHighScoreRow row;
		row.sName="<type your name>";
		row.eDifficulty=sGameState.eDifficulty;
		row.nScore=sGameState.nPoints;
		
		unsigned int nRow=m_HighScoresTable.m_piHighScoresTable?m_HighScoresTable.m_piHighScoresTable->AddRow(row):-1;
		m_piSTBackground->Show(false);
		m_HighScoresDialog.m_piHighScoresDialog->ShowScores(this,m_HighScoresTable.m_piHighScoresTable,nRow,false);
	}
	else
	{
		m_piGameInterface->Freeze(false);
		m_PlayerData.m_CurrentGame.nPoints=sGameState.nPoints;
		m_PlayerData.m_CurrentGame.nLivesLeft=sGameState.nLivesLeft;
		m_PlayerData.m_CurrentGame.nWeapon=sGameState.nWeapon;
		m_PlayerData.m_CurrentGame.nBombs=3;
		m_PlayerData.m_CurrentGame.nLevel++;
		
		m_PlayerData.m_CurrentGame.nCheckpoint=-1;
		
		if(m_PlayerData.m_CurrentGame.nLevel>=m_nAvailableLevels)
		{
			m_PlayerData.m_CurrentGame.nLevel=m_nAvailableLevels>1?1:0;
			if(m_PlayerData.m_CurrentGame.eDifficulty<eGameDifficulty_VeryHard)
				{m_PlayerData.m_CurrentGame.eDifficulty=(EGameDifficulty)((int)m_PlayerData.m_CurrentGame.eDifficulty+1);}
		}
		m_eStage=eInterfaceStage_LaunchNextLevel;
		m_piSTBackground->Show(true);
		m_piSTBackground->SetText("Loading...");
		m_piGameInterface->StopGame();
		m_piGameInterface->CloseScenario();
	}
}

void CMainWindow::OnWantFocus(bool *pbWant){*pbWant=true;}

void CMainWindow::OnManualStopCourtainClosed()
{
	if(!m_piGameInterface){return;}
	
	if(m_eStage==eInterfaceStage_WaitingForDemoEndCourtainLoading)
	{
		m_piSTBackground->SetText("Loading...");
		m_eStage=eInterfaceStage_WaitingForDemoEndCourtain;
	}
	else if(m_eStage==eInterfaceStage_WaitingForManualGameEndCourtain)
	{
		m_piGameInterface->Freeze(false);
		m_piGameInterface->StopGame();
		m_piGameInterface->CloseScenario();
		m_piGameInterface->LoadScenario("./Background.ges");
		m_piGameInterface->StartDemo();
		m_eStage=eInterfaceStage_MainMenu;
	}
	else if(m_eStage==eInterfaceStage_WaitingForExitCourtain)
	{
		m_eStage=eInterfaceStage_Exit;
		m_piGUIManager->ExitGUILoop();
	}
}

void CMainWindow::OnPaused(bool bPaused)
{
	if(bPaused)
	{
		m_piSTBackground->SetText("Paused");
		m_piSTBackground->Show(true);
	}
	else
	{
		m_piSTBackground->SetText("");
		m_piSTBackground->Show(false);
	}
}


void CMainWindow::OnCheckpoint()
{
	SGameState sGameState;
	m_piGameInterface->GetGameState(&sGameState);
	m_PlayerData.m_CurrentGame.nPoints=sGameState.nPoints;
	m_PlayerData.m_CurrentGame.nLivesLeft=sGameState.nLivesLeft;
	m_PlayerData.m_CurrentGame.nWeapon=sGameState.nWeapon;
	m_PlayerData.m_CurrentGame.nBombs=sGameState.nBombs;
	m_PlayerData.m_CurrentGame.nCheckpoint=sGameState.nCheckpoint;
}


void CMainWindow::OnPlayerKilled()
{
	SGameState sGameState;
	m_piGameInterface->GetGameState(&sGameState);
	m_PlayerData.m_CurrentGame.nPoints=sGameState.nPoints;
	m_PlayerData.m_CurrentGame.nLivesLeft=sGameState.nLivesLeft;
	m_PlayerData.m_CurrentGame.nWeapon=sGameState.nWeapon;
	m_PlayerData.m_CurrentGame.nBombs=sGameState.nBombs;
	m_PlayerData.m_CurrentGame.nCheckpoint=sGameState.nCheckpoint;
}

void CMainWindow::OnDrawMouseCursor(SGamePos position,IGenericRender *piRender,bool *pbDrawed)
{
#ifndef ANDROID
	CGameWindowBase::OnDrawMouseCursor(position,piRender,pbDrawed);
#endif
}
