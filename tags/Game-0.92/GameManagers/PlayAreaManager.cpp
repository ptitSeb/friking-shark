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
	m_bPlayerLandingEnabled=false;
	m_bPlayerTakeOffEnabled=false;
	
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

	m_dCameraFollowFactor=1.0;

	m_piMusicSound=NULL;
	m_piIntroMusicSound=NULL;
}

CPlayAreaManager::~CPlayAreaManager(void)
{
}

bool CPlayAreaManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
    if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
    if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(200,this);}
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
	PrepareResources();
}

void CPlayAreaManager::PrepareResources()
{
	//	Generacion de objetos basada directamente en ColorMap
	for(unsigned long l=0;l<m_vEntityLayers.size();l++)
	{
		SEntityLayerData *pLayer=&m_vEntityLayers[l];
		if(pLayer->m_EntityType.m_piEntityType==NULL){continue;}
		pLayer->m_EntityType.m_piEntityType->PrepareResources();
	}
	
	for(unsigned x=0;x<m_vElements.size();x++)
	{
		IPlayAreaElement *piElement=m_vElements[x].m_piElement;
		piElement->PrepareResources();
	}
	CEntityTypeWrapper playerTypeWrapper;
	playerTypeWrapper.Attach("EntityTypes","Player");
	if(playerTypeWrapper.m_piEntityType)
	{
		playerTypeWrapper.m_piEntityType->PrepareResources();
	}
	
}

void CPlayAreaManager::SaveScenario(ISystemPersistencyNode *piNode)
{
	PersistencySave(piNode->AddNode("AreaDeJuego"),"ScenarioProps");
}

void CPlayAreaManager::CloseScenario()
{
	m_vEntityLayerElements.clear();
	m_vDynamicElements.clear();
	m_vEntityLayers.clear();
    m_vElements.clear();
	m_IntroMusic.Destroy();
	m_Music.Destroy();
	m_CameraWrapper.Destroy();
	m_PlayerLandingRoute.Clear();
	m_PlayerTakeOffRoute.Clear();
	m_bPlayerLandingEnabled=false;
	m_bPlayerTakeOffEnabled=false;	
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
	m_piPlayer=m_piPlayerEntity?dynamic_cast<IPlayer*>(m_piPlayerEntity):NULL;
	if(m_piPlayer)
	{
		m_piPlayerEntity->GetPhysicInfo()->vAngles.c[YAW]=vAngles.c[YAW];
		m_piPlayerEntity->GetPhysicInfo()->vAngles.c[PITCH]=vAngles.c[PITCH];
		m_piPlayerEntity->GetPhysicInfo()->dMaxVelocity=m_piPlayer->GetSpeed()*0.5;
	}	
	SUBSCRIBE_TO_CAST(m_piPlayerEntity,IEntityEvents);

	m_bScenarioCompleted=false;
	m_dPlayMovementCurrentRoll=0;
	m_dwPlayMovementLastRollTime=0;
	m_dPlayMovementCurrentForward=0;
	m_dPlayMovementCurrentRight=0;
	
	SetPlayMovementPosition(m_vPlayerRouteStart);
	
	if(m_IntroMusic.m_piSoundType)
	{
		m_piIntroMusicSound=m_IntroMusic.m_piSoundType->CreateInstance();
		if(m_piIntroMusicSound)
		{
			m_piIntroMusicSound->SetLoop(false);
			m_piIntroMusicSound->Play();
		}
	}
	if(m_Music.m_piSoundType)
	{
		m_piMusicSound=m_Music.m_piSoundType->CreateInstance();
		if(m_piMusicSound){m_piMusicSound->SetLoop(true);}
	}
}

