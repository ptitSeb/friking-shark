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
#include "ParticleSphereEmitterType.h"

CParticleSphereEmitterType::CParticleSphereEmitterType()
{
	m_dSphereMinPitch=0;
	m_dSphereMaxPitch=90;
	m_dSphereMinRadius=20;
	m_dSphereMaxRadius=20;
	m_dSphereDensity=20;
	m_dSphereMinExitVelocity=10;
	m_dSphereMaxExitVelocity=10;
	m_dwEmitStartTime=0;
    m_dwEmitEndTime=0;
    m_dwParticleDuration=1000;
    m_dStartRate=1;
    m_dEndRate=1;
    m_dwMovementType=PHYSIC_MOVE_TYPE_FLY;
    m_dMinVelocity=0;
    m_dMaxVelocity=0;
    m_dMinAngularVelocity=0;
    m_dMaxAngularVelocity=0;
    m_dMinAngle=0;
    m_dMaxAngle=0;
    m_dwParticleCount=0;
    m_bFixedPositionOnParent=false;
}

CParticleSphereEmitterType::~CParticleSphereEmitterType(void)
{
}

IParticleEmitter *CParticleSphereEmitterType::CreateInstance(unsigned int dwCurrentTime)
{
    CParticleSphereEmitter *pEmitter=new CParticleSphereEmitter(this,dwCurrentTime);
	pEmitter->SetPosition(m_vPosition);
    return pEmitter;
}

CParticleSphereEmitter::CParticleSphereEmitter(CParticleSphereEmitterType *pType,unsigned int dwCurrentTime)
{
    m_pType=pType;
    m_bActive=1;
    m_dwLastEmitTime=0;
    m_dwEmitStartTime=dwCurrentTime+m_pType->m_dwEmitStartTime;
    m_dwEmitEndTime=m_pType->m_dwEmitEndTime?dwCurrentTime+m_pType->m_dwEmitEndTime:0;
    m_dwEmittedCycles=0;
}

bool CParticleSphereEmitter::IsActive()
{
    return m_bActive;
}

string CParticleSphereEmitter::GetName()
{
    return m_pType->m_sName;
}

