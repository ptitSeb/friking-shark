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
#include "GUISystems.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GameInterface.h"
#include "PlayAreaElements.h"

CGameInterface::CGameInterface(void)
{
	m_eState=eGameInterfaceState_Idle;
	m_bActive=false;
	m_eGameMode=eGameMode_Normal;
	m_eGameDifficulty=eGameDifficulty_Easy;
	m_bCompleted=false;
	m_piSystemManager   =NULL;
	m_piGameSystem			=NULL;
	m_piPlayerEntity=NULL;
	m_piPlayer=NULL;
	m_dwNextAcceptedControlKeyTime=0;
	m_dwNextAcceptedPauseKeyTime=0;
	m_bFrozen=false;
	m_bResumeAfterFreeze=false;
	m_bShowPerformanceIndicators=false;
	m_bPlayerKilledOnPreviousFrame=false;
	m_nCheckpoint=-1;
	m_bDemoMode=true;
	
	m_piSTFrameRate=NULL;
	m_piSTGameTime=NULL;
	m_piSTObjectCount=NULL;
	m_piSTEntityCount=NULL;

	m_bGameStarted=false;
	m_nHighScore=0;
	m_bCourtainOpen=false;
	m_bCourtainClosed=true;
	m_bCourtainOpening=false;
	m_bCourtainClosing=false;
	m_nCourtainStartTime=0;
	m_nPoints=0;
	m_nLivesLeft=0;
	m_nWeapon=0;
	m_nBombs=0;
	m_bGameSystemInitialized=false;
	m_piPointCountSound=NULL;
	m_piBombCountSound=NULL;
}

CGameInterface::~CGameInterface(void)
{
}

bool CGameInterface::InitWindow(IGameWindow *piParent,bool bPopup)
{
	bool bResult=CGameWindowBase::InitWindow(piParent,bPopup);

	if(m_piSTGameTime){m_piSTGameTime->Show(m_bShowPerformanceIndicators);}
	if(m_piSTFrameRate){m_piSTFrameRate->Show(m_bShowPerformanceIndicators);}
	if(m_piSTObjectCount){m_piSTObjectCount->Show(m_bShowPerformanceIndicators);}
	if(m_piSTEntityCount){m_piSTEntityCount->Show(m_bShowPerformanceIndicators);}
	return bResult;
}

void CGameInterface::DestroyWindow()
{
	delete m_piPointCountSound;
	delete m_piBombCountSound;
	m_piPointCountSound=NULL;
	m_piBombCountSound=NULL;
	m_PointCountSoundWrapper.Destroy();
	m_BombCountSoundWrapper.Destroy();
	
	StopGame();
	CloseScenario();	
	
	if(m_GameControllerWrapper.m_piGameController)
	{
		m_GameControllerWrapper.m_piGameController->Stop();
		m_GameControllerWrapper.m_piGameController->CloseScenario();
		m_GameControllerWrapper.m_piGameController->EndGame();
	}
	
	m_FrameManagerWrapper.Detach();
	m_EntityManagerWrapper.Detach();
	m_PlayerManagerWrapper.Detach();
	m_PlayAreaManagerWrapper.Detach();
	m_GameControllerWrapper.Detach();
	m_WorldManagerWrapper.Detach();
	m_PlayerProfile.Detach();
	
	if(m_piGameSystem){m_piGameSystem->DestroyAllObjects();}
	if(m_piGameSystem){m_piGameSystem->Destroy();}
	
	REL(m_piGameSystem);
	REL(m_piSystemManager);
	
	CGameWindowBase::DestroyWindow();
}

void CGameInterface::InitializeGameSystem()
{
	if(m_bGameSystemInitialized){return;}
	CSystemLoaderHelper     systemLoader;
	m_piGameSystem=systemLoader.LoadSystem("Scripts/GameSystem.cfg","GameSystem");

	m_GameControllerWrapper.Attach("GameSystem","GameController");
	m_GameControllerWrapper.m_piGameController->SetupGame();

	m_FrameManagerWrapper.Attach("GameSystem","FrameManager");
	m_PlayerManagerWrapper.Attach("GameSystem","PlayerManager");
	m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager");
	m_EntityManagerWrapper.Attach("GameSystem","EntityManager");
	m_WorldManagerWrapper.Attach("GameSystem","WorldManager");
	m_bGameSystemInitialized=true;
	
	if(m_PointCountSoundWrapper.m_piSoundType)
	{
		m_piPointCountSound=m_PointCountSoundWrapper.m_piSoundType->CreateInstance();
	}
	if(m_BombCountSoundWrapper.m_piSoundType)
	{
		m_piBombCountSound=m_BombCountSoundWrapper.m_piSoundType->CreateInstance();
	}
}

