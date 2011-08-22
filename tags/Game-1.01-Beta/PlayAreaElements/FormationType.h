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

struct SFormationElement
{
public:
    CRoute               m_Route;
	CEntityTypeWrapper   m_EntityType;
    unsigned int         m_nEntityCount;
    unsigned int         m_nInterval;
	unsigned int 		 m_nDelay;

    SFormationElement(){m_nDelay=0;m_nInterval=0;m_nEntityCount=0;}
    ~SFormationElement(){}
};

BEGIN_STRUCT_PROPS(SFormationElement)
	PROP(m_EntityType,"EntityType")
    PROP(m_Route,"Route")
    PROP_VALUE_FLAGS(m_nEntityCount,"Count",1,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_nInterval,"Interval",1000,MRPF_NORMAL|MRPF_OPTIONAL)
	PROP_VALUE_FLAGS(m_nDelay,"Delay",0,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class CFormationType: virtual public CSystemObjectBase, virtual public IFormationType,virtual public IFormationTypeDesign
{
public:
	vector<SFormationElement> m_vElements;

    BEGIN_PROP_MAP(CFormationType)
        PROP(m_vElements,"Elementos")
    END_PROP_MAP()

   IFormation *CreateInstance(CVector vPosition,unsigned int dwCurrentTime);
   bool PrepareResources();
   
	void DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetAABBox(CVector &vPosition,CVector &vAngles,CVector *pvMins,CVector *pvMaxs);
	double DesignGetRadius();
	CTraceInfo DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 );

	// IFormationTypeDesign
	
	unsigned long	AddElement();
	void			RemoveElement(unsigned int nIndex);
	unsigned int	GetElements();
	
	void 		 SetElementEntityType(unsigned int nElement,IEntityType *piEntityType);
	void		 GetElementEntityType(unsigned int nElement,IEntityType **piEntityType);
	
	void 		 SetElementEntityCount(unsigned int nElement,unsigned int nCount);
	unsigned int GetElementEntityCount(unsigned int nElement);
	
	void 		 SetElementEntityInterval(unsigned int nElement,unsigned int nMilliseconds);
	unsigned int GetElementEntityInterval(unsigned int nElement);
	
	void 		 SetElementEntityDelay(unsigned int nElement,unsigned int nMilliseconds);
	unsigned int GetElementEntityDelay(unsigned int nElement);

	unsigned int GetElementRoutePoints(unsigned int nElement);
	bool 		 GetElementRoutePoint(unsigned int nElement,unsigned int nIndex,SRoutePoint *psPoint);
	bool 		 AddElementRoutePoint(unsigned int nElement,unsigned int nIndex,const SRoutePoint &sPoint);
	bool 		 SetElementRoutePoint(unsigned int nElement,unsigned int nIndex,const SRoutePoint &sPoint);
	void		 RemoveElementRoutePoint(unsigned int nElement,unsigned int nIndex);
	void 		 ClearElementRoute(unsigned int nElement);	
	
    CFormationType();
    ~CFormationType();
};

struct SFormationElementRunTimeInfo
{
    unsigned int               m_dwLastEntityTime;
    unsigned int               m_dwCreatedEntities;
    SFormationElement   *m_pFormationTypeElement;

    SFormationElementRunTimeInfo(){m_dwLastEntityTime=0;m_dwCreatedEntities=0;m_pFormationTypeElement=NULL;}
};

class CFormation:virtual public CSystemObjectBase,virtual public IFormation,virtual public IEntityEvents
{
    vector<SFormationElementRunTimeInfo>  m_vElementRunTimeInfo; // indica el numero de entidades creadas y el ultimo tiempo de creacion de cada elemento del tipo de formacion
    CFormationType                       *m_pType;
    CVector                               m_vPosition;
    set<IEntity*>                         m_sEntities;
    bool                                  m_bAllUnitsCreated;
	unsigned int						  m_nActivationTime;

public:
    void AddEntity(IEntity *piEntity);

    //IFormation
    bool ProcessFrame(unsigned int dwCurrentTime,double dInterval);

    //IEntityEvents
    void OnRemoved(IEntity *pUnit);
    void OnKilled(IEntity *pUnit);

    CFormation(CFormationType *pType,CVector vPosition);
    ~CFormation();
};
