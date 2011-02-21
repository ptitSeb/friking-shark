#include "./stdafx.h"
#include "GameRunTimeLib.h"
#include "GameGUI.h"
#include "PlayAreaManager.h"



CPlayAreaManager::CPlayAreaManager(void)
{
	m_dPlayAreaHeight=20;
    m_dPlayMovementSpeed=5;
    m_dCameraPitch=0;
	m_dCameraFollowFactor=0;
    m_dCameraDistanceFromPlayer=115;
	m_dPlayMovementMaxHorzScroll=0;
	m_piPlayer=NULL;
    m_piPlayerEntity=NULL;
	m_bStarted=false;
	m_bScenarioCompleted=false;

	m_dPlayMovementMaxRoll=30;
	m_dPlayMovementMaxForward=0;
	m_dPlayMovementMaxRight=0;
	m_dPlayMovementMinForward=0;
	m_dPlayMovementMinRight=0;
	m_dPlayMovementCurrentForward=0;
	m_dPlayMovementCurrentRight=0;
	m_dPlayMovementCurrentRoll=0;
	m_dPlayMovementRollVelocity=360;
	m_dwPlayMovementLastRollTime=0;

	m_bProcessingPlayerIntroPhase1=false;
	m_bProcessingPlayerOutroPhase1=false;
	m_bProcessingPlayerOutroPhase2=false;

	m_dCameraFollowFactor=1.0;

}

CPlayAreaManager::~CPlayAreaManager(void)
{
}

bool CPlayAreaManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
    if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
    if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(400,this);}
	if(bOk){bOk=m_FrameManagerWrapper.Attach("GameSystem","FrameManager");}
	return bOk;
}

void CPlayAreaManager::Destroy()
{
	m_CameraWrapper.Detach();
    if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->UnregisterManager(this);}
    m_FrameManagerWrapper.Detach();
	m_GameControllerWrapper.Detach();
	CSystemObjectBase::Destroy();
}

void CPlayAreaManager::CreateScenario()
{
	CloseScenario();
	m_CameraWrapper.Create(m_piSystem,"Camera","PlayCamera");
	
	PersistencyDefaultValue("ScenarioProps");
	UpdatePlayArea();
}

void CPlayAreaManager::LoadScenario(ISystemPersistencyNode *piNode)
{
    PersistencyLoad(piNode->GetNode("AreaDeJuego"),"ScenarioProps");
	UpdateEntityLayers();
	UpdatePlayArea();
}

void CPlayAreaManager::SaveScenario(ISystemPersistencyNode *piNode)
{
	PersistencySave(piNode->AddNode("AreaDeJuego"),"ScenarioProps");
}

void CPlayAreaManager::CloseScenario()
{
	m_vEntityLayerElements.clear();
	m_vEntityLayers.clear();
    m_vElements.clear();
	m_CameraWrapper.Destroy();
}

void CPlayAreaManager::Start()
{
	m_bStarted=true;

	UpdatePlayArea();

    CEntityTypeWrapper playerTypeWrapper;
    CVector vAngles,vDir;
    vDir=m_vPlayerRouteEnd-m_vPlayerRouteStart;
    vDir.N();
    AnglesFromVector(vDir,&vAngles);
	m_vPlayMovementPos=m_vPlayerRouteStart;

    playerTypeWrapper.Attach("EntityTypes","Player");
    m_piPlayerEntity=playerTypeWrapper.m_piEntityType->CreateInstance(NULL,0);
    m_piPlayerEntity->GetPhysicInfo()->vAngles.c[YAW]=vAngles.c[YAW];
    m_piPlayerEntity->GetPhysicInfo()->vAngles.c[PITCH]=vAngles.c[PITCH];
    m_piPlayerEntity->GetPhysicInfo()->vPosition=m_vPlayerRouteStart;
	m_piPlayer=m_piPlayerEntity?dynamic_cast<IPlayer*>(m_piPlayerEntity):NULL;
	if(m_piPlayer)
	{
		m_dNormalPlayerSpeed=m_piPlayer->GetSpeed();

		if(m_bProcessingPlayerIntroPhase1)
		{
			m_piPlayerEntity->GetPhysicInfo()->dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
			m_piPlayer->SetSpeed(m_dNormalPlayerSpeed*0.5);
		}
	}

	m_bScenarioCompleted=false;
	m_bProcessingPlayerIntroPhase1=true;
	m_bProcessingPlayerOutroPhase1=false;
	m_bProcessingPlayerOutroPhase2=false;

	m_dPlayMovementCurrentRoll=0;
	m_dwPlayMovementLastRollTime=0;
	m_dPlayMovementCurrentForward=0;
	m_dPlayMovementCurrentRight=0;
	if(m_bProcessingPlayerIntroPhase1)
	{
		m_dPlayMovementCurrentForward=m_dPlayMovementMinForward-(m_dPlayMovementMaxForward-m_dPlayMovementMinForward)*0.1;
	}
	else
	{
		m_dPlayMovementCurrentForward=m_dPlayMovementMinForward+(m_dPlayMovementMaxForward-m_dPlayMovementMinForward)*0.1;
	}

	m_CameraLight.Create(m_piSystem,"Light","");
	m_CameraLight.m_piLight->SetColor(CVector(1,1,1));
	//m_CameraLight.m_piLight->SetPosition(m_CameraWrapper.m_piCamera->GetPosition());
	//m_CameraLight.m_piLight->SetOmni(m_dCameraDistanceFromPlayer*2.0);
	/*
	DWORD dLightStrength=255;
	m_pCameraLight=new CLight("CameraLight",LIGHT_TYPE_OMNI,m_CameraWrapper.m_piCamera->GetPosition(),RGB(dLightStrength,dLightStrength,dLightStrength));
	m_pCameraLight->m_bIsActive=true;
	m_pCameraLight->m_vSpotDir=CVector(0,-1,0);
	m_pCameraLight->m_fSpotAngle=40;
	m_pCameraLight->m_fAttenuationConstant=0;
	m_pCameraLight->m_fAttenuationLinear=(float)0.005;
	m_RenderWrapper.m_piRender->AddLight(m_pCameraLight);
	m_RenderWrapper.m_piRender->SetCamera(m_CameraWrapper.m_piCamera);
	*/
	//m_RenderWrapper.m_piRender->AddLight(m_CameraLight.m_piLight);
//	m_RenderWrapper.m_piRender->SetCamera(m_CameraWrapper.m_piCamera);

}