bool CGameInterface::LoadScenario(std::string sScenario)
{
	if(!m_bGameSystemInitialized){return false;}
	bool bResult=true;
	m_GameControllerWrapper.m_piGameController->LoadScenario(sScenario);
	
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		m_sCheckpointPositions.clear();
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->EnumeratePlayAreaElements(this);
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&m_vDemoStartPosition,&m_vDemoEndPosition);
		
		set<double>::iterator i=m_sCheckpointPositions.begin();
		if(i!=m_sCheckpointPositions.end()){m_vDemoStartPosition.c[0]=*i;i++;}
		if(i!=m_sCheckpointPositions.end()){m_vDemoEndPosition.c[0]=*i;i++;}
	}
		
	m_bCompleted=false;
	return bResult;
}

void CGameInterface::StartGameInternal(unsigned int nPoints, unsigned int nLivesLeft,unsigned int nWeaponLevel, unsigned int nBombs, int nCheckpoint)
{
	if(!m_bGameSystemInitialized){return;}
	if(m_bGameStarted){StopGame();}
	m_nPoints=nPoints;
	m_nLivesLeft=nLivesLeft;
	m_nWeapon=nWeaponLevel;
	m_nBombs=nBombs;
	m_nCheckpoint=nCheckpoint;
	m_FrameManagerWrapper.m_piFrameManager->Reset();
	if(m_FrameManagerWrapper.m_piFrameManager->IsPaused())
	{
		m_FrameManagerWrapper.m_piFrameManager->TogglePauseOnNextFrame();
	}
	m_bPlayerKilledOnPreviousFrame=false;
	
	CVector vStart,vEnd;
	m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerProfile(m_PlayerProfile.m_piPlayerProfile);
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
	
	if(m_bDemoMode)
	{
		m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerStart(m_vDemoStartPosition);
	}
	else
	{
		CVector vStart,vEnd;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
		if(m_nCheckpoint!=-1)
		{
			set<double>::iterator i=m_sCheckpointPositions.begin();
			for(int x=0;x<=m_nCheckpoint;x++,i++){if(x==m_nCheckpoint){vStart.c[0]=*i;break;}}
		}
		m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerStart(vStart);
	}
	m_GameControllerWrapper.m_piGameController->Start();
	m_piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
	if(m_piPlayerEntity)
	{
		m_piPlayer=dynamic_cast<IPlayer*>(m_piPlayerEntity);
		
		SUBSCRIBE_TO_CAST(m_piPlayerEntity,IEntityEvents);
	}
	if(m_piPlayer && m_piPlayerEntity)
	{
		m_piPlayer->SetLivesLeft(m_nLivesLeft);
		m_piPlayer->SetPoints(m_nPoints);
		m_piPlayer->SetGodMode(m_eGameMode==eGameMode_God);
		IWeapon *piWeapon=m_piPlayerEntity->GetWeapon(0);
		if(piWeapon){piWeapon->SetCurrentLevel(m_nWeapon);}
		IWeapon *piBombs=m_piPlayerEntity->GetWeapon(1);
		if(piBombs){piBombs->SetAmmo(m_nBombs);}
	}
	
	m_bGameStarted=true;
	m_eState=eGameInterfaceState_StartCourtain;
	OpenCourtain();
	m_nLastCountTime=m_FrameManagerWrapper.m_piFrameManager?m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime():0;
	m_nLastCountSoundTime=0;

	UpdateGUI(0);
}

