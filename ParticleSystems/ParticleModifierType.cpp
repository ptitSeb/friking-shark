#include "StdAfx.h"
#include ".\particlesystems.h"
#include ".\particlemodifiertype.h"

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

