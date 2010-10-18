#include "StdAfx.h"
#include ".\truckType.h"

CTruckType::CTruckType()
{
  m_dMaxHealth=0;
  m_dMaxSpeed=0;
  m_dMaxAngularSpeed=0;
}

CTruckType::~CTruckType()
{
}

IEntity *CTruckType::CreateInstance(IEntity *piParent,DWORD dwCurrentTime)
{
    CTruck *piEntity=new CTruck(this);
    InitializeEntity(piEntity,dwCurrentTime);
    piEntity->SetCurrentAnimation(0);
    return piEntity;
}

CTruck::CTruck(CTruckType *pType)
{
    m_sClassName="CTruck";
    m_pType=pType;
    m_dwDamageType=DAMAGE_TYPE_NORMAL;
    m_dMaxHealth=m_dHealth=pType->m_dMaxHealth;
}

void CTruck::OnKilled()
{
  bool bRemove=false;
  m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
  if(m_dAnimations.size()>1)
  {
    m_dwDamageType=DAMAGE_TYPE_NONE;
    m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
    m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
    m_PhysicInfo.dwCollisionType=PHYSIC_COLLISION_TYPE_STUCK;
    SetCurrentAnimation(1);
  }
  else
  {
    bRemove=true;
  }
  CEntityBase::OnKilledInternal(bRemove);
}