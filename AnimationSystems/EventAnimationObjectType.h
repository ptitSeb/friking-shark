#pragma once

class CEventAnimationObjectType: public CAnimationObjectTypeBase
{
public:

    string      m_sEvent;
    string      m_sParams;
    DWORD       m_dwTime;
    
    IAnimationObject *CreateInstance(IAnimation *piAnimation,DWORD dwCurrentTime);

    BEGIN_PROP_MAP(CEventAnimationObjectType)
        PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
        PROP(m_dwTime,"Tiempo")
        PROP(m_sEvent,"Evento")
        PROP_VALUE_FLAGS(m_sParams,"Parametros","",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    CEventAnimationObjectType();
    ~CEventAnimationObjectType();
};


class CEventAnimationObject: public CAnimationObjectBase
{
    bool                        m_bFired;
    CEventAnimationObjectType  *m_pType;
public:

    bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval);

    CEventAnimationObject(CEventAnimationObjectType *pType,IAnimation *piAnimation);
};
