#pragma once

struct SSizeTransition
{
    double  dTime;// de 0 a 1, en tiempo de vida de la particula
    double  value;
};

BEGIN_STRUCT_PROPS(SSizeTransition)
    PROP(dTime,"Tiempo")
    PROP(value,"Valor")
END_STRUCT_PROPS()

class CParticleSizeModifierType:public CParticleModifierType
{
public:

    // Propiedades Persistentes

    double  m_dStartSize;
    double  m_dEndSize;
    double  m_dSizeTransitionStart;// de 0 a 1, en tiempo de vida de la particula
    double  m_dSizeTransitionEnd;  // de 0 a 1, en tiempo de vida de la particula
    deque<SSizeTransition> m_dIntermediateSizeTransitions;

    IParticleModifier *CreateInstance(unsigned int dwCurrentTime);

    BEGIN_PROP_MAP(CParticleSizeModifierType)
        PROP_CLASS_CHAIN(CParticleModifierType)
        PROP_VALUE_FLAGS(m_dStartSize,"TamañoInicial",1,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dEndSize,"TamañoFinal",1,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dSizeTransitionStart,"InicioTransicionTamaño",0,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dSizeTransitionEnd,"FinTransicionTamaño",1,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_FLAGS(m_dIntermediateSizeTransitions,"TransicionesIntermediasTamaño",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP()

    CParticleSizeModifierType();
    ~CParticleSizeModifierType();
};

class CParticleSizeModifier:public CParticleModifier
{
    CParticleSizeModifierType *m_pType;
    
public:

    void ProcessParticle(IParticle *pParticle,IParticleSystem *pSystem,unsigned int dwCurrentTime,double dInterval);

    CParticleSizeModifier(CParticleSizeModifierType *pType);
};
