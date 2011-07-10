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

enum EGroundBossState
{
	eGroundBossState_Normal=ENTITY_STATE_BASE,
	eGroundBossState_Destroyed
};

class CGroundBossType: public CEntityTypeBase
{
public:
    IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

    CVector m_vFixedAngles;
    bool    m_bUseFixedAngles;

    void       DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected);
    CTraceInfo DesignGetTrace(const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    BEGIN_PROP_MAP(CGroundBossType)
        PROP_CLASS_CHAIN(CEntityTypeBase)
	PROP_VALUE_FLAGS(m_bUseFixedAngles,"UseFixedAngles",true,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_vFixedAngles,"FixedAngles",Origin,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();
	
	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eGroundBossState_Destroyed,"Destroyed")
	END_ENTITY_STATE_MAP()

    CGroundBossType();
    ~CGroundBossType();
};


class CGroundBoss: public CEntityBase
{
    CGroundBossType  *m_pType;
	int m_nRoutePoint;
	bool m_bRouteFinished;
	unsigned int m_dwNextShotTime;
	unsigned int m_nConfiguredDamageType;
	unsigned int m_nPauseEnd;
	bool 		 m_bFirstFrame;
	
	void AcquireTarget();
	
	void Render(IGenericRender *piRender,IGenericCamera *piCamera);
	
	
	static void FindBuilding(IEntity *piEntity,void *pParam1,void *pParam2);
	bool IsInsideBuilding(IEntity *piEntity);
	IEntity *m_piContainerBuilding;
	
public:

	bool HasFinishedRoute();
	void SetRoute(IRoute *piRoute);
	void OnKilled();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
	
	// IEntityEvents
	
	void OnRemoved(IEntity *piEntity);
	void OnKilled(IEntity *piEntity);
	
    CGroundBoss(CGroundBossType *pType);
};
