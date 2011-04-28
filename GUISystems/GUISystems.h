#pragma once

#include "GameRunTimeLib.h"
#include "GameGUILib.h"

class IGameInterfaceWindow: virtual public IGameWindow
{	
public:

	virtual void StartGame()=0;
	virtual void StopGame()=0;
	
	virtual bool LoadScenario(std::string sFileName)=0;
	virtual void CloseScenario()=0;
	
	virtual void	Freeze(bool bFreeze)=0;
	virtual bool	IsFrozen()=0;

	virtual ~IGameInterfaceWindow(){}
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

	virtual ~IGameInterfaceWindowEvents(){}
};