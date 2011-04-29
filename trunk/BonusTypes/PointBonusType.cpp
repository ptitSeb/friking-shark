#include "./stdafx.h"
#include "PointBonusType.h"

CPointBonusType::CPointBonusType()
{
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	m_nCollisionType=PHYSIC_COLLISION_TYPE_NONE;
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
	piEntity->SetState(ENTITY_STATE_BASE,ANIMATION_RANDOM);
    return piEntity;
}

CPointBonus::CPointBonus(CPointBonusType *pType)
{
  m_sClassName="CPointBonus";
  m_sName="PointBonus";
  m_pType=pType;
  m_dwDamageType=DAMAGE_TYPE_NONE;
  
  IPlayer *piPlayer=dynamic_cast<IPlayer*>(GetEntityManager()->FindEntity("Player"));
  if(piPlayer)
  {
	  SEntityTypeConfig sConfig;
	  m_pType->GetEntityTypeConfig(&sConfig);
	  if(sConfig.nPoints){piPlayer->AddPoints(sConfig.nPoints);}
	  Remove();
  }
}

void CPointBonus::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	Remove();
}