void CGameInterface::StartDemo()
{
	if(!m_bGameSystemInitialized){return;}
	m_bDemoMode=true;
	m_eGameMode=eGameMode_God;
	m_eGameDifficulty=eGameDifficulty_Easy;
	m_nPoints=0;
	m_nLivesLeft=0;
	m_nWeapon=0;
	m_nCheckpoint=-1;
	m_PlayerProfile.Detach();
	StartGameInternal(0,3,0,0,-1);
}

void CGameInterface::StartGame(IPlayerProfile *piProfile,SGameState *pGame)
{
	if(!m_bGameSystemInitialized){return;}
	m_bDemoMode=false;
	m_eGameMode=pGame->eMode;
	m_eGameDifficulty=pGame->eDifficulty;
	
	m_PlayerProfile.Attach(piProfile);
	if(m_PlayerProfile.m_piPlayerProfile)
	{
		double dDifficulty=0;
		switch(pGame->eDifficulty)
		{
			 case eGameDifficulty_Easy: dDifficulty=0;break;
			 case eGameDifficulty_Normal: dDifficulty=2;break;
			 case eGameDifficulty_Hard: dDifficulty=4;break;
			 case eGameDifficulty_VeryHard: dDifficulty=6;break;
			 default: dDifficulty=0;break;
		}
		m_PlayerProfile.m_piPlayerProfile->SetDifficulty(dDifficulty);
	}
	StartGameInternal(pGame->nPoints,pGame->nLivesLeft,pGame->nWeapon,pGame->nBombs,pGame->nCheckpoint);
}

void CGameInterface::StopGame()
{
	if(!m_bGameSystemInitialized){return;}
	if(!m_bGameStarted){return;}
	if(m_piPlayerEntity)
	{
		UNSUBSCRIBE_FROM_CAST(m_piPlayerEntity,IEntityEvents);
		m_piPlayerEntity=NULL;
		m_piPlayer=NULL;
	}
	m_GameControllerWrapper.m_piGameController->Stop();
	m_piPlayer=NULL;
	m_bGameStarted=false;
	m_bPlayerKilledOnPreviousFrame=false;
	m_eState=eGameInterfaceState_Idle;
}

void CGameInterface::StopManuallyWithCourtain()
{
	m_eState=eGameInterfaceState_StopManuallyWithCourtain;
	CloseCourtain();
}

void CGameInterface::ResetGame()
{
	if(!m_bGameSystemInitialized){return;}

	StopGame();
	StartGameInternal(m_nPoints,m_nLivesLeft,m_nWeapon,3,m_nCheckpoint);
}

void CGameInterface::ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating)
{
	IPlayAreaCheckPoint *piCheckPoint=QI(IPlayAreaCheckPoint,piElement);
	if(piCheckPoint){m_sCheckpointPositions.insert(piCheckPoint->GetCheckPointPosition().c[0]);}
	REL(piCheckPoint);
}

void CGameInterface::CloseScenario()
{
	if(!m_bGameSystemInitialized){return;}
	if(m_GameControllerWrapper.m_piGameController)
	{
		m_GameControllerWrapper.m_piGameController->Stop();
		m_GameControllerWrapper.m_piGameController->CloseScenario();
	}
	m_bCompleted=false;
}

void CGameInterface::RenderCourtain(IGenericRender *piRender)
{
	double dCourtainPosition=0;
	unsigned int dwCurrentTime=GetTimeStamp();
	
	if(m_bCourtainOpen){return;}
	if(m_bCourtainClosing)
	{
		double dCourtainProgress=((double)(dwCurrentTime-m_nCourtainStartTime))/350.0;
		if(dCourtainProgress>1.0){dCourtainProgress=1.0;m_bCourtainClosing=false;m_bCourtainClosed=true;}
		dCourtainPosition=dCourtainProgress;
	}
	else if(m_bCourtainOpening)
	{
		double dCourtainProgress=((double)(dwCurrentTime-m_nCourtainStartTime))/350.0;
		if(dCourtainProgress>1.0){dCourtainProgress=1.0;m_bCourtainOpening=false;m_bCourtainOpen=true;}
		dCourtainPosition=1.0-dCourtainProgress;
	}

	piRender->PushState();
	piRender->SetOrthographicProjection(m_rRealRect.w,m_rRealRect.h);
	piRender->SetViewport(m_rRealRect.x,m_rRealRect.y,m_rRealRect.w,m_rRealRect.h);
	piRender->SetCamera(CVector(m_rRealRect.w*0.5,m_rRealRect.h*0.5,200),90,0,0);

	piRender->DeactivateDepth();
	piRender->ActivateSolid();
	piRender->SetColor(CVector(0,0,0),0);
	
	if(m_bCourtainClosed)
	{
		piRender->RenderRect(0,0,m_rRealRect.w,m_rRealRect.h);
	}
	else
	{
		double cx=m_rRealRect.w*0.5*dCourtainPosition;
		double cy=m_rRealRect.h*0.5*dCourtainPosition;
		piRender->RenderRect(0,0,cx,cy);
		piRender->RenderRect(m_rRealRect.w-cx,0,cx,cy);
		piRender->RenderRect(0,m_rRealRect.h-cy,cx,cy);
		piRender->RenderRect(m_rRealRect.w-cx,m_rRealRect.h-cy,cx,cy);
	}
	piRender->PopState();
}

