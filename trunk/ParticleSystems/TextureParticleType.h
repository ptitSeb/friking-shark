//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


#pragma once

#include <GL/gl.h>

DECLARE_CUSTOM_WRAPPER1(CTextureWrapper,IGenericTexture,m_piTexture)

class CTextureParticleType: virtual public CSystemObjectBase,virtual public IParticleType
{
public:

    CTextureWrapper m_Texture;
    bool        m_bCastShadow;
    unsigned int       m_dwBlendOp1;
    unsigned int       m_dwBlendOp2;

    IParticle *CreateInstance(IParticleEmitter *piEmiter,unsigned int dwCurrentTime);

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