void CPlayAreaManager::Stop()
{
	if(m_CameraLight.m_piLight)
	{
		//m_RenderWrapper.m_piRender->RemoveLight(m_CameraLight.m_piLight);
	}
	m_CameraLight.Destroy();

	for(unsigned x=0;x<m_vElements.size();x++)
    {
        IPlayAreaElement *piElement=m_vElements[x].m_piElement;
		piElement->Reset();
    }
	for(unsigned x=0;x<m_vEntityLayerElements.size();x++)
	{
		IPlayAreaElement *piElement=m_vEntityLayerElements[x].m_piElement;
		piElement->Reset();
	}    

	if(m_piPlayerEntity){m_piPlayerEntity->Remove();m_piPlayerEntity=NULL;}
	m_piPlayer=NULL;

	m_bScenarioCompleted=false;
	m_bStarted=false;
	m_bProcessingPlayerIntroPhase1=false;
	m_bProcessingPlayerOutroPhase1=false;
	m_bProcessingPlayerOutroPhase2=false;
}
CVector vSunAngles(0,90,0);
void CPlayAreaManager::ProcessFrame(DWORD dwCurrentTime,double dTimeFraction)
{
	if(!m_bStarted)
	{
		return;
	}

	// Comprobar si el Jugador ha llegado al final, si lo ha hecho finalizar la partida.
	if(m_piPlayerEntity && m_piPlayerEntity->GetHealth()>0 && m_bScenarioCompleted==false)
	{
		double dTotalDistance=m_vPlayerRouteEnd-m_vPlayerRouteStart;
		double dCompletedDistance=m_vPlayMovementPos-m_vPlayerRouteStart;
		if(dCompletedDistance>=dTotalDistance)
		{
			m_bScenarioCompleted=true;
			m_bProcessingPlayerOutroPhase1=true;
			double dSpeed=m_piPlayer->GetSpeed();
			m_piPlayer->SetSpeed(dSpeed*0.5);
			m_piPlayerEntity->GetPhysicInfo()->dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		}
	}

	UpdatePlayCameraPosition();

	if(!m_FrameManagerWrapper.m_piFrameManager->IsPaused() && m_piPlayerEntity && m_piPlayerEntity->GetHealth()>0)
	{
		if(m_dwPlayMovementLastRollTime+10<m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime())
		{
			if(m_dPlayMovementCurrentRoll>0)
			{
				m_dPlayMovementCurrentRoll-=m_dPlayMovementRollVelocity*m_FrameManagerWrapper.m_piFrameManager->GetTimeFraction();
				if(m_dPlayMovementCurrentRoll<0){m_dPlayMovementCurrentRoll=0;}
			}
			if(m_dPlayMovementCurrentRoll<0)
			{
				m_dPlayMovementCurrentRoll+=m_dPlayMovementRollVelocity*m_FrameManagerWrapper.m_piFrameManager->GetTimeFraction();
				if(m_dPlayMovementCurrentRoll>0){m_dPlayMovementCurrentRoll=0;}
			}
		}

		if(m_piPlayerEntity)
		{
			m_piPlayerEntity->GetPhysicInfo()->vAngles.c[ROLL]=m_dPlayMovementCurrentRoll;
			m_piPlayerEntity->GetPhysicInfo()->vPosition=m_vPlayMovementPos+m_vPlayMovementRight*m_dPlayMovementCurrentRight+m_vPlayMovementForward*m_dPlayMovementCurrentForward;
		}
	}

	if(m_piPlayerEntity && m_bProcessingPlayerIntroPhase1)
	{
		double dTargetForward=m_dPlayMovementMinForward+(m_dPlayMovementMaxForward-m_dPlayMovementMinForward)*0.2;
		if(m_dPlayMovementCurrentForward<(dTargetForward-5))
		{
			MovePlayer(KEY_FORWARD);
		}
		else
		{
			m_piPlayerEntity->GetPhysicInfo()->dwBoundsType=PHYSIC_BOUNDS_TYPE_BBOX;
			m_piPlayer->SetSpeed(m_dNormalPlayerSpeed);
			m_bProcessingPlayerIntroPhase1=false;
		}
	}

	if(m_piPlayerEntity && m_bProcessingPlayerOutroPhase1)
	{
		bool bCentered=true;
		double dTargetRight=0;
		double dTargetForward=m_dPlayMovementMinForward+(m_dPlayMovementMaxForward-m_dPlayMovementMinForward)*0.1;

		if(m_dPlayMovementCurrentRight>(dTargetRight+5))
		{
			bCentered=false;
			MovePlayer(KEY_LEFT);
		}
		if(m_dPlayMovementCurrentRight<(dTargetRight-5))
		{
			bCentered=false;
			MovePlayer(KEY_RIGHT);
		}
		if(m_dPlayMovementCurrentForward>(dTargetForward+5))
		{
			bCentered=false;
			MovePlayer(KEY_BACK);
		}
		if(m_dPlayMovementCurrentForward<(dTargetForward-5))
		{
			bCentered=false;
			MovePlayer(KEY_FORWARD);
		}
		if(bCentered)
		{
			m_bProcessingPlayerOutroPhase1=false;
			m_bProcessingPlayerOutroPhase2=true;
		}
	}

	if(m_piPlayerEntity && m_bProcessingPlayerOutroPhase2)
	{
		double dSpeed=m_piPlayer->GetSpeed();
		m_piPlayer->SetSpeed(dSpeed*(1.0+2*m_FrameManagerWrapper.m_piFrameManager->GetTimeFraction()));
		MovePlayer(KEY_FORWARD);

		if(m_dPlayMovementCurrentForward>=m_dPlayMovementMaxForward*1.3)
		{
			m_bProcessingPlayerOutroPhase2=false;
		}
	}
	if(m_piPlayerEntity && !m_bProcessingPlayerIntroPhase1 && !m_bProcessingPlayerOutroPhase1 && !m_bProcessingPlayerOutroPhase2 && !m_bScenarioCompleted)
	{
		m_vPlayMovementPos+=m_vPlayMovementForward*dTimeFraction*m_dPlayMovementSpeed;
	}

	CalculateAirPlayArea();

	CVector vCameraPos=m_vPlayMovementPos+m_vPlayMovementUp*m_dCameraDistanceFromPlayer;
	CVector vRect[4];
	vRect[0]=CVector(m_vVisibleAirPlayAreaMins.c[0],m_vVisibleAirPlayAreaMins.c[1],m_vVisibleAirPlayAreaMins.c[2]);
	vRect[1]=CVector(m_vVisibleAirPlayAreaMins.c[0],m_vVisibleAirPlayAreaMins.c[1],m_vVisibleAirPlayAreaMaxs.c[2]);
	vRect[2]=CVector(m_vVisibleAirPlayAreaMaxs.c[0],m_vVisibleAirPlayAreaMins.c[1],m_vVisibleAirPlayAreaMaxs.c[2]);
	vRect[3]=CVector(m_vVisibleAirPlayAreaMaxs.c[0],m_vVisibleAirPlayAreaMins.c[1],m_vVisibleAirPlayAreaMins.c[2]);

	m_PlayArea.nPlaneCount=2;
	m_PlayArea.planes[SPlayAreaInfo::Far]=CPlane(vCameraPos,vRect[3],vRect[2]);
	m_PlayArea.planes[SPlayAreaInfo::Near]=CPlane(vCameraPos,vRect[1],vRect[0]);

	unsigned x;
	for(x=0;x<m_vElements.size();x++)
	{
		IPlayAreaElement *piElement=m_vElements[x].m_piElement;
		piElement->ProcessFrame(m_vPlayMovementPos,&m_PlayArea,dwCurrentTime,dTimeFraction);
	}
	for(x=0;x<m_vEntityLayerElements.size();x++)
	{
		IPlayAreaElement *piElement=m_vEntityLayerElements[x].m_piElement;
		piElement->ProcessFrame(m_vPlayMovementPos,&m_PlayArea,dwCurrentTime,dTimeFraction);
	}
}

