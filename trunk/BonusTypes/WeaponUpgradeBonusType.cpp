#include "./stdafx.h"
#include "WeaponUpgradeBonusType.h"

CWeaponUpgradeBonusType::CWeaponUpgradeBonusType()
{
	m_dwLevels=0;
	m_dwSlot=0;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
}

CWeaponUpgradeBonusType::~CWeaponUpgradeBonusType()
{
}

IEntity *CWeaponUpgradeBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CWeaponUpgradeBonus *piEntity=new CWeaponUpgradeBonus(this);
  SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
  InitializeEntity(piEntity,dwCurrentTime);
  pPhysicInfo->vPosition=piParent?piParent->GetPhysicInfo()->vPosition:Origin;
  pPhysicInfo->vAngleVelocity.c[0]=50.0;
  pPhysicInfo->vAngleVelocity.c[1]=150.0;
  piEntity->SetState(eWeaponUpgradeBonusState_Normal,ANIMATION_RANDOM);
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
    SetState(eWeaponUpgradeBonusState_Taken);
    Remove();
  }
  return false;
}
