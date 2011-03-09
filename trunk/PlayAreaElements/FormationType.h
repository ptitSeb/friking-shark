#pragma once

#include "Route.h"

struct SFormationElement
{
public:
    CRoute               m_Route;
	CEntityTypeWrapper   m_EntityType;
    unsigned int         m_nEntityCount;
    unsigned int         m_nTimeInterval;

    SFormationElement(){m_nTimeInterval=0;m_nEntityCount=0;}
    ~SFormationElement(){}
};

BEGIN_STRUCT_PROPS(SFormationElement)
	PROP(m_EntityType,"Tipo")
    PROP(m_Route,"Ruta")
    PROP_VALUE_FLAGS(m_nEntityCount,"Numero",1,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_nTimeInterval,"TiempoEntreEntidades",1000,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class CFormationType: virtual public CSystemObjectBase, virtual public IFormationType,virtual public IFormationTypeDesign
{
public:
	vector<SFormationElement> m_vElements;

    BEGIN_PROP_MAP(CFormationType)
        PROP(m_vElements,"Elementos")
    END_PROP_MAP()

   IFormation *CreateInstance(CVector vPosition,unsigned int dwCurrentTime);
	void DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
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
