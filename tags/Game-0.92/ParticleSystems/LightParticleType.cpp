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
#include "LightParticleType.h"

CLightParticleType::CLightParticleType()
{
}

CLightParticleType::~CLightParticleType()
{
}

IParticle *CLightParticleType::CreateInstance(IParticleEmitter *piEmiter,unsigned int dwCurrentTime)
{
	CLightParticle *pParticle=new CLightParticle(this,piEmiter);
    return pParticle;
}

bool CLightParticleType::Init( std::string sClass,std::string sName,ISystem *piSystem )
{
	bool bOk=CSystemObjectBase::Init(sClass,sName,piSystem);
	m_Render.Attach("GameGUI","Render");
	return bOk;
}
void CLightParticleType::Destroy()
{
	m_Render.Detach();
	CSystemObjectBase::Destroy();
}

CLightParticle::CLightParticle(CLightParticleType *pType,IParticleEmitter *piEmiter)
{
	m_Light.Create("GameGUI","Light","");
    m_piEmiter=piEmiter;
    m_pType=pType;

	if(m_pType->m_Render.m_piRender && m_Light.m_piLight)
	{
		m_pType->m_Render.m_piRender->AddLight(m_Light.m_piLight);
	}
}

void CLightParticle::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
    if(m_Light.m_piLight)
	{
		m_Light.m_piLight->SetAmbientColor(m_vColor);
		m_Light.m_piLight->SetDiffuseColor(m_vColor);
		m_Light.m_piLight->SetSpecularColor(m_vColor);
		m_Light.m_piLight->SetPosition(m_PhysicInfo.vPosition+CVector(0,2,0));
		m_Light.m_piLight->SetOmni(m_dSize);
	}
}

CLightParticle::~CLightParticle()
{
	if(m_pType->m_Render.m_piRender && m_Light.m_piLight)
	{
		m_pType->m_Render.m_piRender->RemoveLight(m_Light.m_piLight);
	}
}