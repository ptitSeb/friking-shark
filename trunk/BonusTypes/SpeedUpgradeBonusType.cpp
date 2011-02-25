#include "./stdafx.h"
#include "SpeedUpgradeBonusType.h"

CSpeedUpgradeBonusType::CSpeedUpgradeBonusType(){m_dSpeed=0;}
CSpeedUpgradeBonusType::~CSpeedUpgradeBonusType(){}

IEntity *CSpeedUpgradeBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CSpeedUpgradeBonus *piEntity=new CSpeedUpgradeBonus(this);
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

CSpeedUpgradeBonus::CSpeedUpgradeBonus(CSpeedUpgradeBonusType *pType)
{
  m_sClassName="CSpeedUpgradeBonus";
  m_sName="SpeedUpgradeBonus";
  m_pType=pType;
  m_dwDamageType=DAMAGE_TYPE_NONE;
}

bool CSpeedUpgradeBonus::OnCollision(IEntity *pOther,CVector &vCollisionPos)
{
  IPlayer *piPlayer=dynamic_cast<IPlayer*>(pOther);
  if(piPlayer)
  {
    piPlayer->SetSpeed(piPlayer->GetSpeed()+m_pType->m_dSpeed);
    if(m_dAnimations.size()>1){SetCurrentAnimation(1);}
    Remove();
  }
  return false;
}

