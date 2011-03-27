#include "./stdafx.h"
#include "PointBonusType.h"

CPointBonusType::CPointBonusType()
{
	m_dwPoints=0;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_THROUGH;
}

CPointBonusType::~CPointBonusType()
{
	
}

IEntity *CPointBonusType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CPointBonus *piEntity=new CPointBonus(this);
  SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
  InitializeEntity(piEntity,dwCurrentTime);
  pPhysicInfo->vPosition=piParent?piParent->GetPhysicInfo()->vPosition:Origin;
  pPhysicInfo->vAngleVelocity.c[0]=50.0;
  pPhysicInfo->vAngleVelocity.c[1]=150.0;
  piEntity->SetState(ePointBonusState_Normal);
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
	SetState(ePointBonusState_Taken);
    Remove();
  }
  return false;
}