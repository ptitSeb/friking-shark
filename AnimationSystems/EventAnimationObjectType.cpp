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
    if(dwCurrentTime>m_pType->m_nTime)
    {
        IEntity *piEntity=m_piAnimation->GetEntity();
        if(piEntity){piEntity->OnAnimationEvent(m_pType->m_sEvent,m_pType->m_sParams);}
        m_bFired=true;
    }
    return true;
}