bool CGameInterface::IsPaused()
{
	if(m_bFrozen){return !m_bResumeAfterFreeze;}
	else {return m_FrameManagerWrapper.m_piFrameManager?m_FrameManagerWrapper.m_piFrameManager->IsPaused():false;}
}

bool CGameInterface::IsPlayerInControl()
{
	if(m_PlayerManagerWrapper.m_piPlayerManager==NULL){return false;}
	return m_PlayerManagerWrapper.m_piPlayerManager->GetStage()==ePlayerManagerGameStage_Moving;
}

void CGameInterface::OnDraw(IGenericRender *piRender)
{
	if(!m_bGameSystemInitialized)
	{
		UpdateGUI(0);
		return;
	}
	unsigned int dwCurrentTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime();

	if(!m_bDemoMode)
	{
		// Update player data.
		
		if(m_piPlayerEntity)
		{
			IWeapon *piWeapon=m_piPlayerEntity->GetWeapon(0);
			m_nWeapon=piWeapon?piWeapon->GetCurrentLevel():0;
			
			IWeapon *piBombWeapon=m_piPlayerEntity->GetWeapon(1);
			m_nBombs=piBombWeapon?piBombWeapon->GetAmmo():0;
			
			m_nPoints=m_piPlayer->GetPoints();
			m_nLivesLeft=m_piPlayer->GetLivesLeft();
		}
		
		// Update current checkpoint.
		
		CVector vCameraPos=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementPosition();
		
		int nCurrentCheckpoint=-1;
		for(set<double>::iterator i=m_sCheckpointPositions.begin();i!=m_sCheckpointPositions.end();i++)
		{
			if(*i<vCameraPos.c[0])
			{
				nCurrentCheckpoint++;
			}
			else
			{
				break;
			}
		}
		if(nCurrentCheckpoint>m_nCheckpoint)
		{
			m_nCheckpoint=nCurrentCheckpoint;
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnCheckpoint());
		}
	}
	
	
	if(m_eState==eGameInterfaceState_GameOverCourtain)
	{
		if(!m_bCourtainClosing)
		{
			m_eState=eGameInterfaceState_Idle;
			m_bCompleted=true;
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnGameOverCourtainClosed());
			return;
		}
	}

	if(m_eState==eGameInterfaceState_KilledCourtain)
	{
		if(!m_bCourtainClosing && !m_bFrozen)
		{
			ResetGame();
			return;
		}
	}

	if(m_eState==eGameInterfaceState_StopManuallyWithCourtain)
	{
		if(!m_bCourtainClosing)
		{
			m_eState=eGameInterfaceState_Idle;
			m_bCompleted=true;
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnManualStopCourtainClosed());
			return;
		}
	}
	
	if(m_bPlayerKilledOnPreviousFrame)
	{
		m_bPlayerKilledOnPreviousFrame=false;
		if(m_nLivesLeft==0)
		{
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnScenarioFinished(eScenarioFinishedReason_GameOver));
			m_eState=eGameInterfaceState_GameOverCourtain;
		}
		else
		{
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnPlayerKilled());
			m_eState=eGameInterfaceState_KilledCourtain;
		}		
		CloseCourtain();
	}	

	m_FrameManagerWrapper.m_piFrameManager->ProcessFrame();
	ProcessInput();
	
	if(!m_FrameManagerWrapper.m_piFrameManager->IsPaused())
	{
		m_PlayerManagerWrapper.m_piPlayerManager->ProcessInput(m_piGUIManager,m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime(),m_FrameManagerWrapper.m_piFrameManager->GetTimeFraction());
		m_GameControllerWrapper.m_piGameController->ProcessFrame(m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime(),m_FrameManagerWrapper.m_piFrameManager->GetTimeFraction());
	}
	
	if(m_eState==eGameInterfaceState_StartCourtain)
	{
		if(!m_bCourtainOpening)
		{
			m_eState=eGameInterfaceState_Playing;
			m_nLastCountTime=dwCurrentTime;
		}
	}
	
	if(m_eState==eGameInterfaceState_Playing && m_PlayerManagerWrapper.m_piPlayerManager)
	{
		if(!m_bDemoMode)
		{
			if(m_PlayerManagerWrapper.m_piPlayerManager->IsScenarioCompleted())
			{
				m_eState=eGameInterfaceState_CountingWait;
				m_nEndBombs=0;
				m_nEndPoints=0;
				m_nLastCountTime=dwCurrentTime;
			}
		}
		else
		{
			CVector vPos=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementPosition();
			if(vPos.c[0]>m_vDemoEndPosition.c[0])
			{
				m_PlayAreaManagerWrapper.m_piPlayAreaManager->SetPlayMovementPosition(m_vDemoStartPosition);
			}
		}
	}
	
	if(m_eState==eGameInterfaceState_CountingWait)
	{
		if(m_nLastCountTime+1000<dwCurrentTime)
		{
			m_eState=eGameInterfaceState_CountingBombs;
			m_nLastCountTime=dwCurrentTime;
		}
	}
	
	if(m_eState==eGameInterfaceState_CountingBombs)
	{
		if(m_nLastCountTime+500<dwCurrentTime)
		{
			IWeapon *piWeapon=m_piPlayerEntity?m_piPlayerEntity->GetWeapon(1):NULL;
			unsigned int nBombsLeft=piWeapon?piWeapon->GetAmmo():0;
			if(nBombsLeft)
			{
				if(m_piBombCountSound){m_piBombCountSound->Stop();m_piBombCountSound->Play();}
				if(piWeapon){piWeapon->SetAmmo(nBombsLeft-1);}
				m_nEndBombs++;
				m_nEndPoints+=3000;
			}
			else
			{
				m_eState=eGameInterfaceState_CountingPoints;
				m_nLastCountSoundTime=0;
			}
			m_nLastCountTime=dwCurrentTime;
		}
	}
	
	if(m_eState==eGameInterfaceState_CountingPoints)
	{
		if(m_nLastCountSoundTime+20<dwCurrentTime)
		{
			if(m_nEndPoints)
			{
				if(m_piPointCountSound){m_piPointCountSound->Stop();m_piPointCountSound->Play();}
			}
			m_nLastCountSoundTime=dwCurrentTime;
		}
		if(m_nLastCountTime+10<dwCurrentTime)
		{
			if(m_nEndPoints)
			{
				int nPoints=m_nEndPoints<50?m_nEndPoints:50;
				if(m_piPointCountSound){m_piPointCountSound->Stop();m_piPointCountSound->Play();}
				if(m_piPlayer){m_piPlayer->AddPoints(nPoints);}
				m_nEndPoints-=nPoints;
			}
			else
			{
				m_eState=eGameInterfaceState_EndWait;
			}
			m_nLastCountTime=dwCurrentTime;
		}
	}
	
	if(m_eState==eGameInterfaceState_EndWait)
	{
		if(m_nLastCountTime+2000<dwCurrentTime)
		{
			m_eState=eGameInterfaceState_EndCourtain;
			m_nLastCountTime=dwCurrentTime;
			CloseCourtain();
		}
	}
	
	if(m_eState==eGameInterfaceState_EndCourtain)
	{
		if(!m_bCourtainClosing)
		{
			m_eState=eGameInterfaceState_Idle;
			m_bCompleted=true;
			
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnScenarioFinished(eScenarioFinishedReason_Completed));
			m_nLastCountTime=dwCurrentTime;
			return;
		}
	}
	
	if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		// Actual rendering:
		IGenericCamera *piCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera();
		if(piCamera)
		{
			SGameRect sParentRect;
			SGameRect sRect;
			
			m_piParent->GetRealRect(&sParentRect);
			double dPlayAreaAspectRatio=piCamera->GetAspectRatio();
			sRect.y=0;
			sRect.w=sParentRect.h*dPlayAreaAspectRatio;
			sRect.x=(sParentRect.w-sRect.w)*0.5;
			sRect.h=sParentRect.h;
			
			SetReferenceSystem(eGameGUIReferenceSystem_Absolute);
			SetRect(&sRect);
			

			double dNearPlane=0,dFarPlane=0;
			double dViewAngle=piCamera->GetViewAngle();
			CVector vAngles,vPosition;

			piCamera->GetClippingPlanes(dNearPlane,dFarPlane);
			vAngles=piCamera->GetAngles();
			vPosition=piCamera->GetPosition();

			piRender->SetViewport(sRect.x,sRect.y,sRect.w,sRect.h);
			piRender->SetPerspectiveProjection(dViewAngle,dNearPlane,dFarPlane);
			piRender->SetCamera(vPosition,vAngles.c[YAW],vAngles.c[PITCH],vAngles.c[ROLL]);

			piRender->PushOptions();
			piRender->PushState();
			piRender->ActivateDepth();
			piRender->EnableTextures();
			piRender->EnableSolid();
			piRender->EnableBlending();
			piRender->EnableLighting();	
			piRender->EnableShadows();
			piRender->EnableShaders();
			piRender->EnableHeightFog();
			piRender->DisableAutoShadowVolume();
			
			piRender->StartStagedRendering();
			m_WorldManagerWrapper.m_piWorldManager->SetupRenderingEnvironment(piRender);
			m_EntityManagerWrapper.m_piEntityManager->RenderEntities(piRender,piCamera);
	//unsigned int nRenderStart=GetTimeStamp();
			piRender->EndStagedRendering();
	//unsigned int nRenderEnd=GetTimeStamp();

			//RTTRACE("FrameTime %dms, render %d",nRenderEnd-nRenderStart,nRenderStart-nFrameStart);
			piRender->DeactivateDepth();
			piRender->PopState();
			piRender->PopOptions();	
		}
		REL(piCamera);
	}

	RenderCourtain(piRender);
	
	UpdateGUI(dwCurrentTime);
}