void CPlayAreaManager::GetPlayerRoute(CVector *pvStart,CVector *pvEnd)
{
	*pvStart=m_vPlayerRouteStart;
	*pvEnd=m_vPlayerRouteEnd;
}

void CPlayAreaManager::SetPlayMovementPosition(CVector vPosition){m_vPlayMovementPos=vPosition;}
CVector CPlayAreaManager::GetPlayMovementPosition(){return m_vPlayMovementPos;}
CVector CPlayAreaManager::GetPlayMovementForward(){return m_vPlayMovementForward;}
CVector CPlayAreaManager::GetPlayMovementRight(){return m_vPlayMovementRight;}
CVector CPlayAreaManager::GetPlayMovementUp(){return m_vPlayMovementUp;}
bool  CPlayAreaManager::IsScenarioCompleted()
{
	return m_bScenarioCompleted&&m_bProcessingPlayerOutroPhase1==false&&m_bProcessingPlayerOutroPhase2==false;
}

void CPlayAreaManager::GetAirPlayPlane(CVector *pPlayAreaMins,CVector *pPlayAreaMaxs){*pPlayAreaMins=m_vAirPlayAreaMins,*pPlayAreaMaxs=m_vAirPlayAreaMaxs;}
void CPlayAreaManager::GetVisibleAirPlayPlane(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs){*pVisiblePlayAreaMins=m_vVisibleAirPlayAreaMins;*pVisiblePlayAreaMaxs=m_vVisibleAirPlayAreaMaxs;}

