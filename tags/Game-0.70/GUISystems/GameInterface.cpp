#include "./stdafx.h"
#include "GUISystems.h"
#include "GameRunTimeLib.h"
#include "GameGUILib.h"
#include "GameInterface.h"
#include "PlayAreaElements.h"

#define MOVEMENT_TYPE_UNKNOWN	0
#define MOVEMENT_TYPE_INSPECT	1
#define MOVEMENT_TYPE_PLAY		2

CGameInterface::CGameInterface(void)
{
	m_bCompleted=false;
	m_piSystemManager   =NULL;
	m_piGameSystem			=NULL;
	m_piPlayerEntity=NULL;
	m_piPlayer=NULL;
	m_dwNextAcceptedControlKeyTime=0;
	m_dwNextAcceptedPauseKeyTime=0;
	m_dwMovementType=MOVEMENT_TYPE_PLAY;
	m_dMovementInspectionSpeed=300;
	m_bFrozen=false;
	m_bResumeAfterFreeze=false;
	m_bShowPerformanceIndicators=false;
	m_bPlayerKilledOnPreviousFrame=false;

	m_piSTFrameRate=NULL;
	m_piSTGameTime=NULL;
	m_piSTObjectCount=NULL;
	m_piSTEntityCount=NULL;
	m_piGamePlayWindow=NULL;

	m_bGameStarted=false;
	m_bPauseOnNextFrame=false;
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

	LoadScenario("/home/javi/workspace/Game/Demo/Resources/new2.ges");
	StartGame();
	return bResult;
}

void CGameInterface::DestroyWindow()
{
	CGameWindowBase::DestroyWindow();

	StopGame();
	CloseScenario();
}

bool CGameInterface::LoadScenario(std::string sScenario)
{
	bool bResult=true;

	CSystemLoaderHelper     systemLoader;
	m_piGameSystem=systemLoader.LoadSystem("Scripts/GameSystem.cfg","GameSystem");

	m_GameControllerWrapper.Attach("GameSystem","GameController");
	m_GameControllerWrapper.m_piGameController->SetupGame();
	m_GameControllerWrapper.m_piGameController->LoadScenario(sScenario);

	m_RenderWrapper.Attach("GameSystem","GameRender");
	m_FrameManagerWrapper.Attach("GameSystem","FrameManager");
	m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager");
	m_EntityManagerWrapper.Attach("GameSystem","EntityManager");
	m_PlayCamera.Attach("GameSystem","PlayCamera");

	m_InspectionCamera.Create("GameSystem","Camera","InspectionCamera");
	m_InspectionCamera.m_piCamera->SetAngles(m_PlayCamera.m_piCamera->GetAngles());
	m_InspectionCamera.m_piCamera->SetPosition(m_PlayCamera.m_piCamera->GetPosition());
	m_InspectionCamera.m_piCamera->SetViewAngle(m_PlayCamera.m_piCamera->GetViewAngle());

	m_bCompleted=false;
	return bResult;
}

void CGameInterface::StartGame()
{
	if(m_bGameStarted){return;}
	m_FrameManagerWrapper.m_piFrameManager->Reset();
	m_bPlayerKilledOnPreviousFrame=false;

	m_GameControllerWrapper.m_piGameController->Start();
	m_piPlayerEntity=m_EntityManagerWrapper.m_piEntityManager->FindEntity("Player");
	if(m_piPlayerEntity)
	{
		m_piPlayer=dynamic_cast<IPlayer*>(m_piPlayerEntity);

		SUBSCRIBE_TO_CAST(m_piPlayerEntity,IEntityEvents);
	}
	if(m_piPlayer)
	{
		m_piPlayer->SetLivesLeft(3);
	}
	m_bGameStarted=true;
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
}

void CGameInterface::ResetGame(bool bGoToLastCheckPoint)
{
	if(bGoToLastCheckPoint)
	{
		if(m_PlayAreaManagerWrapper.m_piPlayAreaManager)
		{
			CVector vStart,vEnd;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayerRoute(&vStart,&vEnd);
			m_vLastCheckPointPosition=vStart;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->EnumeratePlayAreaElements(this);
		}
	}

	StopGame();
	StartGame();
	if(bGoToLastCheckPoint)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->SetPlayMovementPosition(m_vLastCheckPointPosition);
	}
}

