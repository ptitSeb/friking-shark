#include "./stdafx.h"
#include "ParticleSystems.h"
#include "ParticleEmitterType.h"

CParticleEmitterType::CParticleEmitterType()
{
    m_dwEmitStartTime=0;
    m_dwEmitEndTime=0;
    m_dwParticleDuration=1000;
    m_dStartRate=1;
    m_dEndRate=1;
    m_dwMovementType=PHYSIC_MOVE_TYPE_FLY;
    m_vMinDirection=Origin;
    m_vMaxDirection=Origin;
    m_dMinVelocity=0;
    m_dMaxVelocity=0;
    m_dMinAngularVelocity=0;
    m_dMaxAngularVelocity=0;
    m_dMinAngle=0;
    m_dMaxAngle=0;
    m_dwParticleCount=0;
    m_bFixedPositionOnParent=false;
}

CParticleEmitterType::~CParticleEmitterType(void)
{
}

IParticleEmitter *CParticleEmitterType::CreateInstance(unsigned int dwCurrentTime)
{
    CParticleEmitter *pEmitter=new CParticleEmitter(this,dwCurrentTime);
    return pEmitter;
}

CParticleEmitter::CParticleEmitter(CParticleEmitterType *pType,unsigned int dwCurrentTime)
{
    m_pType=pType;
    m_bActive=1;
    m_dwLastEmitTime=0;
    m_dwEmitStartTime=dwCurrentTime+m_pType->m_dwEmitStartTime;
    m_dwEmitEndTime=m_pType->m_dwEmitEndTime?dwCurrentTime+m_pType->m_dwEmitEndTime:0;
    m_dwParticlesEmitted=0;
}

bool CParticleEmitter::IsActive()
{
    return m_bActive;
}

string CParticleEmitter::GetName()
{
    return m_pType->m_sName;
}

void CParticleEmitter::ProcessFrame(IParticleSystem *piSystem,unsigned int dwCurrentTime,double dInterval)
{
    if(!m_bActive){return;}
    if(m_dwEmitStartTime && dwCurrentTime<m_dwEmitStartTime){return;}
    if(m_dwEmitEndTime && dwCurrentTime>m_dwEmitEndTime){m_bActive=false;return;}
    if(m_pType->m_dwParticleCount!=0 && m_dwParticlesEmitted>=m_pType->m_dwParticleCount){m_bActive=false;return;}

    double dLifeSpent=((double)dwCurrentTime-(double)m_dwEmitStartTime)/((double)m_dwEmitEndTime-(double)m_dwEmitStartTime);

    CVector vForward,vRight,vUp;
    piSystem->GetVectors(vForward,vRight,vUp);

    unsigned int dwParticlesToEmit=0;
    if(m_pType->m_dwParticleCount!=0)
    {
        // emision de un numero determinado de particulas
        if(m_dwEmitEndTime==0 || m_dwEmitEndTime==m_dwEmitStartTime)
        {
            // si no hay tiempo de fin o el tiempo inicio=tiempo fin se emiten todas las particulas instantaneamente
            dwParticlesToEmit=m_pType->m_dwParticleCount-m_dwParticlesEmitted;
        }
        else
        {
            // en caso contrario se emiten las particulas que queden.
            dwParticlesToEmit=(unsigned int)(dLifeSpent*(double)m_pType->m_dwParticleCount-m_dwParticlesEmitted);
        }
    }
    else if(m_pType->m_dStartRate>0 || m_pType->m_dEndRate>0)
    {
        // emision por ritmo de emision
        double dCurrentRate=m_pType->m_dStartRate;
        if(m_dwEmitEndTime)
        {
            dCurrentRate=m_pType->m_dStartRate+(m_pType->m_dEndRate-m_pType->m_dStartRate)*dLifeSpent;
        }
        dwParticlesToEmit=(unsigned int)(dCurrentRate*dInterval);
        if(m_dwLastEmitTime==0){m_dwLastEmitTime=dwCurrentTime;}
        if(dwParticlesToEmit==0 && m_dwLastEmitTime!=0)
        {
            dwParticlesToEmit=(unsigned int)(((double)(dwCurrentTime-m_dwLastEmitTime)/1000.0)*dCurrentRate);
        }
    }
    for(unsigned int x=0;x<dwParticlesToEmit;x++)
    {
        m_dwLastEmitTime=dwCurrentTime;

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
			pParticle->m_PhysicInfo.vPosition=piSystem->GetPosition();
			pParticle->m_PhysicInfo.vPosition+=vForward*pParticle->m_vPositionOnParent.c[0];
			pParticle->m_PhysicInfo.vPosition+=vUp*pParticle->m_vPositionOnParent.c[1];
			pParticle->m_PhysicInfo.vPosition+=vRight*pParticle->m_vPositionOnParent.c[2];
			int c;
			for(c=0;c<3;c++)
			{
				pParticle->m_PhysicInfo.vVelocity.c[c]=(m_pType->m_vMinDirection.c[c]+(m_pType->m_vMaxDirection.c[c]-m_pType->m_vMinDirection.c[c])*drand());
			}

			pParticle->m_PhysicInfo.vVelocity*=(m_pType->m_dMinVelocity+(m_pType->m_dMaxVelocity-m_pType->m_dMinVelocity)*drand());
			pParticle->m_PhysicInfo.vAngleVelocity.c[ROLL]=(m_pType->m_dMinAngularVelocity+(m_pType->m_dMaxAngularVelocity-m_pType->m_dMinAngularVelocity)*drand());
			pParticle->m_PhysicInfo.vAngles.c[ROLL]=(m_pType->m_dMinAngle+(m_pType->m_dMaxAngle-m_pType->m_dMinAngle)*drand());
			piSystem->AddParticle(pParticle);
		}
	}
    m_dwParticlesEmitted+=dwParticlesToEmit;
}
void CParticleEmitter::Deactivate()
{
  if(m_pType->m_dwEmitEndTime!=0){return;}
  if(m_pType->m_dwParticleCount!=0 && m_dwParticlesEmitted<m_pType->m_dwParticleCount){return;}
  m_bActive=false;
}

