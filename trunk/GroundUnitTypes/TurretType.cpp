#include "./stdafx.h"
#include "TurretType.h"

CTurretType::CTurretType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
}

CTurretType::~CTurretType()
{
}

IEntity *CTurretType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CTurret *piEntity=new CTurret(this);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CTurretType::InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(eTurretState_Normal);
}

CTurret::CTurret(CTurretType *pType)
{
	m_sClassName="CTurret";
	m_pType=pType;
}

void CTurret::OnKilled()
{
	bool bRemove=false;
	m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
	
	if(m_pTypeBase->GetStateAnimations(eTurretState_Destroyed))
	{
		m_dwDamageType=DAMAGE_TYPE_NONE;
		m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
		m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
		
		SetState(eTurretState_Destroyed);
	}
	else
	{
		bRemove=true;
	}
	CEntityBase::OnKilledInternal(bRemove);
}


void CTurret::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	if(m_piTarget && GetState()!=eTurretState_Destroyed)
	{
		CVector vWorldDir=m_piTarget->GetPhysicInfo()->vPosition-m_PhysicInfo.vPosition;
		CMatrix m;
		m.Ref(m_PhysicInfo.vRefSysX,m_PhysicInfo.vRefSysY,m_PhysicInfo.vRefSysZ);
		vWorldDir*=m;
		double dIdealYaw=AnglesFromVector(vWorldDir).c[YAW];
		m_PhysicInfo.vLocalAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vLocalAngles.c[YAW],dIdealYaw,180.0*dTimeFraction);
	}
}