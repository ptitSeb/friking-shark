#include "./stdafx.h"
#include "TurretType.h"

CTurretType::CTurretType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	m_dMaxAngle=0;
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
	m_bTargetLocked=false;
	m_dwNextShotTime=0;
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
		CVector vLocalAngles=AnglesFromVector(vWorldDir);
		double dIdealYaw=vLocalAngles.c[YAW];
		m_PhysicInfo.vLocalAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vLocalAngles.c[YAW],dIdealYaw,180.0*dTimeFraction);
		if(m_pType->m_dMaxAngle>0)
		{
			if(dIdealYaw>180){dIdealYaw=360-dIdealYaw;}
			if(dIdealYaw>m_pType->m_dMaxAngle){dIdealYaw=m_pType->m_dMaxAngle;}
			if(dIdealYaw<0){dIdealYaw=0;}
			m_PhysicInfo.vLocalAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vLocalAngles.c[YAW],dIdealYaw,180.0*dTimeFraction);
		}
		m_bTargetLocked=std::max(fabs(vLocalAngles.c[YAW]-m_PhysicInfo.vLocalAngles.c[YAW]),fabs(vLocalAngles.c[PITCH]-m_PhysicInfo.vLocalAngles.c[PITCH]))<5.0;
	}
	if(m_bTargetLocked && dwCurrentTime>m_dwNextShotTime && m_vWeapons.size())
	{
		FireWeapon(0,dwCurrentTime);
		m_dwNextShotTime=dwCurrentTime+100;
	}
}