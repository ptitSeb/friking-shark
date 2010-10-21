#pragma once

DECLARE_CUSTOM_WRAPPER1(CParticleSystemTypeWrapper,IParticleSystemType,m_piParticleSystemType);

class CParticleSystemAnimationObjectType: public CAnimationObjectTypeBase
{
public:

    CParticleSystemTypeWrapper m_ParticleSystemType;
    CVector              m_vPosition;
    DWORD                m_dwStartTime;
    DWORD                m_dwEndTime;
    bool                 m_bTrackEntity;

    IAnimationObject *CreateInstance(IAnimation *piAnimation,DWORD dwCurrentTime);

    BEGIN_PROP_MAP(CParticleSystemAnimationObjectType)
        PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
        PROP(m_ParticleSystemType,"SistemaDeParticulas")
        PROP_FLAGS(m_vPosition,"Posicion",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_bTrackEntity,"Seguimiento",false,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwStartTime,"TiempoInicio",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwEndTime,"TiempoFin",0,MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    CParticleSystemAnimationObjectType();
    ~CParticleSystemAnimationObjectType();
};


class CParticleSystemAnimationObject: public CAnimationObjectBase
{
    CParticleSystemAnimationObjectType  *m_pType;
    IParticleSystem                     *m_piParticleSystem;
    bool                                 m_bPositionAndAnglesInitialized;

    void UpdatePositionAndAngles();
    void CheckActivation(DWORD dwCurrentTime);

public:

    void Activate(DWORD dwCurrentTime);
    void Deactivate();

    bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CParticleSystemAnimationObject(CParticleSystemAnimationObjectType *pType,IAnimation *piAnimation);
    ~CParticleSystemAnimationObject();
};
