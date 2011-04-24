#pragma once

DECLARE_CUSTOM_WRAPPER1(CParticleEmiterTypeWrapper,IParticleEmitterType,m_piEmitterType)
DECLARE_CUSTOM_WRAPPER1(CParticleModifierTypeWrapper,IParticleModifierType,m_piModifierType)
DECLARE_CUSTOM_WRAPPER1(CParticleEmitterModifierTypeWrapper,IParticleEmitterModifierType,m_piEmitterModifierType)

class CParticleSystemType: virtual public CSystemObjectBase,virtual public IParticleSystemType
{
public:

    // Propiedades Persistentes

    deque<CParticleEmiterTypeWrapper>  m_dEmitters;
    deque<CParticleModifierTypeWrapper> m_dModifiers;
	deque<CParticleEmitterModifierTypeWrapper> m_dEmitterModifiers;

    IParticleSystem *CreateInstance(unsigned int dwCurrentTime);

    BEGIN_PROP_MAP(CParticleSystemType);
        PROP(m_dEmitters,"Emisores");
        PROP(m_dModifiers,"Modificadores");
		PROP_FLAGS(m_dEmitterModifiers,"EmitterModifiers",MRPF_NORMAL|MRPF_OPTIONAL);
	END_PROP_MAP();

    CParticleSystemType();
    ~CParticleSystemType();
};

class CParticleSystem:public IParticleSystem
{
protected:

    deque<IParticleEmitter *>   m_dEmitters;
	deque<IParticleModifier *>  m_dModifiers;
	deque<IParticleEmitterModifier *>   m_dEmitterModifiers;
	list<IParticle*>            m_lParticles;
    CParticleSystemType        *m_pType;
    CVector                     m_vPosition;
    CVector                     m_vAngles;
    CVector                     m_vForward;
    CVector                     m_vRight;
    CVector                     m_vUp;
    bool                        m_bAnglesDirty;
    bool                        m_bAllEmittersDeactivated;

public:

    void DeactivateAllEmitters();

	void AddEmitter(IParticleEmitter    *pParticleEmitter);
	void AddEmitterModifier(IParticleEmitterModifier    *pParticleEmitterModifier);
	void AddModifier(IParticleModifier  *pParticleModifier);
    void AddParticle(IParticle          *pParticle);

    IParticleEmitter *GetEmitter(string sName);

    CVector GetPosition();
    void    SetPosition(CVector vPosition);
    CVector GetAngles();
    void    SetAngles(CVector vAngles);

    void    GetVectors(CVector &vForward,CVector &vRight,CVector &vUp);

    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);
    bool ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval);

    CParticleSystem(CParticleSystemType *pType);
    virtual ~CParticleSystem();
};