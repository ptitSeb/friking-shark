#include "./stdafx.h"
#include "BulletProjectileType.h"

CBulletProjectileType::CBulletProjectileType()
{
  m_dDamage=1.0;
  m_dwDuration=1000;
  m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
  m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
  
}

CBulletProjectileType::~CBulletProjectileType(){}

IEntity *CBulletProjectileType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CBulletProjectile *piEntity=new CBulletProjectile(this,piParent);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetAlignment(piParent->GetAlignment());
  piEntity->SetState(eBulletState_Normal);
  return piEntity;
}

CBulletProjectile::CBulletProjectile(CBulletProjectileType *pType,IEntity *piParent)
{
	m_dwNextTryAcquireTarget=0;
	m_sClassName="CBulletProjectile";
	m_sName="BulletProjectile";
	m_pType=pType;
	m_piParent=piParent;
	m_bTargetAcquired=false;
}

void CBulletProjectile::AcquireTargetOperation(IEntity *piEntity,void *pParam1,void *pParam2)
{
	CBulletProjectile *pThis=(CBulletProjectile *)pParam1;
	if(piEntity->IsRemoved()){return;}
	if(piEntity->GetAlignment()==pThis->m_dwAlignment){return;}
	if(piEntity->GetAlignment()==ENTITY_ALIGNMENT_NEUTRAL ){return;}

	if(piEntity->GetDamageType()!=DAMAGE_TYPE_NONE && piEntity->GetHealth()>0.0)
	{
		double dTargetDistance=pThis->m_PhysicInfo.vPosition-piEntity->GetPhysicInfo()->vPosition;
		double dMin=piEntity->GetPhysicInfo()->vMins.c[2]+piEntity->GetPhysicInfo()->vPosition.c[2];
		double dMax=piEntity->GetPhysicInfo()->vMaxs.c[2]+piEntity->GetPhysicInfo()->vPosition.c[2];
		if(  pThis->m_PhysicInfo.vPosition.c[0]<piEntity->GetPhysicInfo()->vMins.c[0] &&
		     pThis->m_PhysicInfo.vPosition.c[2]>=dMin && pThis->m_PhysicInfo.vPosition.c[2]<=dMax &&
			(pThis->m_bTargetAcquired==false || dTargetDistance<(pThis->m_PhysicInfo.vPosition-pThis->m_vTargetPosition)))
		{
			pThis->m_vTargetPosition=piEntity->GetPhysicInfo()->vPosition;
			pThis->m_vTargetPosition.c[2]=pThis->m_PhysicInfo.vPosition.c[2];
			pThis->m_bTargetAcquired=true;
		}
	}
}
void CBulletProjectile::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

	if((m_dwCreationTime+m_pType->m_dwDuration)<dwCurrentTime)
	{
		Remove();
	}
	else if(m_dwAlignment==ENTITY_ALIGNMENT_PLAYER && m_dwNextTryAcquireTarget<dwCurrentTime &&  !m_bTargetAcquired)
	{
		GetEntityManager()->PerformUnaryOperation(AcquireTargetOperation,this,NULL);
		if(m_bTargetAcquired)
		{
			CVector vDir=m_vTargetPosition-m_PhysicInfo.vPosition;
			vDir.N();
			double dVeloc=m_PhysicInfo.vVelocity;
			m_PhysicInfo.vVelocity=vDir*dVeloc;
		}
		else
		{
			m_dwNextTryAcquireTarget=dwCurrentTime+100;
		}
	}
}

bool CBulletProjectile::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
  if(piOther->GetAlignment()!=m_dwAlignment && piOther->GetDamageType()!=DAMAGE_TYPE_NONE && piOther->GetHealth()>0.0 )
  {
    piOther->OnDamage(m_pType->m_dDamage,m_piParent);

	if(m_pTypeBase->GetStateAnimations(eBulletState_Hit))
    {
		SetState(eBulletState_Hit);
    }
    Remove();
  }
  return false;
}

