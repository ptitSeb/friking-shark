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
  m_sClassName="CBulletProjectile";
  m_sName="BulletProjectile";
  m_pType=pType;
  m_piParent=piParent;
}

void CBulletProjectile::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

  if((m_dwCreationTime+m_pType->m_dwDuration)<dwCurrentTime)
  {
    Remove();
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

