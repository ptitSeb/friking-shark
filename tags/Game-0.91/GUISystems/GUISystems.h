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

enum EGameMode 
{
	eGameMode_Normal=0,
	eGameMode_God,
	eGameMode_InfiniteLives,
	eGameMode_Count
};

class IGameInterfaceWindow: virtual public IGameWindow
{	
public:

	virtual void StartGame(EGameMode eMode,unsigned int nPoints, unsigned int nLivesLeft,unsigned int nWeaponLevel)=0;
	virtual void StopGame()=0;
	
	virtual void SetHighScore(unsigned int nScore)=0;
	virtual unsigned int GetScore()=0;
	
	virtual bool LoadScenario(std::string sFileName)=0;
	virtual void CloseScenario()=0;
	
	virtual void	Freeze(bool bFreeze)=0;
	virtual bool	IsFrozen()=0;

	virtual ~IGameInterfaceWindow(){}
};

class ILevelOptions: virtual public IGameWindow
{	
public:
	
	virtual void SelectOptions(IGameWindow *piParent,EGameMode *pMode,unsigned int *pnSelectedLevel)=0;
	
	virtual ~ILevelOptions(){}
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

	virtual void	OnScenarioFinished(eScenarioFinishedReason eReason,unsigned int nPoints, unsigned int nLivesLeft,unsigned int nWeaponLevel)=0;

	virtual ~IGameInterfaceWindowEvents(){}
};