void CPlayAreaManager::GetPlayAreaPlaneAt(CVector vPos,CVector *pPlayAreaMins,CVector *pPlayAreaMaxs)
{
    double dCameraDistance=(m_vPlayerRouteStart.c[1]+m_dCameraDistanceFromPlayer)-vPos.c[1];
    double dViewAngle=m_CameraWrapper.m_piCamera->GetViewAngle();
    double dAspectRatio=m_CameraWrapper.m_piCamera->GetAspectRatio();
    CVector vOffset;
    vOffset.c[0]=dCameraDistance*tan(DegreesToRadians((dViewAngle*dAspectRatio)/2.0));
    vOffset.c[2]=dCameraDistance*tan(DegreesToRadians(dViewAngle)/2.0);
    *pPlayAreaMins=vPos;
    *pPlayAreaMaxs=vPos;
    *pPlayAreaMins-=m_vPlayMovementRight*vOffset.c[0];
    *pPlayAreaMaxs+=m_vPlayMovementRight*vOffset.c[0];
    *pPlayAreaMins-=m_vPlayMovementForward*vOffset.c[2];
    *pPlayAreaMaxs+=m_vPlayMovementForward*vOffset.c[2];
}

void CPlayAreaManager::CalculateAirPlayArea()
{
	CVector vStartMins,vStartMaxs,vEndMins,vEndMaxs;
    GetPlayAreaPlaneAt(m_vPlayerRouteStart,&vStartMins,&vStartMaxs);
	GetPlayAreaPlaneAt(m_vPlayerRouteEnd,&vEndMins,&vEndMaxs);
	m_vAirPlayAreaMins.Mins(vStartMins,vEndMins);
	m_vAirPlayAreaMaxs.Maxs(vStartMaxs,vEndMaxs);
    GetPlayAreaPlaneAt(m_vPlayMovementPos,&m_vVisibleAirPlayAreaMins,&m_vVisibleAirPlayAreaMaxs);
}

void CPlayAreaManager::EnumeratePlayAreaElements(IPlayAreaElementEnumerationCallback *piCallback)
{
	bool bStopEnumerating=false;
	std::vector<CPlayAreaElementWrapper>::iterator i;

	for(i=m_vElements.begin();i!=m_vElements.end() && !bStopEnumerating;i++)
	{
		CPlayAreaElementWrapper *pElement=&(*i);
		piCallback->ProcessEnumeratedPlayAreaElement(pElement->m_piElement,&bStopEnumerating);
	}
}

IGenericCamera *CPlayAreaManager::GetCamera(){return ADD(m_CameraWrapper.m_piCamera);}

void CPlayAreaManager::UpdatePlayCameraPosition()
{
	CVector vCameraAngles;
	CVector vCameraPosition;

	AnglesFromVector(m_vPlayMovementForward,&vCameraAngles);
	vCameraAngles.c[PITCH]-=90;
	vCameraAngles.c[PITCH]+=m_dCameraPitch;

	vCameraPosition=m_vPlayMovementPos;
	vCameraPosition+=m_vPlayMovementUp*(-sin(vCameraAngles.c[PITCH]*2*PI/360)*m_dCameraDistanceFromPlayer);
	vCameraPosition+=m_vPlayMovementForward*(-cos(vCameraAngles.c[PITCH]*2*PI/360)*m_dCameraDistanceFromPlayer);
	vCameraPosition+=m_vPlayMovementRight*(m_dPlayMovementCurrentRight*m_dCameraFollowFactor);

	m_CameraWrapper.m_piCamera->SetPosition(vCameraPosition);
	m_CameraWrapper.m_piCamera->SetAngles(vCameraAngles);

	m_CameraLight.m_piLight->SetPosition(m_vPlayMovementPos+CVector(0,m_dCameraDistanceFromPlayer,0));
	m_CameraLight.m_piLight->SetColor(CVector(1,1,1));
	m_CameraLight.m_piLight->SetOmni(m_dCameraDistanceFromPlayer*1.5);
}

