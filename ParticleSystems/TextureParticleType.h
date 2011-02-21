#pragma once

#include <GL/GL.h>

DECLARE_CUSTOM_WRAPPER1(CTextureWrapper,IGenericTexture,m_piTexture)

class CTextureParticleType: virtual public CSystemObjectBase,virtual public IParticleType
{
public:

    CTextureWrapper m_Texture;
    bool        m_bCastShadow;
    DWORD       m_dwBlendOp1;
    DWORD       m_dwBlendOp2;

    IParticle *CreateInstance(IParticleEmitter *piEmiter,DWORD dwCurrentTime);

    BEGIN_PROP_MAP(CTextureParticleType);
        PROP(m_Texture,"Textura")
        PROP_VALUE_FLAGS(m_bCastShadow,"EmiteSombras",false,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwBlendOp1,"OperadorMezcla1",GL_SRC_ALPHA,MRPF_NORMAL|MRPF_OPTIONAL)
        PROP_VALUE_FLAGS(m_dwBlendOp2,"OperadorMezcla2",GL_ONE_MINUS_SRC_ALPHA,MRPF_NORMAL|MRPF_OPTIONAL)
    END_PROP_MAP();

    CTextureParticleType();
    ~CTextureParticleType();
};

class CTextureParticle: public IParticle
{
    CTextureParticleType *m_pType;

public:

    void CustomRender(IGenericRender *piRender,IGenericCamera *piCamera);

    CTextureParticle(CTextureParticleType *pType,IParticleEmitter *pEmiter);
};
