#pragma once

struct SRoutePoint
{
public:

    bool        bAbsolutePoint; // si es falso son valores de 0 a 1 relativos al plano de juego aereo
    CVector     vPosition;
};

BEGIN_STRUCT_PROPS(SRoutePoint)
    PROP(vPosition,"Posicion")
    PROP_VALUE_FLAGS(bAbsolutePoint,"Absoluto",true,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class CRoute:public IRoute
{
    deque<SRoutePoint>   m_dPoints;
    bool                 m_bCyclic;


public:
	CVector GetAbsoluteRoutePoint(SRoutePoint *p);

    CRoute();

	CVector     GetRelativePoint(unsigned int nIndex);
	unsigned    GetPointCount();
	CVector		GetAbsolutePoint(unsigned nIndex);
	CVector     GetPoint(unsigned nIndex);
    CVector     GetDirection(unsigned nSection);
    unsigned    GetNextPointIndex(unsigned nIndex);
    void        ProcessPoint(IEntity *piEntity,DWORD dwCurrentFrame,double dTimeFraction);

    BEGIN_PROP_MAP(CRoute)
        PROP(m_dPoints,"Puntos")
        PROP(m_bCyclic,"Ciclica")
    END_PROP_MAP()
	CVector GetPlayAreaElementRoutePoint(CVector vPlayAreaElementPos,SRoutePoint *pPoint);
	CVector GetPlayAreaElementPoint(CVector vPlayAreaElementPos,unsigned nIndex);
};

DECLARE_SERIALIZABLE(CRoute);

struct SFormationElement
{
public:
    string               m_sEntityType;
    CRoute               m_Route;
    IEntityType         *m_piEntityType;
    DWORD                m_dwEntityCount;
    DWORD                m_dwTimeBetweenEntities;

    SFormationElement(){m_piEntityType=NULL;m_dwTimeBetweenEntities=0;m_dwEntityCount=0;}
    ~SFormationElement(){}
};

BEGIN_STRUCT_PROPS(SFormationElement)
    PROP(m_sEntityType,"Tipo")
    PROP(m_Route,"Ruta")
    PROP_VALUE_FLAGS(m_dwEntityCount,"Numero",1,MRPF_NORMAL|MRPF_OPTIONAL)
    PROP_VALUE_FLAGS(m_dwTimeBetweenEntities,"TiempoEntreEntidades",1000,MRPF_NORMAL|MRPF_OPTIONAL)
END_STRUCT_PROPS()

class CFormationType: virtual public CSystemObjectBase, virtual public IFormationType
{
	bool Unserialize(ISystemPersistencyNode *piNode);

public:
	deque<SFormationElement> m_dElements;

    BEGIN_PROP_MAP(CFormationType)
        PROP(m_dElements,"Elementos")
    END_PROP_MAP()

   IFormation *CreateInstance(CVector vPosition,DWORD dwCurrentTime);
	void DesignRender(IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	double DesignGetRadius();
	CTraceInfo DesignGetTrace( const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    CFormationType();
    ~CFormationType();
};

struct SFormationElementRunTimeInfo
{
    DWORD               m_dwLastEntityTime;
    DWORD               m_dwCreatedEntities;
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
    bool ProcessFrame(DWORD dwCurrentTime,double dInterval);

    //IEntityEvents
    void OnRemoved(IEntity *pUnit);
    void OnKilled(IEntity *pUnit);

    CFormation(CFormationType *pType,CVector vPosition);
    ~CFormation();
};
