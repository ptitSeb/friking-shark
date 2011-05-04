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

enum ESBArriveSpeed
{
	eSBArriveSpeed_Fast=1,
	eSBArriveSpeed_Normal,
	eSBArriveSpeed_Slow
};

class CSteeringBehaviours
{
	CVector m_vWanderTarget;
	CVector m_vLastWanderTarget;

	IEntity *m_pSeekTarget;
	IEntity *m_pFleeTarget;
	IEntity *m_pArriveTarget;
	IEntity *m_pPursueTarget;
	IEntity *m_pEvadeTarget;
	IEntity *m_pInterposeTarget1;
	IEntity *m_pInterposeTarget2;
	IEntity *m_pOffsetPursueTarget;
	IRoute  *m_piRoute;
	unsigned int m_nRoutePoint;
	ESBArriveSpeed m_eArriveSpeed;

	CVector m_vPursueEstimatedPosition;
	CVector m_vEvadeEstimatedPosition;
	CVector m_vInterposeEstimatedPosition;
	CVector m_vOffsetPursueOffset;

	bool m_bWanderEnabled;
	double m_dWanderDistance;
	double m_dWanderRadius;
	double m_dWanderJitter;

	CVector Seek(IEntity *pEntity,CVector &vPos);
	CVector Flee(IEntity *pEntity,CVector &vPos);
	CVector Arrive(IEntity *pEntity,CVector &vPos,ESBArriveSpeed eArriveSpeed);
	CVector Pursue(IEntity *pEntity,IEntity *pTarget,CVector *pOutEstimatedPosition);
	CVector Evade(IEntity *pEntity,IEntity *pTarget,CVector *pOutEstimatedPosition);
	CVector Wander(IEntity *pEntity,double dDistance,double dRadius,double dJitter,double dTimeFraction);
	CVector Interpose(IEntity *pEntity,IEntity *pTarget1,IEntity *pTarget2,CVector *pOutEstimatedPosition);
	CVector OffsetPursue(IEntity *pEntity,IEntity *pTarget1,CVector &vOffset);

public:

	void    SeekTarget(IEntity *pTarget){m_pSeekTarget=pTarget;}
	void    FleeTarget(IEntity *pTarget){m_pFleeTarget=pTarget;}
	void    ArriveTarget(IEntity *pTarget,ESBArriveSpeed eArriveSpeed){m_pArriveTarget=pTarget;m_eArriveSpeed=eArriveSpeed;}
	void    PursueTarget(IEntity *pTarget){m_pPursueTarget=pTarget;}
	void    EvadeTarget(IEntity *pTarget){m_pEvadeTarget=pTarget;}
	void    Wander(bool bEnabled,double dDistance,double dRadius,double dJitter){m_bWanderEnabled=bEnabled;m_dWanderDistance=dDistance;m_dWanderRadius=dRadius;m_dWanderJitter=dJitter;}
	void    Interpose(IEntity *pTarget1,IEntity *pTarget2){m_pInterposeTarget1=pTarget1;m_pInterposeTarget2=pTarget2;}
	void    OffsetPursue(IEntity *pTarget,CVector &vOffset){m_pOffsetPursueTarget=pTarget;m_vOffsetPursueOffset=vOffset;}
	void    FollowRoute(IRoute *piRoute){m_piRoute=piRoute;m_nRoutePoint=0;}
	bool    HasFinishedRoute(){return m_piRoute==NULL || (m_piRoute->GetNextPointIndex(m_nRoutePoint)==m_nRoutePoint);}
	
	CVector ProcessBehaviours(IEntity *piEntity,double dTimeFraction);

	CSteeringBehaviours(void);
	~CSteeringBehaviours(void);
};
