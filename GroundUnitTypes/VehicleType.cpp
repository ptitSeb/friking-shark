#include "./stdafx.h"
#include "VehicleType.h"

CVehicleType::CVehicleType()
{
  PersistencyInitialize();
}

CVehicleType::~CVehicleType()
{
}

IEntity *CVehicleType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
    CVehicle *piEntity=new CVehicle(this);
    InitializeEntity(piEntity,dwCurrentTime);
    piEntity->SetState(eVehicleState_Normal);
	piEntity->GetPhysicInfo()->dMass=1;
	piEntity->GetPhysicInfo()->dwCollisionType=PHYSIC_COLLISION_TYPE_SLIDE;
    return piEntity;
}

CVehicle::CVehicle(CVehicleType *pType)
{
	m_piTarget=NULL;
    m_sClassName="CVehicle";
    m_pType=pType;
    m_dwDamageType=DAMAGE_TYPE_NORMAL;
	m_nRoutePoint=0;
	m_bRouteFinished=false;
}

void CVehicle::OnKilled()
{
  bool bRemove=false;
  m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
  if(m_pTypeBase->GetStateAnimations(eVehicleState_Destroyed))
  {
    m_dwDamageType=DAMAGE_TYPE_NONE;
    m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
    m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NONE;
    m_PhysicInfo.dwCollisionType=PHYSIC_COLLISION_TYPE_STUCK;
	
	SetState(eVehicleState_Destroyed);
  }
  else
  {
    bRemove=true;
  }
  
  CEntityBase::OnKilledInternal(bRemove);
}

void CVehicle::AcquireTarget()
{
	IEntity 		*piTarget=NULL;
	IEntityManager 	*piManager=GetEntityManager();
	if(piManager){piTarget=piManager->FindEntity("Player");}
	SetTarget(piTarget);
}

void CVehicle::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);

	
	if(m_piTarget==NULL){AcquireTarget();}
	
	if(m_bRouteFinished)
	{
		double dVel=m_PhysicInfo.vVelocity.N();
		if(dVel!=0)
		{
			double dNewVel=dVel-m_PhysicInfo.dMaxVelocity*2.0*dTimeFraction;
			if(dNewVel<0){dNewVel=0;}
			m_PhysicInfo.vVelocity*=dNewVel;
		}
	}
	else if(m_piRoute)
	{
		CVector vDest,vDir;
		// Just follow the configured route
		CVector vForward,vRight,vUp;
		double dCurrentAngularSpeed=m_pType->m_dMaxAngularSpeed;
		VectorsFromAngles(m_PhysicInfo.vAngles.c[YAW],m_PhysicInfo.vAngles.c[PITCH],0,vForward,vRight,vUp);
		vDest=m_piRoute->GetAbsolutePoint(m_nRoutePoint);
		vDir=vDest-m_PhysicInfo.vPosition;
		double dDist=vDir.N();  
		
		bool bNext=false;
		int nNext=m_piRoute->GetNextPointIndex(m_nRoutePoint);
		if(nNext!=m_nRoutePoint)
		{
			CVector vDirNext=m_piRoute->GetAbsolutePoint(m_piRoute->GetNextPointIndex(nNext))-vDest;
			vDirNext.N();
			
			double dCirclePerimeter=(m_PhysicInfo.dMaxVelocity*360.0/m_pType->m_dMaxAngularSpeed);
			double dCapableRadius=(dCirclePerimeter/(2*PI));
			
			CVector vPerpB=vDirNext^vUp;
			CVector vPB1=vDest+vDirNext*dDist;
			CPlane  vPlaneA=CPlane(vDir,m_PhysicInfo.vPosition);
			
			double dSide1=vPlaneA.GetSide(vPB1);
			double dSide2=vPlaneA.GetSide(vPB1+vPerpB*10000.0);
			double dLength=(dSide1-dSide2);
			double dFraction=dLength?dSide1/dLength:0;
			double dFinalRadius=fabs(10000.0*dFraction);
			
			bNext=(dFinalRadius<dCapableRadius);
		}
		bNext=(bNext || dDist<m_PhysicInfo.dMaxVelocity*0.1);
		if(bNext)
		{
			if(nNext==m_nRoutePoint)
			{
				m_bRouteFinished=true;
			}
			else
			{
				m_nRoutePoint=nNext;
			}
		}
		// Head to the desired direction
		double dDesiredYaw=0,dDesiredPitch=0;
		AnglesFromVector(vDir,dDesiredYaw,dDesiredPitch);
		m_PhysicInfo.vAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vAngles.c[YAW],dDesiredYaw,-dCurrentAngularSpeed*dTimeFraction);
		VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vVelocity);
		m_PhysicInfo.vVelocity*=m_PhysicInfo.dMaxVelocity;
		m_dwNextProcessFrame=dwCurrentTime+10;
	}
}

void CVehicle::SetRoute( IRoute *piRoute )
{
	CEntityBase::SetRoute(piRoute);
}

bool CVehicle::HasFinishedRoute()
{
	return m_piRoute==NULL || m_bRouteFinished;
}
