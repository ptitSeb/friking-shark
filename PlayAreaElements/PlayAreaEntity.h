#pragma once

#include "Route.h"

class CPlayAreaEntity: virtual public CPlayAreaElementBase,virtual public IPlayAreaEntity,virtual public IEntityEvents
{
    CEntityTypeWrapper   m_EntityType;
    set<IEntity*>        m_sEntities;
	double				m_dRadius;

    C3DSVector          m_vPosition;
    CVector             m_vAngles;
	CRoute				m_Route;

	unsigned int         m_nCreatedEntities;
	unsigned int         m_nEntityCount;
    unsigned int         m_nInterval;
	unsigned int         m_nDelay;

	unsigned int         m_nLastEntityTime;

public:
    
    BEGIN_PROP_MAP(CPlayAreaEntity)
        PROP(m_vPosition,"Posicion")
        PROP(m_vAngles,"Angulos")
        PROP(m_EntityType,"Tipo")
		PROP_FLAGS(m_Route,"Ruta",MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nEntityCount,"Count",1,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nInterval,"Interval",0,MRPF_NORMAL|MRPF_OPTIONAL)
		PROP_VALUE_FLAGS(m_nDelay,"Delay",0,MRPF_NORMAL|MRPF_OPTIONAL)
	END_PROP_MAP()

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    void Activate(unsigned int dwCurrentTime);
    void Deactivate();

    bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pAreaInfo,unsigned int dwCurrentTime,double dInterval);

	void DesignRender(IGenericRender *piRender,bool bSelected);
	CTraceInfo DesignGetTrace(const CVector &p1,const CVector &p2);

    void OnKilled(IEntity *pUnit);
    void OnRemoved(IEntity *pUnit);
 
	//	IPlayAreaEntity

	void SetPosition(const CVector &vPosition);
	void SetAngles(const CVector &vAngles);
	void SetEntityType(IEntityType *piEntityType);
	
	void SetCount(unsigned int nCount);
	void SetDelay(unsigned int nDelay);
	void SetInterval(unsigned int nInterval);

	CVector GetPosition();
	CVector GetAngles();
	void	GetEntityType(IEntityType **piEntityType);
	
	unsigned int GetCount();
	unsigned int GetDelay();
	unsigned int GetInterval();


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
