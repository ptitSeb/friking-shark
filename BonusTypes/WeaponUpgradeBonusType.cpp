#include "StdAfx.h"
#include ".\WeaponUpgradeBonusType.h"

CWeaponUpgradeBonusType::CWeaponUpgradeBonusType(){m_dwLevels=0;m_dwSlot=0;}
CWeaponUpgradeBonusType::~CWeaponUpgradeBonusType(){}

IEntity *CWeaponUpgradeBonusType::CreateInstance(IEntity *piParent,DWORD dwCurrentTime)
{
  CWeaponUpgradeBonus *piEntity=new CWeaponUpgradeBonus(this);
  SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
  InitializeEntity(piEntity,dwCurrentTime);
  pPhysicInfo->dwMoveType=PHYSIC_MOVE_TYPE_NONE;
  pPhysicInfo->dwCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
  pPhysicInfo->dwBoundsType=PHYSIC_BOUNDS_TYPE_BBOX;
  pPhysicInfo->vPosition=piParent->GetPhysicInfo()->vPosition;
  pPhysicInfo->vAngleVelocity.c[0]=50.0;
  pPhysicInfo->vAngleVelocity.c[1]=150.0;
  piEntity->SetCurrentAnimation(0);
  return piEntity;
}

CWeaponUpgradeBonus::CWeaponUpgradeBonus(CWeaponUpgradeBonusType *pType)
{
  m_sClassName="CWeaponUpgradeBonus";
  m_sName="WeaponUpgradeBonus";
  m_pType=pType;
  m_dwDamageType=DAMAGE_TYPE_NONE;
}

bool CWeaponUpgradeBonus::OnCollision(IEntity *pOther,CVector &vCollisionPos)
{
  IPlayer *piPlayer=dynamic_cast<IPlayer*>(pOther);
  if(piPlayer)
  {
    vector<IWeapon*> vWeapons;
    piPlayer->GetWeaponsOnSlot(m_pType->m_dwSlot,&vWeapons);
    for(size_t x=0;x<vWeapons.size();x++)
    {
      IWeapon *piWeapon=vWeapons[x];
      piWeapon->SetCurrentLevel(piWeapon->GetCurrentLevel()+m_pType->m_dwLevels);
    }
    if(m_dAnimations.size()>1){SetCurrentAnimation(1);}
    Remove();
  }
  return false;
}
