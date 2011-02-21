#include "./stdafx.h"
#include "StaticEntityType.h"

CStaticEntityType::CStaticEntityType()
{
	m_dMaxHealth=0;
}

CStaticEntityType::~CStaticEntityType()
{
}

IEntity *CStaticEntityType::CreateInstance(IEntity *piParent,DWORD dwCurrentTime)
{
	CStaticEntity *piEntity=new CStaticEntity(this);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CStaticEntityType::InitializeEntity( CEntityBase *piEntity,DWORD dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->GetPhysicInfo()->dwMoveType=PHYSIC_MOVE_TYPE_NONE;
	piEntity->SetCurrentAnimation(0);
}

CStaticEntity::CStaticEntity(CStaticEntityType *pType)
{
	m_sClassName="CStaticEntity";
	m_pType=pType;
	m_dwDamageType=DAMAGE_TYPE_NORMAL;
	m_dMaxHealth=m_dHealth=pType->m_dMaxHealth;
}

void CStaticEntity::OnKilled()
{
	bool bRemove=false;
	m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
	if(m_dAnimations.size()>1)
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
		SetCurrentAnimation(1);
	}
	else
	{
		bRemove=true;
	}
	CEntityBase::OnKilledInternal(bRemove);
}
