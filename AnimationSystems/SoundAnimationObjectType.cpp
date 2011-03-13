#include "./stdafx.h"
#include "AnimationSystems.h"
#include "AnimationObjectBase.h"
#include "SoundSystems.h"
#include "SoundAnimationObjectType.h"

CSoundAnimationObjectType::CSoundAnimationObjectType()
{
  m_nStartTime=0;
  m_nEndTime=0;
}

CSoundAnimationObjectType::~CSoundAnimationObjectType()
{
}

IAnimationObject *CSoundAnimationObjectType::CreateInstance(IAnimation *piAnimation,unsigned int dwCurrentTime)
{
    CSoundAnimationObject *pSound=new CSoundAnimationObject(this,piAnimation);
    return pSound;
}

void CSoundAnimationObjectType::GetConfig(SSoundAnimationObjectTypeConfig *pConfig)
{
	pConfig->nStartTime=m_nStartTime;
	pConfig->nEndTime=m_nEndTime;
}
void CSoundAnimationObjectType::SetConfig(SSoundAnimationObjectTypeConfig *pConfig)
{
	m_nStartTime=pConfig->nStartTime;
	m_nEndTime=pConfig->nEndTime;
}

void CSoundAnimationObjectType::SetSound(ISoundType *piSound){m_SoundType.Attach(piSound);}
void CSoundAnimationObjectType::GetSound(ISoundType **ppiSound){(*ppiSound)=ADD(m_SoundType.m_piSoundType);}

CSoundAnimationObject::CSoundAnimationObject(CSoundAnimationObjectType *pType,IAnimation *piAnimation)
:CAnimationObjectBase(pType,piAnimation)
{
    m_piSound=NULL;
    m_pType=pType;
}
CSoundAnimationObject::~CSoundAnimationObject()
{
    if(m_piSound){delete m_piSound;m_piSound=NULL;}
}

void CSoundAnimationObject::Activate(unsigned int dwCurrentTime)
{
    if(m_piSound){delete m_piSound;m_piSound=NULL;}
    CAnimationObjectBase::Activate(dwCurrentTime);
    if(m_pType->m_SoundType.m_piSoundType)
    {
      m_piSound=m_pType->m_SoundType.m_piSoundType->CreateInstance(m_piAnimation->GetEntity(),dwCurrentTime);
      if(m_piSound){m_piSound->Activate(dwCurrentTime);}
    }
}

void CSoundAnimationObject::Deactivate()
{
    if(m_piSound){m_piSound->Deactivate();}
    CAnimationObjectBase::Deactivate();
}

void CSoundAnimationObject::CustomRender(IGenericRender *piRender,IGenericCamera *piCamera)
{
}

bool CSoundAnimationObject::ProcessFrame(IPhysicManager *pPhysicManager,unsigned int dwCurrentTime,double dInterval)
{
    if(m_piSound)
    {
      m_piSound->ProcessFrame(pPhysicManager,dwCurrentTime,dInterval);
      if(!m_piSound->HasFinished())
      {
        return true;
      }
      else
      {
        m_piSound->Deactivate();
      }
    }
    return false;
}

