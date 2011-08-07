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

enum EGroundBossHatchState
{
	eGroundBossHatchState_Normal=ENTITY_STATE_BASE,
	eGroundBossHatchState_Closed,
	eGroundBossHatchState_Open
};

class CGroundBossHatchType: public CEntityTypeBase
{
public:
	unsigned int m_nClosedTime;
	unsigned int m_nOpenTime;
	
	double m_dTimeFirstShotMin;
	double m_dTimeFirstShotMax;
	double m_dTimeBetweenShotsMin;
	double m_dTimeBetweenShotsMax;
	
	IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);
	void InitializeEntity(CEntityBase *piEntity,unsigned int dwCurrentTime);

	BEGIN_ENTITY_STATE_MAP()
		ENTITY_STATE_CHAIN(CEntityTypeBase)
		ENTITY_STATE(eGroundBossHatchState_Closed,"Closed")
		ENTITY_STATE(eGroundBossHatchState_Open,"Open")
	END_ENTITY_STATE_MAP()
	
	BEGIN_PROP_MAP(CGroundBossHatchType)
	PROP_CLASS_CHAIN(CEntityTypeBase)
		PROP_VALUE_FLAGS(m_nClosedTime,"ClosedTime",1000,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_nOpenTime,"OpenTime",2500,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeFirstShotMin,"TimeFirstShotMin",500,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeFirstShotMax,"TimeFirstShotMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeBetweenShotsMin,"TimeBetweenShotsMin",2000,MRPF_NORMAL|MRPF_OPTIONAL);
		PROP_VALUE_FLAGS(m_dTimeBetweenShotsMax,"TimeBetweenShotsMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();	
	CGroundBossHatchType();
	~CGroundBossHatchType();
};


class CGroundBossHatch: public CEntityBase
{
	CGroundBossHatchType  *m_pType;
	unsigned int m_dwNextStateChange;
	
	double m_dRadius;
	bool m_bIsOpen;
	bool m_bFirstTimeVisible;
	unsigned int m_dwNextShotTime;
		
	IEntity *GetTarget();
	void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
	
public:
	void OnAnimationEvent(string sEvent,string sParams);

	CGroundBossHatch(CGroundBossHatchType *pType,unsigned int dwCurrentTime);
};