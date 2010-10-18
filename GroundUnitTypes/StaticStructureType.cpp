#include "StdAfx.h"
#include "StaticStructureType.h"

CStaticStructureType::CStaticStructureType()
{
	m_dMaxHealth=0;
}

CStaticStructureType::~CStaticStructureType()
{
}

IEntity *CStaticStructureType::CreateInstance(IEntity *piParent,DWORD dwCurrentTime)
{
	CStaticStructure *piEntity=new CStaticStructure(this);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CStaticStructureType::InitializeEntity( CEntityBase *piEntity,DWORD dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->GetPhysicInfo()->dwMoveType=PHYSIC_MOVE_TYPE_NONE;
	piEntity->SetCurrentAnimation(0);
}

CStaticStructure::CStaticStructure(CStaticStructureType *pType)
{
	m_sClassName="CStaticStructure";
	m_pType=pType;
	m_dwDamageType=DAMAGE_TYPE_NORMAL;
	m_dMaxHealth=m_dHealth=pType->m_dMaxHealth;
}

void CStaticStructure::OnKilled()
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