void CPlayAreaManager::ProcessInput(IGameGUIManager *piGUIManager)
{
	if( m_piPlayer==NULL || m_piPlayerEntity==NULL || m_piPlayerEntity->GetHealth()<=0 || 
		m_bProcessingPlayerOutroPhase1 || m_bProcessingPlayerOutroPhase2 || m_bProcessingPlayerIntroPhase1)
	{
		return;
	}

	if(piGUIManager->IsKeyDown(VK_UP) || piGUIManager->IsKeyDown(VK_NUMPAD8) || piGUIManager->IsKeyDown('W')){MovePlayer(KEY_FORWARD);}
	if(piGUIManager->IsKeyDown(VK_DOWN) || piGUIManager->IsKeyDown(VK_NUMPAD2) || piGUIManager->IsKeyDown('S')){MovePlayer(KEY_BACK);}
	if(piGUIManager->IsKeyDown(VK_LEFT) || piGUIManager->IsKeyDown(VK_NUMPAD4) || piGUIManager->IsKeyDown('A')){MovePlayer(KEY_LEFT);}
	if(piGUIManager->IsKeyDown(VK_RIGHT) || piGUIManager->IsKeyDown(VK_NUMPAD6) || piGUIManager->IsKeyDown('D')){MovePlayer(KEY_RIGHT);}
	if(piGUIManager->IsKeyDown(VK_LCONTROL)){m_piPlayer->FireWeaponsOnSlot(0,m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime());}
	if(piGUIManager->IsKeyDown(VK_MENU)){m_piPlayer->FireWeaponsOnSlot(1,m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime());}
}

void CPlayAreaManager::MovePlayer(unsigned long nKey)
{
	m_PlayerKilledVelocity=Origin;

	double dTimeFraction=m_FrameManagerWrapper.m_piFrameManager->GetTimeFraction();
	CVector vPlayMovementForward=GetPlayMovementForward();
	CVector vPlayMovementRight=GetPlayMovementRight();


	if(nKey==KEY_LEFT)
	{
		m_dPlayMovementCurrentRoll-=m_dPlayMovementRollVelocity*dTimeFraction;
		if(m_dPlayMovementCurrentRoll>m_dPlayMovementMaxRoll){m_dPlayMovementCurrentRoll=m_dPlayMovementMaxRoll;}
		if(m_dPlayMovementCurrentRoll<-m_dPlayMovementMaxRoll){m_dPlayMovementCurrentRoll=-m_dPlayMovementMaxRoll;}
		m_dPlayMovementCurrentRight-=m_piPlayer->GetSpeed()*dTimeFraction;
		if(m_dPlayMovementCurrentRight<m_dPlayMovementMinRight)
		{
			m_dPlayMovementCurrentRight=m_dPlayMovementMinRight;
		}
		m_dwPlayMovementLastRollTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime();

		m_PlayerKilledVelocity-=vPlayMovementRight*(m_piPlayer->GetSpeed()*0.25);
	}

	if(nKey==KEY_RIGHT)
	{
		m_dPlayMovementCurrentRoll+=m_dPlayMovementRollVelocity*dTimeFraction;
		if(m_dPlayMovementCurrentRoll>m_dPlayMovementMaxRoll){m_dPlayMovementCurrentRoll=m_dPlayMovementMaxRoll;}
		if(m_dPlayMovementCurrentRoll<-m_dPlayMovementMaxRoll){m_dPlayMovementCurrentRoll=-m_dPlayMovementMaxRoll;}
		m_dPlayMovementCurrentRight+=m_piPlayer->GetSpeed()*dTimeFraction;
		if(m_dPlayMovementCurrentRight>m_dPlayMovementMaxRight)
		{
			m_dPlayMovementCurrentRight=m_dPlayMovementMaxRight;
		}
		m_dwPlayMovementLastRollTime=m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime();

		m_PlayerKilledVelocity+=vPlayMovementRight*(m_piPlayer->GetSpeed()*0.25);
	}

	if(nKey==KEY_FORWARD)
	{
		m_dPlayMovementCurrentForward+=m_piPlayer->GetSpeed()*dTimeFraction;
		if(m_bProcessingPlayerOutroPhase2==false && m_dPlayMovementCurrentForward>m_dPlayMovementMaxForward)
		{
			m_dPlayMovementCurrentForward=m_dPlayMovementMaxForward;
		}

		m_PlayerKilledVelocity+=vPlayMovementForward*(m_piPlayer->GetSpeed()*0.25);
	}
	if(nKey==KEY_BACK)
	{
		m_dPlayMovementCurrentForward-=m_piPlayer->GetSpeed()*dTimeFraction;
		if(m_dPlayMovementCurrentForward<m_dPlayMovementMinForward)
		{
			m_dPlayMovementCurrentForward=m_dPlayMovementMinForward;
		}

		m_PlayerKilledVelocity-=vPlayMovementForward*(m_piPlayer->GetSpeed()*0.25);
	}

	if(m_piPlayerEntity->GetHealth()>0)
	{
		m_piPlayerEntity->GetPhysicInfo()->vVelocity=m_PlayerKilledVelocity;
	}
}

