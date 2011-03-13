#include "./stdafx.h"
#include "StaticEntityType.h"

CStaticEntityType::CStaticEntityType()
{
	m_dMaxHealth=0;
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
}

CStaticEntityType::~CStaticEntityType()
{
}

IEntity *CStaticEntityType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CStaticEntity *piEntity=new CStaticEntity(this);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CStaticEntityType::InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(eStaticEntityState_Normal);
}

CStaticEntity::CStaticEntity(CStaticEntityType *pType)
{
	m_sClassName="CStaticEntity";
	m_pType=pType;
	m_dMaxHealth=m_dHealth=pType->m_dMaxHealth;
}

void CStaticEntity::OnKilled()
{
	bool bRemove=false;
	m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
	
	if(m_pTypeBase->GetStateAnimations(eStaticEntityState_Destroyed))
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
		
		SetState(eStaticEntityState_Destroyed);
	}
	else
	{
		bRemove=true;
	}
	CEntityBase::OnKilledInternal(bRemove);
}
