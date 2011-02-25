#include "./stdafx.h"
#include "BulletProjectileType.h"

CBulletProjectileType::CBulletProjectileType()
{
  m_dDamage=1.0;
  m_dwDuration=1000;
}

CBulletProjectileType::~CBulletProjectileType(){}

IEntity *CBulletProjectileType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CBulletProjectile *piEntity=new CBulletProjectile(this,piParent);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetAlignment(piParent->GetAlignment());
  piEntity->SetCurrentAnimation(0);
  return piEntity;
}

CBulletProjectile::CBulletProjectile(CBulletProjectileType *pType,IEntity *piParent)
{
  m_sClassName="CBulletProjectile";
  m_sName="BulletProjectile";
  m_pType=pType;
  m_piParent=piParent;
  m_PhysicInfo.dwCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
  m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_BBOX;
  m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_FLY;
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

    if(m_dAnimations.size()>1)
    {
      SetCurrentAnimation(1);
    }
    Remove();
  }
  return false;
}

