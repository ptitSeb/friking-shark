#pragma once

class CParticleEmitterModifierType: virtual public CSystemObjectBase,virtual public IParticleEmitterModifierType
{
public:
    // Propiedades Persistentes

    string          m_sName;
    deque<string>   m_dEmitterNames;

    BEGIN_PROP_MAP(CParticleEmitterModifierType);
        PROP_FLAGS(m_sName,"Nombre",MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_FLAGS(m_dEmitterNames,"Emisores",MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    virtual unsigned GetEmitterNameCount();
    virtual string   GetEmitterName(unsigned index);

    CParticleEmitterModifierType();
    ~CParticleEmitterModifierType();
};

