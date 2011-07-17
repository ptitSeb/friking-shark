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
#include "PlayerManager.h"

CPlayerManager::CPlayerManager(void)
{
	m_piPlayer=NULL;
    m_piPlayerEntity=NULL;
	m_bStarted=false;
	m_bPlayerLandingEnabled=false;
	m_bPlayerTakeOffEnabled=false;
	m_eGameStage=ePlayerManagerGameStage_TakeOff;
	m_dScenarioDifficulty=0;
	
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
	
	PersistencyInitialize();
}

CPlayerManager::~CPlayerManager(void)
{
}

bool CPlayerManager::Init(std::string sClass,std::string sName,ISystem *piSystem)
{
    bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
    if(bOk){bOk=m_GameControllerWrapper.Attach("GameSystem","GameController");}
    if(bOk){bOk=m_PlayAreaManagerWrapper.Attach("GameSystem","PlayAreaManager");}
    if(bOk){m_GameControllerWrapper.m_piGameController->RegisterManager(150,this);}
    SetPlayerProfile(NULL);
	return bOk;
}

void CPlayerManager::Destroy()
{
	if(m_GameControllerWrapper.m_piGameController){m_GameControllerWrapper.m_piGameController->UnregisterManager(this);}
    m_GameControllerWrapper.Detach();
	m_PlayAreaManagerWrapper.Detach();
	m_PlayerProfile.Detach();
	CSystemObjectBase::Destroy();
}

void CPlayerManager::CreateScenario()
{
	CloseScenario();
	PersistencyDefaultValue("ScenarioProps");
	m_PlayerEntityType.Attach("EntityTypes","Player");
}

void CPlayerManager::LoadScenario(ISystemPersistencyNode *piNode)
{
    PersistencyLoad(piNode->GetNode("Player"),"ScenarioProps");
	PrepareResources();
}

void CPlayerManager::PrepareResources()
{
	if(m_PlayerEntityType.m_piEntityType){m_PlayerEntityType.m_piEntityType->PrepareResources();}
}

void CPlayerManager::SaveScenario(ISystemPersistencyNode *piNode)
{
	PersistencySave(piNode->AddNode("Player"),"ScenarioProps");
}

void CPlayerManager::CloseScenario()
{
	m_PlayerEntityType.Detach();
	m_PlayerLandingRoute.Clear();
	m_PlayerTakeOffRoute.Clear();
	m_bPlayerLandingEnabled=false;
	m_bPlayerTakeOffEnabled=false;	
}

void CPlayerManager::Start()
{
	m_bStarted=true;

	m_piPlayerEntity=m_PlayerEntityType.m_piEntityType?m_PlayerEntityType.m_piEntityType->CreateInstance(NULL,0):NULL;
	m_piPlayer=m_piPlayerEntity?dynamic_cast<IPlayer*>(m_piPlayerEntity):NULL;
	if(m_piPlayerEntity){m_piPlayerEntity->GetPhysicInfo()->dMaxVelocity=m_piPlayer->GetSpeed()*0.5;}	
	if(m_piPlayerEntity){SUBSCRIBE_TO_CAST(m_piPlayerEntity,IEntityEvents);}

	m_eGameStage=m_bPlayerTakeOffEnabled?ePlayerManagerGameStage_TakeOff:ePlayerManagerGameStage_Moving;
	m_dPlayMovementCurrentRoll=0;
	m_dwPlayMovementLastRollTime=0;
	m_dPlayMovementCurrentForward=0;
	m_dPlayMovementCurrentRight=0;
	
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->SetPlayMovementPosition(m_vPlayerStart);
	
	if(m_eGameStage==ePlayerManagerGameStage_Moving)
	{
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->StartMovingCamera();
	}
	else if(m_eGameStage==ePlayerManagerGameStage_TakeOff)
	{
		m_piPlayerEntity->SetRoute(&m_PlayerTakeOffRoute);
		m_PlayAreaManagerWrapper.m_piPlayAreaManager->StopMovingCamera();
	}	
}

