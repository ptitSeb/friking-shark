#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"
#include "EventAnimationObjectType.h"

CEventAnimationObjectType::CEventAnimationObjectType()
{
    m_dwTime=0;
}

CEventAnimationObjectType::~CEventAnimationObjectType()
{
}

IAnimationObject *CEventAnimationObjectType::CreateInstance(IAnimation *piAnimation,DWORD dwCurrentTime)
{
    CEventAnimationObject *pParticle=new CEventAnimationObject(this,piAnimation);
    return pParticle;
}

CEventAnimationObject::CEventAnimationObject(CEventAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_bFired=false;
    m_pType=pType;
}

bool CEventAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval)
{
    if(!m_bActive){return false;}
    if(m_bFired){return false;}
    if(dwCurrentTime>m_pType->m_dwTime)
    {
        IEntity *piEntity=m_piAnimation->GetEntity();
        if(piEntity){piEntity->OnAnimationEvent(m_pType->m_sEvent,m_pType->m_sParams);}
        m_bFired=true;
    }
    return true;
}

