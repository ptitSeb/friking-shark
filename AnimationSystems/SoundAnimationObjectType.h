#pragma once

DECLARE_CUSTOM_WRAPPER1(CSoundTypeWrapper,ISoundType,m_piSoundType)

class CSoundAnimationObjectType: public CAnimationObjectTypeBase
{
public:

    CSoundTypeWrapper    m_SoundType;
    unsigned int                m_dwStartTime;
    unsigned int                m_dwEndTime;

    IAnimationObject *CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime);

    BEGIN_PROP_MAP(CSoundAnimationObjectType)
        PROP(m_SoundType,"TipoDeSonido")
        PROP_VALUE_FLAGS(m_dwStartTime,"TiempoInicio",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwEndTime,"TiempoFin",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_CLASS_CHAIN(CAnimationObjectTypeBase)
    END_PROP_MAP();

    CSoundAnimationObjectType();
    ~CSoundAnimationObjectType();
};


class CSoundAnimationObject: public CAnimationObjectBase
{
    CSoundAnimationObjectType  *m_pType;
    ISound                     *m_piSound;

public:

    void Activate(unsigned int dwCurrentTime);
    void Deactivate();

    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CSoundAnimationObject(CSoundAnimationObjectType *pType,IAnimation *piAnimation);
    ~CSoundAnimationObject();
};
