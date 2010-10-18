#include "StdAfx.h"
#include ".\particlesystems.h"
#include ".\Pointparticletype.h"

CPointParticleType::CPointParticleType()
{
  m_bCastShadow=false;
  m_dwBlendOp1=GL_SRC_ALPHA;
  m_dwBlendOp2=GL_ONE_MINUS_SRC_ALPHA;
}

CPointParticleType::~CPointParticleType()
{
}

IParticle *CPointParticleType::CreateInstance(IParticleEmitter *piEmiter,DWORD dwCurrentTime)
{
    CPointParticle *pParticle=new CPointParticle(this,piEmiter);
    return pParticle;
}

CPointParticle::CPointParticle(CPointParticleType *pType,IParticleEmitter *piEmiter)
{
    m_piEmiter=piEmiter;
    m_pType=pType;
}

void CPointParticle::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
	piRender->DeactivateShadowReception();
	piRender->ActivateBlending();
	piRender->SetBlendingFunction(m_pType->m_dwBlendOp1,m_pType->m_dwBlendOp2);
	piRender->RenderPoint(m_PhysicInfo.vPosition,m_dSize,m_vColor,m_dAlpha);
	piRender->SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	piRender->DeactivateBlending();
	piRender->ActivateShadowReception();
}

