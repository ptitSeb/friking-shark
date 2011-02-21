#include "./stdafx.h"
#include "ParticleSystems.h"
#include "LightParticleType.h"

CLightParticleType::CLightParticleType()
{
}

CLightParticleType::~CLightParticleType()
{
}

IParticle *CLightParticleType::CreateInstance(IParticleEmitter *piEmiter,DWORD dwCurrentTime)
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
		m_Light.m_piLight->SetColor(m_vColor);
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