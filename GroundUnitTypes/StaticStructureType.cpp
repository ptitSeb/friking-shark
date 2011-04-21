#include "./stdafx.h"
#include "StaticStructureType.h"

CStaticStructureType::CStaticStructureType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	
}

CStaticStructureType::~CStaticStructureType()
{
}

IEntity *CStaticStructureType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CStaticStructure *piEntity=new CStaticStructure(this);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CStaticStructureType::InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(eStaticStructureState_Normal);
}

CStaticStructure::CStaticStructure(CStaticStructureType *pType)
{
	m_sClassName="CStaticStructure";
	m_pType=pType;
	
	SEntityTypeConfig sconfig;
	m_pType->GetEntityTypeConfig(&sconfig);
	m_nConfiguredDamageType=sconfig.nDamageType;
}

void CStaticStructure::OnKilled()
{
	bool bRemove=false;
	m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
	
	if(m_pTypeBase->GetStateAnimations(eStaticStructureState_Destroyed))
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
		
		SetState(eStaticStructureState_Destroyed);
	}
	else
	{
		bRemove=true;
	}
	CEntityBase::OnKilledInternal(bRemove);
}

void CStaticStructure::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

	bool bAllChildDead=true;
	for(unsigned int x=0;x<m_vChildren.size();x++){if(m_vChildren[x].piEntity->GetHealth()!=0){bAllChildDead=false;}}
	m_dwDamageType=(bAllChildDead?m_nConfiguredDamageType:DAMAGE_TYPE_NONE);
	
	if(m_dwAlignment==ENTITY_ALIGNMENT_ENEMIES)
	{
		GetTarget();
	}
}

IEntity *CStaticStructure::GetTarget()
{
	if(m_piTarget==NULL)
	{
		IEntityManager *piManager=GetEntityManager();
		if(piManager){SetTarget(piManager->FindEntity("Player"));}
	}
	
	return m_piTarget;
}