// Entity Layers

unsigned long CPlayAreaManager::AddEntityLayer( SEntityLayer *pLayer )
{
	int nIndex=m_vEntityLayers.size();
	SEntityLayerData data;
	m_vEntityLayers.push_back(data);

	SEntityLayerData *pData=&m_vEntityLayers[nIndex];
	pData->m_LayerConfig=*pLayer;
	pData->m_EntityType.Attach("EntityTypes",pData->m_LayerConfig.sEntityType);
	return nIndex;
}

void CPlayAreaManager::UpdateEntityLayer( unsigned int nIndex,SEntityLayer *pLayer )
{
	if(nIndex>=m_vEntityLayers.size()){return;}
	SEntityLayerData *pData=&m_vEntityLayers[nIndex];
	pData->m_LayerConfig=*pLayer;
	pData->m_EntityType.Attach("EntityTypes",pData->m_LayerConfig.sEntityType);
}


void CPlayAreaManager::RemoveEntityLayer( unsigned int nIndex )
{
	unsigned int x;
	std::vector<SEntityLayerData>::iterator i;
	for(x=0,i=m_vEntityLayers.begin();i!=m_vEntityLayers.end();i++,x++)
	{
		if(x==nIndex)
		{
			m_vEntityLayers.erase(i);
			break;
		}
	}
}

void CPlayAreaManager::GetEntityLayer( unsigned int nIndex,SEntityLayer *pLayer,IEntityType **ppiEntityType)
{
	if(ppiEntityType){*ppiEntityType=NULL;}

	if(nIndex>=m_vEntityLayers.size()){return;}
	SEntityLayerData *pData=&m_vEntityLayers[nIndex];
	if(pLayer){*pLayer=pData->m_LayerConfig;}
	if(ppiEntityType){*ppiEntityType=ADD(pData->m_EntityType.m_piEntityType);}
}

unsigned int CPlayAreaManager::GetEntityLayers()
{
	return m_vEntityLayers.size();
}

struct SEntityGenerationPositionKey
{
	bool operator ()(const CVector &k1,const CVector &k2)
	{
		if(k1.c[0]<k2.c[0]){return true;};
		if(k1.c[0]>k2.c[0]){return false;};
		if(k1.c[2]<k2.c[2]){return true;};
		if(k1.c[2]>k2.c[2]){return false;};
		return false;
	}
};


