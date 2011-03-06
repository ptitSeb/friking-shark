#include "./stdafx.h"
#include "PointBonusType.h"

CPointBonusType::CPointBonusType(){m_dwPoints=0;}
CPointBonusType::~CPointBonusType(){}

IEntity *CPointBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CPointBonus *piEntity=new CPointBonus(this);
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

CPointBonus::CPointBonus(CPointBonusType *pType)
{
  m_sClassName="CPointBonus";
  m_sName="PointBonus";
  m_pType=pType;
  m_dwDamageType=DAMAGE_TYPE_NONE;
}

bool CPointBonus::OnCollision(IEntity *pOther,CVector &vCollisionPos)
{
  IPlayer *piPlayer=dynamic_cast<IPlayer*>(pOther);
  if(piPlayer)
  {
    piPlayer->AddPoints(m_pType->m_dwPoints);
    if(m_dAnimations.size()>1){SetCurrentAnimation(1);}
    Remove();
  }
  return false;
}