void CGameInterface::ProcessInput()
{
	if(!m_bGameSystemInitialized){return;}
	bool bControlKeyPressed=false;
	if(m_bFrozen){return;}
	if(m_bDemoMode){return;}
	if(m_bCourtainOpening || m_bCourtainClosing){return;}
	if(m_nLivesLeft==0){return;}
	if(m_dwNextAcceptedControlKeyTime<m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime())
	{
		if(m_piGUIManager->IsKeyDown(GK_F4))
		{
			bControlKeyPressed=true;
			m_bShowPerformanceIndicators=!m_bShowPerformanceIndicators;
			if(m_piSTGameTime){m_piSTGameTime->Show(m_bShowPerformanceIndicators);}
			if(m_piSTFrameRate){m_piSTFrameRate->Show(m_bShowPerformanceIndicators);}
			if(m_piSTObjectCount){m_piSTObjectCount->Show(m_bShowPerformanceIndicators);}
			if(m_piSTEntityCount){m_piSTEntityCount->Show(m_bShowPerformanceIndicators);}
		}
		if(bControlKeyPressed)
		{
			m_dwNextAcceptedControlKeyTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()+300;
		}
	}
	if(m_piGUIManager->IsKeyDown(GK_PAUSE)){ProcessKey(KEY_PAUSE);}
}

