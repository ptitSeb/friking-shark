#pragma once

DECLARE_CUSTOM_WRAPPER1(CParticleEmiterTypeWrapper,IParticleEmitterType,m_piEmitterType)
DECLARE_CUSTOM_WRAPPER1(CParticleModifierTypeWrapper,IParticleModifierType,m_piModifierType)

class CParticleSystemType: virtual public CSystemObjectBase,virtual public IParticleSystemType
{
public:

    // Propiedades Persistentes

    deque<CParticleEmiterTypeWrapper>  m_dEmitters;
    deque<CParticleModifierTypeWrapper> m_dModifiers;

    IParticleSystem *CreateInstance(DWORD dwCurrentTime);

    BEGIN_PROP_MAP(CParticleSystemType);
        PROP(m_dEmitters,"Emisores");
        PROP(m_dModifiers,"Modificadores");
    END_PROP_MAP();

    CParticleSystemType();
    ~CParticleSystemType();
};

class CParticleSystem:public IParticleSystem
{
protected:

    deque<IParticleEmitter *>   m_dEmitters;
    deque<IParticleModifier *>  m_dModifiers;
    list<IParticle*>            m_lParticles;
    CParticleSystemType        *m_pType;
    CVector                     m_vPosition;
    CVector                     m_vAngles;
    CVector                     m_vForward;
    CVector                     m_vRight;
    CVector                     m_vUp;
    bool                        m_bAnglesDirty;

public:

    void DeactivateAllEmitters();

    void AddEmitter(IParticleEmitter    *pParticleEmitter);
    void AddModifier(IParticleModifier  *pParticleModifier);
    void AddParticle(IParticle          *pParticle);

    IParticleEmitter *GetEmitter(string sName);

    CVector GetPosition();
    void    SetPosition(CVector vPosition);
    CVector GetAngles();
    void    SetAngles(CVector vAngles);

    void    GetVectors(CVector &vForward,CVector &vRight,CVector &vUp);

    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);
    bool ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval);

    CParticleSystem(CParticleSystemType *pType);
    virtual ~CParticleSystem();
};