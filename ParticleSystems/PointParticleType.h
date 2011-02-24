#pragma once
#pragma once

#include <GL/gl.h>

class CPointParticleType: virtual public CSystemObjectBase,virtual public IParticleType
{
public:

    bool        m_bCastShadow;
    DWORD       m_dwBlendOp1;
    DWORD       m_dwBlendOp2;

    IParticle *CreateInstance(IParticleEmitter *piEmiter,DWORD dwCurrentTime);

    BEGIN_PROP_MAP(CPointParticleType);
        PROP_VALUE_FLAGS(m_bCastShadow,"EmiteSombras",false,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwBlendOp1,"OperadorMezcla1",GL_SRC_ALPHA,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwBlendOp2,"OperadorMezcla2",GL_ONE_MINUS_SRC_ALPHA,MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    CPointParticleType();
    ~CPointParticleType();
};

class CPointParticle: public IParticle
{
    CPointParticleType *m_pType;

public:

    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CPointParticle(CPointParticleType *pType,IParticleEmitter *pEmiter);
};
