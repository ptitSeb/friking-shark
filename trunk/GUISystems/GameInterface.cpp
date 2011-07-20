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
	m_eGameDifficulty=eGameDifficulty_Normal;
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

	m_piSTFrameRate=NULL;
	m_piSTGameTime=NULL;
	m_piSTObjectCount=NULL;
	m_piSTEntityCount=NULL;

	m_bGameStarted=false;
	m_nHighScore=0;
	m_nScore=0;	
	m_bCourtainOpen=false;
	m_bCourtainClosed=true;
	m_bCourtainOpening=false;
	m_bCourtainClosing=false;
	m_nCourtainStartTime=0;
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
	StopGame();
	CloseScenario();	
	CGameWindowBase::DestroyWindow();
}

bool CGameInterface::LoadScenario(std::string sScenario)
{
	bool bResult=true;

	CSystemLoaderHelper     systemLoader;
	m_piGameSystem=systemLoader.LoadSystem("Scripts/GameSystem.cfg","GameSystem");

	m_GameControllerWrapper.Attach("GameSystem","GameController");
	m_GameControllerWrapper.m_piGameController->SetupGame();
	m_GameControllerWrapper.m_piGameController->LoadScenario(sScenario);

	m_FrameManagerWrapper.Attach("GameSystem","FrameManager");
	m_PlayerManagerWrapper.Attach("GameSystem","PlayerManager");
	m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager");
	m_EntityManagerWrapper.Attach("GameSystem","EntityManager");
	m_WorldManagerWrapper.Attach("GameSystem","WorldManager");
	m_PlayerProfile.Create("GameSystem","CPlayerProfile","");
	m_bCompleted=false;
	return bResult;
}

void CGameInterface::StartGameInternal(EGameMode eMode,EGameDifficulty eDifficulty,unsigned int nPoints, unsigned int nLivesLeft,unsigned int nWeaponLevel, bool bGoToLastCheckPoint)
{
	if(m_bGameStarted){return;}
	if(m_bGameStarted){return;}
	m_eGameMode=eMode;
	m_eGameDifficulty=eDifficulty;
	m_FrameManagerWrapper.m_piFrameManager->Reset();
	m_bPlayerKilledOnPreviousFrame=false;
	
	
	
	if(m_PlayerProfile.m_piPlayerProfile)
	{
		double dDifficulty=0;
		switch(m_eGameDifficulty)
		{
			case eGameDifficulty_Easy: dDifficulty=0;break;
			case eGameDifficulty_Normal: dDifficulty=2;break;
			case eGameDifficulty_Hard: dDifficulty=4;break;
			case eGameDifficulty_VeryHard: dDifficulty=6;break;
			default: dDifficulty=0;break;
		}
		m_PlayerProfile.m_piPlayerProfile->SetDifficulty(dDifficulty);
	}
	
	CVector vStart,vEnd;
	m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerProfile(m_PlayerProfile.m_piPlayerProfile);
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
	
	if(bGoToLastCheckPoint)
	{
		m_PlayerManagerWrapper.m_piPlayerManager->SetPlayerStart(m_vLastCheckPointPosition);
	}
	else
	{
		CVector vStart,vEnd;
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
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
		m_piPlayer->SetLivesLeft(nLivesLeft);
		m_piPlayer->SetPoints(nPoints);
		m_piPlayer->SetGodMode(m_eGameMode==eGameMode_God);
		IWeapon *piWeapon=m_piPlayerEntity->GetWeapon(0);
		if(piWeapon){piWeapon->SetCurrentLevel(nWeaponLevel);}
	}
	
	m_bGameStarted=true;
	m_eState=eGameInterfaceState_StartCourtain;
	unsigned int nCurrentTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime();
	OpenCourtain(nCurrentTime);
	m_nLastCountTime=nCurrentTime;
}

void CGameInterface::StartGame(EGameMode eMode,EGameDifficulty eDifficulty,unsigned int nPoints, unsigned int nLivesLeft,unsigned int nWeaponLevel)
{
	StartGameInternal(eMode,eDifficulty,nPoints,nLivesLeft,nWeaponLevel,false);
}

void CGameInterface::StopGame()
{
	if(!m_bGameStarted){return;}
	if(m_piPlayerEntity)
	{
		UNSUBSCRIBE_FROM_CAST(m_piPlayerEntity,IEntityEvents);
	}
	m_GameControllerWrapper.m_piGameController->Stop();
	m_piPlayerEntity=NULL;
	m_piPlayer=NULL;
	m_bGameStarted=false;
	m_bPlayerKilledOnPreviousFrame=false;
	m_eState=eGameInterfaceState_Idle;
}

