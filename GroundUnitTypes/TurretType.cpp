#include "./stdafx.h"
#include "TurretType.h"

CTurretType::CTurretType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_NONE;
	
	PersistencyInitialize();
}

CTurretType::~CTurretType()
{
}

IEntity *CTurretType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
	CTurret *piEntity=new CTurret(this,dwCurrentTime);
	InitializeEntity(piEntity,dwCurrentTime);
	return piEntity;
}

void CTurretType::InitializeEntity( CEntityBase *piEntity,unsigned int dwCurrentTime )
{
	CEntityTypeBase::InitializeEntity(piEntity,dwCurrentTime);
	piEntity->SetState(eTurretState_Normal);
}

CTurret::CTurret(CTurretType *pType,unsigned int dwCurrentTime)
{
	m_bTargetLocked=false;
	m_sClassName="CTurret";
	m_pType=pType;
	m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeFirstShotMax-m_pType->m_dTimeFirstShotMin)+m_pType->m_dTimeFirstShotMin;
}

void CTurret::OnRemoved(IEntity *piEntity)
{
	if(piEntity==m_piTarget){SetTarget(NULL);}
}

void CTurret::OnKilled(IEntity *piEntity)
{
	if(piEntity==m_piTarget){SetTarget(NULL);}
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
void CTurret::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	CEntityBase::Render(piRender,piCamera);
}
void CTurret::SetTarget(IEntity *piTarget)
{
	if(m_piTarget){UNSUBSCRIBE_FROM_CAST(m_piTarget,IEntityEvents);}
	CEntityBase::SetTarget(piTarget);
	if(m_piTarget){SUBSCRIBE_TO_CAST(m_piTarget,IEntityEvents);}
}

void CTurret::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	if(GetState()==eTurretState_Destroyed){return;}
	if(m_piTarget)
	{
		CVector vTargetVel=m_piTarget->GetPhysicInfo()->vVelocity;
		CVector vTargetPos=m_piTarget->GetPhysicInfo()->vPosition;
		
		bool bTargetDir=false;
		CVector vWorldDir;
		if(m_vWeapons.size())
		{
			// Average of my weapons
			for(unsigned int y=0;y<m_vWeapons.size();y++)
			{			
				vWorldDir+=m_vWeapons[y]->GetIdealHeadingToTarget(vTargetPos,vTargetVel);
				bTargetDir=true;
			}
		}
		else
		{
			// Average of the children weapons
			for(unsigned int x=0;x<m_vChildren.size();x++)
			{
				for(unsigned int y=0;y<m_vChildren[x].piEntity->GetWeapons();y++)
				{
					vWorldDir+=m_vChildren[x].piEntity->GetWeapon(y)->GetIdealHeadingToTarget(vTargetPos,vTargetVel);
					bTargetDir=true;
				}
			}
		}
		if(!bTargetDir){vWorldDir=m_PhysicInfo.vPosition-vTargetPos;}
		
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
		double dYawDiff,dPitchDiff;
		dYawDiff=fabs(AngleDiff(vLocalAngles.c[YAW],m_PhysicInfo.vLocalAngles.c[YAW]));
		dPitchDiff=fabs(AngleDiff(vLocalAngles.c[PITCH]	,m_PhysicInfo.vLocalAngles.c[PITCH]));
		m_bTargetLocked=std::max(dYawDiff,dPitchDiff)<20.0;
	}
	else
	{
		m_PhysicInfo.vLocalAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vLocalAngles.c[YAW],0,180.0*dTimeFraction);
		m_bTargetLocked=false;
	}
	if(m_piTarget && m_bTargetLocked && dwCurrentTime>m_dwNextShotTime && m_vWeapons.size())
	{
		FireWeapon(0,dwCurrentTime);
		m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeBetweenShotsMax-m_pType->m_dTimeBetweenShotsMin)+m_pType->m_dTimeBetweenShotsMin;
	}
}