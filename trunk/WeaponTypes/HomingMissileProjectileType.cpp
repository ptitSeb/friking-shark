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
#include "HomingMissileProjectileType.h"

CHomingMissileProjectileType::CHomingMissileProjectileType()
{
  m_dDamage=1.0;
  m_dwDuration=1000;
  m_dwFallDuration=5000;
  m_dMaxSpeed=100;
  m_dMaxAngularSpeed=150;
  m_dwTimeBettwenAcquireTargetAttemps=300;
  m_dwTimeToWaitToAcquireTargets=500;
  m_dwMaximunTargetsToAcquire=0;
  
  m_nDamageType=DAMAGE_TYPE_NORMAL;
  m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
  m_nMovementType=PHYSIC_MOVE_TYPE_FLY;  
}

CHomingMissileProjectileType::~CHomingMissileProjectileType(){}

IEntity *CHomingMissileProjectileType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CHomingMissileProjectile *piEntity=new CHomingMissileProjectile(this,piParent);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetAlignment(piParent->GetAlignment());
  piEntity->SetState(eHomingMissileState_Normal);
  return piEntity;
}

CHomingMissileProjectile::CHomingMissileProjectile(CHomingMissileProjectileType *pType,IEntity *piParent)
{
  m_sClassName="CHomingMissileProjectile";
  m_sName="HomingMissileProjectile";
  m_pType=pType;
  m_piParent=piParent;
  m_piTarget=NULL;
  m_dwNextTimeToAcquireTarget=0;
  m_dwTargetsAcquired=0;
  m_dHealth=1;
}

double CustomApproachAngle(double actual,double ideal, double ammount) 
{
  double destRespectOrig,result;
  double addammount;
  addammount=0.0;
  destRespectOrig=ideal-actual;
  if(destRespectOrig<0.0){destRespectOrig=destRespectOrig+360.0;}

  if(destRespectOrig>=0.0 && destRespectOrig<180.0)
  {addammount=1.0;}else{addammount=0.0;}

  // if almost in perfect back make take a random direction (it gets nicer)
  if(destRespectOrig>(180.0-ammount/2.0) && destRespectOrig<(180.0+ammount/2.0))
  {
    if(drand()<=0.5){addammount=1;}else{addammount=0;}
  }
  if(addammount)
  {
    if(fabs(actual-ideal)<ammount)
    {result=ideal;}
    else
    {result=actual+ammount;}
  }
  else
  {
    if(fabs(actual-ideal)<ammount)
    {result=ideal;}
    else
    {result=actual-ammount;}
  }
  if(result<0.0){result=result+360.0;}
  if(result>360.0){result=result+(-360.0);}
  return result;
}

void CustomApproachAngles(CVector *pvActual,CVector vIdeal,double dAmmount)
{
  for(int c=0;c<2;c++)
  {
    pvActual->c[c]=CustomApproachAngle(pvActual->c[c],vIdeal.c[c],dAmmount);
  }
}

void CHomingMissileProjectile::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

  if((m_dwCreationTime+m_pType->m_dwDuration)<dwCurrentTime)
  {
	if(m_pTypeBase->GetStateAnimations(eHomingMissileState_OutOfFuel) && GetState()!=eHomingMissileState_OutOfFuel)
    {
      SetState(eHomingMissileState_OutOfFuel);
    }
    m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
    return;
  }

  if((m_dwCreationTime+m_pType->m_dwDuration+m_pType->m_dwFallDuration)<dwCurrentTime)
  {
    Remove();
    return;
  }
  /*if((m_dwCreationTime+m_pType->m_dwTimeToWaitToAcquireTargets)<dwCurrentTime)
  {
    return;
  }*/
 
  if(m_piTarget==NULL)
  {
    /*if(m_dwTargetsAcquired>=m_pType->m_dwMaximunTargetsToAcquire && m_pType->m_dwMaximunTargetsToAcquire!=0)
    {
      return;
    }*/
    // Try to acquire a new target if the specified ammount of time has passed.

    if(m_dwNextTimeToAcquireTarget<dwCurrentTime)
    {
      m_dwNextTimeToAcquireTarget=dwCurrentTime+(unsigned int)(m_pType->m_dwTimeBettwenAcquireTargetAttemps);
      m_dAcquireTarget_TempDistance=0;
      GetEntityManager()->PerformUnaryOperation(AcquireTargetOperation,this,NULL);
      if(m_piTarget)
      {
        m_dwTargetsAcquired++;
        SUBSCRIBE_TO_CAST(m_piTarget,IEntityEvents);
      }
    }
  }
  else
  {
    // Head Target
    CVector vTargetPos=m_piTarget->GetPhysicInfo()->vPosition;
    CVector vDir=vTargetPos-m_PhysicInfo.vPosition;
    CVector vDesiredAngles;
    AnglesFromVector(vDir,&vDesiredAngles);
    CustomApproachAngles(&m_PhysicInfo.vAngles,vDesiredAngles,m_pType->m_dMaxAngularSpeed*dTimeFraction);
    VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vVelocity);
    m_PhysicInfo.vVelocity*=m_pType->m_dMaxSpeed;
  }
}

void CHomingMissileProjectile::OnRemoved(IEntity *piEntity)
{
  if(piEntity==m_piTarget)
  {
    UNSUBSCRIBE_FROM_CAST(m_piTarget,IEntityEvents);
    m_piTarget=NULL;
  }
}
void CHomingMissileProjectile::OnKilled(IEntity *piEntity)
{
  if(piEntity==m_piTarget)
  {
    UNSUBSCRIBE_FROM_CAST(m_piTarget,IEntityEvents);
    m_piTarget=NULL;
  }
}

bool CHomingMissileProjectile::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
  if(piOther->GetAlignment()!=m_dwAlignment && piOther->GetHealth()>0.0 && piOther->GetDamageType()!=DAMAGE_TYPE_NONE)
  {
    piOther->OnDamage(m_pType->m_dDamage,m_piParent);

	if(m_pTypeBase->GetStateAnimations(eHomingMissileState_Hit))
	{
		SetState(eHomingMissileState_Hit);
	}
    Remove();
  }
  return false;
}

void CHomingMissileProjectile::AcquireTargetOperation(IEntity *piEntity,void *pParam1,void *pParam2)
{
  CHomingMissileProjectile *pThis=(CHomingMissileProjectile *)pParam1;
  if(!piEntity->IsRemoved() && piEntity->GetAlignment()!=pThis->m_dwAlignment && piEntity->GetAlignment()!=ENTITY_ALIGNMENT_NEUTRAL && piEntity->GetHealth()>0.0)
  {
    double dDistance=piEntity->GetPhysicInfo()->vPosition-pThis->m_PhysicInfo.vPosition;
    if(pThis->m_piTarget==NULL || dDistance<pThis->m_dAcquireTarget_TempDistance)
    {
      pThis->m_piTarget=piEntity;
      pThis->m_dAcquireTarget_TempDistance=dDistance;
    }
  }
}

void CHomingMissileProjectile::OnKilled()
{
	if(m_pTypeBase->GetStateAnimations(eHomingMissileState_Hit))
	{
		SetState(eHomingMissileState_Hit);
	}
	CEntityBase::OnKilled();
}
