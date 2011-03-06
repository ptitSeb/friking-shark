#include "./stdafx.h"
#include "LiveBonusType.h"

CLiveBonusType::CLiveBonusType(){}
CLiveBonusType::~CLiveBonusType(){}

IEntity *CLiveBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CLiveBonus *piEntity=new CLiveBonus(this);
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

CLiveBonus::CLiveBonus(CLiveBonusType *pType)
{
  m_sClassName="CLiveBonus";
  m_sName="LiveBonus";
  m_pType=pType;
  m_dwDamageType=DAMAGE_TYPE_NONE;
}

bool CLiveBonus::OnCollision(IEntity *pOther,CVector &vCollisionPos)
{
  IPlayer *piPlayer=dynamic_cast<IPlayer*>(pOther);
  if(piPlayer)
  {
    piPlayer->AddLivesLeft(1);
    if(m_dAnimations.size()>1){SetCurrentAnimation(1);}
    Remove();
  }
  return false;
}