bool CPlayAreaManager::UpdateEntityLayers()
{
	m_vEntityLayerElements.clear();

	CWorldManagerWrapper worldManager;
	worldManager.Attach("GameSystem","WorldManager");

	IGenericModel   *piTerrainModel=NULL;
	IGenericTexture *piColorMap=NULL;
	if(worldManager.m_piTerrain){worldManager.m_piTerrain->GetTerrainColorMap(NULL,&piColorMap);}
	if(worldManager.m_piTerrain){worldManager.m_piTerrain->GetTerrainModel(&piTerrainModel);}
	if(piColorMap==NULL || piTerrainModel==NULL)
	{
		REL(piColorMap);
		REL(piTerrainModel);
		return false;
	}

	unsigned int nTextureWidth=0,nTextureHeight=0;
	CVector vTerrainSize,vTerrainMins,vTerrainMaxs;
	piColorMap->GetSize(&nTextureWidth,&nTextureHeight);
	piTerrainModel->GetFrameBBox(0,0,&vTerrainMins,&vTerrainMaxs);
	vTerrainSize=piTerrainModel->GetFrameSize(0,0);


	//	Generacion de objetos basada directamente en ColorMap
	for(unsigned long l=0;l<m_vEntityLayers.size();l++)
	{
		SEntityLayerData *pLayer=&m_vEntityLayers[l];
		if(pLayer->m_EntityType.m_piEntityType==NULL){continue;}

		set<CVector,SEntityGenerationPositionKey> vEntities;
		set<CVector,SEntityGenerationPositionKey>::iterator i;

		double dSeparation=pLayer->m_LayerConfig.dSeparation;// in Entities
		double dJitter=pLayer->m_LayerConfig.dPositionJitter;// in Entities
		double dYawJitter=pLayer->m_LayerConfig.dYawJitter;// 0-360
		CVector vEntitiesize=pLayer->m_EntityType.m_piEntityType->GetSize();

		srand(0);

		for(double dX=vTerrainMins.c[0];dX<=vTerrainMaxs.c[0];dX+=(dSeparation*vEntitiesize.c[0]))
		{
			for(double dZ=vTerrainMins.c[2];dZ<=vTerrainMaxs.c[2];dZ+=(dSeparation*vEntitiesize.c[2]))
			{
				double dRandMaxCenter=((double)RAND_MAX)/2.0;

				CVector vPoint(dX,0,dZ);
				vPoint.c[0]+=((dRandMaxCenter-(double)rand())/(double)RAND_MAX)*dJitter*vEntitiesize.c[0];
				vPoint.c[2]+=((dRandMaxCenter-(double)rand())/(double)RAND_MAX)*dJitter*vEntitiesize.c[2];
				vPoint.c[1]=0;

				// check this point against the texture.
				CVector vPixel;
				vPixel.c[0]=(vPoint.c[0]-vTerrainMins.c[0])*((double)nTextureWidth)/vTerrainSize.c[0];
				vPixel.c[1]=((double)nTextureHeight-1.0)-(vPoint.c[2]-vTerrainMins.c[2])*((double)nTextureHeight)/vTerrainSize.c[2];
				CVector vPixelColor=piColorMap->GetPixelColor((unsigned long)vPixel.c[0],(unsigned long)vPixel.c[1]);

				CVector vYUVDiffence=HSVDifference(vPixelColor,pLayer->m_LayerConfig.vColor);
				if( fabs(vYUVDiffence.c[1])<=pLayer->m_LayerConfig.dColorSaturationMargin&& 
					fabs(vYUVDiffence.c[0])<=pLayer->m_LayerConfig.dColorValueMargin)
				{
					double dHeight=0;
					worldManager.m_piTerrain->GetTerrainHeightAt(vPoint,&dHeight);
					vPoint.c[1]=dHeight+FP_PRECISION*10;
					vEntities.insert(vPoint);
				}
			}
		}
		srand(0);

		for(i=vEntities.begin();i!=vEntities.end();i++)
		{
			CPlayAreaEntityWrapper wrapper;
			wrapper.Create(m_piSystem,"CPlayAreaEntity","");
			if(wrapper.m_piElement)
			{
				wrapper.m_piElement->SetEntityType(pLayer->m_EntityType.m_piEntityType);
				wrapper.m_piElement->SetPosition(*i);
				wrapper.m_piElement->SetAngles(CVector(((double)rand()/(double)RAND_MAX)*dYawJitter,0,0));
				m_vEntityLayerElements.push_back(wrapper);
			}
		}
		RTTRACE("Generated %d entities",vEntities.size());
	}
	REL(piTerrainModel);
	REL(piColorMap);
	return true;
}

void CPlayAreaManager::DesignRender( IGenericRender *piRender )
{
	unsigned long x;
	for(x=0;x<m_vElements.size();x++)
	{
		m_vElements[x].m_piElement->DesignRender(piRender,false);
	}
	for(x=0;x<m_vEntityLayerElements.size();x++)
	{
		m_vEntityLayerElements[x].m_piElement->DesignRender(piRender,false);
	}
}

void CPlayAreaManager::GetPlayAreaConfig( SPlayAreaConfig *pConfig )
{
	pConfig->dCameraDistance=m_dCameraDistanceFromPlayer;
	pConfig->dCameraSpeed=m_dPlayMovementSpeed;
	pConfig->dCameraScroll=m_dPlayMovementMaxHorzScroll;
	pConfig->dCameraViewAngle=m_CameraWrapper.m_piCamera->GetViewAngle();
	pConfig->dCameraAspectRatio=m_CameraWrapper.m_piCamera->GetAspectRatio();
	pConfig->dAirPlaneHeight=m_dPlayAreaHeight;
}

void CPlayAreaManager::SetPlayAreaConfig( SPlayAreaConfig *pConfig )
{
	m_dCameraDistanceFromPlayer=pConfig->dCameraDistance;
	m_dPlayMovementMaxHorzScroll=pConfig->dCameraScroll;
	m_dPlayMovementSpeed=pConfig->dCameraSpeed;
	m_CameraWrapper.m_piCamera->SetViewAngle(pConfig->dCameraViewAngle);
	m_CameraWrapper.m_piCamera->SetAspectRatio(pConfig->dCameraAspectRatio);
	m_dPlayAreaHeight=pConfig->dAirPlaneHeight;

	UpdatePlayArea();
}

