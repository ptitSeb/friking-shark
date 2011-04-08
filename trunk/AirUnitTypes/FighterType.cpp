#include "./stdafx.h"
#include "FighterType.h"

CFighterType::CFighterType()
{
	m_nDamageType=DAMAGE_TYPE_NORMAL;
	m_nMovementType=PHYSIC_MOVE_TYPE_FLY;
	g_PlayAreaManagerWrapper.AddRef();
	PersistencyInitialize();
}

CFighterType::~CFighterType()
{
	g_PlayAreaManagerWrapper.Release();	
}

IEntity *CFighterType::CreateInstance(IEntity *piParent,unsigned int dwCurrentTime)
{
  CFighter *piEntity=new CFighter(this,dwCurrentTime);
  InitializeEntity(piEntity,dwCurrentTime);
  piEntity->SetState(eFighterState_Normal);
  return piEntity;
}

CFighter::CFighter(CFighterType *pType,unsigned int dwCurrentTime)
{
  m_sClassName="CFighter";
  m_bWasVisible=false;
  m_bFleeEnabled=false;
  m_dFleeAngle=0;
  m_bFleeing=false;
  m_pType=pType;
  m_nRoutePoint=0;
  m_dwNextProcessFrame=dwCurrentTime+10;
  m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeFirstShotMax-m_pType->m_dTimeFirstShotMin)+m_pType->m_dTimeFirstShotMin;
}

void CFighter::AcquireTarget()
{
	IEntity 		*piTarget=NULL;
	IEntityManager 	*piManager=GetEntityManager();
	if(piManager){piTarget=piManager->FindEntity("Player");}

	// Check if this entity will follow the player and flee instead of blindly follow the configured route
	if(piTarget && m_pType->m_bHeadToTarget)
	{
		double dYaw=0,dPitch=0;
		CVector vDir=piTarget->GetPhysicInfo()->vPosition-m_PhysicInfo.vPosition;
		vDir.N();
		AnglesFromVector(vDir,dYaw,dPitch);
		m_PhysicInfo.vAngles.c[YAW]=dYaw;
		SetTarget(piTarget);
		
		// Check flee configuration and assign flee flag and angle
		if(m_pType->m_bFleeOnSameX || m_pType->m_bFleeOnSameZ)
		{
			CVector vTargetPos=m_piTarget->GetPhysicInfo()->vPosition;
			// Do not flee if the player appears just ahead
			if((m_PhysicInfo.vPosition.c[2]+m_PhysicInfo.vMins.c[2])<=vTargetPos.c[2] &&
			   (m_PhysicInfo.vPosition.c[2]+m_PhysicInfo.vMaxs.c[2])>=vTargetPos.c[2])
			{
				m_bFleeEnabled=false;
			}
			else
			{
				m_bFleeEnabled=true;//drand()>0.3;
			}			
		}
	}
}

void CFighter::OnKilled()
{
  bool bRemove=false;
  if(m_pTypeBase->GetStateAnimations(eFighterState_Falling))
  {
	if(GetState()!=eFighterState_Falling)
    {
      m_PhysicInfo.vAngleVelocity.c[2]+=drand()*300.0-150.0;
	  SetState(eFighterState_Falling);
      m_PhysicInfo.dwMoveType=PHYSIC_MOVE_TYPE_NORMAL;
    }
  }
  else
  {
    m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
    bRemove=true;
  }
  CEntityBase::OnKilledInternal(bRemove);
}

bool CFighter::OnCollision(IEntity *piOther,CVector &vCollisionPos)
{
	if(GetState()==eFighterState_Falling && piOther->GetAlignment()!=ENTITY_ALIGNMENT_PLAYER)
	{
		if(GetState()!=eFighterState_Crashed && m_pTypeBase->GetStateAnimations(eFighterState_Crashed))
		{
			SetState(eFighterState_Crashed);
		}
		Remove();
	} 
	else if(m_dHealth>0 && piOther->GetAlignment()!=m_dwAlignment)
	{
		piOther->OnDamage(m_dHealth,this);
	}
  return false;
}

