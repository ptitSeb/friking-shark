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
