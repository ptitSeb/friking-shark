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
	piRender->DeactivateDepthWrite();
	piRender->SetBlendingFunction(m_pType->m_dwBlendOp1,m_pType->m_dwBlendOp2);
	piRender->RenderParticle(m_pType->m_Texture.m_piTexture,m_PhysicInfo.vPosition,m_PhysicInfo.vAngles,m_dSize,m_dSize,0,0,1,1,m_vColor,m_dAlpha);
	piRender->SetBlendingFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	piRender->ActivateDepthWrite();
}
