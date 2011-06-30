//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#include "./stdafx.h"
#include "SteeringBehaviours.h"

CSteeringBehaviours::CSteeringBehaviours(void)
{
	m_pSeekTarget=NULL;
	m_pFleeTarget=NULL;
	m_pArriveTarget=NULL;
	m_eArriveSpeed=eSBArriveSpeed_Normal;		
	m_pPursueTarget=NULL;
	m_pEvadeTarget=NULL;
	m_pInterposeTarget1=NULL;
	m_pInterposeTarget2=NULL;
	m_pOffsetPursueTarget=NULL;
	m_piRoute=NULL;
	m_nRoutePoint=0;

	m_bWanderEnabled=false;
	m_dWanderDistance=30;
	m_dWanderRadius=20;
	m_dWanderJitter=5;

}

CSteeringBehaviours::~CSteeringBehaviours(void)
{
}

CVector CSteeringBehaviours::Seek(IEntity *pEntity,CVector &vTarget)
{
	CVector vDesiredDir=vTarget-pEntity->GetPhysicInfo()->vPosition;
	vDesiredDir.N();
	vDesiredDir*=pEntity->GetPhysicInfo()->dMaxVelocity;
	CVector vCurrentDir=pEntity->GetPhysicInfo()->vVelocity;
	CVector vForce=vDesiredDir-vCurrentDir;
	return vForce;
}

CVector CSteeringBehaviours::Flee(IEntity *pEntity,CVector &vTarget)
{
	CVector vDesiredDir=pEntity->GetPhysicInfo()->vPosition-vTarget;
	vDesiredDir.N();
	vDesiredDir*=pEntity->GetPhysicInfo()->dMaxVelocity;
	CVector vCurrentDir=pEntity->GetPhysicInfo()->vVelocity;
	CVector vForce=vDesiredDir-vCurrentDir;
	return vForce;
}


CVector CSteeringBehaviours::Arrive(IEntity *pEntity,CVector &vTarget,ESBArriveSpeed eArriveSpeed)
{
	CVector vDesiredDir=vTarget-pEntity->GetPhysicInfo()->vPosition;
	double dDistance=vDesiredDir.N();

	if(dDistance<=0.0){return CVector(0,0,0);}

	const double dDecelerationFactor=0.3;

	double dSpeed=dDistance/(((double)eArriveSpeed)*dDecelerationFactor);
	if(dSpeed>pEntity->GetPhysicInfo()->dMaxVelocity){dSpeed=pEntity->GetPhysicInfo()->dMaxVelocity;}

	CVector vDesiredVelocity=vDesiredDir*dSpeed;
	CVector vForce=vDesiredVelocity-pEntity->GetPhysicInfo()->vVelocity;
	return vForce;
}


CVector CSteeringBehaviours::Pursue(IEntity *pEntity,IEntity *pTarget,CVector *pOutEstimatedPosition)
{
	CVector vEntityDistance=pTarget->GetPhysicInfo()->vPosition-pEntity->GetPhysicInfo()->vPosition;
	double dRelativeHeading=pTarget->GetPhysicInfo()->vVelocity*pEntity->GetPhysicInfo()->vVelocity;

	if(vEntityDistance*pEntity->GetPhysicInfo()->vVelocity>0 && dRelativeHeading<-0.95)
	{
		*pOutEstimatedPosition=pTarget->GetPhysicInfo()->vPosition;
		return Seek(pEntity,pTarget->GetPhysicInfo()->vPosition);
	}

	double dLookAheadTime=((double)vEntityDistance)/((double)pEntity->GetPhysicInfo()->dMaxVelocity+(double)pTarget->GetPhysicInfo()->vVelocity);
	*pOutEstimatedPosition=pTarget->GetPhysicInfo()->vPosition+pTarget->GetPhysicInfo()->vVelocity*dLookAheadTime;
	return Seek(pEntity,*pOutEstimatedPosition);
}

CVector CSteeringBehaviours::Evade(IEntity *pEntity,IEntity *pTarget,CVector *pOutEstimatedPosition)
{
	CVector vEntityDistance=pTarget->GetPhysicInfo()->vPosition-pEntity->GetPhysicInfo()->vPosition;
	double dRelativeHeading=pTarget->GetPhysicInfo()->vForward*pEntity->GetPhysicInfo()->vForward;

	if(vEntityDistance*pEntity->GetPhysicInfo()->vForward>0 && dRelativeHeading<-0.95)
	{
		*pOutEstimatedPosition=pTarget->GetPhysicInfo()->vPosition;
		return Flee(pEntity,pTarget->GetPhysicInfo()->vPosition);
	}

	double dLookAheadTime=((double)vEntityDistance)/((double)pEntity->GetPhysicInfo()->dMaxVelocity+(double)pTarget->GetPhysicInfo()->vVelocity);
	*pOutEstimatedPosition=pTarget->GetPhysicInfo()->vPosition+pTarget->GetPhysicInfo()->vVelocity*dLookAheadTime;
	return Flee(pEntity,*pOutEstimatedPosition);
}
//returns a random double between zero and 1
inline double RandFloat()      {return ((rand())/(RAND_MAX+1.0));}

//returns a random double in the range -1 < n < 1
inline double RandomClamped()    {return RandFloat() - RandFloat();}