void CGameInterface::ResetGame(bool bGoToLastCheckPoint)
{
	if(bGoToLastCheckPoint)
	{
		if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
		{
			CVector vStart,vEnd;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
			m_vLastCheckPointPosition=vStart;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->EnumeratePlayAreaElements(this);
		}
	}

	IWeapon *piWeapon=m_piPlayerEntity?m_piPlayerEntity->GetWeapon(0):NULL;
	unsigned int nLives=m_piPlayer?m_piPlayer->GetLivesLeft():0;
	unsigned int nPoints=m_piPlayer?m_piPlayer->GetPoints():0;
	unsigned int nWeapon=!bGoToLastCheckPoint && piWeapon?piWeapon->GetCurrentLevel():0;
	
	StopGame();
	StartGameInternal(m_eGameMode,m_eGameDifficulty,nPoints,nLives,nWeapon,bGoToLastCheckPoint);
}

void CGameInterface::ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating)
{
	IPlayAreaCheckPoint *piCheckPoint=QI(IPlayAreaCheckPoint,piElement);
	if(piCheckPoint)
	{
		CVector vStart,vEnd,vCheckPoint;
		CVector vPos=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementPosition();
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
		vCheckPoint=piCheckPoint->GetCheckPointPosition();
		if((m_vLastCheckPointPosition-vStart)<(vCheckPoint-vStart) && 
			(vCheckPoint-vStart)<=(vPos-vStart))
		{
			m_vLastCheckPointPosition=vCheckPoint;
		}
	}
	REL(piCheckPoint);
}

void CGameInterface::CloseScenario()
{
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
	m_bCompleted=false;
}

