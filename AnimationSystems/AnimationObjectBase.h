#pragma once

class CAnimationObjectTypeBase: virtual public CSystemObjectBase,virtual public IAnimationObjectType
{
public:

    string m_sObjectName;

	void DesignRender(IGenericRender *pRender,CVector &vPosition,CVector &vAngles,bool bSelected);
	void DesignGetBBox(CVector *pvMins,CVector *pvMaxs);
	double DesignGetRadius();
	CTraceInfo DesignGetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    BEGIN_PROP_MAP(CAnimationObjectTypeBase);
        PROP_VALUE_FLAGS(m_sObjectName,"Nombre","",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    CAnimationObjectTypeBase();
};

class CAnimationObjectBase: public IAnimationObject
{
protected:

    bool                        m_bActive;
    IAnimation                  *m_piAnimation;
    CAnimationObjectTypeBase    *m_pTypeBase;

public:

    string      GetName();

    void        Activate(DWORD dwCurrentTime);
    void        Deactivate();
    bool        IsActive();
    IAnimation *GetAnimation();

	CTraceInfo GetTrace(const CVector &vOrigin,const CVector &vAngles,const CVector &p1,const CVector &p2 );

    bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CAnimationObjectBase(CAnimationObjectTypeBase *pType,IAnimation *piAnimation);
};