void CPlayerManager::Stop()
{
	if(m_piPlayerEntity){m_piPlayerEntity->Remove();m_piPlayerEntity=NULL;}
	m_piPlayer=NULL;
	m_bStarted=false;
	m_eGameStage=ePlayerManagerGameStage_TakeOff;
}

void CPlayerManager::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	if(!m_bStarted){return;}
	if(!m_piPlayerEntity){return;}
	IGenericCamera *piCamera=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCamera();
	if(!piCamera){return;}
	
	CVector vStart,vEnd;
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
	CVector vPlayMovementPos=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementPosition();
	double  dPlayMovementSpeed=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraSpeed();
	double  dCameraDistance=piCamera->GetPosition().c[1]-vStart.c[1];
	double  dCameraViewAngle=piCamera->GetViewAngle();
	double  dCameraAspectRatio=piCamera->GetAspectRatio();
	double  dMaxScroll=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementMaxHorzScroll();
	
	m_dPlayMovementMaxRight	=dMaxScroll+dCameraDistance*sin(DegreesToRadians((dCameraViewAngle*dCameraAspectRatio)/2.0));
	m_dPlayMovementMinRight	=-m_dPlayMovementMaxRight;
	m_dCameraFollowFactor    =dMaxScroll/m_dPlayMovementMaxRight;
	
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->SetCameraOffset(CVector(0,0,m_dPlayMovementCurrentRight*m_dCameraFollowFactor));

	m_dPlayMovementMaxForward=dCameraDistance*sin(DegreesToRadians(dCameraViewAngle/2.0));
	m_dPlayMovementMinForward=-m_dPlayMovementMaxForward;	
	
	if(m_eGameStage==ePlayerManagerGameStage_TakeOff)
	{
		if(m_piPlayerEntity->HasFinishedRoute())
		{
			m_eGameStage=ePlayerManagerGameStage_Moving;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->StartMovingCamera();
		}
	}	
	else if(m_eGameStage==ePlayerManagerGameStage_Moving)
	{
		if(m_piPlayerEntity->GetHealth()>0)
		{
			if(m_bPlayerLandingEnabled)
			{
				SRoutePoint sPoint;
				m_PlayerLandingRoute.GetPoint(0,&sPoint);
				if(sPoint.vPosition.c[0]<vPlayMovementPos.c[0])
				{
					m_eGameStage=ePlayerManagerGameStage_Landing;
					m_piPlayerEntity->SetRoute(&m_PlayerLandingRoute);
				}
			}
			else
			{
				if(vEnd.c[0]<vPlayMovementPos.c[0])
				{
					m_eGameStage=ePlayerManagerGameStage_Finished;
				}
			}
		}		
	}
	else if(m_eGameStage==ePlayerManagerGameStage_Landing)
	{
		if(m_piPlayerEntity->HasFinishedRoute())
		{
			m_eGameStage=ePlayerManagerGameStage_Finished;
		}
	}
		
	
	if(dTimeFraction!=0 && m_piPlayerEntity->GetHealth()>0)
	{
		if(m_dwPlayMovementLastRollTime+10<dwCurrentTime)
		{
			if(m_dPlayMovementCurrentRoll>0)
			{
				m_dPlayMovementCurrentRoll-=m_dPlayMovementRollVelocity*dwCurrentTime;
				if(m_dPlayMovementCurrentRoll<0){m_dPlayMovementCurrentRoll=0;}
			}
			if(m_dPlayMovementCurrentRoll<0)
			{
				m_dPlayMovementCurrentRoll+=m_dPlayMovementRollVelocity*dwCurrentTime;
				if(m_dPlayMovementCurrentRoll>0){m_dPlayMovementCurrentRoll=0;}
			}
		}

		m_piPlayerEntity->GetPhysicInfo()->vAngles.c[ROLL]=m_dPlayMovementCurrentRoll;
		
		if(m_eGameStage==ePlayerManagerGameStage_Moving)
		{
			m_piPlayerEntity->GetPhysicInfo()->vPosition=vPlayMovementPos+CVector(m_dPlayMovementCurrentForward,0,m_dPlayMovementCurrentRight);
			m_piPlayerEntity->GetPhysicInfo()->vVelocity=CVector(dPlayMovementSpeed,0,0);
		}
		else
		{
			m_dPlayMovementCurrentRight=m_piPlayerEntity->GetPhysicInfo()->vPosition.c[2]-vPlayMovementPos.c[2];
			m_dPlayMovementCurrentForward=m_piPlayerEntity->GetPhysicInfo()->vPosition.c[0]-vPlayMovementPos.c[0];
		}
	}
	
	if(!m_piPlayerEntity->HasFinishedRoute())
	{
		m_PlayerKilledVelocity=Origin;
	}	
	REL(piCamera);
}

