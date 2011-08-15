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


#pragma once

#include "GameRunTimeLib.h"
#include "GameGUILib.h"


class IGameInterfaceWindow: virtual public IGameWindow
{	
public:

	virtual void InitializeGameSystem()=0;
	virtual void StartGame(IPlayerProfile *piProfile,SGameState *pState)=0;
	virtual void StartDemo()=0;
	virtual void StopGame()=0;
	virtual void StopManuallyWithCourtain()=0;
	
	virtual void GetGameState(SGameState *pState)=0;
	
	virtual void SetHighScore(unsigned int nScore)=0;
	virtual unsigned int GetScore()=0;
	
	virtual bool LoadScenario(std::string sFileName)=0;
	virtual void CloseScenario()=0;
	
	virtual void	Freeze(bool bFreeze)=0;
	virtual bool	IsFrozen()=0;
	virtual bool	IsPaused()=0;

	virtual ~IGameInterfaceWindow(){}
};

enum EHighScoresColumn
{
	eHighScoresColumn_Ranking,
	eHighScoresColumn_Score,
	eHighScoresColumn_Difficulty,
	eHighScoresColumn_Name,
	eHighScoresColumn_Count
};

struct SHighScoreRow
{
	std::string 	sName;
	EGameDifficulty eDifficulty;
	unsigned int	nScore;
	
	SHighScoreRow(){nScore=0;eDifficulty=eGameDifficulty_Easy;}
	SHighScoreRow(std::string name,EGameDifficulty difficulty, unsigned int score){sName=name;eDifficulty=difficulty;nScore=score;}
};

class IHighScoresTable: virtual public ISystemUnknown
{	
public:
	
	virtual unsigned int	GetRowCount()=0;
	virtual SHighScoreRow	GetRow(unsigned int nIndex)=0;
	virtual int 			AddRow(SHighScoreRow &sRow)=0;
	virtual void			SetRow(unsigned int nIndex,SHighScoreRow &sRow)=0;
	
	virtual ~IHighScoresTable(){}
};

class IHighScoresDialog: virtual public ISystemUnknown
{	
public:
	
	virtual void ShowScores(IGameWindow *piParent,IHighScoresTable *piTable,int nScoreToEdit/*-1 to only show the scores*/,bool bFromMainMenu)=0;
	
	virtual ~IHighScoresDialog(){}
};

class IMainMenu: virtual public ISystemUnknown
{	
public:
	
	virtual eMainMenuAction Show(IGameWindow *piParent,bool bAllowContinue, bool bAllowLoad)=0;
	
	virtual ~IMainMenu(){}
};

class IOptionsMenu: virtual public ISystemUnknown
{	
public:
	
	virtual eOptionsMenuAction Show(IGameWindow *piParent)=0;
	
	virtual ~IOptionsMenu(){}
};

class IGameMenu: virtual public ISystemUnknown
{	
public:
	
	virtual eGameMenuAction Show(IGameWindow *piParent)=0;
	
	virtual ~IGameMenu(){}
};

class IKeyCaptureDialog: virtual public ISystemUnknown
{	
public:
	
	virtual bool CaptureKey(IGameWindow *piParent,unsigned int *pKey)=0;
	
	virtual ~IKeyCaptureDialog(){}
};

class ILevelOptions: virtual public IGameWindow
{	
public:
	
	virtual bool SelectOptions(IGameWindow *piParent,EGameMode *pMode,EGameDifficulty *pDifficulty,unsigned int *pnSelectedLevel)=0;
	
	virtual ~ILevelOptions(){}
};

class IAudioOptions: virtual public IGameWindow
{	
public:
	
	virtual bool Show(IGameWindow *piParent)=0;
	
	virtual ~IAudioOptions(){}
};

class ISavedGameRow: virtual public IGameWindow
{	
public:
	
	virtual void SetSavedGame(SGameState *pGame)=0;
	
	virtual ~ISavedGameRow(){}
};

class ISavedGameRowEvents: virtual public IGameWindow
{	
public:
	
	virtual void OnSavedGameSelected(ISavedGameRow *piControl)=0;
	
	virtual ~ISavedGameRowEvents(){}
};

class ILoadDialog: virtual public IGameWindow
{	
public:
	
	virtual bool LoadGame(IGameWindow *piParent,std::vector<SGameState> *pvSavedGames,SGameState *pSelected)=0;
	
	virtual ~ILoadDialog(){}
};

class ISaveDialog: virtual public IGameWindow
{	
public:
	
	virtual bool SaveGame(IGameWindow *piParent,SGameState *pCurrent,std::vector<SGameState> *pvSavedGames)=0;
	
	virtual ~ISaveDialog(){}
};

struct IPlayerProfile;
class IControlsDialog: virtual public IGameWindow
{	
public:
	
	virtual bool SelectControls(IGameWindow *piParent,IPlayerProfile *piProfile)=0;
	
	virtual ~IControlsDialog(){}
};

enum eScenarioFinishedReason
{
	eScenarioFinishedReason_UNKNOWN,
	eScenarioFinishedReason_Completed,
	eScenarioFinishedReason_GameOver
};

class IGameInterfaceWindowEvents
{	
public:
	virtual void	OnScenarioFinished(eScenarioFinishedReason eReason)=0;
	virtual void	OnPlayerKilled()=0;
	virtual void	OnCheckpoint()=0;
	virtual void	OnGameOverCourtainClosed()=0;
	virtual void	OnManualStopCourtainClosed()=0;
	virtual void	OnPaused(bool bPaused)=0;

	virtual ~IGameInterfaceWindowEvents(){}
};