void CGameInterface::ProcessEnumeratedPlayAreaElement(IPlayAreaElement *piElement,bool *pbStopEnumerating)
{
	IPlayAreaCheckPoint *piCheckPoint=QI(IPlayAreaCheckPoint,piElement);
	if(piCheckPoint)
	{
		CVector vStart,vEnd,vCheckPoint;
		CVector vPos=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementPosition();
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayerRoute(&vStart,&vEnd);
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
	m_PlayAreaManagerWrapper.Detach();
	m_GameControllerWrapper.Detach();
	m_RenderWrapper.Detach();

	m_InspectionCamera.Destroy();
	m_PlayCamera.Detach();

	if(m_piGameSystem){m_piGameSystem->DestroyAllObjects();}
	if(m_piGameSystem){m_piGameSystem->Destroy();}
	REL(m_piGameSystem);
	REL(m_piSystemManager);
	m_bCompleted=false;
}

void CGameInterface::OnDraw(IGenericRender *piRender)
{
	char sTempText[512]={0};

	if(!m_piPlayerEntity){return;}


	if(m_bPlayerKilledOnPreviousFrame)
	{
		m_bPlayerKilledOnPreviousFrame=false;
		if(m_piPlayer->GetLivesLeft()==0)
		{
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnScenarioFinished(eScenarioFinishedReason_GameOver));
		}
		else
		{
			unsigned long nLivesLeft=m_piPlayer->GetLivesLeft();
			unsigned long nPoints=m_piPlayer->GetPoints();
			ResetGame(true);
			m_piPlayer->SetLivesLeft(nLivesLeft);
			m_piPlayer->SetPoints(nPoints);
		}
		return;
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
			unsigned int dwCurrentTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime();
			unsigned int dwMinutes=0,dwSeconds=0,dwMilliseconds=0;
			dwCurrentTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime();
			dwMinutes=dwCurrentTime/(1000*60);
			dwSeconds=dwCurrentTime/1000-dwMinutes*60;
			dwMilliseconds=dwCurrentTime-(dwSeconds*1000+dwMinutes*60*1000);
			sprintf(sTempText,"%02d:%02d.%03d %s",dwMinutes,dwSeconds,dwMilliseconds,m_FrameManagerWrapper.m_piFrameManager->IsPaused()?"  <PAUSED>":"");
			m_piSTGameTime->SetText(sTempText);
		}
	}
	

	m_FrameManagerWrapper.m_piFrameManager->ProcessFrame();
	if(m_bPauseOnNextFrame)
	{
		m_bPauseOnNextFrame=false;
		m_FrameManagerWrapper.m_piFrameManager->SetPauseOnNextFrame(true);
	}

	ProcessInput();
	
	if((m_dwMovementType==MOVEMENT_TYPE_PLAY)||
		(m_dwMovementType==MOVEMENT_TYPE_INSPECT && m_piGUIManager->IsKeyDown(GK_LSHIFT)))
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->ProcessInput(m_piGUIManager);
	}

	if(!m_FrameManagerWrapper.m_piFrameManager->IsPaused())
	{
		m_GameControllerWrapper.m_piGameController->ProcessFrame(m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime(),m_FrameManagerWrapper.m_piFrameManager->GetTimeFraction());
	}
	if(!m_bCompleted && m_PlayAreaManagerWrapper.m_piPlayAreaManager)
	{
		if(m_PlayAreaManagerWrapper.m_piPlayAreaManager->IsScenarioCompleted())
		{
			m_bCompleted=true;
			NOTIFY_EVENT(IGameInterfaceWindowEvents,OnScenarioFinished(eScenarioFinishedReason_Completed));
		}
	}
}

