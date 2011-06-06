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
#include "BomberType.h"

CBomberType::CBomberType()
{
  m_nDamageType=DAMAGE_TYPE_NORMAL;
  m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
  PersistencyInitialize();
}

CBomberType::~CBomberType(){}

IEntity *CBomberType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CBomber *piEntity=new CBomber(this,dwCurrentTime);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetState(ENTITY_STATE_BASE,0);
  return piEntity;
}
void	CBomberType::DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected)
{
	CEntityTypeBase::DesignRender(piRender,vPosition,m_bUseFixedAngles?m_vFixedAngles:vAngles,bSelected);
}
	
CTraceInfo CBomberType::DesignGetTrace(const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 )
{
	return CEntityTypeBase::DesignGetTrace(vPosition,m_bUseFixedAngles?m_vFixedAngles:vAngles,p1,p2);
}

CBomber::CBomber(CBomberType *pType,unsigned int dwCurrentTime)
{
  m_sClassName="CBomber";
  m_pType=pType;
  m_dwNextProcessFrame=dwCurrentTime+100;
  m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeFirstShotMax-m_pType->m_dTimeFirstShotMin)+m_pType->m_dTimeFirstShotMin;
  m_nRoutePoint=0;
  m_bRouteFinished=false;
  m_nPauseEnd=0;
  m_dRadius=m_pType->DesignGetRadius();
  
}

void CBomber::OnKilled()
{
	bool bRemove=false;
	
	if(m_pTypeBase->GetStateAnimations(eBomberState_Destroyed))
	{
		if(GetState()!=eBomberState_Destroyed)
		{
			SetState(eBomberState_Destroyed);
		}
	}
	else
	{
		bRemove=true;
	}
	m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;

	CEntityBase::OnKilledInternal(bRemove);
}
void CBomber::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
  CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
 
  if(m_dHealth<=0){return;}
  if(m_piTarget==NULL){GetTarget();}
  
  size_t nAnimationToSet=0;
  double dMaxHealth=GetMaxHealth();
  if(dMaxHealth)
  {
	  unsigned int nAnimations=m_pTypeBase->GetStateAnimations(ENTITY_STATE_BASE);
	  nAnimationToSet=(size_t)(((dMaxHealth-m_dHealth)/dMaxHealth)*((double)nAnimations));
	  if(nAnimationToSet>nAnimations-1){nAnimationToSet=nAnimations-1;}
	  SetState(ENTITY_STATE_BASE,(int)nAnimationToSet);
  }
  
  if(m_bRouteFinished){Remove();return;}
  if(dwCurrentTime<m_nPauseEnd)
  {
	  double dVel=m_PhysicInfo.vVelocity.N();
	  if(dVel!=0)
	  {
		  double dNewVel=dVel-m_PhysicInfo.dMaxVelocity*2.0*dTimeFraction;
		  if(dNewVel<0){dNewVel=0;}
		  m_PhysicInfo.vVelocity*=dNewVel;
	  }
  }
  else if(m_piRoute)
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
			  m_piRoute->GetPoint(nNext,&sPoint);
			  m_nPauseEnd=sPoint.nPause?dwCurrentTime+sPoint.nPause:0;
			  m_nRoutePoint=nNext;
		  }
	  }
	  
	  m_PhysicInfo.vVelocity=vDir;
	  m_PhysicInfo.vVelocity*=m_PhysicInfo.dMaxVelocity;
	  if(m_pType->m_bUseFixedAngles){m_PhysicInfo.vAngles=m_pType->m_vFixedAngles;}
	  m_dwNextProcessFrame=dwCurrentTime+10;
  }
  
  if(m_piTarget && m_vWeapons.size() && dwCurrentTime>m_dwNextShotTime)
  {
	  bool bVisible=g_PlayAreaManagerWrapper.m_piInterface && g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,m_dRadius,true);
	  if(bVisible){FireWeapon(0,dwCurrentTime);}
	  m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeBetweenShotsMax-m_pType->m_dTimeBetweenShotsMin)+m_pType->m_dTimeBetweenShotsMin;
  }
}

IEntity *CBomber::GetTarget()
{
  if(m_piTarget==NULL)
  {
    IEntityManager *piManager=GetEntityManager();
    if(piManager)
    {
      IEntity *piTarget=piManager->FindEntity("Player");
	  if(piTarget && piTarget->GetHealth()>0)
	  {
			m_piTarget=piTarget;
			SUBSCRIBE_TO_CAST(m_piTarget,IEntityEvents);
	  }
    }
  }
  return m_piTarget;
}

bool CBomber::HasFinishedRoute()
{
	return m_piRoute==NULL || m_bRouteFinished || m_dHealth==0;
}

void CBomber::SetRoute(IRoute *piRoute)
{
	CEntityBase::SetRoute(piRoute);
	if(piRoute)
	{
		// Si hay ruta configurara se establece la velocidad maxima en la direccion de los primeros puntos
		// ya que el bombardero aplica fuerzas para moverse y le cuesta coger velocidad.
		m_PhysicInfo.vVelocity=m_piRoute->GetDirection(0)*m_PhysicInfo.dMaxVelocity;
	}
}
void CBomber::OnKilled(IEntity *piEntity)
{
	if(piEntity==m_piTarget){UNSUBSCRIBE_FROM_CAST(m_piTarget,IEntityEvents);m_piTarget=NULL;}
}

void CBomber::OnRemoved(IEntity *piEntity)
{
	if(piEntity==m_piTarget){UNSUBSCRIBE_FROM_CAST(m_piTarget,IEntityEvents);m_piTarget=NULL;}
}
