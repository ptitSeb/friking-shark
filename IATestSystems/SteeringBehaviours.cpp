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
#include <GL/gl.h>
#include "IAEntityBase.h"
#include "SteeringBehaviours.h"
#include "IATestUtils.h"

CSteeringBehaviours::CSteeringBehaviours(void)
{
}

CSteeringBehaviours::~CSteeringBehaviours(void)
{
}

CVector CSteeringBehaviours::Seek(CIAEntityBase *pEntity,CVector &vTarget)
{
	CVector vDesiredDir=vTarget-pEntity->GetPosition();
	vDesiredDir.N();
	vDesiredDir*=pEntity->GetMaxVelocity();
	CVector vCurrentDir=pEntity->GetVelocity();
	CVector vForce=vDesiredDir-vCurrentDir;
	return vForce;
}

CVector CSteeringBehaviours::Flee(CIAEntityBase *pEntity,CVector &vTarget)
{
	CVector vDesiredDir=pEntity->GetPosition()-vTarget;
	vDesiredDir.N();
	vDesiredDir*=pEntity->GetMaxVelocity();
	CVector vCurrentDir=pEntity->GetVelocity();
	CVector vForce=vDesiredDir-vCurrentDir;
	return vForce;
}


CVector CSteeringBehaviours::Arrive(CIAEntityBase *pEntity,CVector &vTarget,ESBArriveSpeed eArriveSpeed)
{
	CVector vDesiredDir=vTarget-pEntity->GetPosition();
	double dDistance=vDesiredDir.N();

	if(dDistance<=0.0){return CVector(0,0,0);}

	const double dDeleterationFactor=0.3;

	double dSpeed=dDistance/(((double)eArriveSpeed)*dDeleterationFactor);
	if(dSpeed>pEntity->GetMaxVelocity()){dSpeed=pEntity->GetMaxVelocity();}

	CVector vDesiredVelocity=vDesiredDir*dSpeed;
	CVector vForce=vDesiredVelocity-pEntity->GetVelocity();
	return vForce;
}


CVector CSteeringBehaviours::Pursue(CIAEntityBase *pEntity,CIAEntityBase *pTarget,CVector *pOutEstimatedPosition)
{
	CVector vEntityDistance=pTarget->GetPosition()-pEntity->GetPosition();
	double dRelativeHeading=pTarget->GetVelocity()*pEntity->GetVelocity();

	if(vEntityDistance*pEntity->GetVelocity()>0 && dRelativeHeading<-0.95)
	{
		*pOutEstimatedPosition=pTarget->GetPosition();
		return Seek(pEntity,*pOutEstimatedPosition);
	}

	double dLookAheadTime=((double)vEntityDistance)/((double)pEntity->GetMaxVelocity()+(double)pTarget->GetVelocity());
	*pOutEstimatedPosition=pTarget->GetPosition()+pTarget->GetVelocity()*dLookAheadTime;
	return Seek(pEntity,*pOutEstimatedPosition);
}

CVector CSteeringBehaviours::Evade(CIAEntityBase *pEntity,CIAEntityBase *pTarget,CVector *pOutEstimatedPosition)
{
	CVector vEntityDistance=pTarget->GetPosition()-pEntity->GetPosition();
	double dRelativeHeading=pTarget->GetHeading()*pEntity->GetHeading();

	if(vEntityDistance*pEntity->GetHeading()>0 && dRelativeHeading<-0.95)
	{
		*pOutEstimatedPosition=pTarget->GetPosition();
		return Flee(pEntity,*pOutEstimatedPosition);
	}

	double dLookAheadTime=((double)vEntityDistance)/((double)pEntity->GetMaxVelocity()+(double)pTarget->GetVelocity());
	*pOutEstimatedPosition=pTarget->GetPosition()+pTarget->GetVelocity()*dLookAheadTime;
	return Flee(pEntity,*pOutEstimatedPosition);
}
//returns a random double between zero and 1
inline double RandFloat()      {return ((rand())/(RAND_MAX+1.0));}

//returns a random double in the range -1 < n < 1
inline double RandomClamped()    {return RandFloat() - RandFloat();}


CVector CSteeringBehaviours::Wander(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter,double dTimeFraction)
{
	double dJitterFraction=dJitter*dTimeFraction;
	m_vWanderTarget+=CVector(RandomClamped()*dJitterFraction,RandomClamped()*dJitterFraction,0);
	m_vWanderTarget.N();
	m_vWanderTarget*=dRadius;

	CVector localTarget=m_vWanderTarget+CVector(dDistance,0,0);
	CMatrix mEntityToWorld;
	CVector worldTarget=localTarget;
	CVector vAxisX,vAxisY;
	vAxisX=pEntity->GetHeading();
	vAxisX.N();
	vAxisY=pEntity->GetHeading()^CVector(0,0,1);
	vAxisY.N();
	mEntityToWorld.Ref(vAxisX,vAxisY,CVector(0,0,1));
	worldTarget*=mEntityToWorld;
	mEntityToWorld.T(pEntity->GetPosition());
	worldTarget*=mEntityToWorld;
	m_vLastWanderTarget=worldTarget;

	return (worldTarget-pEntity->GetPosition());
}

void CSteeringBehaviours::RenderWanderLocal(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter)
{
	glColor3d(1,1,1);
	Circle3D(dDistance,0,dRadius);
}

void CSteeringBehaviours::RenderWanderGlobal(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter)
{
	CVector localTarget=m_vLastWanderTarget;
	RenderBox(localTarget-CVector(5,5,5),localTarget+CVector(5,5,5),CVector(1,0,1));
}

CVector CSteeringBehaviours::Interpose(CIAEntityBase *pEntity,CIAEntityBase *pTarget1,CIAEntityBase *pTarget2,CVector *pOutEstimatedPosition)
{
	CVector vMidway=(pTarget1->GetPosition()+pTarget2->GetPosition())*0.5;
	double dMidwayDistance=(vMidway-pEntity->GetPosition()).N();
	double dTimeToMidway=dMidwayDistance/pEntity->GetMaxVelocity();
	CVector vTarget;
	vTarget+=pTarget1->GetPosition()+pTarget1->GetVelocity()*dTimeToMidway;
	vTarget+=pTarget2->GetPosition()+pTarget2->GetVelocity()*dTimeToMidway;
	vTarget*=0.5;
	*pOutEstimatedPosition=vTarget;
	return Arrive(pEntity,vTarget,eSBArriveSpeed_Fast);
}

CVector CSteeringBehaviours::OffsetPursue(CIAEntityBase *pEntity,CIAEntityBase *pTarget,CVector &vOffset)
{
	CVector vWorldTarget;
	CVector vAxisX,vAxisY;
	CMatrix mEntityToWorld;
	vAxisX=pTarget->GetHeading();
	vAxisX.N();
	vAxisY=pTarget->GetHeading()^CVector(0,0,1);
	vAxisY.N();

	vWorldTarget=vOffset;
	mEntityToWorld.Ref(vAxisX,vAxisY,CVector(0,0,1));
	vWorldTarget*=mEntityToWorld;
	mEntityToWorld.T(pTarget->GetPosition());
	vWorldTarget*=mEntityToWorld;

	// Try to predict the offset position in the future.

	CVector vWorldOffset=vWorldTarget-pEntity->GetPosition();
	double dTime=vWorldOffset/(pEntity->GetMaxVelocity()+(double)pTarget->GetVelocity());

	CVector vTargetPos=vWorldTarget+pTarget->GetVelocity()*dTime;
	return Arrive(pEntity,vTargetPos,eSBArriveSpeed_Fast);
}
