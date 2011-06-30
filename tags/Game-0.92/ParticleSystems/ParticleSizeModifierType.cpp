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


#include "./stdafx.h"
#include "ParticleSystems.h"
#include "ParticleModifierType.h"
#include "ParticleSizeModifierType.h"

CParticleSizeModifierType::CParticleSizeModifierType(void)
{
    m_dStartSize=1;
    m_dEndSize=1;
    m_dSizeTransitionStart=0;// de 0 a 1, en tiempo de vida de la particula
    m_dSizeTransitionEnd=1;  // de 0 a 1, en tiempo de vida de la particula
}

CParticleSizeModifierType::~CParticleSizeModifierType(void){}
IParticleModifier *CParticleSizeModifierType::CreateInstance(unsigned int dwCurrentTime){return new CParticleSizeModifier(this);}

CParticleSizeModifier::CParticleSizeModifier(CParticleSizeModifierType *pType)
{
    m_pType=pType;
}

void CParticleSizeModifier::ProcessParticle(IParticle *pParticle,IParticleSystem *pSystem,unsigned int dwCurrentTime,double dInterval)
{
    if(m_sEmitters.size()!=0 && m_sEmitters.find(pParticle->m_piEmiter)==m_sEmitters.end()){return;}
    pParticle->m_dSize=GetTransitionValue(m_pType->m_dSizeTransitionStart,m_pType->m_dStartSize,m_pType->m_dSizeTransitionEnd,m_pType->m_dEndSize,&m_pType->m_dIntermediateSizeTransitions,pParticle->m_dLifeSpent);
}