void CParticleSphereEmitter::ProcessFrame(IParticleSystem *piSystem,unsigned int dwCurrentTime,double dInterval)
{
    if(!m_bActive){return;}
    if(m_dwEmitStartTime && dwCurrentTime<m_dwEmitStartTime){return;}
    if(m_dwEmitEndTime && dwCurrentTime>m_dwEmitEndTime)
	{
		// Fixes Issue 6. Sometimes sphere particle emitter emits no particle
		bool bMinimunEmissionPending=(m_dwEmittedCycles==0 && (m_pType->m_dStartRate>0 || m_pType->m_dEndRate>0));
		if(!bMinimunEmissionPending)
		{
			m_bActive=false;
			return;
		}
	}
    if(m_pType->m_dwParticleCount!=0 && m_dwEmittedCycles>=m_pType->m_dwParticleCount){m_bActive=false;return;}

    double dLifeSpent=((double)dwCurrentTime-(double)m_dwEmitStartTime)/((double)m_dwEmitEndTime-(double)m_dwEmitStartTime);

	unsigned long dwCyclesToEmitt=0;
	
    CVector vForward,vRight,vUp;
    piSystem->GetVectors(vForward,vRight,vUp);
	
	CVector vCurrentWorldPosition=piSystem->GetPosition();
	vCurrentWorldPosition+=m_vPosition;
	if(m_dwEmittedCycles==0){m_vLastWorldPosition=vCurrentWorldPosition;}
	
    if(m_pType->m_dStartRate>0 || m_pType->m_dEndRate>0)
    {
        // emision por ritmo de emision
        double dCurrentRate=m_pType->m_dStartRate;
        if(m_dwEmitEndTime)
        {
            dCurrentRate=m_pType->m_dStartRate+(m_pType->m_dEndRate-m_pType->m_dStartRate)*dLifeSpent;
        }
        if(m_dwLastEmitTime==0){m_dwLastEmitTime=dwCurrentTime;}
		dwCyclesToEmitt=(unsigned int)(((double)(dwCurrentTime-m_dwLastEmitTime)/1000.0)*dCurrentRate);
	}
    CVector vEmitterPositionIncrement=vCurrentWorldPosition-m_vLastWorldPosition;
	if(dwCyclesToEmitt)
	{
		vEmitterPositionIncrement/=(double)dwCyclesToEmitt;
		m_vLastWorldPosition=vCurrentWorldPosition;
		m_dwLastEmitTime=dwCurrentTime;
	}
	double dPitchIncrement=1.0/m_pType->m_dSphereDensity;
	
	CVector vParticleOffset=vEmitterPositionIncrement;
	for(unsigned int x=0;x<dwCyclesToEmitt;x++)
    {
		for(double dPitch=m_pType->m_dSphereMinPitch;dPitch<m_pType->m_dSphereMaxPitch;dPitch+=dPitchIncrement)
		{
			double dCosine=cos(DegreesToRadians(dPitch));
			if(dCosine<0.001){continue;}
			double dYawDensity=m_pType->m_dSphereDensity*dCosine;
			double dYawIncrement=1.0/dYawDensity;
			for(double dYaw=0;dYaw<360.0;dYaw+=dYawIncrement)
			{
				IParticle *pParticle=m_pType->m_ParticleType.m_piParticleType->CreateInstance(this,dwCurrentTime);
				if(pParticle)
				{
					pParticle->m_dwStartTime=dwCurrentTime;
					if(m_pType->m_dwParticleDuration){pParticle->m_dwEndTime=dwCurrentTime+m_pType->m_dwParticleDuration;}
					pParticle->m_vPositionOnParent.c[0]=m_pType->m_vMinPosition.c[0]+(m_pType->m_vMaxPosition.c[0]-m_pType->m_vMinPosition.c[0])*drand();
					pParticle->m_vPositionOnParent.c[1]=m_pType->m_vMinPosition.c[1]+(m_pType->m_vMaxPosition.c[1]-m_pType->m_vMinPosition.c[1])*drand();
					pParticle->m_vPositionOnParent.c[2]=m_pType->m_vMinPosition.c[2]+(m_pType->m_vMaxPosition.c[2]-m_pType->m_vMinPosition.c[2])*drand();
					pParticle->m_bFixedPositionOnParent=m_pType->m_bFixedPositionOnParent;
					pParticle->m_PhysicInfo.dwMoveType=m_pType->m_dwMovementType;
					pParticle->m_PhysicInfo.dwBoundsType=PHYSIC_BOUNDS_TYPE_NONE;
					pParticle->m_PhysicInfo.vPosition=vCurrentWorldPosition;
					pParticle->m_PhysicInfo.vPosition+=vParticleOffset;
					pParticle->m_PhysicInfo.vPosition+=vForward*pParticle->m_vPositionOnParent.c[0];
					pParticle->m_PhysicInfo.vPosition+=vUp*pParticle->m_vPositionOnParent.c[1];
					pParticle->m_PhysicInfo.vPosition+=vRight*pParticle->m_vPositionOnParent.c[2];

					CVector vDirection;
					CVector vAngles(dYaw,dPitch,0);
					VectorsFromAngles(vAngles,&vDirection);
			
						
					vParticleOffset+=vEmitterPositionIncrement;
					
					int c;
					for(c=0;c<3;c++)
					{
						pParticle->m_PhysicInfo.vVelocity.c[c]+=(m_pType->m_vMinDirection.c[c]+(m_pType->m_vMaxDirection.c[c]-m_pType->m_vMinDirection.c[c])*drand());
					}

					pParticle->m_PhysicInfo.vVelocity*=(m_pType->m_dMinVelocity+(m_pType->m_dMaxVelocity-m_pType->m_dMinVelocity)*drand());
					pParticle->m_vWhirlPoolVelocity=pParticle->m_PhysicInfo.vVelocity;
					pParticle->m_PhysicInfo.vAngleVelocity.c[ROLL]=(m_pType->m_dMinAngularVelocity+(m_pType->m_dMaxAngularVelocity-m_pType->m_dMinAngularVelocity)*drand());
					pParticle->m_PhysicInfo.vAngles.c[ROLL]=(m_pType->m_dMinAngle+(m_pType->m_dMaxAngle-m_pType->m_dMinAngle)*drand());
					
					double dRadius=(m_pType->m_dSphereMinRadius+drand()*(m_pType->m_dSphereMaxRadius-m_pType->m_dSphereMinRadius));
					pParticle->m_PhysicInfo.vPosition+=vDirection*dRadius;
					pParticle->m_PhysicInfo.vVelocity+=vDirection*(m_pType->m_dSphereMinExitVelocity+drand()*(m_pType->m_dSphereMaxExitVelocity-m_pType->m_dSphereMinExitVelocity));
					piSystem->AddParticle(pParticle);
				}
			}
		}
	}
	m_dwEmittedCycles+=dwCyclesToEmitt;
}
void CParticleSphereEmitter::Deactivate()
{
  if(m_pType->m_dwEmitEndTime!=0){return;}
  if(m_pType->m_dwParticleCount!=0 && m_dwEmittedCycles<m_pType->m_dwParticleCount){return;}
  m_bActive=false;
}

CVector CParticleSphereEmitter::GetPosition(){return m_vPosition;}
void    CParticleSphereEmitter::SetPosition(CVector vPosition){m_vPosition=vPosition;}

