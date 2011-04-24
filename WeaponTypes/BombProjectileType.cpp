#include "./stdafx.h"
#include "BombProjectileType.h"

struct SBombDamageData
{
	double dTimeFraction;
	double dDamage;
	double dRadius;
	
	SBombDamageData(){dTimeFraction=0;dDamage=0;dRadius=0;}
};

CBombProjectileType::CBombProjectileType()
{
   m_nTimeToExplode=400;
   m_nCollisionType=PHYSIC_COLLISION_TYPE_NONE;
   m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
   m_dDamagePerSecond=0;
   m_dDamageStartRadius=0;
   m_dDamageEndRadius=0;
   m_nDamageStartTime=0;
   m_nDamageEndTime=0;
}

CBombProjectileType::~CBombProjectileType()
{
}

IEntity *CBombProjectileType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CBombProjectile *piEntity=new CBombProjectile(this,piParent);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetAlignment(piParent->GetAlignment());
  piEntity->SetState(eBombState_Normal);
  return piEntity;
}

CBombProjectile::CBombProjectile(CBombProjectileType *pType,IEntity *piParent)
{
  m_nCurrentTime=0;
  m_sClassName="CBombProjectile";
  m_sName="BombProjectile";
  m_pType=pType;
  m_piParent=piParent;
}

CBombProjectile::~CBombProjectile()
{
}

void CBombProjectile::ApplyDamageOperation(IEntity *piEntity,void *pParam1,void *pParam2)
{
	CBombProjectile *pThis=(CBombProjectile *)pParam1;
	SBombDamageData *pDamageData=((SBombDamageData*)pParam2);
	
	if(pThis->m_piParent==NULL){return;}
	if(piEntity->IsRemoved()){return;}
	if(piEntity->GetAlignment()==pThis->m_dwAlignment){return;}
	if(piEntity->GetAlignment()==ENTITY_ALIGNMENT_NEUTRAL ){return;}
	if(piEntity->GetDamageType()==DAMAGE_TYPE_NONE){return;}
	if(piEntity->GetHealth()<=0.0){return;}
	
	CVector vTemp=piEntity->GetPhysicInfo()->vPosition;
	vTemp.c[1]=pThis->m_PhysicInfo.vPosition.c[1];
	CVector vDist=vTemp-pThis->m_PhysicInfo.vPosition;
	double dDist=vDist;
	if(dDist<pDamageData->dRadius)
	{
		piEntity->OnDamage(pDamageData->dDamage,pThis);
	}
}

void CBombProjectile::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	m_nCurrentTime=dwCurrentTime;
	if(GetState()==eBombState_Normal)
	{
		if(dwCurrentTime>(m_dwCreationTime+m_pType->m_nTimeToExplode))
		{
			if(m_pTypeBase->GetStateAnimations(eBombState_Hit))
			{
				m_PhysicInfo.vVelocity=Origin;
				m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
				
				SetState(eBombState_Hit);
			}
			else
			{
				Remove();
			}
		}
	}
	else if(GetState()==eBombState_Hit)
	{
		if(dwCurrentTime>(m_dwCreationTime+m_pType->m_nTimeToExplode+m_pType->m_nDamageEndTime))
		{
			Remove();
		}
		else if(dwCurrentTime>(m_dwCreationTime+m_pType->m_nTimeToExplode+m_pType->m_nDamageStartTime))
		{
			double dElapsedTime=((double)(dwCurrentTime-(m_dwCreationTime+m_pType->m_nTimeToExplode+m_pType->m_nDamageStartTime)));
			double dTotalDuration=((double)(m_pType->m_nDamageEndTime-m_pType->m_nDamageStartTime));
			double dElapsedFraction=dElapsedTime/dTotalDuration;
			
			SBombDamageData data;
			data.dTimeFraction=dTimeFraction;
			data.dDamage=m_pType->m_dDamagePerSecond*dTimeFraction;
			data.dRadius=(m_pType->m_dDamageEndRadius-m_pType->m_dDamageStartRadius)*dElapsedFraction+m_pType->m_dDamageStartRadius;
			
			GetEntityManager()->PerformUnaryOperation(ApplyDamageOperation,this,&data);
		}
	}
}







	
