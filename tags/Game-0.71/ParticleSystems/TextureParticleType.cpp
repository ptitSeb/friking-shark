#include "./stdafx.h"
#include "ParticleSystems.h"
#include "TextureParticleType.h"

CTextureParticleType::CTextureParticleType()
{
  m_bCastShadow=false;
  m_dwBlendOp1=GL_SRC_ALPHA;
  m_dwBlendOp2=GL_ONE_MINUS_SRC_ALPHA;
}

CTextureParticleType::~CTextureParticleType()
{
}

IParticle *CTextureParticleType::CreateInstance(IParticleEmitter *piEmiter,unsigned int dwCurrentTime)
{
  CTextureParticle *pParticle=new CTextureParticle(this,piEmiter);
  return pParticle;
}

CTextureParticle::CTextureParticle(CTextureParticleType *pType,IParticleEmitter *piEmiter)
{
  m_piEmiter=piEmiter;
  m_pType=pType;
}

void CTextureParticle::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
	piRender->DeactivateShadowReception();
	piRender->ActivateBlending();
	piRender->SetBlendingFunction(m_pType->m_dwBlendOp1,m_pType->m_dwBlendOp2);
	piRender->RenderParticle(m_pType->m_Texture.m_piTexture,m_PhysicInfo.vPosition,m_PhysicInfo.vAngles,m_dSize,m_dSize,m_vColor,m_dAlpha);
	piRender->SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	piRender->DeactivateBlending();
	piRender->ActivateShadowReception();
}
