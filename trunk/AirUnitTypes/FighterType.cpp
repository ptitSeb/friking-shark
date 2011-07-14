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
#include "FighterType.h"

CFighterType::CFighterType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
	g_PlayAreaManagerWrapper.AddRef();
	PersistencyInitialize();
}

CFighterType::~CFighterType()
{
	g_PlayAreaManagerWrapper.Release();	
}

IEntity *CFighterType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CFighter *piEntity=new CFighter(this,dwCurrentTime);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetState(eFighterState_Normal);
  return piEntity;
}

CFighter::CFighter(CFighterType *pType,unsigned int dwCurrentTime)
{
  m_sClassName="CFighter";
  m_bWasVisible=false;
  m_bFleeEnabled=false;
  m_dFleeAngle=0;
  m_bFleeing=false;
  m_pType=pType;
  m_nCurrentTime=dwCurrentTime;
  m_nRoutePoint=0;
  m_nFallStartTime=0;
  m_nFleeShadowKeepEndTime=0;
  m_dwNextProcessFrame=dwCurrentTime+10;
  m_dwNextShotTime=0;
  m_bFirstTimeVisible=true;
  m_dRadius=m_pType->DesignGetRadius();
}

void CFighter::AcquireTarget()
{
	IEntity 		*piTarget=NULL;
	IEntityManager 	*piManager=GetEntityManager();
	if(piManager){piTarget=piManager->FindEntity("Player");}

	if(piTarget)
	{
		
		SetTarget(piTarget);
		
		// Check if this entity will follow the player and flee instead of blindly follow the configured route
		if(m_pType->m_bHeadToTarget)
		{
			double dYaw=0,dPitch=0;
			CVector vDir=piTarget->GetPhysicInfo()->vPosition-m_PhysicInfo.vPosition;
			vDir.N();
			AnglesFromVector(vDir,dYaw,dPitch);
			m_PhysicInfo.vAngles.c[YAW]=dYaw;
		}
		// Check flee configuration and assign flee flag and angle
		if(m_pType->m_bFleeOnSameX || m_pType->m_bFleeOnSameZ)
		{
			CVector vTargetPos=m_piTarget->GetPhysicInfo()->vPosition;
			// Do not flee if the player appears just ahead
			if((m_PhysicInfo.vPosition.c[2]-m_dRadius)<=vTargetPos.c[2] &&
				(m_PhysicInfo.vPosition.c[2]+m_dRadius)>=vTargetPos.c[2])
			{
				m_bFleeEnabled=false;
			}
			else
			{
				m_bFleeEnabled=drand()<m_pType->m_dFleeProbability;
			}			
		}
	}
}

void CFighter::OnKilled()
{
  bool bRemove=false;
  if(m_pTypeBase->GetStateAnimations(eFighterState_Falling))
  {
	if(GetState()!=eFighterState_Falling)
    {
      m_PhysicInfo.vAngleVelocity.c[2]+=drand()*300.0-150.0;
	  SetState(eFighterState_Falling);
	  m_nFallStartTime=m_nCurrentTime;
      m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
    }
  }
  else
  {
    m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
    bRemove=true;
  }
  CEntityBase::OnKilledInternal(bRemove);
}

bool CFighter::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
	if(GetState()==eFighterState_Falling && piOther->GetPlacement()!=ENTITY_PLACEMENT_AIR)
	{
		if(GetState()!=eFighterState_Crashed && m_pTypeBase->GetStateAnimations(eFighterState_Crashed))
		{
			SetState(eFighterState_Crashed);
			m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
			m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
		}
		if(piOther->GetDamageType()!=DAMAGE_TYPE_NONE)
		{
			piOther->OnDamage(m_dMaxHealth,this);
		}
		
		Remove();
	}
	return false;
}

