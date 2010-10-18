#pragma once

struct ISoundType;
struct ISound;

class CSoundAnimationObjectType: public CAnimationObjectTypeBase
{
public:

    ISoundType          *m_piSoundType;
    string               m_sSoundType;
    DWORD                m_dwStartTime;
    DWORD                m_dwEndTime;

    IAnimationObject *CreateInstance(IAnimation *piAnimation,DWORD dwCurrentTime);

    BEGIN_PROP_MAP(CSoundAnimationObjectType)
        PROP(m_sSoundType,"TipoDeSonido")
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

    void Activate(DWORD dwCurrentTime);
    void Deactivate();

    bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval);
    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CSoundAnimationObject(CSoundAnimationObjectType *pType,IAnimation *piAnimation);
    ~CSoundAnimationObject();
};