CVector CSteeringBehaviours::Wander(IEntity *pEntity,double dDistance,double dRadius,double dJitter,double dTimeFraction)
{
	double dJitterFraction=dJitter*dTimeFraction;
	m_vWanderTarget+=CVector(RandomClamped()*dJitterFraction,RandomClamped()*dJitterFraction,0);
	m_vWanderTarget.N();
	m_vWanderTarget*=dRadius;

	CVector localTarget=m_vWanderTarget+CVector(dDistance,0,0);
	CMatrix mEntityToWorld;
	CVector worldTarget=localTarget;
	CVector vAxisX,vAxisY;
	vAxisX=pEntity->GetPhysicInfo()->vForward;
	vAxisX.N();
	vAxisY=pEntity->GetPhysicInfo()->vForward^CVector(0,0,1);
	vAxisY.N();
	mEntityToWorld.Ref(vAxisX,vAxisY,CVector(0,0,1));
	worldTarget*=mEntityToWorld;
	mEntityToWorld.T(pEntity->GetPhysicInfo()->vPosition);
	worldTarget*=mEntityToWorld;
	m_vLastWanderTarget=worldTarget;

	return (worldTarget-pEntity->GetPhysicInfo()->vPosition);
}

CVector CSteeringBehaviours::Interpose(IEntity *pEntity,IEntity *pTarget1,IEntity *pTarget2,CVector *pOutEstimatedPosition)
{
	CVector vMidway=(pTarget1->GetPhysicInfo()->vPosition+pTarget2->GetPhysicInfo()->vPosition)*0.5;
	double dMidwayDistance=(vMidway-pEntity->GetPhysicInfo()->vPosition).N();
	double dTimeToMidway=dMidwayDistance/pEntity->GetPhysicInfo()->dMaxVelocity;
	CVector vTarget;
	vTarget+=pTarget1->GetPhysicInfo()->vPosition+pTarget1->GetPhysicInfo()->vVelocity*dTimeToMidway;
	vTarget+=pTarget2->GetPhysicInfo()->vPosition+pTarget2->GetPhysicInfo()->vVelocity*dTimeToMidway;
	vTarget*=0.5;
	*pOutEstimatedPosition=vTarget;
	return Arrive(pEntity,vTarget,eSBArriveSpeed_Fast);
}

CVector CSteeringBehaviours::OffsetPursue(IEntity *pEntity,IEntity *pTarget,CVector &vOffset)
{
	CVector vWorldTarget;
	CVector vAxisX,vAxisY;
	CMatrix mEntityToWorld;
	vAxisX=pTarget->GetPhysicInfo()->vForward;
	vAxisX.N();
	vAxisY=pTarget->GetPhysicInfo()->vForward^CVector(0,0,1);
	vAxisY.N();

	vWorldTarget=vOffset;
	mEntityToWorld.Ref(vAxisX,vAxisY,CVector(0,0,1));
	vWorldTarget*=mEntityToWorld;
	mEntityToWorld.T(pTarget->GetPhysicInfo()->vPosition);
	vWorldTarget*=mEntityToWorld;

	// Try to predict the offset position in the future.

	CVector vWorldOffset=vWorldTarget-pEntity->GetPhysicInfo()->vPosition;
	double dTime=vWorldOffset/(pEntity->GetPhysicInfo()->dMaxVelocity+(double)pTarget->GetPhysicInfo()->vVelocity);

	CVector vTarget=vWorldTarget+pTarget->GetPhysicInfo()->vVelocity*dTime;
	return Arrive(pEntity,vTarget,eSBArriveSpeed_Fast);
}


CVector CSteeringBehaviours::ProcessBehaviours(IEntity *piEntity,double dTimeFraction)
{
	CVector vForce=CVector(0,0,0);
	if(m_pSeekTarget){vForce+=Seek(piEntity,m_pSeekTarget->GetPhysicInfo()->vPosition);}
	if(m_pFleeTarget){vForce+=Flee(piEntity,m_pFleeTarget->GetPhysicInfo()->vPosition);}
	if(m_pArriveTarget){vForce+=Arrive(piEntity,m_pArriveTarget->GetPhysicInfo()->vPosition,m_eArriveSpeed);}
	if(m_pPursueTarget){vForce+=Pursue(piEntity,m_pPursueTarget,&m_vPursueEstimatedPosition);}
	if(m_pEvadeTarget){vForce+=Evade(piEntity,m_pEvadeTarget,&m_vEvadeEstimatedPosition);}
	if(m_bWanderEnabled){vForce+=Wander(piEntity,m_dWanderDistance,m_dWanderRadius,m_dWanderJitter,dTimeFraction)*piEntity->GetPhysicInfo()->dMass;}
	if(m_pOffsetPursueTarget){vForce+=OffsetPursue(piEntity,m_pOffsetPursueTarget,m_vOffsetPursueOffset);}
	if(m_pInterposeTarget1 && m_pInterposeTarget2){vForce+=Interpose(piEntity,m_pInterposeTarget1,m_pInterposeTarget2,&m_vInterposeEstimatedPosition);}
	if(m_piRoute && m_nRoutePoint<m_piRoute->GetPointCount())
	{
		CVector vPos=m_piRoute->GetAbsolutePoint(m_nRoutePoint);
		double dDistance=vPos-piEntity->GetPhysicInfo()->vPosition;
		double dVelocity=piEntity->GetPhysicInfo()->vVelocity;
		if(dDistance<dVelocity)
		{
			// change point
			m_nRoutePoint=m_piRoute->GetNextPointIndex(m_nRoutePoint);
			vPos=m_piRoute->GetAbsolutePoint(m_nRoutePoint);
		}
		unsigned int nNextPoint=m_piRoute->GetNextPointIndex(m_nRoutePoint);
		if(nNextPoint==m_nRoutePoint)
		{
			vForce+=Arrive(piEntity,vPos,eSBArriveSpeed_Fast);
		}
		else
		{
			vForce+=Seek(piEntity,vPos);
		}
	}
	return vForce;
}

