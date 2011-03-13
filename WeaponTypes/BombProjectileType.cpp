#include "./stdafx.h"
#include "BombProjectileType.h"

CBombProjectileType::CBombProjectileType()
{
  m_dDamage=1.0;
  m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
  m_nMovementType=PHYSIC_MOVE_TYPE_NORMAL;
}

CBombProjectileType::~CBombProjectileType(){}

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
  m_sClassName="CBombProjectile";
  m_sName="BombProjectile";
  m_pType=pType;
  m_piParent=piParent;
}

bool CBombProjectile::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
  if(piOther->GetAlignment()!=m_dwAlignment && piOther->GetDamageType()!=DAMAGE_TYPE_NONE)
  {
    piOther->OnDamage(m_pType->m_dDamage,m_piParent);

	if(m_pTypeBase->GetStateAnimations(eBombState_Hit))
    {
		SetState(eBombState_Hit);
    }
    Remove();
  }
  return false;
}

