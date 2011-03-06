#include "./stdafx.h"
#include "TruckType.h"
#include "GameGraphics.h"

CTruckType::CTruckType()
{
  m_dMaxHealth=0;
  m_dMaxSpeed=0;
  m_dMaxAngularSpeed=0;
}

CTruckType::~CTruckType()
{
}

IEntity *CTruckType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
    CTruck *piEntity=new CTruck(this);
    InitializeEntity(piEntity,dwCurrentTime);
    piEntity->SetCurrentAnimation(0);
	piEntity->GetPhysicInfo()->dMaxVelocity=m_dMaxSpeed;
	piEntity->GetPhysicInfo()->dMaxForce=m_dMaxSpeed;
	piEntity->GetPhysicInfo()->dMass=1;
	piEntity->GetPhysicInfo()->dwCollisionType=PHYSIC_COLLISION_TYPE_SLIDE;
	piEntity->GetPhysicInfo()->dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
    return piEntity;
}

CTruck::CTruck(CTruckType *pType)
{
	m_piTarget=NULL;
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

void CTruck::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

	RTTRACE("On surface %d",m_PhysicInfo.bOnSurface);
	VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vForward);
	if(m_PhysicInfo.bOnSurface)
	{
		VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vForward);
		CVector vForce=m_Behaviours.ProcessBehaviours(this,dTimeFraction);
		m_PhysicInfo.fOwnForce.dForce=vForce.N();
		m_PhysicInfo.fOwnForce.vDir=vForce;
		m_PhysicInfo.fOwnForce.dwForceType=PHYSIC_FORCE_NORMAL;
		m_PhysicInfo.fOwnForce.dMaxVelocity=m_PhysicInfo.dMaxVelocity;
	}
	else
	{
		m_PhysicInfo.fOwnForce.vDir=Origin;
		m_PhysicInfo.fOwnForce.dForce=0;
		m_PhysicInfo.fOwnForce.dMaxVelocity=0;
	}
	m_PhysicInfo.vAngles=AnglesFromVector(m_PhysicInfo.vVelocity);
}

void CTruck::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
	CEntityBase::Render(piRender,piCamera);
	piRender->RenderLine(m_PhysicInfo.vPosition,m_PhysicInfo.vPosition+m_PhysicInfo.fOwnForce.vDir*m_PhysicInfo.fOwnForce.dForce,CVector(1,0,0),0xFFFF);
	piRender->RenderLine(m_PhysicInfo.vPosition,m_PhysicInfo.vPosition+m_PhysicInfo.vVelocity,CVector(0,1,0),0xFFFF);

	CVector vVelocityDir=m_PhysicInfo.vVelocity;
	vVelocityDir.N();
	piRender->RenderLine(m_PhysicInfo.vPosition,m_PhysicInfo.vPosition+vVelocityDir*m_PhysicInfo.dMaxVelocity,CVector(0,0.5,0));
}

void CTruck::SetRoute( IRoute *piRoute )
{
	CEntityBase::SetRoute(piRoute);
	m_Behaviours.FollowRoute(piRoute);
}