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

#include "Route.h"

class CPlayAreaEntity: virtual public CPlayAreaElementBase,virtual public IPlayAreaEntity,virtual public IEntityEvents
{
    CEntityTypeWrapper   m_EntityType;
	CEntityTypeWrapper   m_BonusType;
	map<IEntity*,bool>   m_mEntities; // bool indicates if the entity has been visible anytime
	double				 m_dRTRadius;
	double 				 m_dRTActivationMin;
	double 				 m_dRTActivationMax;
	double				 m_dRTShadowPreactivation;
	CVector				 m_vRTMins;
	CVector				 m_vRTMaxs;
	
    C3DSVector           m_vPosition;
    CVector              m_vAngles;
	CRoute				 m_Route;

	unsigned int         m_nCreatedEntities;
	unsigned int         m_nKilledEntities;
	unsigned int         m_nEntityCount;
    unsigned int         m_nInterval;
	int                  m_nDelay;
	unsigned int         m_nRouteDelay;
	
	unsigned int         m_nLastEntityTime;
	bool                 m_bFirstFrame;
	bool				 m_bDoNotActivate;
	bool                 m_bDynamic;
	int					 m_nBonusOnChild;
	
public:
    
    BEGIN_PROP_MAP(CPlayAreaEntity)
        PROP(m_vPosition,"Posicion")
        PROP(m_vAngles,"Angulos")
        PROP(m_EntityType,"Tipo")
		PROP_FLAGS(m_Route,"Ruta",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nEntityCount,"Count",1,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nInterval,"Interval",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nDelay,"Delay",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nRouteDelay,"RouteDelay",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nBonusOnChild,"BonusOnChild",-1,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_FLAGS(m_BonusType,"Bonus",MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();
	
	void Start();
	void Stop();
	
    void Activate(unsigned int dwCurrentTime);
    void Deactivate();

    bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pAreaInfo,unsigned int dwCurrentTime,double dInterval);

	void DesignRender(IGenericRender *piRender,bool bSelected);
	CTraceInfo DesignGetTrace(const CVector &p1,const CVector &p2);

    void OnKilled(IEntity *pUnit);
    void OnRemoved(IEntity *pUnit);
	void OnChildKilled(IEntity *piEntity,unsigned int nChildId,IEntity *piChildEntity);
	
	//	IPlayAreaEntity

	void SetPosition(const CVector &vPosition);
	void SetAngles(const CVector &vAngles);
	void SetEntityType(IEntityType *piEntityType);
	void SetBonusType(IEntityType *piEntityType);
	void SetBonusOnChild(int nChildIndex);
	void SetDynamic(bool bDynamic);
	
	void SetCount(unsigned int nCount);
	void SetDelay(int nDelay);
	void SetRouteDelay(unsigned int nDelay);
	void SetInterval(unsigned int nInterval);

	CVector GetPosition();
	CVector GetAngles();
	void	GetEntityType(IEntityType **piEntityType);
	void	GetBonusType(IEntityType **ppiEntityType);
	int		GetBonusOnChild();
	
	unsigned int GetCount();
	int          GetDelay();
	unsigned int GetRouteDelay();
	unsigned int GetInterval();
	
	bool PrepareResources();

	// Route edition

	unsigned int GetRoutePoints();
	bool 		 GetRoutePoint(unsigned int nIndex,SRoutePoint *psPoint);
	bool 		 AddRoutePoint(unsigned int nIndex,const SRoutePoint &sPoint);
	bool 		 SetRoutePoint(unsigned int nIndex,const SRoutePoint &sPoint);
	void		 RemoveRoutePoint(unsigned int nIndex);
	void 		 ClearRoute();

     CPlayAreaEntity();
    ~CPlayAreaEntity();
};