void CPlayerManager::ProcessInput(IGameGUIManager *piGUIManager,unsigned int dwCurrentTime,double dTimeFraction)
{
	if( m_piPlayer==NULL || 
		m_piPlayerEntity==NULL || 
		m_piPlayerEntity->GetHealth()<=0 || 
		m_eGameStage!=ePlayerManagerGameStage_Moving)
	{
		return;
	}
	m_PlayerKilledVelocity=CVector(m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraSpeed(),0,0);
	if(CheckKey(piGUIManager,"MoveForward")){MovePlayer(KEY_FORWARD,dwCurrentTime,dTimeFraction);}
	if(CheckKey(piGUIManager,"MoveBackward")){MovePlayer(KEY_BACK,dwCurrentTime,dTimeFraction);}
	if(CheckKey(piGUIManager,"MoveLeft")){MovePlayer(KEY_LEFT,dwCurrentTime,dTimeFraction);}
	if(CheckKey(piGUIManager,"MoveRight")){MovePlayer(KEY_RIGHT,dwCurrentTime,dTimeFraction);}
	if(CheckKey(piGUIManager,"FireBullets")){m_piPlayer->FireWeaponsOnSlot(0,dwCurrentTime);}
	if(CheckKey(piGUIManager,"FireBomb")){m_piPlayer->FireWeaponsOnSlot(1,dwCurrentTime);}
}

bool CPlayerManager::CheckKey(IGameGUIManager *piGUIManager,const char *pKeyName)
{
	std::map<std::string,SKeyMapping>::iterator i=m_KeyboardMapping.find(pKeyName);
	if(i!=m_KeyboardMapping.end())
	{
		SKeyMapping &keyMapping=i->second;
		for(unsigned int c=0;c<keyMapping.vValidCombinations.size();c++)
		{
			SKeyCombination &combination=keyMapping.vValidCombinations[c];
			if(piGUIManager->IsKeyDown(combination.nKey) &&
				(combination.nModifierA==0||piGUIManager->IsKeyDown(combination.nModifierA)) &&
				(combination.nModifierB==0||piGUIManager->IsKeyDown(combination.nModifierB)))
				{
					return true;
				}
		}
	}
	return false;
	
}

