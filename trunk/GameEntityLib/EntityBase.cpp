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
#include "GameEntityLib.h"
#include "EntityBase.h"
#include "SingletonWrapper.h"

CSingletonWrapper<IEntityManager> g_EntityManagerSingleton("GameSystem","EntityManager");
CSingletonWrapper<IPhysicManager> g_PhysicManagerSingleton("GameSystem","PhysicManager");
CSingletonWrapper<IFrameManager>  g_FrameManagerSingleton("GameSystem","FrameManager");

CEntityBase::CEntityBase()
{
    g_EntityManagerSingleton.AddRef();
    g_PhysicManagerSingleton.AddRef();
    g_FrameManagerSingleton.AddRef();

	m_dwCreationTime=g_FrameManagerSingleton.m_piInterface->GetCurrentTime();
    m_dwNextProcessFrame=0;
	m_nNextChildId=0;
    m_bRemoved=false;
    m_dwDamageType=DAMAGE_TYPE_NONE;
    m_dDamage=0;
    m_dHealth=0;
    m_dMaxHealth=0;
	m_nCurrentState=ENTITY_STATE_INVALID;
	m_nCurrentStateAnimation=ANIMATION_INVALID;
	
    g_EntityManagerSingleton.m_piInterface->AddEntity(this);
    m_piRoute=NULL;
	m_nRouteDelay=0;
    m_dwAlignment=ENTITY_ALIGNMENT_NEUTRAL;
    m_piTarget=NULL;
	m_piParent=NULL;
	m_dLastFrameDamage=0;
}

CEntityBase::~CEntityBase()
{
    unsigned x;
    for(x=0;x<m_vActiveAnimations.size();x++)
    {
        IAnimation *piAnimation=m_vActiveAnimations[x];
        delete piAnimation;
    }
    for(x=0;x<m_vWeapons.size();x++)
    {
      IWeapon *piWeapon=m_vWeapons[x];
      delete piWeapon;
    }
	
    g_EntityManagerSingleton.Release();
    g_PhysicManagerSingleton.Release();
    g_FrameManagerSingleton.Release();
}

void CEntityBase::OnDamage(double dDamage,IEntity *piAggresor)
{
    if(m_dwDamageType==DAMAGE_TYPE_NONE){return;}
    if(m_dHealth<=0){return;}
	
	m_dHealth-=dDamage;
	if(m_dHealth<=0)
	{
		OnKilled();
		m_dHealth=0;
	}
	m_dLastFrameDamage+=dDamage;
}

void CEntityBase::Kill()
{
	m_dHealth=0;
	OnKilled();
}

void         CEntityBase::OnKilledInternal(bool bRemove)
{
	SEntityTypeConfig config;
	m_pTypeBase->GetEntityTypeConfig(&config);
	if(config.nPoints){GivePoints(config.nPoints);}

	if(m_vChildren.size())
	{
		vector<SChildEntity> sTemp=m_vChildren;
		for(unsigned int x=0;x<sTemp.size();x++)
		{
			SChildEntity *pChildEntity=&sTemp[x];
			pChildEntity->piEntity->Kill();
			UNSUBSCRIBE_FROM_CAST(pChildEntity->piEntity,IEntityEvents);
		}
		sTemp.clear();
	}
	
	NOTIFY_EVENT(IEntityEvents,OnKilled(this));
	if(bRemove){Remove();}
	
}
void         CEntityBase::OnKilled(){OnKilledInternal(true);}
bool         CEntityBase::OnCollision(IEntity *pOther,CVector &vCollisionPos){return true;}
void         CEntityBase::Remove()
{
	m_bRemoved=true;
	SetState(ENTITY_STATE_INVALID,ANIMATION_INVALID);
	NOTIFY_EVENT(IEntityEvents,OnRemoved(this));
	
	if(m_vChildren.size())
	{
		vector<SChildEntity> sTemp=m_vChildren;
		for(unsigned int x=0;x<sTemp.size();x++)
		{
			SChildEntity *pChildEntity=&sTemp[x];
			pChildEntity->piEntity->Remove();
			UNSUBSCRIBE_FROM_CAST(pChildEntity->piEntity,IEntityEvents);
		}
		sTemp.clear();
	}
	if(m_piParent){m_piParent->RemoveChild(this);}
}

bool         CEntityBase::IsRemoved(){return m_bRemoved;}
void         CEntityBase::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	m_dLastFrameDamage=0;
}

