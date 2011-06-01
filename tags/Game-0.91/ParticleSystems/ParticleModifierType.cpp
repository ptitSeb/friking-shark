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

CParticleModifierType::CParticleModifierType(void)
{
}

CParticleModifierType::~CParticleModifierType(void)
{
}

unsigned CParticleModifierType::GetEmitterNameCount(){return (unsigned)m_dEmitterNames.size();}
string   CParticleModifierType::GetEmitterName(unsigned index){return m_dEmitterNames[index];}

CParticleModifier::CParticleModifier(void)
{
}

void CParticleModifier::AddEmitter(IParticleEmitter *pEmitter){m_sEmitters.insert(pEmitter);}

