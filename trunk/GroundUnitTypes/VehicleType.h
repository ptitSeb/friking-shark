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

enum EVehicleState
{
	eVehicleState_Normal=ENTITY_STATE_BASE,
	eVehicleState_Destroyed
};

class CVehicleType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

    double  m_dMaxAngularSpeed;
	bool    m_bFollowRouteDestroyed;
	
    BEGIN_PROP_MAP(CVehicleType)
        PROP_CLASS_CHAIN(CEntityTypeBase)
        PROP_VALUE_FLAGS(m_dMaxAngularSpeed,"MaxAngularVelocity",60,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_bFollowRouteDestroyed,"FollowRouteDestroyed",false,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
	
	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eVehicleState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()

    CVehicleType();
    ~CVehicleType();
};


class CVehicle: public CEntityBase,virtual public IEntityEvents
{
	IEntity *m_piTarget;
    CVehicleType  *m_pType;
	int m_nRoutePoint;
	bool m_bRouteFinished;
	bool m_bFirstFrame;
	unsigned int m_dwNextShotTime;
	unsigned int m_nConfiguredDamageType;
	
	static void FindBuilding(IEntity *piEntity,void *pParam1,void *pParam2);
	
	bool IsInsideBuilding(IEntity *piEntity);
	IEntity *m_piContainerBuilding;
	
	void AcquireTarget();
	
	void Render(IGenericRender *piRender,IGenericCamera *piCamera);
	
	void OnRemoved(IEntity *piEntity);
	void OnKilled(IEntity *piEntity);
public:

	bool HasFinishedRoute();
	void SetRoute(IRoute *piRoute);
	void OnKilled();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);

    CVehicle(CVehicleType *pType);
};