void CGameInterface::ProcessKey(unsigned short nKey)
{
	if(nKey==KEY_PAUSE)
	{
		if(m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()>m_dwNextAcceptedPauseKeyTime)
		{
			bool bWasPaused=m_FrameManagerWrapper.m_piFrameManager->IsPaused();
			m_dwNextAcceptedPauseKeyTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()+200;
			m_FrameManagerWrapper.m_piFrameManager->TogglePauseOnNextFrame();

			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnPaused(!bWasPaused));
		}
	}
}

void	CGameInterface::Freeze(bool bFreeze)
{
	if(m_bFrozen==bFreeze){return;}

	if(bFreeze)
	{
		m_bResumeAfterFreeze=!m_FrameManagerWrapper.m_piFrameManager->IsPaused();
		m_FrameManagerWrapper.m_piFrameManager->SetPauseOnNextFrame(true);
	}
	else
	{
		if(m_bResumeAfterFreeze){m_FrameManagerWrapper.m_piFrameManager->SetPauseOnNextFrame(false);}
	}
	m_bFrozen=bFreeze;
}

bool	CGameInterface::IsFrozen()
{
	return m_bFrozen;
}
void CGameInterface::OnRemoved(IEntity *piEntity)
{
	if(piEntity==m_piPlayerEntity)
	{
		m_nPoints=m_piPlayer->GetPoints();
		m_nLivesLeft=m_piPlayer->GetLivesLeft();
		m_nWeapon=0;

		if(m_eGameMode!=eGameMode_Normal)
		{
			m_nLivesLeft=3;
		}

		m_bPlayerKilledOnPreviousFrame=true;
		UNSUBSCRIBE_FROM_CAST(m_piPlayerEntity,IEntityEvents);
		m_piPlayerEntity=NULL;
		m_piPlayer=NULL;
	}
}