void CPlayAreaManager::UpdatePlayArea()
{
	CWorldManagerWrapper worldManager;
	worldManager.Attach("GameSystem","WorldManager");

	IGenericModel   *piTerrainModel=NULL;
	if(worldManager.m_piTerrain){worldManager.m_piTerrain->GetTerrainModel(&piTerrainModel);}
	CVector vTerrainSize,vTerrainMins,vTerrainMaxs;
	if(piTerrainModel)
	{
		piTerrainModel->GetFrameBBox(0,0,&vTerrainMins,&vTerrainMaxs);
		vTerrainSize=piTerrainModel->GetFrameSize(0,0);
	}
	REL(piTerrainModel);

	m_vPlayMovementPos.c[1]=vTerrainMaxs.c[1]+m_dPlayAreaHeight;

	// dinside representa el offset que hay que desplazar el inicio y final de la ruta
	// para que al empezar y terminar se vea el terreno adecuadamente dado el angulo de la camara
	// si no se hace esto la ruta del player empieza justo donde lo hace el terreno y se ve una zona negra
	// en la parte inferior de la pantalla

	double dCameraHeight=m_vPlayMovementPos.c[1]+m_dCameraDistanceFromPlayer;
	double dInside=(dCameraHeight-vTerrainMins.c[1])*tan(DegreesToRadians(m_CameraWrapper.m_piCamera->GetViewAngle()/2.0));
	
	// Si el terreno no es lo suficientemente grande para la camara se ajusta al minimo requerido por esta
	// y se le añade 1.0 por cada lado para crear un vector de ruta valido.

	if(dInside>vTerrainSize.c[0])
	{
		vTerrainMins.c[0]=0-(dInside+1.0);
		vTerrainMaxs.c[0]=dInside+1.0;
		vTerrainSize=vTerrainMaxs-vTerrainMins;
	}
	if(dInside>vTerrainSize.c[2])
	{
		vTerrainMins.c[2]=0-(dInside+1.0);
		vTerrainMaxs.c[2]=dInside+1.0;
		vTerrainSize=vTerrainMaxs-vTerrainMins;
	}

	m_vPlayerRouteStart=vTerrainMins+CVector(dInside,0,vTerrainSize.c[2]*0.5);
	m_vPlayerRouteEnd=m_vPlayerRouteStart+CVector(vTerrainSize.c[0]-dInside*2,0,0);
	m_vPlayerRouteStart.c[1]=vTerrainMaxs.c[1]+m_dPlayAreaHeight;
	m_vPlayerRouteEnd.c[1]=vTerrainMaxs.c[1]+m_dPlayAreaHeight;

	double dMaxScroll=m_dPlayMovementMaxHorzScroll;
	m_dPlayMovementMaxRight	=dMaxScroll+m_dCameraDistanceFromPlayer*sin(DegreesToRadians((m_CameraWrapper.m_piCamera->GetViewAngle()*m_CameraWrapper.m_piCamera->GetAspectRatio())/2.0));
	m_dPlayMovementMinRight	=-m_dPlayMovementMaxRight;
	m_dCameraFollowFactor    =dMaxScroll/m_dPlayMovementMaxRight;

	m_dPlayMovementMaxForward=m_dCameraDistanceFromPlayer*sin(DegreesToRadians(m_CameraWrapper.m_piCamera->GetViewAngle()/2.0));
	m_dPlayMovementMinForward=-m_dPlayMovementMaxForward;


	CVector vAngles,vDir;
	vDir=m_vPlayerRouteEnd-m_vPlayerRouteStart;
	vDir.N();
	AnglesFromVector(vDir,&vAngles);
	VectorsFromAngles(vAngles,&m_vPlayMovementForward,&m_vPlayMovementRight,&m_vPlayMovementUp);

	CalculateAirPlayArea();
}

unsigned long CPlayAreaManager::AddElement( std::string sType )
{
	unsigned long nElement=m_vElements.size();
	CPlayAreaElementWrapper wrapper;
	wrapper.Create(m_piSystem,sType,"");
	if(wrapper.m_piElement){m_vElements.push_back(wrapper);}
	return nElement;
}

void CPlayAreaManager::RemoveElement( unsigned int nIndex )
{
	if(nIndex>=m_vElements.size()){return;}
	unsigned int x;
	std::vector<CPlayAreaElementWrapper>::iterator i;
	for(x=0,i=m_vElements.begin();i!=m_vElements.end();i++,x++)
	{
		if(x==nIndex)
		{
			m_vElements.erase(i);
			break;
		}
	}
}

void CPlayAreaManager::GetElement( unsigned int nIndex,IPlayAreaElement **ppiElement)
{
	if(ppiElement){(*ppiElement)=NULL;}
	if(nIndex>=m_vElements.size()){return;}
	if(ppiElement){(*ppiElement)=ADD(m_vElements[nIndex].m_piElement);}
}

unsigned int CPlayAreaManager::GetElements()
{
	return m_vElements.size();
}