void CPlayerManager::MovePlayer(unsigned long nKey,unsigned int dwCurrentTime,double dTimeFraction)
{
	CVector vPlayMovementForward=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementForward();
	CVector vPlayMovementRight=m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetPlayMovementRight();

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
		m_dwPlayMovementLastRollTime=dwCurrentTime;

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
		m_dwPlayMovementLastRollTime=dwCurrentTime;

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

void CPlayerManager::OnRemoved(IEntity *piEntity)
{
	if(piEntity==m_piPlayerEntity){m_piPlayer=NULL;m_piPlayerEntity=NULL;}
}

void CPlayerManager::OnKilled(IEntity *piEntity)
{
	if(piEntity==m_piPlayerEntity)
	{
		m_piPlayerEntity->GetPhysicInfo()->vVelocity=m_PlayerKilledVelocity;
	}
}

void CPlayerManager::GetPlayerConfig( SPlayerConfig *pConfig )
{
	pConfig->bPlayerLandingEnabled=m_bPlayerLandingEnabled;
	pConfig->bPlayerTakeOffEnabled=m_bPlayerTakeOffEnabled;
	pConfig->dDifficulty=m_dScenarioDifficulty;
	
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

void CPlayerManager::SetPlayerConfig( SPlayerConfig *pConfig )
{
	m_bPlayerLandingEnabled=pConfig->bPlayerLandingEnabled;
	m_bPlayerTakeOffEnabled=pConfig->bPlayerTakeOffEnabled;
	m_dScenarioDifficulty=pConfig->dDifficulty;
	
	m_PlayerLandingRoute.Clear();
	for(int x=0;x<4;x++){m_PlayerLandingRoute.AddPoint(x,SRoutePoint(true,pConfig->pvPlayerLandingPoints[x]));}
	m_PlayerTakeOffRoute.Clear();
	for(int x=0;x<4;x++){m_PlayerTakeOffRoute.AddPoint(x,SRoutePoint(true,pConfig->pvPlayerTakeOffPoints[x]));}
}

bool CPlayerManager::IsScenarioCompleted()
{
	return m_eGameStage>=ePlayerManagerGameStage_Finished;
}

double CPlayerManager::GetEffectiveDifficulty()
{
	double dPlayerDifficulty=m_PlayerProfile.m_piProfile?m_PlayerProfile.m_piProfile->GetDifficulty():0;
	return m_dBaseDifficulty+(m_dScenarioDifficulty*m_dLevelDifficultyWeight)+(dPlayerDifficulty*m_dPlayerDifficultyWeight);
}

void CPlayerManager::SetPlayerStart(CVector vPosition)
{
	SRoutePoint sLandPoint;
	SRoutePoint sTakeOffPoint;
	
	CVector vStart,vEnd;
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->GetCameraRoute(&vStart,&vEnd);
	
	m_PlayerLandingRoute.GetPoint(0,&sLandPoint);
	m_PlayerTakeOffRoute.GetPoint(0,&sTakeOffPoint);

	if(m_piPlayerEntity)
	{
		if(m_bPlayerTakeOffEnabled && vPosition.c[0]<=vStart.c[0])
		{
			m_piPlayerEntity->GetPhysicInfo()->vPosition=sTakeOffPoint.vPosition;
			m_piPlayerEntity->SetRoute(&m_PlayerTakeOffRoute);
			m_eGameStage=ePlayerManagerGameStage_TakeOff;
		}
		else if(m_bPlayerLandingEnabled && vPosition.c[0]>=sLandPoint.vPosition.c[0])
		{
			m_piPlayerEntity->GetPhysicInfo()->vPosition=sLandPoint.vPosition;
			m_piPlayerEntity->SetRoute(&m_PlayerLandingRoute);
			m_eGameStage=ePlayerManagerGameStage_Landing;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->StartMovingCamera();
		}
		else
		{
			m_piPlayerEntity->SetRoute(NULL);
			m_eGameStage=ePlayerManagerGameStage_Moving;
			m_PlayAreaManagerWrapper.m_piPlayAreaManager->StartMovingCamera();
		}
	}
	m_vPlayerStart=vPosition;
	m_PlayAreaManagerWrapper.m_piPlayAreaManager->SetPlayMovementPosition(m_vPlayerStart);
}

void CPlayerManager::SetPlayerProfile(IPlayerProfile *piProfile)
{
	if(m_PlayerProfile.m_piProfile){UNSUBSCRIBE_FROM_CAST(m_PlayerProfile.m_piProfile,IPlayerProfileEvents);}
	m_KeyboardMapping.clear();
	
	if(piProfile){m_PlayerProfile.Attach(piProfile);}
			 else{m_PlayerProfile.Create(m_piSystem,"CPlayerProfile","");}
			 
	if(m_PlayerProfile.m_piProfile)
	{
		SUBSCRIBE_TO_CAST(m_PlayerProfile.m_piProfile,IPlayerProfileEvents);
		m_PlayerProfile.m_piProfile->GetKeyboardMapping(&m_KeyboardMapping);
	}
}

void CPlayerManager::OnPlayerNameChanged(const std::string &sName){}
void CPlayerManager::OnDifficultyChanged(double dDifficulty){}
void CPlayerManager::OnKeyboardMappingChanged()
{
	m_PlayerProfile.m_piProfile->GetKeyboardMapping(&m_KeyboardMapping);
}