SPhysicInfo *CEntityBase::GetPhysicInfo(){return &m_PhysicInfo;}
string      *CEntityBase::GetEntityClass(){return &m_sClassName;}
string      *CEntityBase::GetEntityName(){return &m_sName;}
unsigned int        CEntityBase::GetNextProcessFrame(){return m_dwNextProcessFrame;}
unsigned int        CEntityBase::GetAlignment(){return m_dwAlignment;}
void         CEntityBase::SetAlignment(unsigned int dwAlignment){m_dwAlignment=dwAlignment;}
double       CEntityBase::GetHealth(){return m_dHealth;}
double       CEntityBase::GetMaxHealth(){return m_dMaxHealth;}
void		 CEntityBase::SetHealth(double dHealth){m_dHealth=dHealth;}
void		 CEntityBase::SetMaxHealth(double dMaxHealth){m_dMaxHealth=dMaxHealth;}
// Animaciones

unsigned int  CEntityBase::GetState(){return m_nCurrentState;}

void CEntityBase::SetState(unsigned int nState,unsigned int nAnimation)
{
	if(nState==m_nCurrentState && nAnimation==ANIMATION_RANDOM){return;}
	if(nState==m_nCurrentState && m_nCurrentStateAnimation==nAnimation){return;}
	
	m_nCurrentState=nState;
	m_nCurrentStateAnimation=(nState==ENTITY_STATE_INVALID)?ANIMATION_INVALID:nAnimation;
	
	for(unsigned int x=0;x<m_vActiveAnimations.size();x++)
	{
		IAnimation *piAnimation=m_vActiveAnimations[x];
		if(piAnimation && !piAnimation->HasFinished())
		{
			piAnimation->Deactivate();
		}
	}
	if(nState!=ENTITY_STATE_INVALID)
	{
		unsigned long nTime=g_FrameManagerSingleton.m_piInterface->GetCurrentTime();
		IAnimation *piAnimation=m_pTypeBase->CreateStateAnimation(this,nState,nAnimation,nTime);
		if(piAnimation)
		{
			piAnimation->Activate(nTime);
			m_vActiveAnimations.push_back(piAnimation);
		}
	}
}

void CEntityBase::ProcessAnimations(unsigned int dwCurrentTime,double dTimeFraction,bool *pbAnimationsFinished)
{
    (*pbAnimationsFinished)=true;
	std::vector<IAnimation*>::iterator i;
	for(i=m_vActiveAnimations.begin();i!=m_vActiveAnimations.end();)
    {
        IAnimation *piAnimation=*i;
        if(piAnimation->HasFinished())
		{
			i=m_vActiveAnimations.erase(i);
			delete piAnimation;
		}
		else
        {
            (*pbAnimationsFinished)=false;
            piAnimation->ProcessFrame(g_PhysicManagerSingleton.m_piInterface,dwCurrentTime,dTimeFraction);
			i++;
        }
    }
    m_dwNextProcessFrame=dwCurrentTime+1;
}

void CEntityBase::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
    unsigned x;
	for(x=0;x<m_vActiveAnimations.size();x++)
    {
		IAnimation *piAnimation=m_vActiveAnimations[x];
        if(piAnimation && !piAnimation->HasFinished())
        {
            piAnimation->CustomRender(piRender,piCamera);
        }
    }
}

void CEntityBase::OnAnimationEvent(string sEvent,string sParams)
{
}

void CEntityBase::SetRoute(IRoute *piRoute){m_piRoute=piRoute;}
void CEntityBase::SetRouteDelay(unsigned int nDelay){m_nRouteDelay=nDelay;}
IRoute *CEntityBase::GetRoute(){return m_piRoute;}
bool CEntityBase::HasFinishedRoute(){return true;}


unsigned int CEntityBase::AddWeapon(IWeapon *piWeapon)
{
	unsigned int nIndex=m_vWeapons.size();
	m_vWeapons.push_back(piWeapon);
	return nIndex;
}

unsigned int  CEntityBase::GetWeapons()
{
	return m_vWeapons.size();
}

IWeapon *CEntityBase::GetWeapon(unsigned int nIndex)
{
	if(nIndex>=m_vWeapons.size()){return NULL;}
	return m_vWeapons[nIndex];
}


void CEntityBase::FireWeapon(unsigned int dwWeaponSlot,unsigned int dwCurrentTime)
{
  size_t x;
  for(x=0;x<m_vWeapons.size();x++)
  {
    IWeapon *piWeapon=m_vWeapons[x];
    if(piWeapon->GetSlot()==dwWeaponSlot)
    {
      piWeapon->Fire(dwCurrentTime);
    }
  }
}

IEntityManager *CEntityBase::GetEntityManager(){return g_EntityManagerSingleton.m_piInterface;}
IEntity *CEntityBase::GetTarget(){return m_piTarget;}
void CEntityBase::SetTarget(IEntity *piTarget)
{
	IEntity *piOldTarget=m_piTarget;
	m_piTarget=piTarget;
	if(piOldTarget!=m_piTarget)
	{
		for(unsigned int x=0;x<m_vChildren.size();x++)
		{
			m_vChildren[x].piEntity->SetTarget(m_piTarget);
		}
	}
}

