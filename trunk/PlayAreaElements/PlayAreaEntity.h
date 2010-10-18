#pragma once


class CPlayAreaEntity: virtual public CPlayAreaElementBase,virtual public IPlayAreaEntity,virtual public IEntityEvents
{
    CEntityTypeWrapper   m_EntityType;
    IEntity             *m_piEntity;
	double				m_dRadius;

    C3DSVector          m_vPosition;
    CVector             m_vAngles;

public:
    
    BEGIN_PROP_MAP(CPlayAreaEntity)
        PROP(m_vPosition,"Posicion")
        PROP(m_vAngles,"Angulos")
        PROP(m_EntityType,"Tipo")
    END_PROP_MAP()

    bool Init(std::string sClass,std::string sName,ISystem *piSystem);
    void Destroy();

    void Activate(DWORD dwCurrentTime);
    void Deactivate();

    bool ProcessFrame(CVector vPlayPosition,SPlayAreaInfo *pAreaInfo,DWORD dwCurrentTime,double dInterval);

	void DesignRender(IGenericRender *piRender,bool bSelected);
	CTraceInfo DesignGetTrace(const CVector &p1,const CVector &p2);

    void OnKilled(IEntity *pUnit);
    void OnRemoved(IEntity *pUnit);
 
	//	IPlayAreaEntity

	void SetPosition(CVector &vPosition);
	void SetAngles(CVector &vAngles);
	void SetEntityType(IEntityType *piEntityType);

	CVector GetPosition();
	CVector GetAngles();
	void	GetEntityType(IEntityType **piEntityType);


     CPlayAreaEntity();
    ~CPlayAreaEntity();
};
