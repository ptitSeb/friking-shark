#include "./stdafx.h"
#include "LiveBonusType.h"

CLiveBonusType::CLiveBonusType()
{
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
}

CLiveBonusType::~CLiveBonusType()
{
}

IEntity *CLiveBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CLiveBonus *piEntity=new CLiveBonus(this);
  SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
  InitializeEntity(piEntity,dwCurrentTime);
  pPhysicInfo->vPosition=piParent->GetPhysicInfo()->vPosition;
  pPhysicInfo->vAngleVelocity.c[0]=50.0;
  pPhysicInfo->vAngleVelocity.c[1]=150.0;
  piEntity->SetState(eLiveBonusState_Normal);
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
	SetState(eLiveBonusState_Taken);
    Remove();
  }
  return false;
}

