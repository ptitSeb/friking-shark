#include "./stdafx.h"
#include "BombProjectileType.h"

CBombProjectileType::CBombProjectileType()
{
  m_dDamage=1.0;
}

CBombProjectileType::~CBombProjectileType(){}

IEntity *CBombProjectileType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CBombProjectile *piEntity=new CBombProjectile(this,piParent);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetAlignment(piParent->GetAlignment());
  piEntity->SetCurrentAnimation(0);
  return piEntity;
}

CBombProjectile::CBombProjectile(CBombProjectileType *pType,IEntity *piParent)
{
  m_sClassName="CBombProjectile";
  m_sName="BombProjectile";
  m_pType=pType;
  m_piParent=piParent;
  m_PhysicInfo.dwCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
  m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_BBOX;
  m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
}

bool CBombProjectile::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
  if(piOther->GetAlignment()!=m_dwAlignment && piOther->GetDamageType()!=DAMAGE_TYPE_NONE)
  {
    piOther->OnDamage(m_pType->m_dDamage,m_piParent);

    if(m_dAnimations.size()>1)
    {
      SetCurrentAnimation(1);
    }
    Remove();
  }
  return false;
}