void CGameInterface::OnKilled(IEntity *piEntity){}

void CGameInterface::RenderEntity(IEntity *piEntity,void *pParam1,void *pParam2)
{
	piEntity->Render((IGenericRender*)pParam1,(IGenericCamera*)pParam2);
}

void CGameInterface::UpdateGUI(unsigned int dwCurrentTime)
{
	
	char sTempText[512]={0};

	IEntity	*piPlayerEntity=NULL;
	IPlayer *piPlayer=NULL;

	bool bShowInterface=(!m_bDemoMode && (m_piPlayerEntity || m_eState==eGameInterfaceState_KilledCourtain));
	

	if(m_piSTUpperIndicatorRow0){m_piSTUpperIndicatorRow0->Show(bShowInterface);}
	if(m_piSTUpperIndicatorRow1){m_piSTUpperIndicatorRow1->Show(bShowInterface);}
	if(m_piSTUpperIndicatorRow2){m_piSTUpperIndicatorRow2->Show(bShowInterface);}

	m_piSTCentralPanel->Show(bShowInterface &&
							 (m_eState==eGameInterfaceState_CountingBombs || 
							  m_eState==eGameInterfaceState_CountingPoints || 
							  m_eState==eGameInterfaceState_EndWait));
	if(m_piSTCentralPanel)
	{
		if(m_piSTEndPoints)
		{
			char sTemp[200]={0};
			sprintf(sTemp,"%d",m_nEndPoints);
			m_piSTEndPoints->SetText(sTemp);
		}
		for(unsigned int x=0;x<MAX_BOMBS_TO_DISPLAY;x++)
		{
			if(m_piSTEndBombs[x]){m_piSTEndBombs[x]->Show(x<m_nEndBombs);}
		}
	}
	if(m_piSTDifficulty)
	{
		switch(m_eGameDifficulty)
		{
			case eGameDifficulty_Easy: m_piSTDifficulty->SetText("Easy");break;
			case eGameDifficulty_Normal: m_piSTDifficulty->SetText("Normal");break;
			case eGameDifficulty_Hard: m_piSTDifficulty->SetText("Hard");break;
			case eGameDifficulty_VeryHard: m_piSTDifficulty->SetText("Very Hard");break;
			default: m_piSTDifficulty->SetText("Easy");break;
		}
	}
	if(m_piSTMode)
	{
		switch(m_eGameMode)
		{
			case eGameMode_God: m_piSTMode->SetText("God");break;
			case eGameMode_InfiniteLives: m_piSTMode->SetText("Infinite Lives");break;
			case eGameMode_Normal: m_piSTMode->SetText("Classic");break;
			default: m_piSTMode->SetText("Classic");break;
		}
	}
	if(m_EntityManagerWrapper.m_piEntityManager!=NULL)
	{
		piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
		if(piPlayerEntity){piPlayer=dynamic_cast<IPlayer*>(piPlayerEntity);}
	}
	
	if(m_EntityManagerWrapper.m_piEntityManager)
	{
		sprintf(sTempText,"%u entities",(unsigned int)m_EntityManagerWrapper.m_piEntityManager->GetEntityCount());
		if(m_piSTEntityCount){m_piSTEntityCount->SetText(sTempText);}
	}
	if(m_EntityManagerWrapper.m_piEntityManager)
	{
		ISystemManager *piSystemManager=GetSystemManager();
		sprintf(sTempText,"%u objects",(unsigned int)piSystemManager->DebugGetRegisteredObjectCount());
		if(m_piSTObjectCount){m_piSTObjectCount->SetText(sTempText);}
		REL(piSystemManager);
	}
	if(m_FrameManagerWrapper.m_piFrameManager)
	{
		sprintf(sTempText,"%f fps",m_FrameManagerWrapper.m_piFrameManager->GetCurrentFps());
		if(m_piSTFrameRate){m_piSTFrameRate->SetText(sTempText);}
		
		if(m_piSTGameTime)
		{
			unsigned int dwMinutes=0,dwSeconds=0,dwMilliseconds=0;
			dwMinutes=dwCurrentTime/(1000*60);
			dwSeconds=dwCurrentTime/1000-dwMinutes*60;
			dwMilliseconds=dwCurrentTime-(dwSeconds*1000+dwMinutes*60*1000);
			sprintf(sTempText,"%02d:%02d.%03d %s",dwMinutes,dwSeconds,dwMilliseconds,m_FrameManagerWrapper.m_piFrameManager->IsPaused()?"  <PAUSED>":"");
			m_piSTGameTime->SetText(sTempText);
		}
	}
	
	if(m_piSTPoints)
	{
		char sTemp[200]={0};
		sprintf(sTemp,"%d",m_nPoints);
		m_piSTPoints->SetText(sTemp);
	}
	
	if(m_piSTHighScore)
	{
		char sTemp[200]={0};
		sprintf(sTemp,"%d",m_nPoints>m_nHighScore?m_nPoints:m_nHighScore);
		m_piSTHighScore->SetText(sTemp);
	}

	for(int x=0;x<MAX_LIVES_TO_DISPLAY;x++)
	{
		if(m_piSTLives[x]){m_piSTLives[x]->Show(bShowInterface && x<(((int)m_nLivesLeft)-1));}
	}
	for(int x=0;x<MAX_BOMBS_TO_DISPLAY;x++)
	{
		if(m_piSTBombs[x]){m_piSTBombs[x]->Show(bShowInterface  && x<(int)m_nBombs);}
	}
}

void CGameInterface::SetHighScore(unsigned int nScore){m_nHighScore=nScore;}
unsigned int CGameInterface::GetScore(){return m_nPoints;}

void CGameInterface::OpenCourtain()
{
	if(m_bCourtainOpen){return;}
	m_bCourtainOpen=false;
	m_bCourtainClosed=false;
	m_bCourtainOpening=true;
	m_bCourtainClosing=false;
	m_nCourtainStartTime=GetTimeStamp();
}

void CGameInterface::CloseCourtain()
{
	if(m_bCourtainClosed){return;}
	m_bCourtainOpen=false;
	m_bCourtainClosed=false;
	m_bCourtainOpening=false;
	m_bCourtainClosing=true;
	m_nCourtainStartTime=GetTimeStamp();
}

void CGameInterface::GetGameState(SGameState *pState)
{
	pState->nPoints=m_nPoints;
	pState->nWeapon=m_nWeapon;
	pState->nBombs=m_nBombs;
	pState->nLivesLeft=m_nLivesLeft;
	pState->eDifficulty=m_eGameDifficulty;
	pState->eMode=m_eGameMode;
	pState->nCheckpoint=m_nCheckpoint;
}