void CGameInterface::RenderCourtain(IGenericRender *piRender,unsigned int dwCurrentTime)
{
	double dCourtainPosition=0;
	
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
	
void CGameInterface::OnDraw(IGenericRender *piRender)
{
	unsigned int dwCurrentTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime();
	
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
	
	if(!m_piPlayerEntity){return;}
	
	if(m_bPlayerKilledOnPreviousFrame && m_eState!=eGameInterfaceState_GameOverCourtain)
	{
		if(m_piPlayer->GetLivesLeft()==0)
		{
			m_bPlayerKilledOnPreviousFrame=false;
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnScenarioFinished(eScenarioFinishedReason_GameOver,m_piPlayer->GetPoints(),0,0));
			m_eState=eGameInterfaceState_GameOverCourtain;
			dwCurrentTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime();
			CloseCourtain(dwCurrentTime);
		}
		else
		{
			if(m_eGameMode==eGameMode_InfiniteLives)
			{
				m_piPlayer->SetLivesLeft(3);
			}
			if(m_bCourtainClosed)
			{
				m_bPlayerKilledOnPreviousFrame=false;
				
				unsigned long nLivesLeft=m_piPlayer->GetLivesLeft();
				unsigned long nPoints=m_piPlayer->GetPoints();
				
				ResetGame(true);
				m_piPlayer->SetLivesLeft(nLivesLeft);
				m_piPlayer->SetPoints(nPoints);
				return;
			}
			else if(!m_bCourtainClosing)
			{
				CloseCourtain(dwCurrentTime);
			}
		}		
	}	

	m_FrameManagerWrapper.m_piFrameManager->ProcessFrame();
	ProcessInput();
	m_PlayerManagerWrapper.m_piPlayerManager->ProcessInput(m_piGUIManager,m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime(),m_FrameManagerWrapper.m_piFrameManager->GetTimeFraction());

	if(!m_FrameManagerWrapper.m_piFrameManager->IsPaused())
	{
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
		if(m_PlayerManagerWrapper.m_piPlayerManager->IsScenarioCompleted())
		{
			m_eState=eGameInterfaceState_CountintWait;
			m_nEndBombs=0;
			m_nEndPoints=0;
			m_nLastCountTime=dwCurrentTime;
		}
	}
	
	if(m_eState==eGameInterfaceState_CountintWait)
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
				if(piWeapon){piWeapon->SetAmmo(nBombsLeft-1);}
				m_nEndBombs++;
				m_nEndPoints+=3000;
			}
			else
			{
				m_eState=eGameInterfaceState_CountingPoints;
			}
			m_nLastCountTime=dwCurrentTime;
		}
	}
	
	if(m_eState==eGameInterfaceState_CountingPoints)
	{
		if(m_nLastCountTime+10<dwCurrentTime)
		{
			if(m_nEndPoints)
			{
				if(m_piPlayer){m_piPlayer->AddPoints(50);}
				m_nEndPoints-=50;
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
			CloseCourtain(dwCurrentTime);
		}
	}
	
	if(m_eState==eGameInterfaceState_EndCourtain)
	{
		if(!m_bCourtainClosing)
		{
			m_eState=eGameInterfaceState_Idle;
			m_bCompleted=true;
			
			IWeapon *piWeapon=m_piPlayerEntity?m_piPlayerEntity->GetWeapon(0):NULL;
			unsigned int nWeapon=piWeapon?piWeapon->GetCurrentLevel():0;
			
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnScenarioFinished(eScenarioFinishedReason_Completed,m_piPlayer->GetPoints(),m_piPlayer->GetLivesLeft(),nWeapon));
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

	RenderCourtain(piRender,dwCurrentTime);
	
	UpdateGUI(dwCurrentTime);
}

void CGameInterface::ProcessInput()
{
	bool bControlKeyPressed=false;
	if(m_bFrozen)
	{
		return;
	}
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
	// en modo de inspeccion todos los movimientos se calculan en tiempo realm no en el tiempo
	// del sistema de entidades.

	if(nKey==KEY_PAUSE)
	{
		if(m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()>m_dwNextAcceptedPauseKeyTime)
		{
			m_dwNextAcceptedPauseKeyTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()+100;
			m_FrameManagerWrapper.m_piFrameManager->TogglePauseOnNextFrame();
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
		m_bPlayerKilledOnPreviousFrame=true;
	}
}

void CGameInterface::OnKilled(IEntity *piEntity){}

void CGameInterface::RenderEntity(IEntity *piEntity,void *pParam1,void *pParam2)
{
	piEntity->Render((IGenericRender*)pParam1,(IGenericCamera*)pParam2);
}

void CGameInterface::UpdateGUI(unsigned int dwCurrentTime)
{
	if(m_FrameManagerWrapper.m_piFrameManager==NULL){return;}
	if(m_EntityManagerWrapper.m_piEntityManager==NULL){return;}
	
	char sTempText[512]={0};
	
	int nLivesLeft=0;
	int nBombsLeft=0;

	IEntity	*piPlayerEntity=NULL;
	IPlayer *piPlayer=NULL;
	
	m_piSTCentralPanel->Show(m_eState==eGameInterfaceState_CountingBombs || 
							 m_eState==eGameInterfaceState_CountingPoints || 
							 m_eState==eGameInterfaceState_EndWait);
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
	
	if(piPlayer)
	{
		IWeapon *piBombWeapon=piPlayerEntity->GetWeapon(1);
		m_nScore=piPlayer->GetPoints();
		nLivesLeft=piPlayer->GetLivesLeft();
		nBombsLeft=piBombWeapon?piBombWeapon->GetAmmo():0;
	}
	else
	{
		m_nScore=0;
	}
	if(m_piSTPoints)
	{
		char sTemp[200]={0};
		sprintf(sTemp,"%d",m_nScore);
		m_piSTPoints->SetText(sTemp);
	}
	
	if(m_piSTHighScore)
	{
		char sTemp[200]={0};
		sprintf(sTemp,"%d",m_nScore>m_nHighScore?m_nScore:m_nHighScore);
		m_piSTHighScore->SetText(sTemp);
	}

	for(int x=0;x<MAX_LIVES_TO_DISPLAY;x++)
	{
		if(m_piSTLives[x]){m_piSTLives[x]->Show(x<(nLivesLeft-1));}
	}
	for(int x=0;x<MAX_BOMBS_TO_DISPLAY;x++)
	{
		if(m_piSTBombs[x]){m_piSTBombs[x]->Show(x<nBombsLeft);}
	}
}

void CGameInterface::SetHighScore(unsigned int nScore){m_nHighScore=nScore;}
unsigned int CGameInterface::GetScore(){return m_nScore;}

void CGameInterface::OpenCourtain(unsigned int nCurrentTime)
{
	m_bCourtainOpen=false;
	m_bCourtainClosed=false;
	m_bCourtainOpening=true;
	m_bCourtainClosing=false;
	m_nCourtainStartTime=nCurrentTime;
}

void CGameInterface::CloseCourtain(unsigned int nCurrentTime)
{
	m_bCourtainOpen=false;
	m_bCourtainClosed=false;
	m_bCourtainOpening=false;
	m_bCourtainClosing=true;
	m_nCourtainStartTime=nCurrentTime;
}
