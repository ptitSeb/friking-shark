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
#include "GroundBossType.h"
#include "GameGraphics.h"

CGroundBossType::CGroundBossType()
{
  PersistencyInitialize();
}

CGroundBossType::~CGroundBossType()
{
}

IEntity *CGroundBossType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
    CGroundBoss *piEntity=new CGroundBoss(this);
    InitializeEntity(piEntity,dwCurrentTime);
    piEntity->SetState(eGroundBossState_Normal,0);
	piEntity->GetPhysicInfo()->dMass=1;
	piEntity->GetPhysicInfo()->dwCollisionType=PHYSIC_COLLISION_TYPE_SLIDE;
    return piEntity;
}


void CGroundBossType::DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected)
{
        CEntityTypeBase::DesignRender(piRender,vPosition,m_bUseFixedAngles?m_vFixedAngles:vAngles,bSelected);
}
        
CTraceInfo CGroundBossType::DesignGetTrace(const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
        return CEntityTypeBase::DesignGetTrace(vPosition,m_bUseFixedAngles?m_vFixedAngles:vAngles,p1,p2);
}

CGroundBoss::CGroundBoss(CGroundBossType *pType)
{
    m_sClassName="CGroundBoss";
    m_pType=pType;
    m_dwDamageType=DAMAGE_TYPE_NORMAL;
	m_nRoutePoint=0;
	m_bRouteFinished=false;
	SEntityTypeConfig sconfig;
	m_pType->GetEntityTypeConfig(&sconfig);
	m_nConfiguredDamageType=sconfig.nDamageType;
	m_nPauseEnd=0;
	m_nNextCheckContainerBuilding=0;
	m_piContainerBuilding=NULL;
}

void CGroundBoss::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	//piRender->RenderBBox(m_PhysicInfo.vPosition,Origin,m_PhysicInfo.vMins,m_PhysicInfo.vMaxs,CVector(1,1,1),0x8888);
	
	CEntityBase::Render(piRender,piCamera);	
}

void CGroundBoss::OnRemoved(IEntity *piEntity)
{
	CEntityBase::OnRemoved(piEntity);
	
	if(piEntity==m_piTarget){SetTarget(NULL);}
	if(piEntity==m_piContainerBuilding)
	{
		UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
		m_piContainerBuilding=NULL;
	}
}

void CGroundBoss::OnKilled(IEntity *piEntity)
{
	CEntityBase::OnKilled(piEntity);
	
	if(piEntity==m_piTarget){SetTarget(NULL);}
	if(piEntity==m_piContainerBuilding)
	{
		UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
		m_piContainerBuilding=NULL;
	}
}

void CGroundBoss::OnKilled()
{
  bool bRemove=false;
  m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
  if(m_pTypeBase->GetStateAnimations(eGroundBossState_Destroyed))
  {
    m_dwDamageType=DAMAGE_TYPE_NONE;
    m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
    m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
    m_PhysicInfo.dwCollisionType=PHYSIC_COLLISION_TYPE_STUCK;
	
	SetState(eGroundBossState_Destroyed);
  }
  else
  {
    bRemove=true;
  }
  
  CEntityBase::OnKilledInternal(bRemove);

  for(unsigned int x=0;x<m_vChildren.size();x++){m_vChildren[x].piEntity->Remove();}
}

void CGroundBoss::AcquireTarget()
{
	IEntity 		*piTarget=NULL;
	IEntityManager 	*piManager=GetEntityManager();
	if(piManager){piTarget=piManager->FindEntity("Player");}
	SetTarget(piTarget);
}

bool CGroundBoss::IsInsideBuilding(IStaticStructure *piStaticStructure)
{
	const std::vector<SBBox> &vBBoxes=piStaticStructure->GetProtectiveRegions();
	if(vBBoxes.size()==0){return false;}
	
	
	CVector vFake1,vFake2;
	CVector vForward,vRight,vUp;
	ComputeReferenceSystem(piStaticStructure->GetPhysicInfo()->vPosition,piStaticStructure->GetPhysicInfo()->vAngles,Origin,Origin,&vFake1,&vFake2,&vForward,&vUp,&vRight);
	
	CVector vMyCenter=m_PhysicInfo.vPosition;
	vMyCenter-=piStaticStructure->GetPhysicInfo()->vPosition;
	CMatrix m;
	m.Ref(vForward,vUp,vRight);
	vMyCenter*=m;
	
	bool bInside=false;
	for(unsigned int b=0;!bInside && b<vBBoxes.size();b++)
	{
		bInside=true;
		for(int c=0;c<3;c++)
		{
			if(vMyCenter.c[c]<vBBoxes[b].vMins.c[c]){bInside=false;break;}
			if(vMyCenter.c[c]>vBBoxes[b].vMaxs.c[c]){bInside=false;break;}
		}
	}
	return bInside;
}

