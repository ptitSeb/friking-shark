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
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"
#include "ParticleSystemAnimationObjectType.h"

CParticleSystemAnimationObjectType::CParticleSystemAnimationObjectType()
{
    m_nStartTime=0;
    m_nEndTime=0;
    m_bTrackEntity=false;
}

CParticleSystemAnimationObjectType::~CParticleSystemAnimationObjectType()
{
}

IAnimationObject *CParticleSystemAnimationObjectType::CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)
{
    CParticleSystemAnimationObject *pParticle=new CParticleSystemAnimationObject(this,piAnimation);
    return pParticle;
}

void CParticleSystemAnimationObjectType::GetConfig(SParticleSystemAnimationObjectTypeConfig *pConfig)
{
	pConfig->nStartTime=m_nStartTime;
	pConfig->nEndTime=m_nEndTime;
	pConfig->bTrackEntity=m_bTrackEntity;
}

void CParticleSystemAnimationObjectType::SetConfig(SParticleSystemAnimationObjectTypeConfig *pConfig)
{
	m_nStartTime=pConfig->nStartTime;
	m_nEndTime=pConfig->nEndTime;
	m_bTrackEntity=pConfig->bTrackEntity;
}

void CParticleSystemAnimationObjectType::SetParticleSystemType(IParticleSystemType *piParticleSystem){m_ParticleSystemType.Attach(piParticleSystem);}
void CParticleSystemAnimationObjectType::GetParticleSystemType(IParticleSystemType **ppiParticleSystem){(*ppiParticleSystem)=ADD(m_ParticleSystemType.m_piParticleSystemType);}
std::string CParticleSystemAnimationObjectType::GetAnimationObjectDescription(){return m_ParticleSystemType.m_piObject?m_ParticleSystemType.m_piObject->GetName():"Unknown";}

CVector CParticleSystemAnimationObjectType::GetPosition(){return m_vPosition;}
void	CParticleSystemAnimationObjectType::SetPosition(CVector vPosition){m_vPosition=vPosition;}


CParticleSystemAnimationObject::CParticleSystemAnimationObject(CParticleSystemAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_piParticleSystem=NULL;
    m_pType=pType;
    m_bPositionAndAnglesInitialized=false;
}
CParticleSystemAnimationObject::~CParticleSystemAnimationObject()
{
    if(m_piParticleSystem){delete m_piParticleSystem;m_piParticleSystem=NULL;}
}

void CParticleSystemAnimationObject::Activate(unsigned int dwCurrentTime)
{
    if(m_piParticleSystem){delete m_piParticleSystem;m_piParticleSystem=NULL;}
    CAnimationObjectBase::Activate(dwCurrentTime);
    CheckActivation(dwCurrentTime);
}

void CParticleSystemAnimationObject::CheckActivation(unsigned int dwCurrentTime)
{
    unsigned int dwRelativeTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase();
    if(m_piParticleSystem==NULL)
    {
        if(m_pType->m_ParticleSystemType.m_piParticleSystemType && dwRelativeTime>=m_pType->m_nStartTime)
        {
            m_piParticleSystem=m_pType->m_ParticleSystemType.m_piParticleSystemType->CreateInstance(dwCurrentTime);
        }
    }
    else
    {
        if(m_pType->m_ParticleSystemType.m_piParticleSystemType && m_pType->m_nEndTime && dwRelativeTime>=m_pType->m_nEndTime)
        {
            Deactivate();
        }
    }
}

void CParticleSystemAnimationObject::Deactivate()
{
    if(m_piParticleSystem){m_piParticleSystem->DeactivateAllEmitters();}
    CAnimationObjectBase::Deactivate();
    m_bPositionAndAnglesInitialized=false;
}


void CParticleSystemAnimationObject::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
    if(m_piParticleSystem){m_piParticleSystem->CustomRender(piRender,piCamera);}
}

void CParticleSystemAnimationObject::UpdatePositionAndAngles()
{
    if(m_pType->m_bTrackEntity || !m_bPositionAndAnglesInitialized)
    {
        m_bPositionAndAnglesInitialized=true;
		CVector vForward,vRight,vUp,vPosition,vAngles;
		IEntity *piEntity=m_piAnimation->GetEntity();
        if(piEntity)
        {
            SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
            vPosition=pPhysicInfo->vPosition;
			vAngles=pPhysicInfo->vAngles;
		}
		m_piParticleSystem->SetAngles(vAngles);
		m_piParticleSystem->GetVectors(vForward,vRight,vUp);

		if(m_pType->m_vPosition!=Origin)
		{
			vPosition+=vForward*m_pType->m_vPosition.c[0];
			vPosition+=vUp*m_pType->m_vPosition.c[1];
			vPosition+=vRight*m_pType->m_vPosition.c[2];
		}
		m_piParticleSystem->SetPosition(vPosition);
	}
}

bool CParticleSystemAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
    CheckActivation(dwCurrentTime);
    if(!m_piParticleSystem){return false;}
    UpdatePositionAndAngles();
    return m_piParticleSystem->ProcessFrame(pPhysicManager,dwCurrentTime,dInterval);
}