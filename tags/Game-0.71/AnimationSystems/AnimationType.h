#pragma once

DECLARE_CUSTOM_WRAPPER1(CAnimationObjectTypeWrapper,IAnimationObjectType,m_piObjectType)

class CAnimationType: virtual public CSystemObjectBase,virtual public IAnimationType
{
public:

    // Propiedades Persistentes

    deque<CAnimationObjectTypeWrapper>  m_dObjects;
    bool                                m_bCyclic;

    IAnimation *CreateInstance(IEntity *piEntity,unsigned int dwCurrentTime);

	void DesignRender( IGenericRender *piRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	double DesignGetRadius();
	CTraceInfo DesignGetTrace(const CVector &vPosition,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    BEGIN_PROP_MAP(CAnimationType);
        PROP(m_dObjects,"Objetos");
        PROP_VALUE_FLAGS(m_bCyclic,"Repetir",false,MRPF_NORMAL|MRPF_OPTIONAL);
    END_PROP_MAP();

    CAnimationType();
    ~CAnimationType();
};

class CAnimation:public IAnimation
{
protected:

    deque<IAnimationObject *>   m_dObjects;
    CAnimationType             *m_pType;
    bool                        m_bActive;
    bool                        m_bFinished;
    IEntity                    *m_piEntity;
    unsigned int                       m_dwCurrentTimeBase;

public:

    void Activate(unsigned int dwCurrentTime);
    void Deactivate();
    bool IsActive();

    bool HasFinished();

    unsigned int GetCurrentTimeBase();

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);
	CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    void                AddObject(IAnimationObject *pObject);
    IAnimationObject    *GetObject(string sName);
    IEntity             *GetEntity();

    CAnimation(CAnimationType *pType,IEntity *piEntity,unsigned int dwCurrentTimeBase);
    virtual ~CAnimation();
};
