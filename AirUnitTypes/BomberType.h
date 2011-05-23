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

enum EBomberState
{
	eBomberState_Normal=ENTITY_STATE_BASE,
	eBomberState_Destroyed
};

class CBomberType: public CEntityTypeBase
{
public:
  double  m_dTimeFirstShotMin;
  double  m_dTimeFirstShotMax;
  double  m_dTimeBetweenShotsMin;
  double  m_dTimeBetweenShotsMax;
  CVector m_vFixedAngles;
  bool    m_bUseFixedAngles;
  
  virtual void	DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected);
  virtual CTraceInfo DesignGetTrace(const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 );
  
  IEntity *CreateInstance(IEntity *piParent,unsigned int dwCurrentTime);

  BEGIN_PROP_MAP(CBomberType)
    PROP_CLASS_CHAIN(CEntityTypeBase)
	PROP_VALUE_FLAGS(m_bUseFixedAngles,"UseFixedAngles",true,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_vFixedAngles,"FixedAngles",Origin,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dTimeFirstShotMin,"TimeFirstShotMin",500,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dTimeFirstShotMax,"TimeFirstShotMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dTimeBetweenShotsMin,"TimeBetweenShotsMin",2000,MRPF_NORMAL|MRPF_OPTIONAL);
	PROP_VALUE_FLAGS(m_dTimeBetweenShotsMax,"TimeBetweenShotsMax",5000,MRPF_NORMAL|MRPF_OPTIONAL);
  END_PROP_MAP();
  
  BEGIN_ENTITY_STATE_MAP()
	ENTITY_STATE_CHAIN(CEntityTypeBase)
	ENTITY_STATE(eBomberState_Destroyed,"Destroyed")
  END_ENTITY_STATE_MAP()
  
  CBomberType();
  ~CBomberType();
};


class CBomber: public CEntityBase
{
  double    m_dwNextShotTime;
  int m_nRoutePoint;
  bool m_bRouteFinished;
  unsigned int m_nPauseEnd;
  double m_dRadius;
  
  CBomberType  *m_pType;

public:
  bool HasFinishedRoute();
  void SetRoute(IRoute *piRoute);
  
  void OnKilled();
  void ProcessFrame(unsigned int dwCurrentTime,double dTimeFraction);
  IEntity *GetTarget();

  CBomber(CBomberType *pType,unsigned int dwCurrentTime);
};