void CGameInterface::ProcessInput()
{
	bool bSideMovement=false;
	bool bControlKeyPressed=false;
	if(m_bFrozen)
	{
		return;
	}
	if(m_dwNextAcceptedControlKeyTime<m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime())
	{
		if(m_piGUIManager->IsKeyDown('I'))
		{
			ResetGame(false);
			m_dwNextAcceptedControlKeyTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()+300;
			return;
		}
		if(m_piGUIManager->IsKeyDown('C'))
		{
			ResetGame(true);
			m_dwNextAcceptedControlKeyTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()+300;
			return;
		}

		if(m_piGUIManager->IsKeyDown(GK_F1))
		{
			bControlKeyPressed=true;
			SetMovementType(m_dwMovementType == MOVEMENT_TYPE_INSPECT ? MOVEMENT_TYPE_PLAY : MOVEMENT_TYPE_INSPECT);
		}
		if(m_piGUIManager->IsKeyDown(GK_F2)){bControlKeyPressed=true;m_RenderWrapper.m_piRender->ToggleFlag(RENDER_SHOW_BBOXES);}
		if(m_piGUIManager->IsKeyDown(GK_F3)){bControlKeyPressed=true;m_RenderWrapper.m_piRender->ToggleFlag(RENDER_ENABLE_TEXTURES|RENDER_ENABLE_SOLID);}
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
	if(m_piGUIManager->IsKeyDown('P')){ProcessKey(KEY_PROCESS_ONE_FRAME);}

	if(m_dwMovementType==MOVEMENT_TYPE_INSPECT || 
		(m_dwMovementType==MOVEMENT_TYPE_PLAY && !m_FrameManagerWrapper.m_piFrameManager->IsPaused()))
	{
		if(m_piGUIManager->IsKeyDown(GK_UP) || m_piGUIManager->IsKeyDown(GK_NUMPAD8) || m_piGUIManager->IsKeyDown('W')){ProcessKey(KEY_FORWARD);}
		if(m_piGUIManager->IsKeyDown(GK_DOWN) || m_piGUIManager->IsKeyDown(GK_NUMPAD2) || m_piGUIManager->IsKeyDown('S')){ProcessKey(KEY_BACK);}
		if(m_piGUIManager->IsKeyDown(GK_LEFT) || m_piGUIManager->IsKeyDown(GK_NUMPAD4) || m_piGUIManager->IsKeyDown('A')){ProcessKey(KEY_LEFT);bSideMovement=true;}
		if(m_piGUIManager->IsKeyDown(GK_RIGHT) || m_piGUIManager->IsKeyDown(GK_NUMPAD6) || m_piGUIManager->IsKeyDown('D')){ProcessKey(KEY_RIGHT);bSideMovement=true;}
		if(m_piGUIManager->IsKeyDown(GK_NUMPAD9) || m_piGUIManager->IsKeyDown('R')){ProcessKey(KEY_UP);}
		if(m_piGUIManager->IsKeyDown(GK_NUMPAD3) || m_piGUIManager->IsKeyDown('F')){ProcessKey(KEY_DOWN);}
	}
}

void CGameInterface::MoveInspection(unsigned short nKey)
{
	double dForwardSpeed=m_dMovementInspectionSpeed*m_FrameManagerWrapper.m_piFrameManager->GetRealTimeFraction();
	if(m_piGUIManager->IsKeyDown(GK_LSHIFT)){dForwardSpeed*=3.0;}
	if(nKey==KEY_FORWARD)	{CVector vCameraPos=m_InspectionCamera.m_piCamera->GetPosition()+m_InspectionCamera.m_piCamera->GetForwardVector()*(dForwardSpeed);m_InspectionCamera.m_piCamera->SetPosition(vCameraPos);}
	else if(nKey==KEY_BACK)	{CVector vCameraPos=m_InspectionCamera.m_piCamera->GetPosition()-m_InspectionCamera.m_piCamera->GetForwardVector()*(dForwardSpeed);m_InspectionCamera.m_piCamera->SetPosition(vCameraPos);}
	else if(nKey==KEY_LEFT)	{CVector vCameraPos=m_InspectionCamera.m_piCamera->GetPosition()-m_InspectionCamera.m_piCamera->GetRightVector()*(dForwardSpeed);m_InspectionCamera.m_piCamera->SetPosition(vCameraPos);}
	else if(nKey==KEY_RIGHT){CVector vCameraPos=m_InspectionCamera.m_piCamera->GetPosition()+m_InspectionCamera.m_piCamera->GetRightVector()*(dForwardSpeed);m_InspectionCamera.m_piCamera->SetPosition(vCameraPos);}
	else if(nKey==KEY_UP)	{CVector vCameraPos=m_InspectionCamera.m_piCamera->GetPosition()+m_InspectionCamera.m_piCamera->GetUpVector()*(dForwardSpeed);m_InspectionCamera.m_piCamera->SetPosition(vCameraPos);}
	else if(nKey==KEY_DOWN)	{CVector vCameraPos=m_InspectionCamera.m_piCamera->GetPosition()-m_InspectionCamera.m_piCamera->GetUpVector()*(dForwardSpeed);m_InspectionCamera.m_piCamera->SetPosition(vCameraPos);}
}

void CGameInterface::ProcessKey(unsigned short nKey)
{
	// en modo de inspeccion todos los movimientos se calculan en tiempo realm no en el tiempo
	// del sistema de entidades.

	if(nKey==KEY_PAUSE && !m_bPauseOnNextFrame)
	{
		if(m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()>m_dwNextAcceptedPauseKeyTime)
		{
			m_dwNextAcceptedPauseKeyTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()+100;
			m_FrameManagerWrapper.m_piFrameManager->TogglePauseOnNextFrame();
		}
	}
	if(nKey==KEY_PROCESS_ONE_FRAME && !m_bPauseOnNextFrame)
	{
		if(m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()>m_dwNextAcceptedPauseKeyTime)
		{
			m_dwNextAcceptedPauseKeyTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentRealTime()+30;
			m_FrameManagerWrapper.m_piFrameManager->SetPauseOnNextFrame(false);
			m_bPauseOnNextFrame=true;
		}
	}

	if(m_dwMovementType==MOVEMENT_TYPE_INSPECT && (m_piGUIManager->IsKeyDown(GK_LSHIFT))==0)
	{
		MoveInspection(nKey);
	}
}
void	CGameInterface::Freeze(bool bFreeze)
{
	if(m_bFrozen==bFreeze){return;}

	if(bFreeze)
	{
		m_bResumeAfterFreeze=!m_FrameManagerWrapper.m_piFrameManager->IsPaused();
		m_FrameManagerWrapper.m_piFrameManager->SetPauseOnNextFrame(true);
		if(m_dwMovementType==MOVEMENT_TYPE_INSPECT)
		{
			SetMovementType(MOVEMENT_TYPE_PLAY);
		}
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

void CGameInterface::OnKilled(IEntity *piEntity)
{
	if(piEntity==m_piPlayerEntity)
	{
		m_bPlayerKilledOnPreviousFrame=true;
	}
}

void CGameInterface::OnMouseMove(double x,double y)
{
	if(m_bFrozen){return;}

	if(m_dwMovementType==MOVEMENT_TYPE_INSPECT && m_piGUIManager->HasMouseCapture(this) && m_rRealRect.w && m_rRealRect.h)
	{
		if(m_InspectionMovementStartPoint.x!=x || m_InspectionMovementStartPoint.y!=y)
		{
			CVector vMove(x-m_InspectionMovementStartPoint.x,-1*(y-m_InspectionMovementStartPoint.y),0);

			double dAmmount1=-vMove.c[0]/2.0;
			double dAmmount2=vMove.c[1]/2.0;
			CVector vAngles=m_InspectionCamera.m_piCamera->GetAngles();
			vAngles.c[YAW]+=dAmmount1;
			vAngles.c[PITCH]+=dAmmount2;
			m_InspectionCamera.m_piCamera->SetAngles(vAngles);
			m_piGUIManager->SetMousePosition(this,m_InspectionMovementStartPoint);
		}
	}
	return CGameWindowBase::OnMouseMove(x,y);
}

void CGameInterface::SetMovementType(unsigned long nType)
{
	if(nType==m_dwMovementType){return;}

	if(nType==MOVEMENT_TYPE_PLAY)
	{
#pragma message ("CGameInterface::SetMovementType -> como se ha quitado la camara del render hay que dar otra forma para hacer el cambio a inspeccion")
		m_dwMovementType=nType;
		//m_RenderWrapper.m_piRender->SetCamera(m_PlayCamera.m_piCamera);
		if(m_piGUIManager->HasMouseCapture(this))
		{
			m_piGUIManager->ReleaseMouseCapture();
		}
	}
	else if(nType==MOVEMENT_TYPE_INSPECT)
	{
		m_dwMovementType=nType;
		//m_RenderWrapper.m_piRender->SetCamera(m_InspectionCamera.m_piCamera);

		m_piGUIManager->SetMouseCapture(this);
		m_piGUIManager->GetMousePosition(this,&m_InspectionMovementStartPoint);
	}
}