void CFighter::ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction)
{
	CEntityBase::ProcessFrame(dwCurrentTime,dTimeFraction);
	
	if(m_dHealth<=0){return;}
	
	if(m_piTarget==NULL){AcquireTarget();}
	
	// When the entity is configured to head to the target, the only way to
	// know if it has to be removed is to check if the plane is no longer in
	// the visible play area (including scroll)

	if(m_pType->m_bHeadToTarget && g_PlayAreaManagerWrapper.m_piInterface)
	{
		double dRadius=((IEntityType*)m_pType)->DesignGetRadius();//GetBBoxRadius(m_PhysicInfo.vMins,m_PhysicInfo.vMaxs);
		bool bVisible=g_PlayAreaManagerWrapper.m_piInterface->IsVisible(m_PhysicInfo.vPosition,dRadius,true);
		if(m_bWasVisible && !bVisible)
		{
			Remove();
			return;
		}
		m_bWasVisible=bVisible;
	}

	// By default, continue in the current direction
	CVector vForward,vRight;
	VectorsFromAngles(m_PhysicInfo.vAngles,&vForward,&vRight);
	CVector vDir=vForward;
	CVector vDest=m_PhysicInfo.vPosition+vDir*100.0;
	double dCurrentAngularSpeed=m_pType->m_dMaxAngularSpeed;

	// Check if have to flee
	if(m_bFleeEnabled && !m_bFleeing)
	{
		if(m_piTarget)
		{
			CVector vTargetPos=m_piTarget->GetPhysicInfo()->vPosition;
			
			if(m_pType->m_bFleeOnSameZ &&
			   (m_PhysicInfo.vPosition.c[2]+m_PhysicInfo.vMins.c[2])<=vTargetPos.c[2] &&
			   (m_PhysicInfo.vPosition.c[2]+m_PhysicInfo.vMaxs.c[2])>=vTargetPos.c[2])
			{
				double dTempAngle=drand()*(m_pType->m_dMaxFleeAngle-m_pType->m_dMinFleeAngle)+m_pType->m_dMinFleeAngle;//35.0+20.0;
				m_dFleeAngle=(vTargetPos.c[2]-m_PhysicInfo.vPosition.c[2])>0?90.0+dTempAngle:270-dTempAngle;
				
				m_bFleeing=true;
			}
			
			if(m_pType->m_bFleeOnSameX &&
			   (m_PhysicInfo.vPosition.c[0]+m_PhysicInfo.vMins.c[0])<=vTargetPos.c[0] &&
			   (m_PhysicInfo.vPosition.c[0]+m_PhysicInfo.vMaxs.c[0])>=vTargetPos.c[0])
			{
				double dTempAngle=drand()*(m_pType->m_dMaxFleeAngle-m_pType->m_dMinFleeAngle)+m_pType->m_dMinFleeAngle;//35.0+20.0;
				m_dFleeAngle=(vTargetPos.c[2]-m_PhysicInfo.vPosition.c[2])>0?270.0-dTempAngle:90+dTempAngle;
				
				m_bFleeing=true;
			}
		}
	}
	
	if(m_bFleeing)
	{
		// If fleeing, just ensure we have the right heading
		CVector vFleeAngles=m_PhysicInfo.vAngles;
		vFleeAngles.c[YAW]=m_dFleeAngle;
		VectorsFromAngles(vFleeAngles,&vDir);
		vDest=m_PhysicInfo.vPosition+vDir*100.0;
	}
	else if(m_pType->m_bHeadToTarget)
	{
		// If head to target is enabled and we have a head correction angle, try to head to the target at the configured angular speed 

		if(m_pType->m_dMaxHeadingCorrection!=0 && m_piTarget)
		{
			vDir=m_piTarget->GetPhysicInfo()->vPosition-m_PhysicInfo.vPosition;
			vDir.N();
			vDest=m_PhysicInfo.vPosition+vDir*100.0;
			dCurrentAngularSpeed=m_pType->m_dMaxHeadingCorrection;
		}
	}
	else if(m_piRoute)
	{
		// Just follow the configured route
		CVector vForward,vRight,vUp;
		VectorsFromAngles(m_PhysicInfo.vAngles.c[YAW],m_PhysicInfo.vAngles.c[PITCH],0,vForward,vRight,vUp);
		vDest=m_piRoute->GetAbsolutePoint(m_piRoute->GetNextPointIndex(m_nRoutePoint));
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
				Remove();
			}
			else
			{
				m_nRoutePoint=nNext;
			}
		}
	}

	// Apply the roll visual effect: Proportional to the heading diference to the target
	if(vForward!=vDir)
	{
		CPlane plane(vRight,m_PhysicInfo.vPosition);
		if(plane.GetSide(vDest)>0)
		{
			m_PhysicInfo.vAngles.c[ROLL]+=m_pType->m_dMaxRoll*dTimeFraction;
			if(m_PhysicInfo.vAngles.c[ROLL]>m_pType->m_dMaxRoll){m_PhysicInfo.vAngles.c[ROLL]=m_pType->m_dMaxRoll;}
		}
		else
		{
			m_PhysicInfo.vAngles.c[ROLL]-=m_pType->m_dMaxRoll*dTimeFraction;
			if(m_PhysicInfo.vAngles.c[ROLL]<-m_pType->m_dMaxRoll){m_PhysicInfo.vAngles.c[ROLL]=-m_pType->m_dMaxRoll;}
		}
	}
	else
	{
		if(m_PhysicInfo.vAngles.c[ROLL]>0)
		{
			m_PhysicInfo.vAngles.c[ROLL]-=m_pType->m_dMaxAngularSpeed*dTimeFraction;
			if(m_PhysicInfo.vAngles.c[ROLL]<0){m_PhysicInfo.vAngles.c[ROLL]=0;}
		}
		else
		{
			m_PhysicInfo.vAngles.c[ROLL]+=m_pType->m_dMaxAngularSpeed*dTimeFraction;
			if(m_PhysicInfo.vAngles.c[ROLL]>0){m_PhysicInfo.vAngles.c[ROLL]=0;}
		}
	}	

	// Head to the desired direction
	double dDesiredYaw=0,dDesiredPitch=0;
	AnglesFromVector(vDir,dDesiredYaw,dDesiredPitch);
	m_PhysicInfo.vAngles.c[YAW]=ApproachAngle(m_PhysicInfo.vAngles.c[YAW],dDesiredYaw,-dCurrentAngularSpeed*dTimeFraction);
	VectorsFromAngles(m_PhysicInfo.vAngles,&m_PhysicInfo.vVelocity);
	m_PhysicInfo.vVelocity*=m_PhysicInfo.dMaxVelocity;
	m_dwNextProcessFrame=dwCurrentTime+10;

	if(dwCurrentTime>m_dwNextShotTime)
	{
		FireWeapon(0,dwCurrentTime);
		m_dwNextShotTime=dwCurrentTime+drand()*(m_pType->m_dTimeBetweenShotsMax-m_pType->m_dTimeBetweenShotsMin)+m_pType->m_dTimeBetweenShotsMin;
	}
}

void CFighter::Render(IGenericRender *piRender,IGenericCamera *piCamera)
{
  CEntityBase::Render(piRender,piCamera);
  
  // Render route
  /*
  if(m_piRoute)
  {
	piRender->PushState();
	piRender->DeactivateDepth();
	
	CVector vPreviousPoint=m_piRoute->GetAbsolutePoint(0);
	for(unsigned x=1;x<m_piRoute->GetPointCount();x++)
	{
		double dPointSize=15;
		CVector vPointColor=CVector(0,0,0.8);
		CVector vLineColor=CVector(0,0,0.8);
		
		
		CVector vWorld=m_piRoute->GetAbsolutePoint(x);
		piRender->RenderLine(vPreviousPoint,vWorld,vLineColor);
		piRender->RenderPoint(vWorld,dPointSize,vPointColor,1.0);
		vPreviousPoint=vWorld;
	
	}
	piRender->PopState();
  }*/
}

bool CFighter::HasFinishedRoute()
{
	return m_piRoute==NULL || ((int)m_piRoute->GetNextPointIndex(m_nRoutePoint)==m_nRoutePoint);
}

IEntity *CFighter::GetTarget()
{
  return m_piTarget;
}
 
