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


#pragma once

#include "SteeringBehaviours.h"

#define RENDER_FLAGS_NONE		0x0000
#define RENDER_FLAGS_AXISES		0x0001
#define RENDER_FLAGS_FORCE		0x0002
#define RENDER_FLAGS_VELOCITY	0x0004

#define RENDER_FLAGS_DEFAULT (RENDER_FLAGS_FORCE|RENDER_FLAGS_VELOCITY)

class CIAEntityBase
{
	CVector m_vPosition;
	CVector m_vVelocity;
	CVector m_vHeading;
	CVector m_vColor;
	CVector m_vForce;
	double  m_dMass;
	double  m_dSize;
	double  m_dMaxVelocity;
	double  m_dMaxForce;

	CIAEntityBase *m_pSeekTarget;
	CIAEntityBase *m_pFleeTarget;
	CIAEntityBase *m_pArriveTarget;
	CIAEntityBase *m_pPursueTarget;
	CIAEntityBase *m_pEvadeTarget;
	CIAEntityBase *m_pInterposeTarget1;
	CIAEntityBase *m_pInterposeTarget2;
	CIAEntityBase *m_pOffsetPursueTarget;
	ESBArriveSpeed m_eArriveSpeed;

	CVector m_vPursueEstimatedPosition;
	CVector m_vEvadeEstimatedPosition;
	CVector m_vInterposeEstimatedPosition;
	CVector m_vOffsetPursueOffset;

	bool m_bWanderEnabled;
	double m_dWanderDistance;
	double m_dWanderRadius;
	double m_dWanderJitter;

	unsigned long m_dwRenderFlags;

	CSteeringBehaviours m_Behaviours;

public:


	double  GetSize(){return m_dSize;}
	CVector GetColor(){return m_vColor;}
	CVector GetPosition(){return m_vPosition;}
	CVector GetVelocity(){return m_vVelocity;}
	CVector GetHeading(){return m_vHeading;}
	CVector GetForce(){return m_vForce;}
	double  GetMass(){return m_dMass;}
	double  GetMaxVelocity(){return m_dMaxVelocity;}
	double  GetMaxForce(){return m_dMaxForce;}
	unsigned long GetRenderFlags(){return m_dwRenderFlags;};

	void    SetSize(double dSize){m_dSize=dSize;}
	void    SetColor(CVector vColor){m_vColor=vColor;}
	void    SetPosition(CVector vPosition){m_vPosition=vPosition;}
	void    SetVelocity(CVector vVelocity){m_vVelocity=vVelocity;}
	void    SetHeading(CVector vHeading){m_vHeading=vHeading;}
	void    SetForce(CVector vForce){m_vForce=vForce;}
	void    SetMass(double dMass){m_dMass=dMass;}
	void    SetMaxVelocity(double dMaxVelocity){m_dMaxVelocity=dMaxVelocity;}
	void    SetMaxForce(double dMaxForce){m_dMaxForce=dMaxForce;}
	void	SetRenderFlags(unsigned long dwRenderFlags){m_dwRenderFlags=dwRenderFlags;}

	void    SeekTarget(CIAEntityBase *pTarget){m_pSeekTarget=pTarget;}
	void    FleeTarget(CIAEntityBase *pTarget){m_pFleeTarget=pTarget;}
	void    ArriveTarget(CIAEntityBase *pTarget,ESBArriveSpeed eArriveSpeed){m_pArriveTarget=pTarget;m_eArriveSpeed=eArriveSpeed;}
	void    PursueTarget(CIAEntityBase *pTarget){m_pPursueTarget=pTarget;}
	void    EvadeTarget(CIAEntityBase *pTarget){m_pEvadeTarget=pTarget;}
	void    Wander(bool bEnabled,double dDistance,double dRadius,double dJitter){m_bWanderEnabled=bEnabled;m_dWanderDistance=dDistance;m_dWanderRadius=dRadius;m_dWanderJitter=dJitter;}
	void    Interpose(CIAEntityBase *pTarget1,CIAEntityBase *pTarget2){m_pInterposeTarget1=pTarget1;m_pInterposeTarget2=pTarget2;}
	void    OffsetPursue(CIAEntityBase *pTarget,CVector &vOffset){m_pOffsetPursueTarget=pTarget;m_vOffsetPursueOffset=vOffset;}


	void ProcessIA(double dTimeFraction);
	void Render();


	CIAEntityBase();
	~CIAEntityBase();
};