unsigned int CEntityBase::GetDamageType(){return m_dwDamageType;}
void 		 CEntityBase::SetDamageType(unsigned int dwDamageType){m_dwDamageType=dwDamageType;}

CTraceInfo CEntityBase::GetTrace(const  CVector &p1,const CVector &p2 )
{
	CTraceInfo traceInfo;
	traceInfo.m_dTraceFraction=1.0;
	traceInfo.m_vTracePos=p2;
	
	for(unsigned int x=0;x<m_vActiveAnimations.size();x++)
	{
		IAnimation *piAnimation=m_vActiveAnimations[x];
		if(piAnimation && !piAnimation->HasFinished())
		{
			CTraceInfo tempTraceInfo;
			tempTraceInfo=piAnimation->GetTrace(m_PhysicInfo.vPosition,m_PhysicInfo.vAngles,p1,p2);
			if(tempTraceInfo.m_bTraceHit && tempTraceInfo.m_dTraceFraction<traceInfo.m_dTraceFraction)
			{
				traceInfo=tempTraceInfo;
			}
		}
	}	
	return traceInfo;
}

void CEntityBase::SetEntityTypeBase(CEntityTypeBase *pTypeBase)
{
	m_pTypeBase=pTypeBase;
}

IEntity *CEntityBase::GetParent(){return m_piParent;}
void CEntityBase::SetParent(IEntity *piEntity){m_piParent=piEntity;}

void CEntityBase::AddChild(IEntity *piEntity,CVector vPosition,CVector vAngles)
{
	bool bFound=false;
	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		if(m_vChildren[x].piEntity==piEntity){bFound=true;break;}
	}
	if(!bFound)
	{
		SChildEntity childEntity;
		childEntity.piEntity=piEntity;
		childEntity.vPosition=vPosition;
		childEntity.vAngles=vAngles;
		childEntity.nId=m_nNextChildId;
		m_nNextChildId++;
		
		m_vChildren.push_back(childEntity);
		piEntity->SetParent(this);
		SUBSCRIBE_TO_CAST(piEntity,IEntityEvents);
	}
}
	
void CEntityBase::RemoveChild(IEntity *piEntity)
{
	vector<SChildEntity>::iterator i;
	for(i=m_vChildren.begin();i!=m_vChildren.end();i++)
	{
		if(i->piEntity==piEntity)
		{
			UNSUBSCRIBE_FROM_CAST(piEntity,IEntityEvents);
			piEntity->SetParent(NULL);
			m_vChildren.erase(i);
			break;
		}
	}
}

unsigned int CEntityBase::GetChildren()
{
	return m_vChildren.size();
}

IEntity *CEntityBase::GetChild(unsigned int nIndex)
{
	if(nIndex>=m_vChildren.size()){return NULL;}
	return m_vChildren[nIndex].piEntity;
}

void CEntityBase::SetChildLocation(IEntity *piEntity,CVector vPosition,CVector vAngles)
{
	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		SChildEntity *pChildEntity=&m_vChildren[x];
		if(pChildEntity->piEntity==piEntity)
		{
			pChildEntity->vPosition=vPosition;
			pChildEntity->vAngles=vAngles;
			break;
		}
	}
}

void CEntityBase::GetChildLocation(IEntity *piEntity,CVector &vPosition,CVector &vAngles)
{
	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		SChildEntity *pChildEntity=&m_vChildren[x];
		if(pChildEntity->piEntity==piEntity)
		{
			vPosition=pChildEntity->vPosition;
			vAngles=pChildEntity->vAngles;
			break;
		}
	}
}

void CEntityBase::GivePoints( unsigned int nPoints )
{
	IPlayer *piPlayer=NULL;
	IEntity *piPlayerEntity=NULL;
	IEntityManager *piManager=GetEntityManager();
	if(piManager){piPlayerEntity=piManager->FindEntity("Player");}
	if(piPlayerEntity){piPlayer=dynamic_cast<IPlayer*>(piPlayerEntity);}
	if(piPlayer){piPlayer->AddPoints(nPoints);}
}

void CEntityBase::OnKilled(IEntity *piEntity)
{
	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		SChildEntity *pChildEntity=&m_vChildren[x];
		if(pChildEntity->piEntity==piEntity)
		{
			NOTIFY_EVENT(IEntityEvents,OnChildKilled(this,(int)pChildEntity->nId,piEntity));
		}
	}
}

void CEntityBase::OnRemoved(IEntity *piEntity)
{
	for(unsigned int x=0;x<m_vChildren.size();x++)
	{
		SChildEntity *pChildEntity=&m_vChildren[x];
		if(pChildEntity->piEntity==piEntity)
		{
			NOTIFY_EVENT(IEntityEvents,OnChildRemoved(this,(int)pChildEntity->nId,piEntity));
		}
	}
}