void CGroundBoss::FindBuilding(IEntity *piEntity,void *pParam1,void *pParam2)
{
	CGroundBoss *pThis=(CGroundBoss *)pParam1;
	if(pThis->m_piContainerBuilding){return;}
	if(piEntity->IsRemoved()){return;}
	if(*piEntity->GetEntityClass()!="CStaticStructure"){return;}
	
	IStaticStructure *piStructure=dynamic_cast<IStaticStructure*>(piEntity);
	if(piStructure==NULL){return;}
	
	bool bInside=pThis->IsInsideBuilding(piStructure);
	if(bInside){pThis->m_piContainerBuilding=piStructure;}
}

void CGroundBoss::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
		
	if(GetState()==eGroundBossState_Destroyed)
	{
		if(m_vActiveAnimations.size()==0){Remove();}
		return;
	}
	
	if(GetState()==eGroundBossState_Normal)
	{
		size_t nAnimationToSet=0;
		double dMaxHealth=GetMaxHealth();
		unsigned int nAnimations=m_pTypeBase->GetStateAnimations(ENTITY_STATE_BASE);
		nAnimationToSet=(size_t)(((dMaxHealth-m_dHealth)/dMaxHealth)*((double)nAnimations));
		if(nAnimationToSet>nAnimations-1){nAnimationToSet=nAnimations-1;}
		SetState(ENTITY_STATE_BASE,(int)nAnimationToSet);
	}
	
	if(m_nNextCheckContainerBuilding<dwCurrentTime)
	{
		if(m_piContainerBuilding)
		{
			if(!IsInsideBuilding(m_piContainerBuilding))
			{
				UNSUBSCRIBE_FROM_CAST(m_piContainerBuilding,IEntityEvents);
				m_piContainerBuilding=NULL;			
			}
		}
		else
		{
			m_piContainerBuilding=NULL;
			GetEntityManager()->PerformUnaryOperation(FindBuilding,this,0);
			if(m_piContainerBuilding){SUBSCRIBE_TO_CAST(m_piContainerBuilding,IEntityEvents);}
		}
		m_nNextCheckContainerBuilding=dwCurrentTime+100;
	}
	
	if(m_piContainerBuilding)
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
	}
	else
	{
		bool bAllChildDead=true;
		for(unsigned int x=0;x<m_vChildren.size();x++){if(m_vChildren[x].piEntity->GetHealth()>0){bAllChildDead=false;}}
		m_dwDamageType=(bAllChildDead?m_nConfiguredDamageType:DAMAGE_TYPE_NONE);
		if(m_piTarget==NULL){AcquireTarget();}
	}
		
	if(m_bRouteFinished || (dwCurrentTime<m_nPauseEnd))
	{
		double dVel=m_PhysicInfo.vVelocity.N();
		if(dVel!=0)
		{
			double dNewVel=dVel-m_PhysicInfo.dMaxVelocity*2.0*dTimeFraction;
			if(dNewVel<0){dNewVel=0;}
			m_PhysicInfo.vVelocity*=dNewVel;
		}
	}
	else if(m_piRoute && dwCurrentTime>=(m_dwCreationTime+m_nRouteDelay))
	{
		CVector vDest,vDir;
		// Just follow the configured route
		vDest=m_piRoute->GetAbsolutePoint(m_nRoutePoint);
		vDir=vDest-m_PhysicInfo.vPosition;
		double dDist=vDir.N();
		
		bool bNext=false;
		int nNext=m_piRoute->GetNextPointIndex(m_nRoutePoint);
		bNext=(dDist<m_PhysicInfo.dMaxVelocity*0.1);
		if(bNext)
		{
			if(nNext==m_nRoutePoint)
			{
				m_bRouteFinished=true;
			}
			else
			{
				SRoutePoint sPoint;
				m_piRoute->GetPoint(m_nRoutePoint,&sPoint);
				m_nPauseEnd=sPoint.nPause?dwCurrentTime+sPoint.nPause:0;
				m_nRoutePoint=nNext;
			}
		}
		
		SRoutePoint sCurrentPoint;
		m_piRoute->GetPoint(m_nRoutePoint,&sCurrentPoint);
		m_PhysicInfo.vVelocity=vDir;
		m_PhysicInfo.vVelocity*=m_PhysicInfo.dMaxVelocity*sCurrentPoint.dSpeedFactor;
		if(m_pType->m_bUseFixedAngles){m_PhysicInfo.vAngles=m_pType->m_vFixedAngles;}	
		m_dwNextProcessFrame=dwCurrentTime+10;
	}
}

void CGroundBoss::SetRoute( IRoute *piRoute )
{
	CEntityBase::SetRoute(piRoute);
}

bool CGroundBoss::HasFinishedRoute()
{
	return m_piRoute==NULL || m_bRouteFinished || m_dHealth==0;
}

