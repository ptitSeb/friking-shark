#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"
#include "ParticleSystemAnimationObjectType.h"

CParticleSystemAnimationObjectType::CParticleSystemAnimationObjectType()
{
    m_dwStartTime=0;
    m_dwEndTime=0;
    m_bTrackEntity=false;
}

CParticleSystemAnimationObjectType::~CParticleSystemAnimationObjectType()
{
}

IAnimationObject *CParticleSystemAnimationObjectType::CreateInstance(IAnimation *piAnimation,DWORD dwCurrentTime)
{
    CParticleSystemAnimationObject *pParticle=new CParticleSystemAnimationObject(this,piAnimation);
    return pParticle;
}

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

void CParticleSystemAnimationObject::Activate(DWORD dwCurrentTime)
{
    if(m_piParticleSystem){delete m_piParticleSystem;m_piParticleSystem=NULL;}
    CAnimationObjectBase::Activate(dwCurrentTime);
    CheckActivation(dwCurrentTime);
}

void CParticleSystemAnimationObject::CheckActivation(DWORD dwCurrentTime)
{
    DWORD dwRelativeTime=dwCurrentTime-m_piAnimation->GetCurrentTimeBase();
    if(m_piParticleSystem==NULL)
    {
        if(m_pType->m_ParticleSystemType.m_piParticleSystemType && dwRelativeTime>=m_pType->m_dwStartTime)
        {
            m_piParticleSystem=m_pType->m_ParticleSystemType.m_piParticleSystemType->CreateInstance(dwCurrentTime);
        }
    }
    else
    {
        if(m_pType->m_ParticleSystemType.m_piParticleSystemType && m_pType->m_dwEndTime && dwRelativeTime>=m_pType->m_dwEndTime)
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
        IEntity *piEntity=m_piAnimation->GetEntity();
        if(piEntity)
        {
            SPhysicInfo *pPhysicInfo=piEntity->GetPhysicInfo();
            CVector vForward,vRight,vUp,vPosition=pPhysicInfo->vPosition;
            vPosition=pPhysicInfo->vPosition;
            m_piParticleSystem->SetAngles(pPhysicInfo->vAngles);
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
}

bool CParticleSystemAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,DWORD dwCurrentTime,double dInterval)
{
    CheckActivation(dwCurrentTime);
    if(!m_piParticleSystem){return false;}
    UpdatePositionAndAngles();
    return m_piParticleSystem->ProcessFrame(pPhysicManager,dwCurrentTime,dInterval);
}
