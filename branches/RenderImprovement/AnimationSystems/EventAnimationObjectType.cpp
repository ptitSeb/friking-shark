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
#include "EventAnimationObjectType.h"

CEventAnimationObjectType::CEventAnimationObjectType()
{
    m_nTime=0;
}

CEventAnimationObjectType::~CEventAnimationObjectType()
{
}

IAnimationObject *CEventAnimationObjectType::CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)
{
    CEventAnimationObject *pParticle=new CEventAnimationObject(this,piAnimation);
    return pParticle;
}

void CEventAnimationObjectType::GetConfig(SEventAnimationObjectTypeConfig *pConfig)
{
	pConfig->nTime=m_nTime;
	pConfig->sName=m_sEvent;
	pConfig->sParams=m_sParams;
}

void CEventAnimationObjectType::SetConfig(SEventAnimationObjectTypeConfig *pConfig)
{
	m_nTime=pConfig->nTime;
	m_sEvent=pConfig->sName;
	m_sParams=pConfig->sParams;
}

std::string CEventAnimationObjectType::GetAnimationObjectDescription()
{
	return m_sEvent+"("+m_sParams+")";
}

CEventAnimationObject::CEventAnimationObject(CEventAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_bFired=false;
    m_pType=pType;
}

bool CEventAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
    if(!m_bActive){return false;}
    if(m_bFired){return false;}
    if(dwCurrentTime-m_piAnimation->GetCurrentTimeBase()>m_pType->m_nTime)
    {
        IEntity *piEntity=m_piAnimation->GetEntity();
        if(piEntity){piEntity->OnAnimationEvent(m_pType->m_sEvent,m_pType->m_sParams);}
        m_bFired=true;
    }
    return true;
}