void CPlayAreaManager::Stop()
{
	if(m_piMusicSound)
	{
		if(m_piMusicSound->IsPlaying()){m_piMusicSound->Stop();}
		delete m_piMusicSound;
		m_piMusicSound=NULL;
	}
	if(m_piIntroMusicSound)
	{
		if(m_piIntroMusicSound->IsPlaying()){m_piIntroMusicSound->Stop();}
		delete m_piIntroMusicSound;
		m_piIntroMusicSound=NULL;
	}
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
	m_vDynamicElements.clear();
	
	if(m_piPlayerEntity){m_piPlayerEntity->Remove();m_piPlayerEntity=NULL;}
	m_piPlayer=NULL;

	m_bScenarioCompleted=false;
	m_bStarted=false;
}

void CPlayAreaManager::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	if(!m_bStarted){return;}
	if(!m_piPlayerEntity){return;}
	
	// Watch out music playback
	// First see if this level has intro music, if not, or if the intro has finished, play the main theme
	
	if(!m_bScenarioCompleted)
	{
		bool bPlayMainTheme=(m_piIntroMusicSound==NULL || !m_piIntroMusicSound->IsPlaying());
		if(bPlayMainTheme && m_piMusicSound && !m_piMusicSound->IsPlaying())
		{
			m_piMusicSound->Play();
		}
	}
	else 
	{
		if(m_piIntroMusicSound && m_piIntroMusicSound->IsPlaying()){m_piIntroMusicSound->Stop();}
		if(m_piMusicSound && m_piMusicSound->IsPlaying()){m_piMusicSound->Stop();}
	}
		
	// Comprobar si el Jugador ha llegado al final, si lo ha hecho finalizar la partida.
	if(m_piPlayerEntity->GetHealth()>0 && m_bScenarioCompleted==false)
	{
		if(m_bPlayerLandingEnabled)
		{
			SRoutePoint sPoint;
			m_PlayerLandingRoute.GetPoint(0,&sPoint);
			if(sPoint.vPosition.c[0]<m_vPlayMovementPos.c[0])
			{
				m_bScenarioCompleted=true;
				if(m_bPlayerLandingEnabled){m_piPlayerEntity->SetRoute(&m_PlayerLandingRoute);}
			}
		}
		else
		{
			if(m_vPlayerRouteEnd.c[0]<m_vPlayMovementPos.c[0])
			{
				m_bScenarioCompleted=true;
			}
		}
	}

	UpdatePlayCameraPosition();

	if(!m_FrameManagerWrapper.m_piFrameManager->IsPaused() && m_piPlayerEntity->GetHealth()>0)
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

		m_piPlayerEntity->GetPhysicInfo()->vAngles.c[ROLL]=m_dPlayMovementCurrentRoll;
		if( m_piPlayerEntity->HasFinishedRoute() && !m_bScenarioCompleted)
		{
			m_piPlayerEntity->GetPhysicInfo()->vPosition=m_vPlayMovementPos+CVector(m_dPlayMovementCurrentForward,0,m_dPlayMovementCurrentRight);
			m_piPlayerEntity->GetPhysicInfo()->vVelocity=CVector(m_dPlayMovementSpeed,0,0);
		}
		else
		{
			m_dPlayMovementCurrentRight=m_piPlayerEntity->GetPhysicInfo()->vPosition.c[2]-m_vPlayMovementPos.c[2];
			m_dPlayMovementCurrentForward=m_piPlayerEntity->GetPhysicInfo()->vPosition.c[0]-m_vPlayMovementPos.c[0];
		}
	}
	
	if(!m_piPlayerEntity->HasFinishedRoute())
	{
		m_PlayerKilledVelocity=Origin;
	}
	if(m_vPlayMovementPos.c[0]<m_vPlayerRouteEnd.c[0] && (m_piPlayerEntity->GetRoute()!=&m_PlayerTakeOffRoute))
	{
		m_vPlayMovementPos.c[0]+=dTimeFraction*m_dPlayMovementSpeed;
	}

	CalculateAirPlayArea();

	CVector vCameraPos=m_vPlayMovementPos+AxisPosY*m_dCameraDistanceFromPlayer;
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
	for(x=0;x<m_vDynamicElements.size();x++)
	{
		IPlayAreaElement *piElement=m_vDynamicElements[x].m_piElement;
		piElement->ProcessFrame(m_vPlayMovementPos,&m_PlayArea,dwCurrentTime,dTimeFraction);
	}
}

