#pragma once

#include "GameRuntimeLib.h"
#include "GameGUILib.h"

class IGameInterfaceWindow: virtual public IGameWindow
{	
public:

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