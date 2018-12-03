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


// ParticleSystems.cpp : Defines the entry point for the DLL application.
//

#include "./stdafx.h"
#include "ParticleSystems.h"
#include "ParticleModifierType.h"
#include "ParticleEmitterModifierType.h"
#include "ParticleColorModifierType.h"
#include "ParticleSizeModifierType.h"
#include "ParticleWhirlpoolModifierType.h"
#include "ParticleEmitterType.h"
#include "ParticleSphereEmitterType.h"
#include "ParticleSystemType.h"
#include "TextureParticleType.h"
#include "LightParticleType.h"
#include "PointParticleType.h"

BEGIN_SYSTEM_MODULE(ParticleSystems)
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleSystemType,"ParticleSystemType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleEmitterType,"ParticleEmitterType")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleSphereEmitterType,"ParticleSphereEmitter")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleSizeModifierType,"ParticleSizeModifierType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleColorModifierType,"ParticleColorModifierType")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CParticleWhirlpoolModifierType,"ParticleWhirlpoolModifierType")
	SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CTextureParticleType,"TextureParticleType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CLightParticleType,"LightParticleType")
    SYSTEM_MODULE_CLASS_FACTORY_ENTRY(CPointParticleType,"PointParticleType")
END_SYSTEM_MODULE(ParticleSystems)

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
	m_ePositionOnParentReferenceSystem=eParticlePositionReferenceSystem_Relative;
	m_bFixedPositionOnParent=false;
}

