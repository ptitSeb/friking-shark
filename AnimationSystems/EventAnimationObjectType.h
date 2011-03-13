#pragma once

class CEventAnimationObjectType: public CAnimationObjectTypeBase,virtual public IEventAnimationObjectTypeDesign
{
public:

    string      m_sEvent;
    string      m_sParams;
    unsigned int       m_nTime;
    
    IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);

    BEGIN_PROP_MAP(CEventAnimationObjectType)
        PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
		PROP(m_nTime,"Time")
        PROP(m_sEvent,"Event")
        PROP_VALUE_FLAGS(m_sParams,"Parameters","",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

	// IEventAnimationObjectTypeDesign
	void GetConfig(SEventAnimationObjectTypeConfig *pConfig);
	void SetConfig(SEventAnimationObjectTypeConfig *pConfig);
	
	CEventAnimationObjectType();
    ~CEventAnimationObjectType();
};


class CEventAnimationObject: public CAnimationObjectBase
{
    bool                        m_bFired;
    CEventAnimationObjectType  *m_pType;
public:

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);

    CEventAnimationObject(CEventAnimationObjectType *pType,IAnimation *piAnimation);
};
