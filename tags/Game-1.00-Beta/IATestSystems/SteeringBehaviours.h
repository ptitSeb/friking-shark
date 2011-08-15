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

class CIAEntityBase;

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

public:

	CVector Seek(CIAEntityBase *pEntity,CVector &vPos);
	CVector Flee(CIAEntityBase *pEntity,CVector &vPos);
	CVector Arrive(CIAEntityBase *pEntity,CVector &vPos,ESBArriveSpeed eArriveSpeed);
	CVector Pursue(CIAEntityBase *pEntity,CIAEntityBase *pTarget,CVector *pOutEstimatedPosition);
	CVector Evade(CIAEntityBase *pEntity,CIAEntityBase *pTarget,CVector *pOutEstimatedPosition);
	CVector Wander(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter,double dTimeFraction);
	CVector Interpose(CIAEntityBase *pEntity,CIAEntityBase *pTarget1,CIAEntityBase *pTarget2,CVector *pOutEstimatedPosition);
	CVector OffsetPursue(CIAEntityBase *pEntity,CIAEntityBase *pTarget1,CVector &vOffset);

	void RenderWanderLocal(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter);
	void RenderWanderGlobal(CIAEntityBase *pEntity,double dDistance,double dRadius,double dJitter);

	CSteeringBehaviours(void);
	~CSteeringBehaviours(void);
};