void CPlayAreaManager::GetPlayerRoute(CVector *pvStart,CVector *pvEnd)
{
	*pvStart=m_vPlayerRouteStart;
	*pvEnd=m_vPlayerRouteEnd;
}

void CPlayAreaManager::SetPlayMovementPosition(CVector vPosition)
{
	SRoutePoint sLandPoint;
	SRoutePoint sTakeOffPoint;
	m_PlayerLandingRoute.GetPoint(0,&sLandPoint);
	m_PlayerTakeOffRoute.GetPoint(0,&sTakeOffPoint);
	
	if(m_piPlayerEntity)
	{
		if(m_bPlayerTakeOffEnabled && vPosition.c[0]<=m_vPlayerRouteStart.c[0])
		{
			vPosition.c[0]=m_vPlayerRouteStart.c[0];
			m_piPlayerEntity->GetPhysicInfo()->vPosition=sTakeOffPoint.vPosition;
			m_piPlayerEntity->SetRoute(&m_PlayerTakeOffRoute);
		}
		else if(m_bPlayerLandingEnabled && vPosition.c[0]>=sLandPoint.vPosition.c[0])
		{
			vPosition.c[0]=sLandPoint.vPosition.c[0];
			m_piPlayerEntity->GetPhysicInfo()->vPosition=sLandPoint.vPosition;
			m_piPlayerEntity->SetRoute(&m_PlayerLandingRoute);
		}
		else
		{
			if(vPosition.c[0]<m_vPlayerRouteStart.c[0]){vPosition.c[0]=m_vPlayerRouteStart.c[0];}
			if(vPosition.c[0]>m_vPlayerRouteEnd.c[0]){vPosition.c[0]=m_vPlayerRouteEnd.c[0];}
			
			m_piPlayerEntity->SetRoute(NULL);
		}
	}
	m_vPlayMovementPos=vPosition;
	CalculateAirPlayArea();
}
CVector CPlayAreaManager::GetPlayMovementPosition(){return m_vPlayMovementPos;}
CVector CPlayAreaManager::GetPlayMovementForward(){return AxisPosX;}
CVector CPlayAreaManager::GetPlayMovementRight(){return AxisPosZ;}
CVector CPlayAreaManager::GetPlayMovementUp(){return AxisPosY;}
bool  CPlayAreaManager::IsScenarioCompleted()
{
	return m_bScenarioCompleted && (m_piPlayerEntity==NULL || m_piPlayerEntity->HasFinishedRoute());
}

void CPlayAreaManager::GetAirPlayPlane(CVector *pPlayAreaMins,CVector *pPlayAreaMaxs){*pPlayAreaMins=m_vAirPlayAreaMins,*pPlayAreaMaxs=m_vAirPlayAreaMaxs;}
void CPlayAreaManager::GetVisibleAirPlayPlane(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs){*pVisiblePlayAreaMins=m_vVisibleAirPlayAreaMins;*pVisiblePlayAreaMaxs=m_vVisibleAirPlayAreaMaxs;}
void CPlayAreaManager::GetCurrentVisibleArea(CVector *pVisiblePlayAreaMins,CVector *pVisiblePlayAreaMaxs)
{
	*pVisiblePlayAreaMins=m_vVisibleAirPlayAreaMins;
	*pVisiblePlayAreaMaxs=m_vVisibleAirPlayAreaMaxs;
	CVector vCameraPos=m_CameraWrapper.m_piCamera?m_CameraWrapper.m_piCamera->GetPosition():Origin;
	pVisiblePlayAreaMins->c[2]+=vCameraPos.c[2]-m_vPlayerRouteStart.c[2];
	pVisiblePlayAreaMaxs->c[2]+=vCameraPos.c[2]-m_vPlayerRouteStart.c[2];
}

