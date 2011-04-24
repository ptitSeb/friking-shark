#include "./stdafx.h"
#include "ParticleSystems.h"
#include "ParticleEmitterModifierType.h"

CParticleEmitterModifierType::CParticleEmitterModifierType(void)
{
}

CParticleEmitterModifierType::~CParticleEmitterModifierType(void)
{
}

unsigned CParticleEmitterModifierType::GetEmitterNameCount(){return (unsigned)m_dEmitterNames.size();}
string   CParticleEmitterModifierType::GetEmitterName(unsigned index){return m_dEmitterNames[index];}
