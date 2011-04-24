#include "./stdafx.h"
#include "ParticleSystems.h"
#include "ParticleEmitterModifierType.h"
#include "ParticleWhirlpoolModifierType.h"

CParticleWhirlpoolModifierType::CParticleWhirlpoolModifierType(void)
{
	m_dStartRadius=0;
	m_dEndRadius=0;
	m_nStartTime=0;
	m_nEndTime=0;
	m_nLoops=0;
	m_dStartAngle=0;
}

CParticleWhirlpoolModifierType::~CParticleWhirlpoolModifierType(void){}
IParticleEmitterModifier *CParticleWhirlpoolModifierType::CreateInstance(unsigned int dwCurrentTime){return new CParticleWhirlpoolModifier(this,dwCurrentTime);}

CParticleWhirlpoolModifier::CParticleWhirlpoolModifier(CParticleWhirlpoolModifierType *pType,unsigned long dwCurrentTime)
{
    m_pType=pType;
	m_nCreationTime=dwCurrentTime;
}

void CParticleWhirlpoolModifier::AddEmitter(IParticleEmitter *pEmitter)
{
	if(m_mEmitters.find(pEmitter)==m_mEmitters.end())
	{
		unsigned int nIndex=m_mEmitters.size();
		m_mEmitters[pEmitter]=nIndex;
	}	
}

void CParticleWhirlpoolModifier::ProcessEmitter(IParticleEmitter *piParticleEmitter,IParticleSystem *piSystem,unsigned int dwCurrentTime,double dInterval)
{
	if((m_pType->m_nStartTime+m_nCreationTime)>dwCurrentTime || (m_pType->m_nEndTime+m_nCreationTime)<dwCurrentTime){return;}
	if(m_mEmitters.size()==0){return;}
	if(m_mEmitters.find(piParticleEmitter)==m_mEmitters.end()){return;}
	
	double dElapsedTime=((double)(dwCurrentTime-(m_pType->m_nStartTime+m_nCreationTime)));
	double dTotalDuration=((double)(m_pType->m_nEndTime-m_pType->m_nStartTime));
	double dElapsedFraction=dElapsedTime/dTotalDuration;

	unsigned int nIndex=m_mEmitters[piParticleEmitter];
	double dStartAngle=(360/m_mEmitters.size())*nIndex;
	double dTimeAngle=m_pType->m_dStartAngle+dStartAngle+360.0*dElapsedFraction*((double)m_pType->m_nLoops);
	
	CMatrix m;
	m.R(m_pType->m_vRotationAxis,DegreesToRadians(dTimeAngle));
	CVector vNewDirection(1,0,0);
	vNewDirection*=m;
	
	double dCurrentRadius=(m_pType->m_dEndRadius-m_pType->m_dStartRadius)*dElapsedFraction+m_pType->m_dStartRadius;
	piParticleEmitter->SetPosition(vNewDirection*dCurrentRadius);
}