void CFighter::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	
	m_nCurrentTime=dwCurrentTime;
	
	if(m_dHealth<=0){return;}
	
	if(m_piTarget==NULL){AcquireTarget();}
	
	// When the entity is configured to head to the target or is feeing, the only way to
	// know if it has to be removed is to check if the plane is no longer in
	// the visible play area. The entity is removed some seconds after it exits the visible area 
	// just to avoid its shadow to suddenly vanish.

	if((m_pType->m_bHeadToTarget || m_bFleeing) && g_PlayAreaManagerWrapper.m_piInterface)
	{
		bool bVisible=g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,m_dRadius);
		// Reset the shadow keep counter in the rare case the plane is visible again
		if(bVisible){m_nFleeShadowKeepEndTime=0;}
		if(m_bWasVisible && !bVisible)
		{
			if(!m_nFleeShadowKeepEndTime)
			{
				m_nFleeShadowKeepEndTime=dwCurrentTime+m_pType->m_nFleeShadowKeepTime;
			}
		}
		if(m_nFleeShadowKeepEndTime && m_nFleeShadowKeepEndTime<dwCurrentTime)
		{
			Remove();
			return;
		}
		m_bWasVisible=bVisible;
	}

	// By default, continue in the current direction
	CVector vDir=m_PhysicInfo.vOwnX;
	CVector vDest=m_PhysicInfo.vPosition+vDir*100.0;
	double dCurrentAngularSpeed=m_pType->m_dMaxAngularSpeed;

	// Check if have to flee
	if(m_bFleeEnabled && !m_bFleeing)
	{
		if(m_piTarget)
		{
			CVector vTargetPos=m_piTarget->GetPhysicInfo()->vPosition;
			
			if(m_pType->m_bFleeOnSameZ &&
				(m_PhysicInfo.vPosition.c[2]-m_dRadius)<=vTargetPos.c[2] &&
				(m_PhysicInfo.vPosition.c[2]+m_dRadius)>=vTargetPos.c[2])
			{
				double dTempAngle=drand()*(m_pType->m_dMaxFleeAngle-m_pType->m_dMinFleeAngle)+m_pType->m_dMinFleeAngle;//35.0+20.0;
				m_dFleeAngle=(vTargetPos.c[2]-m_PhysicInfo.vPosition.c[2])>0?90.0+dTempAngle:270-dTempAngle;
				
				m_bFleeing=true;
				m_nFleeStartTime=dwCurrentTime;
			}
			
			if(m_pType->m_bFleeOnSameX &&
				(m_PhysicInfo.vPosition.c[0]-m_dRadius)<=vTargetPos.c[0] &&
				(m_PhysicInfo.vPosition.c[0]+m_dRadius)>=vTargetPos.c[0])
			{
				double dTempAngle=drand()*(m_pType->m_dMaxFleeAngle-m_pType->m_dMinFleeAngle)+m_pType->m_dMinFleeAngle;//35.0+20.0;
				m_dFleeAngle=(vTargetPos.c[2]-m_PhysicInfo.vPosition.c[2])>0?270.0-dTempAngle:90+dTempAngle;
				
				m_bFleeing=true;
			}
		}
	}
	
	if(m_bFleeing && m_nFleeStartTime+m_pType->m_nFleeDelay<=dwCurrentTime)
	{
		// If fleeing, just ensure we have the right heading
		CVector vFleeAngles=m_PhysicInfo.vAngles;
		vFleeAngles.c[YAW]=m_dFleeAngle;
		VectorsFromAngles(vFleeAngles,&vDir);
		vDest=m_PhysicInfo.vPosition+vDir*100.0;
	}
	else if(m_pType->m_bHeadToTarget && (m_dwCreationTime+m_pType->m_nHeadToTargetDelay)<=dwCurrentTime)
	{
		// If head to target is enabled and we have a head correction angle, try to head to the target at the configured angular speed 

		if(m_pType->m_dMaxHeadingCorrection!=0 && m_piTarget)
		{
			vDir=m_piTarget->GetPhysicInfo()->vPosition-m_PhysicInfo.vPosition;
			vDir.N();
			vDest=m_PhysicInfo.vPosition+vDir*100.0;
			dCurrentAngularSpeed=m_pType->m_dMaxHeadingCorrection;
		}
	}
	else if(m_piRoute)
	{
		// Just follow the configured route
		vDest=m_piRoute->GetAbsolutePoint(m_piRoute->GetNextPointIndex(m_nRoutePoint));
		vDir=vDest-m_PhysicInfo.vPosition;
		double dDist=vDir.N();  
		
		bool bNext=false;
		int nNext=m_piRoute->GetNextPointIndex(m_nRoutePoint);
		if(nNext!=m_nRoutePoint)
		{
			CVector vDirNext=m_piRoute->GetAbsolutePoint(m_piRoute->GetNextPointIndex(nNext))-vDest;
			vDirNext.N();
			
			double dCirclePerimeter=(m_PhysicInfo.dMaxVelocity*360.0/m_pType->m_dMaxAngularSpeed);
			double dCapableRadius=(dCirclePerimeter/(2*PI));
			
			CVector vPerpB=vDirNext^m_PhysicInfo.vOwnY;
			CVector vPB1=vDest+vDirNext*dDist;
			CPlane  vPlaneA=CPlane(vDir,m_PhysicInfo.vPosition);
			
			double dSide1=vPlaneA.GetSide(vPB1);
			double dSide2=vPlaneA.GetSide(vPB1+vPerpB*10000.0);
			double dLength=(dSide1-dSide2);
			double dFraction=dLength?dSide1/dLength:0;
			double dFinalRadius=fabs(10000.0*dFraction);

			bNext=(dFinalRadius<dCapableRadius);
		}
		bNext=(bNext || dDist<m_PhysicInfo.dMaxVelocity*0.1);
		if(bNext)
		{
			if(nNext==m_nRoutePoint)
			{
				Remove();
			}
			else
			{
				m_nRoutePoint=nNext;
			}
		}
	}

	// Apply the roll visual effect: Proportional to the heading diference to the target
	if(m_PhysicInfo.vOwnX!=vDir)
	{
		CPlane plane(m_PhysicInfo.vOwnZ,m_PhysicInfo.vPosition);
		if(plane.GetSide(vDest)>0)
		{
			m_PhysicInfo.vAngles.c[ROLL]+=m_pType->m_dMaxRoll*dTimeFraction;
			if(m_PhysicInfo.vAngles.c[ROLL]>m_pType->m_dMaxRoll){m_PhysicInfo.vAngles.c[ROLL]=m_pType->m_dMaxRoll;}
		}
		else
		{
			m_PhysicInfo.vAngles.c[ROLL]-=m_pType->m_dMaxRoll*dTimeFraction;
			if(m_PhysicInfo.vAngles.c[ROLL]<-m_pType->m_dMaxRoll){m_PhysicInfo.vAngles.c[ROLL]=-m_pType->m_dMaxRoll;}
		}
	}
	else
	{
		if(m_PhysicInfo.vAngles.c[ROLL]>0)
		{
			m_PhysicInfo.vAngles.c[ROLL]-=m_pType->m_dMaxAngularSpeed*dTimeFraction;
			if(m_PhysicInfo.vAngles.c[ROLL]<0){m_PhysicInfo.vAngles.c[ROLL]=0;}
		}
		else
		{
			m_PhysicInfo.vAngles.c[ROLL]+=m_pType->m_dMaxAngularSpeed*dTimeFraction;
			if(m_PhysicInfo.vAngles.c[ROLL]>0){m_PhysicInfo.vAngles.c[ROLL]=0;}
		}
	}	

	// Head to the desired direction
	double dDesiredYaw=0,dDesiredPitch=0;
	AnglesFromVector(vDir,dDesiredYaw,dDesiredPitch);
	m_PhysicInfo.vAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vAngles.c[YAW],dDesiredYaw,-dCurrentAngularSpeed*dTimeFraction);
	VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vVelocity);
	m_PhysicInfo.vVelocity*=m_PhysicInfo.dMaxVelocity;
	m_dwNextProcessFrame=dwCurrentTime+10;

	if(dwCurrentTime>m_dwNextShotTime )
	{
		bool bVisible=g_PlayAreaManagerWrapper.m_piInterface && g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,0);
		if(bVisible)
		{
			if(m_bFirstTimeVisible)
			{
				m_bFirstTimeVisible=false;
				m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeFirstShotMax-m_pType->m_dTimeFirstShotMin)+m_pType->m_dTimeFirstShotMin;;
			}
			else
			{
				for(unsigned int x=0;x<m_vWeapons.size();x++){FireWeapon(x,dwCurrentTime);}
				m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeBetweenShotsMax-m_pType->m_dTimeBetweenShotsMin)+m_pType->m_dTimeBetweenShotsMin;
			}			
		}
	}
}

void CFighter::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
  CEntityBase::Render(piRender,piCamera);
  
  // Render route
  /*
  if(m_piRoute)
  {
	piRender->PushState();
	piRender->DeactivateDepth();
	
	CVector vPreviousPoint=m_piRoute->GetAbsolutePoint(0);
	for(unsigned x=1;x<m_piRoute->GetPointCount();x++)
	{
		double dPointSize=15;
		CVector vPointColor=CVector(0,0,0.8);
		CVector vLineColor=CVector(0,0,0.8);
		
		
		CVector vWorld=m_piRoute->GetAbsolutePoint(x);
		piRender->RenderLine(vPreviousPoint,vWorld,vLineColor);
		piRender->RenderPoint(vWorld,dPointSize,vPointColor,1.0);
		vPreviousPoint=vWorld;
	
	}
	piRender->PopState();
  }*/
}

bool CFighter::HasFinishedRoute()
{
	return m_piRoute==NULL || ((int)m_piRoute->GetNextPointIndex(m_nRoutePoint)==m_nRoutePoint) || m_dHealth==0;
}

IEntity *CFighter::GetTarget()
{
	return m_piTarget;
}
 
