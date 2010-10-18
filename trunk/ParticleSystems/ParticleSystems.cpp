// ParticleSystems.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ParticleSystems.h"
#include "ParticleModifierType.h"
#include "ParticleColorModifierType.h"
#include "ParticleSizeModifierType.h"
#include "ParticleEmitterType.h"
#include "ParticleSystemType.h"
#include "TextureParticleType.h"
#include "LightParticleType.h"
#include "PointParticleType.h"

BEGIN_SYSTEM_MODULE()
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleSystemType,"ParticleSystemType");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleEmitterType,"ParticleEmitterType");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleSizeModifierType,"ParticleSizeModifierType");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleColorModifierType,"ParticleColorModifierType");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CTextureParticleType,"TextureParticleType");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CLightParticleType,"LightParticleType");
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPointParticleType,"PointParticleType");
END_SYSTEM_MODULE();

IParticle::IParticle()
{
    m_piEmiter=NULL;
    m_dwStartTime=0;
    m_dwEndTime=0;
    m_dLifeSpent=0; 
    m_bActive=1;
    m_bNew=1; 
    m_vColor=CVector(1,1,1);
    m_dAlpha=1.0;
    m_dSize=1.0;
}