void CPlayAreaManager::GetPlayAreaPlaneAt(CVector vPos,CVector *pPlayAreaMins,CVector *pPlayAreaMaxs)
{
	if(m_CameraWrapper.m_piCamera==NULL){return;}
	
    double dCameraDistance=(m_vPlayerRouteStart.c[1]+m_dCameraDistanceFromPlayer)-vPos.c[1];
    double dViewAngle=m_CameraWrapper.m_piCamera->GetViewAngle();
    double dAspectRatio=m_CameraWrapper.m_piCamera->GetAspectRatio();
    CVector vOffset;
	vOffset.c[0]=dCameraDistance*tan(DegreesToRadians(dViewAngle*0.5));
	vOffset.c[2]=dCameraDistance*tan(DegreesToRadians((dViewAngle*0.5)))*dAspectRatio;
    *pPlayAreaMins=vPos;
    *pPlayAreaMaxs=vPos;
	pPlayAreaMins->c[0]-=vOffset.c[0];
	pPlayAreaMaxs->c[0]+=vOffset.c[0];
	pPlayAreaMins->c[2]-=vOffset.c[2];
	pPlayAreaMaxs->c[2]+=vOffset.c[2];
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
double          CPlayAreaManager::GetCameraSpeed(){return m_dPlayMovementSpeed;}

void CPlayAreaManager::UpdatePlayCameraPosition()
{
	if(m_CameraWrapper.m_piCamera==NULL){return;}
	
	CVector vCameraAngles;
	CVector vCameraPosition;

	vCameraAngles.c[PITCH]-=90;
	vCameraAngles.c[PITCH]+=m_dCameraPitch;

	vCameraPosition=m_vPlayMovementPos;
	vCameraPosition.c[0]+=(-cos(vCameraAngles.c[PITCH]*2*PI/360)*m_dCameraDistanceFromPlayer);
	vCameraPosition.c[1]+=(-sin(vCameraAngles.c[PITCH]*2*PI/360)*m_dCameraDistanceFromPlayer);
	vCameraPosition.c[2]+=(m_dPlayMovementCurrentRight*m_dCameraFollowFactor);

	m_CameraWrapper.m_piCamera->SetPosition(vCameraPosition);
	m_CameraWrapper.m_piCamera->SetAngles(vCameraAngles);

}

void CPlayAreaManager::ProcessInput(IGameGUIManager *piGUIManager)
{
	if( m_piPlayer==NULL || 
		m_piPlayerEntity==NULL || 
		m_piPlayerEntity->GetHealth()<=0 || 
		!m_piPlayerEntity->HasFinishedRoute() || 
		m_bScenarioCompleted)
	{
		return;
	}
	m_PlayerKilledVelocity=CVector(m_dPlayMovementSpeed,0,0);
	if(piGUIManager->IsKeyDown(GK_UP) || piGUIManager->IsKeyDown(GK_NUMPAD8) || piGUIManager->IsKeyDown('W')){MovePlayer(KEY_FORWARD);}
	if(piGUIManager->IsKeyDown(GK_DOWN) || piGUIManager->IsKeyDown(GK_NUMPAD2) || piGUIManager->IsKeyDown('S')){MovePlayer(KEY_BACK);}
	if(piGUIManager->IsKeyDown(GK_LEFT) || piGUIManager->IsKeyDown(GK_NUMPAD4) || piGUIManager->IsKeyDown('A')){MovePlayer(KEY_LEFT);}
	if(piGUIManager->IsKeyDown(GK_RIGHT) || piGUIManager->IsKeyDown(GK_NUMPAD6) || piGUIManager->IsKeyDown('D')){MovePlayer(KEY_RIGHT);}
	if(piGUIManager->IsKeyDown(GK_LCONTROL)){m_piPlayer->FireWeaponsOnSlot(0,m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime());}
	if(piGUIManager->IsKeyDown(GK_LMENU)){m_piPlayer->FireWeaponsOnSlot(1,m_FrameManagerWrapper.m_piFrameManager->GetCurrentTime());}	
}

void CPlayAreaManager::MovePlayer(unsigned long nKey)
{
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
		if(m_dPlayMovementCurrentForward>m_dPlayMovementMaxForward)
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
	pConfig->bPlayerLandingEnabled=m_bPlayerLandingEnabled;
	pConfig->bPlayerTakeOffEnabled=m_bPlayerTakeOffEnabled;
	
	int x;
	for(x=0;x<4;x++)
	{
		SRoutePoint sTemp1,sTemp2;
		m_PlayerLandingRoute.GetPoint(x,&sTemp1);
		m_PlayerTakeOffRoute.GetPoint(x,&sTemp2);
		pConfig->pvPlayerLandingPoints[x]=sTemp1.vPosition;
		pConfig->pvPlayerTakeOffPoints[x]=sTemp2.vPosition;
	}
}

void CPlayAreaManager::SetPlayAreaConfig( SPlayAreaConfig *pConfig )
{
	m_dCameraDistanceFromPlayer=pConfig->dCameraDistance;
	m_dPlayMovementMaxHorzScroll=pConfig->dCameraScroll;
	m_dPlayMovementSpeed=pConfig->dCameraSpeed;
	m_CameraWrapper.m_piCamera->SetViewAngle(pConfig->dCameraViewAngle);
	m_CameraWrapper.m_piCamera->SetAspectRatio(pConfig->dCameraAspectRatio);
	m_dPlayAreaHeight=pConfig->dAirPlaneHeight;
	m_bPlayerLandingEnabled=pConfig->bPlayerLandingEnabled;
	m_bPlayerTakeOffEnabled=pConfig->bPlayerTakeOffEnabled;
	
	m_PlayerLandingRoute.Clear();
	for(int x=0;x<4;x++){m_PlayerLandingRoute.AddPoint(x,SRoutePoint(true,pConfig->pvPlayerLandingPoints[x]));}
	m_PlayerTakeOffRoute.Clear();
	for(int x=0;x<4;x++){m_PlayerTakeOffRoute.AddPoint(x,SRoutePoint(true,pConfig->pvPlayerTakeOffPoints[x]));}
	
	UpdatePlayArea();
}

void CPlayAreaManager::UpdatePlayArea()
{
	if(m_CameraWrapper.m_piCamera==NULL){return;}
	
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
	double dInside=(dCameraHeight-vTerrainMins.c[1])*tan(DegreesToRadians(m_CameraWrapper.m_piCamera->GetViewAngle()*0.5));
	
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
	
	double dNear=m_dCameraDistanceFromPlayer-10;
	double dFar=m_dCameraDistanceFromPlayer+vTerrainSize.c[1]+m_dPlayAreaHeight+1.0;
	m_CameraWrapper.m_piCamera->SetClippingPlanes(dNear,dFar);

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

void CPlayAreaManager::OnRemoved(IEntity *piEntity)
{
	if(piEntity==m_piPlayerEntity){m_piPlayer=NULL;m_piPlayerEntity=NULL;}
}

void CPlayAreaManager::OnKilled(IEntity *piEntity)
{
	if(piEntity==m_piPlayerEntity)
	{
		m_piPlayerEntity->GetPhysicInfo()->vVelocity=m_PlayerKilledVelocity;
	}
}

bool CPlayAreaManager::IsVisible(CVector vPos,double dRadius)
{
	if(m_CameraWrapper.m_piCamera==NULL){return false;}
	
	CVector visibleSize;
	CVector vCameraPos=m_CameraWrapper.m_piCamera->GetPosition();
	visibleSize.c[0]=fabs((vCameraPos.c[1]-vPos.c[1])*tan(DegreesToRadians(m_CameraWrapper.m_piCamera->GetViewAngle()*0.5)));
	visibleSize.c[2]=visibleSize.c[0]*m_CameraWrapper.m_piCamera->GetAspectRatio();
	
	CVector vMins=vCameraPos-visibleSize;
	CVector vMaxs=vCameraPos+visibleSize;
	vMins.c[1]=vPos.c[1];
	vMaxs.c[1]=vPos.c[1];
	bool bHidden=((vPos.c[0]+dRadius)<vMins.c[0] ||
	(vPos.c[0]-dRadius)>vMaxs.c[0] ||
	(vPos.c[2]+dRadius)<vMins.c[2] ||
	(vPos.c[2]-dRadius)>vMaxs.c[2]);
	return !bHidden;
}

void CPlayAreaManager::CreateDynamicEntityElement(IEntityType *piEntityType,CVector vPosition,CVector vAngles,IPlayAreaEntity **ppiElement)
{
	if(m_bStarted)
	{
		CPlayAreaEntityWrapper wrapper;
		wrapper.Create(m_piSystem,"CPlayAreaEntity","");
		if(wrapper.m_piElement)
		{
			wrapper.m_piElement->SetEntityType(piEntityType);
			wrapper.m_piElement->SetPosition(vPosition);
			wrapper.m_piElement->SetAngles(vAngles);
			wrapper.m_piElement->SetDynamic(true);
			m_vDynamicElements.push_back(wrapper);
		}
		if(ppiElement){*ppiElement=ADD(wrapper.m_piElement);}
	}
}


bool CPlayAreaManager::SetIntroMusic(std::string sMusicFile)
{
	if(m_piIntroMusicSound)
	{
		if(m_piIntroMusicSound->IsPlaying()){m_piIntroMusicSound->Stop();}
		delete m_piIntroMusicSound;
		m_piIntroMusicSound=NULL;
	}
	
	m_IntroMusic.Destroy();
	bool bOk=m_IntroMusic.Create(m_piSystem,"SoundType","");
	if(bOk){m_IntroMusic.m_piSoundType->Load(sMusicFile);}
	if(bOk && m_bStarted && m_IntroMusic.m_piSoundType)
	{
		m_piIntroMusicSound=m_IntroMusic.m_piSoundType->CreateInstance();
		if(m_piIntroMusicSound)
		{
			m_piIntroMusicSound->SetLoop(false);
			if(!m_bScenarioCompleted)
			{
				if(m_piMusicSound && m_piMusicSound->IsPlaying()){m_piMusicSound->Stop();}
				m_piIntroMusicSound->Play();				
			}
		}
	}
	return bOk;
}

void CPlayAreaManager::GetIntroMusic(std::string *psMusicFile,ISoundType **ppiSoundType)
{
	if(psMusicFile)
	{
		*psMusicFile="";
		if(m_IntroMusic.m_piSoundType){*psMusicFile=m_IntroMusic.m_piSoundType->GetFileName();}
	}
	if(ppiSoundType)
	{
		*ppiSoundType=NULL;
		if(m_IntroMusic.m_piSoundType){*ppiSoundType=m_IntroMusic.m_piSoundType;}
	}
}

bool CPlayAreaManager::SetMusic(std::string sMusicFile)
{
	if(m_piMusicSound)
	{
		if(m_piMusicSound->IsPlaying()){m_piMusicSound->Stop();}
		delete m_piMusicSound;
		m_piMusicSound=NULL;
	}
	
	m_Music.Destroy();
	bool bOk=m_Music.Create(m_piSystem,"SoundType","");
	if(bOk){bOk=m_Music.m_piSoundType->Load(sMusicFile);}
	if(bOk && m_bStarted && m_Music.m_piSoundType)
	{
		m_piMusicSound=m_Music.m_piSoundType->CreateInstance();
		if(m_piMusicSound)
		{
			m_piMusicSound->SetLoop(true);
			if(!m_bScenarioCompleted){m_piMusicSound->Play();}
		}
	}
	return bOk;
}

void CPlayAreaManager::GetMusic(std::string *psMusicFile,ISoundType **ppiSoundType)
{
	if(psMusicFile)
	{
		*psMusicFile="";
		if(m_Music.m_piSoundType){*psMusicFile=m_Music.m_piSoundType->GetFileName();}
	}
	if(ppiSoundType)
	{
		*ppiSoundType=NULL;
		if(m_Music.m_piSoundType){*ppiSoundType=m_Music.m_piSoundType;}
	}
}

